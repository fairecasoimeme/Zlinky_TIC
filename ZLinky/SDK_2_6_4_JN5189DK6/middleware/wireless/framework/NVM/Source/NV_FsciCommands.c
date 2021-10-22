/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* This file implemens the handler functions for he request received from FSCI
* and the NVM moitoring functions.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/

#include "EmbeddedTypes.h"
#include "Messaging.h"
#include "FunctionLib.h"
#include "NVM_Interface.h"

#if gFsciIncluded_c && gNvStorageIncluded_d
#include "FsciInterface.h"
#include "NV_FsciCommands.h"

/******************************************************************************
*******************************************************************************
* Private functions prototype
******************************************************************************
******************************************************************************/

/*****************************************************************************
*****************************************************************************
* Private memory declarations
*****************************************************************************
*****************************************************************************/
#if gNvmEnableFSCIMonitoring_c
bool_t  nvmMonitorOn = TRUE;
#endif

/*****************************************************************************
*****************************************************************************
* Public functions
*****************************************************************************
*****************************************************************************/

/*****************************************************************************
*****************************************************************************
* Private functions
*****************************************************************************
*****************************************************************************/

#if gNvmEnableFSCIRequests_c
void NV_FsciMsgHandler( void* pData, void*param, uint32_t fsciInterface )
{
    bool_t reusePkt = FALSE;

    switch(((clientPacket_t*)pData)->structured.header.opCode)
    {
    case mFsciMsgNVSaveReq_c:
        reusePkt = FSCI_MsgNVSaveReqFunc( pData, fsciInterface );
        break;

    case mFsciMsgGetNVDataSetDescReq_c:
        reusePkt = FSCI_MsgGetNVDataSetDescReqFunc( pData, fsciInterface );
        break;

    case mFsciMsgGetNVCountersReq_c:
        reusePkt = FSCI_MsgGetNVCountersReqFunc( pData, fsciInterface );
        break;

    case mFsciMsgSetNVMonitoringReq_c:
        reusePkt = FSCI_MsgSetNVMonitoring( pData, fsciInterface );
        break;

    case mFsciMsgFormatNvmReq_c:
        reusePkt = FSCI_MsgNVFormatReq( pData, fsciInterface );
        break;

    case mFsciMsgRestoreNvmReq_c:
        reusePkt = FSCI_MsgNVRestoreReq( pData, fsciInterface );
        break;

    default:
        FSCI_Error( gFsciUnknownOpcode_c, fsciInterface );
        break;
    }
    
    if(reusePkt)
    {
        ((clientPacket_t*)pData)->structured.header.opGroup = gNV_FsciCnfOG_d;
        FSCI_transmitFormatedPacket((clientPacket_t*)pData, fsciInterface);
    }
    else
    {
        MSG_Free(pData);
    }
}

/******************************************************************************
Name: FSCI_MsgNVSaveReqFunc
Description:
In:
None
Out:
None
******************************************************************************/
bool_t FSCI_MsgNVSaveReqFunc(void* pData, uint32_t fsciInterface)
{
    uint16_t countInd=0;
    uint16_t dataSetIndex=0;
    uint16_t dataSetElement=0;
    NVM_DataEntry_t* pNVM_DataTableEntry=NULL;
    
    ((clientPacket_t*)pData)->structured.header.len = sizeof(clientPacketStatus_t);
    
    dataSetIndex = ((clientPacket_t*)pData)->structured.payload[1];
    dataSetIndex <<= 8;
    dataSetIndex+= ((clientPacket_t*)pData)->structured.payload[0];
    
    /* Look in NVM table for the dataset entry */
    while (countInd < gNVM_TABLE_entries_c)
    {
        if(dataSetIndex == (pNVM_DataTable+countInd)->DataEntryID)
        {
            pNVM_DataTableEntry = pNVM_DataTable+countInd;
            break;
        }
        countInd++;
    }
    
    if( NULL == pNVM_DataTableEntry)
    {
        /* data set not found */
        ((clientPacket_t*)pData)->structured.payload[0] = gNVM_InvalidTableEntry_c; 
        return TRUE;
    }
    
    if(!((clientPacket_t*)pData)->structured.payload[5])
    {
        dataSetElement = ((clientPacket_t*)pData)->structured.payload[2];
        dataSetElement <<= 8;
        dataSetElement += ((clientPacket_t*)pData)->structured.payload[3];
    }
    
    
    switch(((clientPacket_t*)pData)->structured.payload[4])
    {
    case 0:
        ((clientPacket_t*)pData)->structured.payload[0] = NvSaveOnIdle((uint8_t*)(pNVM_DataTableEntry->pData)+
                                                                       dataSetElement*pNVM_DataTableEntry->ElementSize,
                                                                       !!((clientPacket_t*)pData)->structured.payload[5]);
          break;
    case 1:
        ((clientPacket_t*)pData)->structured.payload[0] = NvSaveOnInterval(pNVM_DataTableEntry->pData);
        break;
    case 2:
        ((clientPacket_t*)pData)->structured.payload[0] = NvSaveOnCount(pNVM_DataTableEntry->pData);
        break;
    case 3:
        if(!!((clientPacket_t*)pData)->structured.payload[6])
        {
            ((clientPacket_t*)pData)->structured.payload[0] = NvAtomicSave();
        }
        else
        {
            ((clientPacket_t*)pData)->structured.payload[0] = 
                NvSyncSave((uint8_t*)(pNVM_DataTableEntry->pData)+
                           dataSetElement*pNVM_DataTableEntry->ElementSize,
                           !!((clientPacket_t*)pData)->structured.payload[5]);
        }
        break;
    default:
        ((clientPacket_t*)pData)->structured.payload[0] = gNVM_SaveRequestRejected_c; 
        break;
    }  
    return TRUE;
}

/******************************************************************************
Name: FSCI_MsgGetNVDataSetDescReqFunc
Description:
In:
None
Out:
None
******************************************************************************/
bool_t FSCI_MsgGetNVDataSetDescReqFunc(void* pData, uint32_t fsciInterface)
{
    uint16_t dataSetIndex;
    uint8_t payload[9];
    uint32_t address; 
    
    /* Left for nvm table count */  
    payload[0]=0;
    dataSetIndex = ((clientPacket_t*)pData)->structured.payload[1];
    dataSetIndex <<= 8;
    dataSetIndex+= ((clientPacket_t*)pData)->structured.payload[0];
    address = (uint32_t)(pNVM_DataTable+dataSetIndex)->pData;
    
    FLib_MemCpy(&payload[1],&address,4);
    FLib_MemCpy(&payload[5],&(pNVM_DataTable+dataSetIndex)->DataEntryID,2);
    payload[7] = ((pNVM_DataTable+dataSetIndex)->ElementSize & 0xFF);
    payload[8] = ((pNVM_DataTable+dataSetIndex)->ElementsCount & 0xFF);
    FSCI_transmitPayload(gNV_FsciCnfOG_d, mFsciMsgGetNVDataSetDescReq_c,payload,9,fsciInterface);  

    return FALSE;
    
}

/*********************************************  *********************************
Name: FSCI_MsgGetNVCountersReqFunc
Description:
In:
None
Out:
None
******************************************************************************/
bool_t FSCI_MsgGetNVCountersReqFunc(void* pData, uint32_t fsciInterface)
{
    NVM_Statistics_t ptrStat;
    uint8_t payload[sizeof(NVM_Statistics_t)+1];

    payload[0] = 0; 
    NvGetPagesStatistics(&ptrStat);
    FLib_MemCpy(&payload[1],&ptrStat.FirstPageEraseCyclesCount,4);
    FLib_MemCpy(&payload[5],&ptrStat.SecondPageEraseCyclesCount,4);
    FSCI_transmitPayload(gNV_FsciCnfOG_d,mFsciMsgGetNVCountersReq_c,payload,sizeof(NVM_Statistics_t)+1,fsciInterface);

    return FALSE;
}

/******************************************************************************
Name: FSCI_MsgSetNVMonitoring
Description:
In:
None
Out:
None
******************************************************************************/
bool_t FSCI_MsgSetNVMonitoring(void* pData, uint32_t fsciInterface)
{
#if !gNvmEnableFSCIMonitoring_c
    (void)pData;
    FSCI_Error( gFsciRequestIsDisabled_c, fsciInterface );    
    return FALSE;
#else
    nvmMonitorOn = !!((clientPacket_t*)pData)->structured.payload[0];
    ((clientPacket_t*)pData)->structured.payload[0] = 0;
    ((clientPacket_t*)pData)->structured.header.len = sizeof(clientPacketStatus_t);
    return TRUE;
#endif/* gNvStorageIncluded_d*/
}

/******************************************************************************
Name: FSCI_MsgNVFormatReq
Description:
In:
None
Out:
None
******************************************************************************/
bool_t FSCI_MsgNVFormatReq(void* pData, uint32_t fsciInterface)
{
    ((clientPacket_t*)pData)->structured.header.len = sizeof(clientPacketStatus_t);
    ((clientPacket_t*)pData)->structured.payload[0] = NvFormat();
    return TRUE;
}

/******************************************************************************
Name: FSCI_MsgNVRestoreReq
Description:
In:
None
Out:
None
******************************************************************************/
bool_t FSCI_MsgNVRestoreReq(void* pData, uint32_t fsciInterface)
{
    uint16_t countInd=0;
    uint16_t dataSetIndex;
    uint16_t dataSetElement=0;
    bool_t fSave;
    uint8_t status = gNVM_InvalidTableEntry_c;
    
    dataSetIndex = ((clientPacket_t*)pData)->structured.payload[3];
    dataSetIndex <<= 8;
    dataSetIndex+= ((clientPacket_t*)pData)->structured.payload[2];
    
    while (countInd < gNVM_TABLE_entries_c)
    {
        fSave=FALSE;
        if(((clientPacket_t*)pData)->structured.payload[0])
        {
            fSave = TRUE;
            dataSetElement=0;
        }
        else
        {
            if(dataSetIndex ==  (pNVM_DataTable+countInd)->DataEntryID)
            {
                if(gNvInvalidDataEntry_c != (pNVM_DataTable+countInd)->DataEntryID)
                {
                    fSave = TRUE;
                }
                else
                {
                    break;
                }
            }  
        }
        if(fSave)
        {    
            uint8_t* pData= (pNVM_DataTable+countInd)->pData;
            pData += dataSetElement * (pNVM_DataTable+countInd)->ElementSize;
            status = NvRestoreDataSet(pData,!!((clientPacket_t*)pData)->structured.payload[1]);
            if((!((clientPacket_t*)pData)->structured.payload[0])||(status))
            {
                break;
            }
        }
        countInd++;
    }

    ((clientPacket_t*)pData)->structured.header.len = sizeof(clientPacketStatus_t);
    ((clientPacket_t*)pData)->structured.payload[0] = status;
    return TRUE;
}
#endif

#if gNvmEnableFSCIMonitoring_c
/******************************************************************************
Name: FSCI_MsgNVPageEraseMonitoring
Description:
In:
None
Out:
None
******************************************************************************/
void FSCI_MsgNVPageEraseMonitoring(uint32_t pageAddress, uint8_t status)
{
    uint8_t payload[5];

    if(!nvmMonitorOn)
        return;
    
    FLib_MemCpy(&payload[0],&pageAddress ,4);
    payload[4]=status;

    FSCI_transmitPayload(gNV_FsciCnfOG_d,
                         mFsciMsgNVPageEraseMonitoring_c,
                         payload,
                         sizeof(payload),
                         gNvmDefaultFsciInterface_c);
}

/******************************************************************************
Name: FSCI_MsgNVWriteMonitoring
Description:
In:
None
Out:
None
******************************************************************************/
void FSCI_MsgNVWriteMonitoring(uint16_t nvmTableEntryId, uint16_t elementIndex, uint8_t saveRestoreAll)
{
    uint8_t payload[5];

    if( !nvmMonitorOn )
        return;
    
    FLib_MemCpy(&payload[0],&nvmTableEntryId ,2);
    FLib_MemCpy(&payload[2],&elementIndex ,2);
    payload[4]=saveRestoreAll;

#if mGetFsciInterfaceFromNvTableEntryId_d
    FSCI_transmitPayload(gNV_FsciCnfOG_d,
                         mFsciMsgNVWriteMonitoring_c,
                         payload,
                         sizeof(payload),
                         nvmTableEntryId>>12);
#else
    FSCI_transmitPayload(gNV_FsciCnfOG_d,
                         mFsciMsgNVWriteMonitoring_c,
                         payload,
                         sizeof(payload),
                         gNvmDefaultFsciInterface_c);
#endif
}

/******************************************************************************
Name: FSCI_MsgNVRestoreMonitoring
Description:
In:
None
Out:
None
******************************************************************************/
void FSCI_MsgNVRestoreMonitoring(uint16_t nvmTableEntryId, bool_t bStart, uint8_t status)
{
    uint8_t payload[4];
    
    if(!nvmMonitorOn)
        return;
    
    FLib_MemCpy(&payload[0],&nvmTableEntryId ,2);
    payload[2]=bStart;
    payload[3]=status;

#if mGetFsciInterfaceFromNvTableEntryId_d
    FSCI_transmitPayload(gNV_FsciCnfOG_d,
                         mFsciMsgRestoreMonitoring_c,
                         payload,
                         sizeof(payload),
                         nvmTableEntryId>>12);
#else
    FSCI_transmitPayload(gNV_FsciCnfOG_d,
                         mFsciMsgRestoreMonitoring_c,
                         payload,
                         sizeof(payload),
                         gNvmDefaultFsciInterface_c);
#endif
}

/******************************************************************************
Name: FSCI_MsgNVVirtualPageMonitoring
Description:
In:
None
Out:
None
******************************************************************************/                        
void FSCI_MsgNVVirtualPageMonitoring(bool_t bStart, uint8_t status)
{
    uint8_t payload[2];
    
    if(!nvmMonitorOn)
        return;

    payload[0]=bStart;
    payload[1]=status;

    FSCI_transmitPayload(gNV_FsciCnfOG_d,
                         mFsciMsgVirtualPageMonitoring_c,
                         payload,
                         sizeof(payload),
                         gNvmDefaultFsciInterface_c); 
}

#endif //#if gNvmEnableFSCIMonitoring_c

#endif //gFsciIncluded_c