/****************************************************************************
 *
 * Copyright 2020 NXP.
 *
 * NXP Confidential. 
 * 
 * This software is owned or controlled by NXP and may only be used strictly 
 * in accordance with the applicable license terms.  
 * By expressly accepting such terms or by downloading, installing, activating 
 * and/or otherwise using the software, you are agreeing that you have read, 
 * and that you agree to comply with and are bound by, such license terms.  
 * If you do not agree to be bound by the applicable license terms, 
 * then you may not retain, install, activate or otherwise use the software. 
 * 
 *
 ****************************************************************************/


/*****************************************************************************
 *
 * MODULE:             Colour Control Cluster
 *
 * COMPONENT:          ColourControlConversions.c
 *
 * DESCRIPTION:        Colour Control colour space conversion functions
 *
 *****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include "math.h"
#include "float.h"

#include "zps_apl.h"
#include "zps_apl_aib.h"

#include "zcl.h"
#include "zcl_customcommand.h"
#include "zcl_options.h"
#include "string.h"
#include "ColourControl.h"
#include "ColourControl_internal.h"


#include "dbg.h"

#ifdef DEBUG_CLD_COLOUR_CONTROL_CONVERSIONS
#define TRACE_COLOUR_CONTROL_CONVERSIONS    TRUE
#else
#define TRACE_COLOUR_CONTROL_CONVERSIONS    FALSE
#endif

#ifdef DEBUG_CLD_COLOUR_CONTROL
#define TRACE_COLOUR_CONTROL   TRUE
#else
#define TRACE_COLOUR_CONTROL   FALSE
#endif

#ifdef COLOUR_CONTROL_SERVER
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/* 3 way MAX and MIN macro's */
#define MIN3(X,Y,Z)     ((Y) <= (Z) ? ((X) <= (Y) ? (X) : (Y)) : ((X) <= (Z) ? (X) : (Z)))

#define MAX3(X,Y,Z)     ((Y) >= (Z) ? ((X) >= (Y) ? (X) : (Y)) : ((X) >= (Z) ? (X) : (Z)))

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/*

Hue                 = CurrentHue * 360 / 254            (degrees)
Saturation          = CurrentSaturation / 254
x                   = CurrentX / 65536
y                   = CurrentY / 65536
Colour Temperature  = 1000000 / ColourTemperature
*/

typedef struct
{
    uint16      u16Temperature;
    float       fx;
    float       fy;
} tsCLD_ColourControlCCT;

typedef struct
{
    float x;
    float y;
} CS_tsPoint;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE void vCLD_ColourControl_HSV2RGB(
        float                       fHue,
        float                       fSaturation,
        float                       fValue,
        float                       *pfRed,
        float                       *pfGreen,
        float                       *pfBlue);

PRIVATE void vCLD_ColourControl_RGB2HSV(
        float                       fRed,
        float                       fGreen,
        float                       fBlue,
        float                       *pfHue,
        float                       *pfSaturation,
        float                       *pfValue);

PRIVATE void vCLD_ColourControl_RGB2XYZ(
        float                       afMatrix[3][3],
        float                       fRed,
        float                       fGreen,
        float                       fBlue,
        float                       *pfX,
        float                       *pfY,
        float                       *pfZ);

PRIVATE void vCLD_ColourControl_XYZ2RGB(
        float                       afMatrix[3][3],
        float                       fX,
        float                       fY,
        float                       fZ,
        float                       *pfRed,
        float                       *pfGreen,
        float                       *pfBlue);

PRIVATE teZCL_Status eCLD_ColourControl_XYZ2xyY(
        float                       fX,
        float                       fY,
        float                       fZ,
        float                       *pfx,
        float                       *pfy,
        float                       *pfY);

PRIVATE teZCL_Status eCLD_ColourControl_xyY2XYZ(
        float                       fx,
        float                       fy,
        float                       fY,
        float                       *pfX,
        float                       *pfY,
        float                       *pfZ);

PRIVATE  bool_t bCLD_ColourControl_NumberIsValid(
        float                       fValue);

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)
PRIVATE bool_t CS_bIsInGamut(float x, float y);

PRIVATE bool_t CS_bDoLinesIntersect(float x1, float y1,
                                    float x2, float y2,
                                    float x3, float y3,
                                    float x4, float y4,
                                    float *x, float *y);
#endif

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/* Colour temperature vs CIE xyY coordinates */
tsCLD_ColourControlCCT asCLD_ColourControlCCTlist[] = {

    /*  CCT     x     y */
    {0,         (float)0.7000,     (float)0.3000},
    {1000,      (float)0.6499,     (float)0.3474},
    {1500,      (float)0.5841,     (float)0.3962},
    {2000,      (float)0.5267,     (float)0.4173},
    {2500,      (float)0.4782,     (float)0.4186},
    {3000,      (float)0.4388,     (float)0.4095},
    {4000,      (float)0.3827,     (float)0.3820},
    {5000,      (float)0.3473,     (float)0.3561},
    {6000,      (float)0.3242,     (float)0.3355},
    {6600,      (float)0.3140,     (float)0.3254},
    {6700,      (float)0.3125,     (float)0.3238},
    {7000,      (float)0.3083,     (float)0.3195},
    {8000,      (float)0.2970,     (float)0.3071},
    {9000,      (float)0.2887,     (float)0.2975},
    {10000,     (float)0.2824,     (float)0.2898},
    {11000,     (float)0.2774,     (float)0.2836},
    {12000,     (float)0.2734,     (float)0.2785},
    {13000,     (float)0.2702,     (float)0.2742},
    {14000,     (float)0.2675,     (float)0.2707},
    {15000,     (float)0.2653,     (float)0.2676},
    {16000,     (float)0.2634,     (float)0.2650},
    {20000,     (float)0.2580,     (float)0.2574},
    {25000,     (float)0.2541,     (float)0.2517},
    {30000,     (float)0.2516,     (float)0.2481},
    {35000,     (float)0.2500,     (float)0.2456},
    {40000,     (float)0.2487,     (float)0.2438},
    {65535,     (float)0.244151,   (float)0.236667}
};


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       eCLD_ColourControl_GetRGB
 **
 ** DESCRIPTION:
 ** Converts from xyY to RGB taking the path xyY->XYZ->RGB
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8SourceEndPointId          End point ID
 ** uint8                       *pu8Red                     Red value
 ** uint8                       *pu8Green                   Green value
 ** uint8                       *pu8Blue                    Blue value
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControl_GetRGB(
        uint8                       u8SourceEndPointId,
        uint8                       *pu8Red,
        uint8                       *pu8Green,
        uint8                       *pu8Blue)
{

    teZCL_Status eStatus;
    tsCLD_ColourControlCustomDataStructure *psCommon;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    tsCLD_ColourControl *psColourControl;

    float x, y, BigY;
    float X, Y, Z;
    float R, G, B;

    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(LIGHTING_CLUSTER_ID_COLOUR_CONTROL, u8SourceEndPointId, TRUE, &psEndPointDefinition, &psClusterInstance, (void*)&psCommon);
    if(eStatus != E_ZCL_SUCCESS)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL_CONVERSIONS, " No Endpoint");
        return eStatus;
    }

    psColourControl = (tsCLD_ColourControl*)psClusterInstance->pvEndPointSharedStructPtr;


    /* Convert xyY to floating point values */
    x    = (float)((float)psColourControl->u16CurrentX / 65535.0);
    y    = (float)((float)psColourControl->u16CurrentY / 65535.0);
    BigY = 1.0;

    /* Convert xyY to XYZ colour space */
    if(eCLD_ColourControl_xyY2XYZ(x, y, BigY, &X, &Y, &Z) != E_ZCL_SUCCESS)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    /* Convert XYZ to RGB colour space */
    vCLD_ColourControl_XYZ2RGB(psCommon->afXYZ2RGB, X, Y, Z, &R, &G, &B);

    *pu8Red     = (uint8)(255.0 * R);
    *pu8Green   = (uint8)(255.0 * G);
    *pu8Blue    = (uint8)(255.0 * B);

    return(E_ZCL_SUCCESS);
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControl_HSV2xyY
 **
 ** DESCRIPTION:
 ** Converts from HSV to xyY taking the path HSV->RGB->XYZ->xyY
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsCLD_ColourControlCustomDataStructure *psCustomDataStructPtr   Pointer to custom data structure
 ** uint8                       u8Hue                       Hue
 ** uint8                       u8Saturation                Saturation
 ** uint8                       u8Value                     Value
 ** uint16                      *pu16x                      x
 ** uint16                      *pu16y                      y
 ** uint8                       *pu8Y                       Y
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControl_HSV2xyY(
        uint8                       u8SourceEndPointId,
        uint16                      u16Hue,
        uint8                       u8Saturation,
        uint8                       u8Value,
        uint16                      *pu16x,
        uint16                      *pu16y,
        uint8                       *pu8Y)
{

    float H, S, V;
    float R, G, B;
    float X, Y, Z;
    float x, y, BigY;

    tsCLD_ColourControlCustomDataStructure *psCustomDataStructPtr;
    tsZCL_EndPointDefinition *psEndPointDefinition;
    tsZCL_ClusterInstance *psClusterInstance;
    teZCL_Status eStatus;
    /* Find pointers to cluster */
    eStatus = eZCL_FindCluster(LIGHTING_CLUSTER_ID_COLOUR_CONTROL,
                               u8SourceEndPointId,
                               TRUE,
                               &psEndPointDefinition,
                               &psClusterInstance,
                               (void*)&psCustomDataStructPtr);
    if(eStatus != E_ZCL_SUCCESS)
    {
        return eStatus;
    }
    
    /* Convert HSV to floating point values */
    H = (float)(((float)u16Hue / 65535.0) * 360.0);
    S = (float)((float)u8Saturation / 255.0);
    V = (float)((float)u8Value / 255.0);

    /* First we need to convert from HSV to RGB colour space */
    vCLD_ColourControl_HSV2RGB(H, S, V, &R, &G, &B);

    /* Now convert RGB to XYZ */
    vCLD_ColourControl_RGB2XYZ(psCustomDataStructPtr->afRGB2XYZ, R, G, B, &X, &Y, &Z);

    /* Finally we can convert from XYZ to xyY chromaticity coordinates and luminocity */
    if(eCLD_ColourControl_XYZ2xyY(X, Y, Z, &x, &y, &BigY) != E_ZCL_SUCCESS)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    /* Convert results to integer values */
    *pu16x = (uint16)(65535.0 * x);
    *pu16y = (uint16)(65535.0 * y);
    *pu8Y  = (uint8)(255.0 * BigY);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       vCLD_ColourControl_CCT2xyY
 **
 ** DESCRIPTION:
 ** Converts from Colour Temperature to xyY using interpolated data from a
 ** lookup table.
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint16                      u16ColourTemperatureMired   Colour Temperature
 ** uint16                      *pu16x                      x
 ** uint16                      *pu16y                      y
 ** uint8                       *pu8Y                       Y
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PUBLIC  void vCLD_ColourControl_CCT2xyY(
        uint16                      u16ColourTemperatureMired,
        uint16                      *pu16x,
        uint16                      *pu16y,
        uint8                       *pu8Y)
{

    uint32 u32CCT;
    float fRange;
    float fSteps;
    float fXdiff, fYdiff;
    float fXstep, fYstep;
    float x, y, BigY = 1.0;
    int n;

    tsCLD_ColourControlCCT *psPrev = &asCLD_ColourControlCCTlist[0];
    tsCLD_ColourControlCCT *psNext = &asCLD_ColourControlCCTlist[0];

    if(u16ColourTemperatureMired == 0) u16ColourTemperatureMired = 1;

    /* Convert Colour temperature none scaled inverse value */
    u32CCT = 1000000 / u16ColourTemperatureMired;

    /* Limit maximum colour temperature to last value in the table */
    if(u32CCT > asCLD_ColourControlCCTlist[(sizeof(asCLD_ColourControlCCTlist) / sizeof(tsCLD_ColourControlCCT)) -1].u16Temperature)
    {
        u32CCT = asCLD_ColourControlCCTlist[(sizeof(asCLD_ColourControlCCTlist) / sizeof(tsCLD_ColourControlCCT)) -1].u16Temperature;
    }

    /* Find 2 closest values in the table */
    for(n = 0; n < sizeof(asCLD_ColourControlCCTlist); n++)
    {
        /* Point to an entry in the Correlated Colour Temperature list */
        psNext = &asCLD_ColourControlCCTlist[n];

        /* If temp pointed to is greater than the one we wish to convert, the point
         * on the curve must be between the Prev and Next values in the table
         */
        if(asCLD_ColourControlCCTlist[n].u16Temperature >= u32CCT)
        {
            break;
        }

        psPrev = psNext;

    }

    /* How far apart are the points on the curve ? */
    fRange = (float)psNext->u16Temperature - (float)psPrev->u16Temperature;
    fXdiff = psNext->fx - psPrev->fx;
    fYdiff = psNext->fy - psPrev->fy;

    /* Work out step per degree for each axis */
    fXstep = fXdiff / fRange;
    fYstep = fYdiff / fRange;

    /* Number of steps in-between the points on the curve */
    fSteps = (float)(u32CCT - psPrev->u16Temperature);

    /* Calculate interpolated xy values */
    x = psPrev->fx + (fXstep * fSteps);
    y = psPrev->fy + (fYstep * fSteps);


    /* Return result */
    *pu16x = (uint16)(x * 65535.0);
    *pu16y = (uint16)(y * 65535.0);
    *pu8Y  = (uint8)(BigY * 255.0);

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControl_xyY2HSV
 **
 ** DESCRIPTION:
 ** Converts from xyY to HSV taking the path xyY->XYZ->RGB->HSV
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint16                      u16x                        x
 ** uint16                      u16y                        y
 ** uint8                       u8Y                         Y
 ** uint8                       *pu8Hue                     Hue
 ** uint8                       *pu8Saturation              Saturation
 ** uint8                       *pu8Value                   Value
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControl_xyY2HSV(
        tsCLD_ColourControlCustomDataStructure  *psCustomDataStructPtr,
        uint16                      u16x,
        uint16                      u16y,
        uint8                       u8Y,
        uint16                      *pu16Hue,
        uint8                       *pu8Saturation,
        uint8                       *pu8Value)
{

    float H, S, V;
    float R, G, B;
    float X, Y, Z;
    float x, y, BigY;

    /* Convert xyY to floating point values */
    x    = (float)((float)u16x / 65535.0);
    y    = (float)((float)u16y / 65535.0);
    BigY = (float)((float)u8Y / 255.0);

    /* Convert xyY to XYZ colour space */
    if(eCLD_ColourControl_xyY2XYZ(x, y, BigY, &X, &Y, &Z) != E_ZCL_SUCCESS)
    {
        return E_ZCL_ERR_PARAMETER_RANGE;
    }

    /* Convert XYZ to RGB colour space */
    vCLD_ColourControl_XYZ2RGB(psCustomDataStructPtr->afXYZ2RGB, X, Y, Z, &R, &G, &B);

    /* Finally, convert from RGB to HSV colour space */
    vCLD_ColourControl_RGB2HSV(R, G, B, &H, &S, &V);

    /* Convert results back to integer values */
    *pu16Hue        = (uint16)((H / 360.0) * 65535.0);
    *pu8Saturation = (uint8)(S * 255.0);
    *pu8Value      = (uint8)(V * 255.0);

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       vCLD_ColourControl_xyY2CCT
 **
 ** DESCRIPTION:
 ** Converts from xyY colour to Colour Temperature using interpolated data from
 ** a lookup table
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8Red                       Red
 ** uint8                       u8Green                     Green
 ** uint8                       u8Blue                      Blue
 ** uint16                      *pu16ColourTemperature      Colour Temperature
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PUBLIC  void vCLD_ColourControl_xyY2CCT(
        uint16                      u16x,
        uint16                      u16y,
        uint8                       u8Y,
        uint16                      *pu16ColourTemperature)
{
    int n;
    float x;

    float fCCT, fCCTrange, fCCTstep;
    float fXrange, fXdiff;

    tsCLD_ColourControlCCT *psPrev = &asCLD_ColourControlCCTlist[0];
    tsCLD_ColourControlCCT *psNext = &asCLD_ColourControlCCTlist[0];

    x = (float)((float)u16x / 65535.0);

    /* Find 2 closest values in the table */
    for(n = 0; n < sizeof(asCLD_ColourControlCCTlist) / sizeof(tsCLD_ColourControlCCT); n++)
    {
        /* Point to an entry in the Correlated Colour Temperature list */
        psNext = &asCLD_ColourControlCCTlist[n];

        /* Find a point on the curve where x values fit */
        if((x <= psPrev->fx) && (x >= psNext->fx))
        {
            break;
        }

        psPrev = psNext;

    }

    /* Interpolate CCT */
    fCCTrange = (float)psNext->u16Temperature - (float)psPrev->u16Temperature;
    fXrange = psPrev->fx - psNext->fx;
    fCCTstep = fCCTrange / fXrange;
    fXdiff = x - psNext->fx;
    fCCT = ((float)psNext->u16Temperature) - (fXdiff * fCCTstep);

    *pu16ColourTemperature = (uint16)(1000000.0 / fCCT);


}

/****************************************************************************
 **
 ** NAME:       eCLD_ColourControlCalculateConversionMatrices
 **
 ** DESCRIPTION:
 ** Creates the conversion matrices for converting between RGB and XYZ
 ** colour spaces for the given primaries and white point.
 **
 ** PARAMETERS:                 Name                        Usage
 ** tsCLD_ColourControlCustomDataStructure  *psCustomDataStructure  Pointer to custom data struct
 ** float                       fRedX                       X coordinate of Red primary
 ** float                       fRedY                       Y coordinate of Red primary
 ** float                       fGreenX                     X coordinate of Green primary
 ** float                       fGreenY                     Y coordinate of Green primary
 ** float                       fBlueX                      X coordinate of Blue primary
 ** float                       fGreenY                     Y coordinate of Blue primary
 ** float                       fWhiteX                     X coordinate of white point
 ** float                       fWhiteY                     Y coordinate of white point
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PUBLIC  teZCL_Status eCLD_ColourControlCalculateConversionMatrices(
        tsCLD_ColourControlCustomDataStructure  *psCustomDataStructure,
        float                                   fRedX,
        float                                   fRedY,
        float                                   fGreenX,
        float                                   fGreenY,
        float                                   fBlueX,
        float                                   fBlueY,
        float                                   fWhiteX,
        float                                   fWhiteY)
{

    float fRedZ, fGreenZ, fBlueZ;
    float D, U, V, u, v, w, Determinant;
    float Crx[3][3];
    float Cxr[3][3];

    /* Calculate missing coordinates */
    fRedZ   = (float)(1.0 - fRedX   - fRedY);
    fGreenZ = (float)(1.0 - fGreenX - fGreenY);
    fBlueZ  = (float)(1.0 - fBlueX  - fBlueY);

    /* Do white point correction */
    D = (fRedX   - fBlueX) * (fGreenY - fBlueY) - (fRedY   - fBlueY) * (fGreenX - fBlueX);
    U = (fWhiteX - fBlueX) * (fGreenY - fBlueY) - (fWhiteY - fBlueY) * (fGreenX - fBlueX);
    V = (fRedX   - fBlueX) * (fWhiteY - fBlueY) - (fRedY   - fBlueY) * (fWhiteX - fBlueX);
    u = U / D;
    v = V / D;
    w = (float)(1.0 - u - v);

    /* Create the conversion matrix for RGB to XYZ (Crx) */
    Crx[0][0] = u * (fRedX / fWhiteY); Crx[0][1] = v * (fGreenX / fWhiteY); Crx[0][2] = w * (fBlueX / fWhiteY);
    Crx[1][0] = u * (fRedY / fWhiteY); Crx[1][1] = v * (fGreenY / fWhiteY); Crx[1][2] = w * (fBlueY / fWhiteY);
    Crx[2][0] = u * (fRedZ / fWhiteY); Crx[2][1] = v * (fGreenZ / fWhiteY); Crx[2][2] = w * (fBlueZ / fWhiteY);

    /* Now we need to create the inverse matrix for XYZ to RGB (Cxr). Calculate the determinant  */
    Determinant = Crx[0][0] * (Crx[1][1] * Crx[2][2] - Crx[2][1] * Crx[1][2])
                - Crx[0][1] * (Crx[1][0] * Crx[2][2] - Crx[1][2] * Crx[2][0])
                + Crx[0][2] * (Crx[1][0] * Crx[2][1] - Crx[1][1] * Crx[2][0]);

    /* Use the determinant to calculate the inverse matrix */
    Cxr[0][0] =  (Crx[1][1] * Crx[2][2] - Crx[2][1] * Crx[1][2]) / Determinant;
    Cxr[1][0] = -(Crx[1][0] * Crx[2][2] - Crx[1][2] * Crx[2][0]) / Determinant;
    Cxr[2][0] =  (Crx[1][0] * Crx[2][1] - Crx[2][0] * Crx[1][1]) / Determinant;
    Cxr[0][1] = -(Crx[0][1] * Crx[2][2] - Crx[0][2] * Crx[2][1]) / Determinant;
    Cxr[1][1] =  (Crx[0][0] * Crx[2][2] - Crx[0][2] * Crx[2][0]) / Determinant;
    Cxr[2][1] = -(Crx[0][0] * Crx[2][1] - Crx[2][0] * Crx[0][1]) / Determinant;
    Cxr[0][2] =  (Crx[0][1] * Crx[1][2] - Crx[0][2] * Crx[1][1]) / Determinant;
    Cxr[1][2] = -(Crx[0][0] * Crx[1][2] - Crx[1][0] * Crx[0][2]) / Determinant;
    Cxr[2][2] =  (Crx[0][0] * Crx[1][1] - Crx[1][0] * Crx[0][1]) / Determinant;

    /* Save resulting matrices */
    memcpy(&psCustomDataStructure->afRGB2XYZ, Crx, sizeof(Crx));
    memcpy(&psCustomDataStructure->afXYZ2RGB, Cxr, sizeof(Cxr));

    return E_ZCL_SUCCESS;

}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
/****************************************************************************
 **
 ** NAME:       vCLD_ColourControl_HSV2RGB
 **
 ** DESCRIPTION:
 ** Converts from HSV colour to RGB
 **
 ** See Wikipedia: http://en.wikipedia.org/wiki/HSL_and_HSV
 **
 ** PARAMETERS:                 Name                        Usage
 ** float                       fHue                        Hue         (0-360)
 ** float                       fSaturation                 Saturation  (0 - 1)
 ** float                       fValue                      Value       (0 - 1)
 ** float                       *pfRed                      Result Red
 ** float                       *pfGreen                    Result Green
 ** float                       *pfBlue                     Result Blue
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE  void vCLD_ColourControl_HSV2RGB(
        float                       fHue,
        float                       fSaturation,
        float                       fValue,
        float                       *pfRed,
        float                       *pfGreen,
        float                       *pfBlue)
{

    float R, G, B;
    float H = fHue;
    float S = fSaturation;
    float V = fValue;
    float C = S * V;
    float Min = V - C;
    float X;

    H -= (float)(360 * floor(H/360));
    H /= 60;
    X = (float)(C * (1 - fabs(H - 2 * floor(H/2) - 1)));

    switch((int)H)
    {

    case 0:
        R = Min + C;
        G = Min + X;
        B = Min;
        break;

    case 1:
        R = Min + X;
        G = Min + C;
        B = Min;
        break;

    case 2:
        R = Min;
        G = Min + C;
        B = Min + X;
        break;

    case 3:
        R = Min;
        G = Min + X;
        B = Min + C;
        break;

    case 4:
        R = Min + X;
        G = Min;
        B = Min + C;
        break;

    case 5:
        R = Min + C;
        G = Min;
        B = Min + X;
        break;

    default:
        R = G = B = 0;
        break;

    }

    *pfRed      = R;
    *pfGreen    = G;
    *pfBlue     = B;

}


/****************************************************************************
 **
 ** NAME:       vCLD_ColourControl_RGB2HSV
 **
 ** DESCRIPTION:
 ** Converts from RGB colour to HSV
 **
 ** See Wikipedia: http://en.wikipedia.org/wiki/HSL_and_HSV
 **
 ** PARAMETERS:                 Name                        Usage
 ** uint8                       u8Red                       Red
 ** uint8                       u8Green                     Green
 ** uint8                       u8Blue                      Blue
 ** uint8                       *pu8Hue                     Hue
 ** uint8                       *pu8Saturation              Saturation
 ** uint8                       *pu8Value                   Value
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE  void vCLD_ColourControl_RGB2HSV(
        float                       fRed,
        float                       fGreen,
        float                       fBlue,
        float                       *pfHue,
        float                       *pfSaturation,
        float                       *pfValue)
{

    float H, S, V;

    float R = fRed;
    float G = fGreen;
    float B = fBlue;

    float Max = MAX3(R, G, B);
    float Min = MIN3(R, G, B);
    float C = Max - Min;

    V = Max;

    if(C > 0)
    {
        if(Max == R)
        {
            H = (G - B) / C;

            if(G < B)
            {
                H += 6;
            }
        }
        else if(Max == G)
        {
            H = 2 + (B - R) / C;
        }
        else
        {
            H = 4 + (R - G) / C;
        }

        H *= 60;
        S = C / Max;
    }
    else
    {
        H = S = 0;
    }

    *pfHue         = H;
    *pfSaturation  = S;
    *pfValue       = V;

}

/****************************************************************************
 **
 ** NAME:       vCLD_ColourControl_RGB2XYZ
 **
 ** DESCRIPTION:
 ** Converts from RGB to XYZ colour space using the given conversion matrix
 **
 ** PARAMETERS:                 Name                        Usage
 ** float                       afMatrix                    Pointer to 3x3 conversion matrix
 ** float                       fRed                        Red value
 ** float                       fGreen                      Green value
 ** float                       fBlue                       Blue value
 ** float                       *pfX                        Result X
 ** float                       *pfY                        Result Y
 ** float                       *pfZ                        Result Z
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE  void vCLD_ColourControl_RGB2XYZ(
        float                       afMatrix[3][3],
        float                       fRed,
        float                       fGreen,
        float                       fBlue,
        float                       *pfX,
        float                       *pfY,
        float                       *pfZ)
{

    *pfX = fRed * afMatrix[0][0] + fGreen * afMatrix[0][1] + fBlue * afMatrix[0][2];
    *pfY = fRed * afMatrix[1][0] + fGreen * afMatrix[1][1] + fBlue * afMatrix[1][2];
    *pfZ = fRed * afMatrix[2][0] + fGreen * afMatrix[2][1] + fBlue * afMatrix[2][2];

}


/****************************************************************************
 **
 ** NAME:       vCLD_ColourControl_XYZ2RGB
 **
 ** DESCRIPTION:
 ** Converts from XYZ to RGB colour space using the given conversion matrix
 **
 ** PARAMETERS:                 Name                        Usage
 ** float                       afMatrix                    Pointer to 3x3 conversion matrix
 ** float                       fX                          X value
 ** float                       fY                          Y value
 ** float                       fZ                          Z value
 ** float                       *pfRed                      Result Red
 ** float                       *pfGreen                    Result Green
 ** float                       *pfBlue                     Result Blue
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE  void vCLD_ColourControl_XYZ2RGB(
        float                       afMatrix[3][3],
        float                       fX,
        float                       fY,
        float                       fZ,
        float                       *pfRed,
        float                       *pfGreen,
        float                       *pfBlue)
{

    float R, G, B, Max;

    R = fX * afMatrix[0][0] + fY * afMatrix[0][1] + fZ * afMatrix[0][2];
    G = fX * afMatrix[1][0] + fY * afMatrix[1][1] + fZ * afMatrix[1][2];
    B = fX * afMatrix[2][0] + fY * afMatrix[2][1] + fZ * afMatrix[2][2];

    Max = MAX3(R, G, B);

    if(Max > 1.000){
        R = R / Max;
        G = G / Max;
        B = B / Max;
    }

    if(R < 0.0) R = 0;
    if(G < 0.0) G = 0;
    if(B < 0.0) B = 0;

    *pfRed      = R;
    *pfGreen    = G;
    *pfBlue     = B;

}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControl_XYZ2xyY
 **
 ** DESCRIPTION:
 ** Converts from XYZ to xyY colour space.
 **
 ** PARAMETERS:                 Name                        Usage
 ** float                       fX                          X value
 ** float                       fY                          Y value
 ** float                       fZ                          Z value
 ** float                       *pfx                        Result x
 ** float                       *pfy                        Result y
 ** float                       *pfY                        Result Y
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControl_XYZ2xyY(
        float                       fX,
        float                       fY,
        float                       fZ,
        float                       *pfx,
        float                       *pfy,
        float                       *pfY)
{

    /* x = X / (X + Y + Z) */
    *pfx = (fX / (fX + fY + fZ));

    /* y = Y / (X + Y + Z) */
    *pfy = (fY / (fX + fY + fZ));

    /* Y = Y */
    *pfY = fY;

    /* Check the results are valid */
    if((bCLD_ColourControl_NumberIsValid(*pfx) == FALSE) ||
       (bCLD_ColourControl_NumberIsValid(*pfy) == FALSE) ||
       (bCLD_ColourControl_NumberIsValid(*pfY) == FALSE))
    {
        return E_ZCL_FAIL;
    }

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       eCLD_ColourControl_xyY2XYZ
 **
 ** DESCRIPTION:
 ** Converts from xyY to XYZ colour space.
 **
 ** PARAMETERS:                 Name                        Usage
 ** float                       fx                          x value
 ** float                       fy                          y value
 ** float                       fY                          Y value
 ** float                       *pfX                        Result X
 ** float                       *pfY                        Result Y
 ** float                       *pfZ                        Result Z
 **
 ** RETURN:
 ** teZCL_Status
 **
 ****************************************************************************/
PRIVATE  teZCL_Status eCLD_ColourControl_xyY2XYZ(
        float                       fx,
        float                       fy,
        float                       fY,
        float                       *pfX,
        float                       *pfY,
        float                       *pfZ)
{

    /* z = 1 - x - y */
    float fz = (float)(1.0 - fx - fy);

    /* X = (x / y) * Y */
    *pfX = (fx / fy) * fY;

    /* Y = Y */
    *pfY = fY;

    /* Z = (z / y) * Y */
    *pfZ = (fz / fy) * fY;

    /* Check the results are valid */
    if((bCLD_ColourControl_NumberIsValid(*pfX) == FALSE) ||
       (bCLD_ColourControl_NumberIsValid(*pfY) == FALSE) ||
       (bCLD_ColourControl_NumberIsValid(*pfZ) == FALSE))
    {
        return E_ZCL_FAIL;
    }

    return E_ZCL_SUCCESS;
}


/****************************************************************************
 **
 ** NAME:       bCLD_ColourControl_NumberIsValid
 **
 ** DESCRIPTION:
 ** Checks is a floating point number is out of range
 **
 ** PARAMETERS:                 Name                        Usage
 ** float                       fValue                      value to test
 **
 ** RETURN:
 ** void
 **
 ****************************************************************************/
PRIVATE  bool_t bCLD_ColourControl_NumberIsValid(
        float                       fValue)
{

    short exp2;

    union
    {
        long    l;
        float   f;
    } uFL;

    uFL.f = fValue;

    exp2 = (unsigned char)(uFL.l >> 23) - 127;

    if (exp2 >= 31)
    {
        /* Number is too large */
        return FALSE;
    }
    else if (exp2 < -23)
    {
        /* Number is too small */
        return FALSE;
    }

    return TRUE;
}

#if (CLD_COLOURCONTROL_COLOUR_CAPABILITIES & COLOUR_CAPABILITY_XY_SUPPORTED)
/****************************************************************************
 **
 ** NAME:       CS_bTransitionIsValid
 **
 ** DESCRIPTION:
 **
 ** PARAMETERS:                 Name                        Usage
 ** u16X1                       Current X position
 ** u16Y1                       Current Y position
 ** u16X2                       Target X position for the transition
 ** u16Y2                       Target Y position for the transition
 ** *pu16X                      New X target if transition is outside of gamut
 ** *pu16Y                      New Y target if transition is outside of gamut
 **
 ** RETURN:
 ** bool_t                      TRUE if transition was inside of gamut, FALSE otherwise
 **
 ****************************************************************************/
PUBLIC bool_t CS_bTransitionIsValid(uint16 u16X1,  uint16 u16Y1,
                                    uint16 u16X2,  uint16 u16Y2,
                                    uint16 *pu16X, uint16 *pu16Y)
{

    float x1, y1;
    float x2, y2;
    float x, y;

    
    x1 = CLD_COLOURCONTROL_WHITE_X;
    y1 = CLD_COLOURCONTROL_WHITE_Y;
    x2 = (float)u16X2 / 65535.0f;
    y2 = (float)u16Y2 / 65535.0f;

    x = x2;
    y = y2;

    //DBG_vPrintf(TRACE_COLOUR_CONTROL, "Checking transition from %f,%f to %f,%f (%f,%f to %f,%f)\n", x1, y1, x2, y2, (float)u16X1, (float)u16Y1, (float)u16X2, (float)u16Y2);
    DBG_vPrintf(TRACE_COLOUR_CONTROL, "Checking transition from %d,%d to %d,%d\n", u16X1, u16Y1, u16X2, u16Y2);

    /* If the destination coordinates lie outside the available colour gamut, check where the path intersects the triangle edge */
    if(CS_bIsInGamut(x2, y2) == FALSE)
    {

        if((CS_bDoLinesIntersect(x1, y1, x2, y2, CLD_COLOURCONTROL_BLUE_X,    CLD_COLOURCONTROL_BLUE_Y,    CLD_COLOURCONTROL_RED_X,     CLD_COLOURCONTROL_RED_Y,     &x, &y) == TRUE) ||
           (CS_bDoLinesIntersect(x1, y1, x2, y2, CLD_COLOURCONTROL_RED_X,  CLD_COLOURCONTROL_RED_Y,     CLD_COLOURCONTROL_GREEN_X,    CLD_COLOURCONTROL_GREEN_Y, &x, &y) == TRUE) ||
           (CS_bDoLinesIntersect(x1, y1, x2, y2, CLD_COLOURCONTROL_GREEN_X, CLD_COLOURCONTROL_GREEN_Y,    CLD_COLOURCONTROL_BLUE_X,     CLD_COLOURCONTROL_BLUE_Y,     &x, &y) == TRUE))
        {
            *pu16X = (uint16)(x * 65535.0);
            *pu16Y = (uint16)(y * 65535.0);

            DBG_vPrintf(TRACE_COLOUR_CONTROL, "Transition invalid, intersection at %f,%f (%d,%d)\n", x, y, *pu16X, *pu16Y);
            DBG_vPrintf(TRACE_COLOUR_CONTROL, "Transition invalid, intersection at (%d,%d)\n", *pu16X, *pu16Y);

            return FALSE;
        }

    }

    return TRUE;

}

/****************************************************************************
 **
 ** NAME:       CS_bIsInGamut
 **
 ** DESCRIPTION:
 ** Checks if the specified coordinates lie inside the available colour gamut
 **
 ** PARAMETERS:                 Name                        Usage
 **
 ** RETURN:
 ** bool_t
 **
 ****************************************************************************/
PRIVATE bool_t CS_bIsInGamut(float x, float y)
{

    CS_tsPoint p1 = {CLD_COLOURCONTROL_RED_X,   CLD_COLOURCONTROL_RED_Y};
    CS_tsPoint p2 = {CLD_COLOURCONTROL_GREEN_X, CLD_COLOURCONTROL_GREEN_Y};
    CS_tsPoint p3 = {CLD_COLOURCONTROL_BLUE_X,  CLD_COLOURCONTROL_BLUE_Y};

    CS_tsPoint p; // <-- You are checking if this point lies in the triangle.

    p.x = x;
    p.y = y;

    // Now, the barycentric coordinates, generally called alpha, beta, and gamma, are calculated as follows:

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "Checking validity of coordinates %f,%f: ", x, y);

    float alpha = ((p2.y - p3.y)*(p.x - p3.x) + (p3.x - p2.x)*(p.y - p3.y)) /
            ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));

    float beta = ((p3.y - p1.y)*(p.x - p3.x) + (p1.x - p3.x)*(p.y - p3.y)) /
           ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));

    float gamma = 1.0f - alpha - beta;

    /* If all of alpha, beta, and gamma are greater than 0, then the point p lies within the triangle made of points p1, p2, and p3. */

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "Alpha=%f Beta=%f Gamma=%f: ", alpha, beta, gamma);

    if(alpha >= 0.0f && beta >= 0.0f && gamma >= 0.0f)
    {
        DBG_vPrintf(TRACE_COLOUR_CONTROL, "Valid\n");
        return TRUE;
    }

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "Invalid\n");

    return FALSE;
}


/****************************************************************************
 **
 ** NAME:       CS_bDoLinesIntersect
 **
 ** DESCRIPTION:
 ** Checks if the 2 lines specified intersect
 **
 ** PARAMETERS:                 Name                        Usage
 **
 ** RETURN:
 ** bool_t
 **
 ****************************************************************************/
PRIVATE bool_t CS_bDoLinesIntersect(float x1, float y1,
                                    float x2, float y2,
                                    float x3, float y3,
                                    float x4, float y4,
                                    float *x, float *y)
{
    float mua, mub;
    float denom, numera, numerb;

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "Checking for intersections of line %f,%f-%f,%f Through line %f,%f-%f,%f: ", x1, y1, x2, y2, x3, y3, x4, y4);

    denom  = (y4-y3) * (x2-x1) - (x4-x3) * (y2-y1);
    numera = (x4-x3) * (y1-y3) - (y4-y3) * (x1-x3);
    numerb = (x2-x1) * (y1-y3) - (y2-y1) * (x1-x3);

    /* Are the line coincident? (do they lie on top of each other) */
    if((fabs(numera) < FLT_EPSILON)  && (fabs(numerb) < FLT_EPSILON) && (fabs(denom) < FLT_EPSILON))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;

        DBG_vPrintf(TRACE_COLOUR_CONTROL, "Lines are coincident: %f,%f\n", *x, *y);
        DBG_vPrintf(TRACE_COLOUR_CONTROL, "Lines are coincident:\n");
        return TRUE;
    }

    /* Are the line parallel */
    if(fabs(denom) < FLT_EPSILON)
    {
        *x = 0;
        *y = 0;

        DBG_vPrintf(TRACE_COLOUR_CONTROL, "Lines are parallel\n");

        return FALSE;
    }

    /* Is the intersection along the the segments or somewhere we don't care about */
    mua = numera / denom;
    mub = numerb / denom;
    if(mua < 0 || mua > 1 || mub < 0 || mub > 1)
    {
        *x = 0;
        *y = 0;

        DBG_vPrintf(TRACE_COLOUR_CONTROL, "No intersection\n");

        return FALSE;
    }

    *x = x1 + mua * (x2 - x1);
    *y = y1 + mua * (y2 - y1);

    DBG_vPrintf(TRACE_COLOUR_CONTROL, "Intersection at %f,%f\n", *x, *y);
    DBG_vPrintf(TRACE_COLOUR_CONTROL, "Intersection \n");


    return TRUE;
}
#endif

#endif /* COLOUR_CONTROL_SERVER */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

