/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */


#include "RNG_Interface.h"
#include "FunctionLib.h"
#include "SecLib.h"
#include "fsl_device_registers.h"
#include "fsl_os_abstraction.h"
#include "fsl_common.h"

#if (cPWR_UsePowerDownMode)
#include "PWR_Interface.h"
#endif

#if FSL_FEATURE_SOC_TRNG_COUNT
#if defined CPU_JN518X
    #include "fsl_rng.h"
    #define TRNG0       RNG
#else
    #include "fsl_trng.h"
#endif
#elif FSL_FEATURE_SOC_RNG_COUNT
    #if defined CPU_QN908X
        #include "fsl_rng.h"
        #include "fsl_power.h"
    #else
        #include "fsl_rnga.h"
    #endif
#endif
#if defined FSL_FEATURE_SOC_SHA_COUNT && (FSL_FEATURE_SOC_SHA_COUNT > 0)
#define JN_SHA_HW_ACC 1
#else
#define JN_SHA_HW_ACC 0
#endif

#if JN_SHA_HW_ACC
#include "fsl_sha.h"
#endif

/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */
#ifndef gRNG_UsePhyRngForInitialSeed_d
#define gRNG_UsePhyRngForInitialSeed_d 0
#endif

#define mPRNG_NoOfBits_c      (256)
#define mPRNG_NoOfBytes_c     (mPRNG_NoOfBits_c/8)
#define mPRNG_NoOfLongWords_c (mPRNG_NoOfBits_c/32)

#if (cPWR_UsePowerDownMode)
#define RNG_DisallowDeviceToSleep() PWR_DisallowDeviceToSleep()
#define RNG_AllowDeviceToSleep()    PWR_AllowDeviceToSleep()
#else
#define RNG_DisallowDeviceToSleep()
#define RNG_AllowDeviceToSleep()
#endif

/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */
static uint32_t XKEY[mPRNG_NoOfLongWords_c];
static uint32_t mPRNG_Requests = gRngMaxRequests_d;

#if FSL_FEATURE_SOC_TRNG_COUNT
uint8_t mRngDisallowMcuSleep = 0;
#endif

/*! *********************************************************************************
*************************************************************************************
* Public prototypes
*************************************************************************************
********************************************************************************** */
#if (FSL_FEATURE_SOC_RNG_COUNT == 0) && \
    (FSL_FEATURE_SOC_TRNG_COUNT == 0) && \
    (gRNG_UsePhyRngForInitialSeed_d)
extern void PhyGetRandomNo(uint32_t *pRandomNo);
#endif


/*! *********************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
********************************************************************************** */
#if FSL_FEATURE_SOC_TRNG_COUNT && !defined (CPU_JN518X)
static void TRNG_ISR(void);
#endif

/* Crypto Library function prototypes. */
extern uint32_t SecLib_set_rng_seed (uint32_t seed);
extern uint32_t SecLib_get_random(void);


/*! *********************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
********************************************************************************** */

/*! *********************************************************************************
* \brief  Initialize the RNG HW module
*
* \return  Status of the RNG initialization sequence
*
********************************************************************************** */
uint8_t RNG_Init(void)
{
    uint32_t seed;
    uint8_t status = gRngSuccess_d;
    static uint8_t initialized = FALSE;

    /* Check if RNG is already initialized */
    if( !initialized )
    {
#if FSL_FEATURE_SOC_RNG_COUNT
#ifdef CPU_QN908X
        POWER_EnableADC(true);
        RNG_Drv_Init(RNG);
        RNG_Enable(RNG, true);
        /* Get seed for pseudo RNG */
        if( RNG_GetRandomData(RNG, (uint8_t*)&seed, sizeof(uint32_t)) )
        {
            status = gRngInternalError_d;
        }
        RNG_Enable(RNG, false);
        POWER_EnableADC(false);
#else
        RNGA_Init(RNG);

        /* Get seed for pseudo RNG */
        if( RNGA_GetRandomData(RNG, &seed, sizeof(uint32_t)) )
        {
            status = gRngInternalError_d;
        }
#endif /* CPU_QN908X */
#elif FSL_FEATURE_SOC_TRNG_COUNT
        trng_config_t config;

        TRNG_GetDefaultConfig(&config);
#if !defined (CPU_JN518X) // TODO :add Feature flag in *_features.h
        config.frequencyCountLimit.minimum = 0x00000100;
        config.frequencyCountLimit.maximum = 0x000F0000;
        config.ringOscDiv = kTRNG_RingOscDiv0;
        config.entropyDelay = 1200;

        OSA_InstallIntHandler(TRNG0_IRQn, TRNG_ISR);
#endif
        if( kStatus_Success != TRNG_Init(TRNG0, &config) )
        {
            status = gRngInternalError_d;
        }
        else
        {
            /* Get seed for pseudo RNG */
            if( kStatus_Success != TRNG_GetRandomData(TRNG0, &seed, sizeof(seed)) )
            {
                status = gRngInternalError_d;
            }
#ifndef CPU_JN518X

            /* Check if the entropy generation ongoing */
            if( (!(TRNG0->MCTL & TRNG_MCTL_ENT_VAL_MASK)) && (!mRngDisallowMcuSleep) )
            {
                mRngDisallowMcuSleep = 1;
                RNG_DisallowDeviceToSleep();
            }
            
            /* Clear Interrupt status.*/
            TRNG0->INT_CTRL &= ~(TRNG_INT_CTRL_ENT_VAL_MASK | 
                                 TRNG_INT_CTRL_HW_ERR_MASK  | 
                                     TRNG_INT_CTRL_FRQ_CT_FAIL_MASK);
            
            /* Enable interrupt.*/
            EnableIRQ(TRNG0_IRQn);
            TRNG0->INT_MASK |= TRNG_INT_MASK_ENT_VAL_MASK | 
                               TRNG_INT_MASK_HW_ERR_MASK  | 
                               TRNG_INT_MASK_FRQ_CT_FAIL_MASK;
#endif
        }

#else
    #if gRNG_UsePhyRngForInitialSeed_d
        /* Use 802.15.4 PHY to generate the seed for RNG */
        PhyGetRandomNo(&seed);
    #else

  #ifdef CPU_HYPNOS_cm33
    /* Temporary stub waiting for RNG support */
    seed = 0x12345678;
  #else
    /* Use MCU unique Id for RNG seed */
    seed = SIM->UIDL;
  #endif

    #endif
#endif
        
        if( status == gRngSuccess_d )
        {
            /* Make sure that the seed is not zero */
            SecLib_set_rng_seed(seed+1);
        }
    }
    
    /* Init Successful */
    return status;
}


/*! *********************************************************************************
* \brief  Read a random number from the HW RNG module.
*         If the HW fails, the SW PRNG is used as backup.
*
* \param[out] pRandomNo - pointer to location where the value will be stored
*
* \return  status of the RNG module
*
********************************************************************************** */
uint8_t RNG_HwGetRandomNo(uint32_t* pRandomNo)
{
    uint8_t status = gRngSuccess_d;

    if(!pRandomNo)
    {
        status =  gRngNullPointer_d;
    }
    else
    {
#if FSL_FEATURE_SOC_RNG_COUNT
#ifndef CPU_QN908X
        if( RNGA_GetRandomData(RNG, pRandomNo, sizeof(uint32_t)) )
        {
            status = gRngInternalError_d;
        }
#else
        POWER_EnableADC(true);
        RNG_Enable(RNG, true);
        if( RNG_GetRandomData(RNG, (uint8_t*)pRandomNo, sizeof(uint32_t)) )
        {
            status = gRngInternalError_d;
        }
        RNG_Enable(RNG, false);
        POWER_EnableADC(false);
#endif
        
#elif FSL_FEATURE_SOC_TRNG_COUNT
        do {
#ifndef CPU_JN518X
            if( !(TRNG0->MCTL & TRNG_MCTL_ENT_VAL_MASK) )
            {
                status = gRngInternalError_d;
                break;
            }
#endif
            if( kStatus_Success != TRNG_GetRandomData(TRNG0, pRandomNo, sizeof(uint32_t)) )
            {
                status = gRngInternalError_d;
                break;
            }
#ifndef CPU_JN518X
            if( (!(TRNG0->MCTL & TRNG_MCTL_ENT_VAL_MASK)) && (!mRngDisallowMcuSleep) )
            {
                mRngDisallowMcuSleep = 1;
                RNG_DisallowDeviceToSleep();
            }
#endif
        } while (0);
        
#elif gRNG_UsePhyRngForInitialSeed_d
        PhyGetRandomNo(pRandomNo);
#else
        status = gRngInternalError_d;
#endif
    }
    return status;
}


/*! *********************************************************************************
* \brief  Generates a 32-bit statistically random number
*         No random number will be generated if the RNG was not initialized
*         or an error occurs.
*
* \param[out]  pRandomNo  Pointer to location where the value will be stored
*
********************************************************************************** */
void RNG_GetRandomNo(uint32_t* pRandomNo)
{
    /* Check for NULL pointers */
    if (NULL != pRandomNo)
    {
        uint32_t n = SecLib_get_random();
        *pRandomNo = n;
    }
}


/*! *********************************************************************************
* \brief  Initialize seed for the PRNG algorithm.
*         If this function is called again, even with a NULL argument,
*         the PRNG will be reseeded.
*
* \param[in]  pSeed  Ignored - please set to NULL
*             This parameter is ignored because it is no longer needed.
*             The PRNG is automatically seeded from the true random source.
*
********************************************************************************** */
void RNG_SetPseudoRandomNoSeed(uint8_t* pSeed)
{
    uint8_t pseudoRNGSeed[mPRNG_NoOfBytes_c] = {0};
    uint32_t i;
    
    mPRNG_Requests = 1;
    
    for (i = 0; i < mPRNG_NoOfBytes_c; i+=4)
    {
        RNG_GetRandomNo((uint32_t*)(&(pseudoRNGSeed[i])));
    }
    FLib_MemCpy( XKEY, pseudoRNGSeed, mPRNG_NoOfBytes_c );
}


/*! *********************************************************************************
* \brief  Pseudo Random Number Generator (PRNG) implementation
*         according to NIST FIPS Publication 186-2, APPENDIX 3
*
*         Let x be the signer's private key.  
*         The following may be used to generate m values of x:
*           Step 1. Choose a new, secret value for the seed-key, XKEY.
*           Step 2. In hexadecimal notation let
*             t = 67452301 EFCDAB89 98BADCFE 10325476 C3D2E1F0.
*             This is the initial value for H0 || H1 || H2 || H3 || H4 in the SHS.
*           Step 3. For j = 0 to m - 1 do
*             a. XSEEDj = optional user input.
*             b. XVAL = (XKEY + XSEEDj) mod 2^b
*             c. xj = G(t,XVAL) mod q
*             d. XKEY = (1 + XKEY + xj) mod 2^b
*
* \param[out]    pOut - pointer to the output buffer
* \param[in]     outBytes - the number of bytes to be copied (1-20)
* \param[in]     pXSEED - optional user SEED. Should be NULL if not used.
*
* \return  The number of bytes copied or -1 if reseed is needed
*
********************************************************************************** */
int16_t RNG_GetPseudoRandomNo(uint8_t* pOut, uint8_t outBytes, uint8_t* pXSEED)
{
    uint32_t i;
    int16_t outputBytes = outBytes;
    /* PRNG buffer used for both input and output */
    uint32_t prngBuffer[mPRNG_NoOfLongWords_c] = {0};

    if(pXSEED)
    {
        mPRNG_Requests = 1;
    }

    if (mPRNG_Requests == gRngMaxRequests_d)
    {
        outputBytes = -1;
    }
    else
    {
        mPRNG_Requests++;
        
        /* a. XSEEDj = optional user input. */
        if (pXSEED)
        {
            /* b. XVAL = (XKEY + XSEEDj) mod 2^b */
            for (i=0; i<mPRNG_NoOfBytes_c; i++)
            {
                ((uint8_t*)prngBuffer)[i] = ((uint8_t*)XKEY)[i] + pXSEED[i];
            }
        }
        else
        {
            for (i=0; i<mPRNG_NoOfBytes_c; i++)
            {
                ((uint8_t*)prngBuffer)[i] = ((uint8_t*)XKEY)[i];
            }
        }
        
        /* c. xj = G(t,XVAL) mod q
        ***************************/
        SHA256_Hash((uint8_t*)prngBuffer, mPRNG_NoOfBytes_c, (uint8_t*)prngBuffer);
        /* d. XKEY = (1 + XKEY + xj) mod 2^b */
        XKEY[0] += 1;
        for (i=0; i<mPRNG_NoOfLongWords_c; i++)
        {
            XKEY[i] += prngBuffer[i];
        }
        
        /* Check if the length provided exceeds the output data size */
        if (outputBytes > mPRNG_NoOfBytes_c)
        {
            outputBytes = mPRNG_NoOfBytes_c;
        }
        
        /* Copy the generated number */
        for (i=0; i < outputBytes; i++)
        {
            pOut[i] = ((uint8_t*)prngBuffer)[i];
        }
    }

    return outputBytes;
}

/*! *********************************************************************************
* \brief  Returns a random number between 0 and 256
*
* \return random number
*
********************************************************************************** */
uint32_t RND_u32GetRand256(void)
{
    uint32_t n;
    
    RNG_GetRandomNo(&n);
    
    return n & 0xFF;
}

/*! *********************************************************************************
* \brief  Returns a random number between the specified minimum and maximum values
*
* \param[in] u32Min  minimum value
* \param[in] u32Max  maximum value
*
* \return random number
*
********************************************************************************** */
uint32_t RND_u32GetRand(uint32_t u32Min, uint32_t u32Max)
{
    uint32_t n;
    
    RNG_GetRandomNo(&n);
    
    return n % (u32Max - u32Min) + u32Min;
}


/*! *********************************************************************************
*************************************************************************************
* Private functions
*************************************************************************************
********************************************************************************** */
#if FSL_FEATURE_SOC_TRNG_COUNT
#ifndef CPU_JN518X
static void TRNG_ISR(void)
{
    /* Clear Interrupt flags */
    TRNG0->INT_CTRL &= ~(TRNG_INT_CTRL_ENT_VAL_MASK | 
                         TRNG_INT_CTRL_HW_ERR_MASK  | 
                         TRNG_INT_CTRL_FRQ_CT_FAIL_MASK);

    if( (TRNG0->MCTL & TRNG_MCTL_ENT_VAL_MASK) && (mRngDisallowMcuSleep) )
    {
        mRngDisallowMcuSleep = 0;
        RNG_AllowDeviceToSleep();
    }
}
#endif
#endif

/********************************** EOF ***************************************/
