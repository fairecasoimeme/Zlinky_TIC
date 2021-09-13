/*! *********************************************************************************
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* Copyright 2016-2020 NXP
* All rights reserved.
*
* \file
*
* This is a source file for the messaging module.
* It contains the implementation of inter-task messaging.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */

/************************************************************************************
*************************************************************************************
* Include
*************************************************************************************
************************************************************************************/
#include "EmbeddedTypes.h"
#include "MemManager.h"
#include "Messaging.h"
#include "fsl_os_abstraction.h"
#include "assert.h"

/************************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private prototypes
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Private type definitions
*************************************************************************************
************************************************************************************/

#if defined(gMemManagerLight) && (gMemManagerLight == 1)
/* If MemManagerLight is used, need to add 16 bytes overhead
   due to the size of the ListHeader structure (16bytes) required
   for chaining the messages */
#define BLOCK_HDR_BYTES_OFFSET          16U
#define BLOCK_HDR_WORD_OFFSET           4U
#else
/* When MemManager is used, the message chaining
   reuses the MemManager header */
#define BLOCK_HDR_BYTES_OFFSET          0U
#define BLOCK_HDR_WORD_OFFSET           0U
#endif

/************************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
************************************************************************************/

/************************************************************************************
*************************************************************************************
* Public functions
*************************************************************************************
************************************************************************************/
/*! *********************************************************************************
* \brief     Links element to the tail of the list for the message system. 
*
* \param[in] list - pointer to the list to insert into.
*            pMsg - data block to add
*
* \return void.
*
* \pre Buffer must be allocated using MemManager.
*
* \post
*
* \remarks
*
********************************************************************************** */
listStatus_t ListAddTailMsg(listHandle_t list, void* pMsg)
{
  pMsg = (listHeader_t *)pMsg - 1;
  return ListAddTail(list, (listElementHandle_t)pMsg);
}

/*! *********************************************************************************
* \brief     Links element to the head of the list for the message system. 
*
* \param[in] list - pointer to the list to insert into.
*            pMsg - data block to add
*
* \return listStatus_t.
*
* \pre Buffer must be allocated using MemManager.
*
* \post
*
* \remarks
*
********************************************************************************** */
listStatus_t ListAddHeadMsg(listHandle_t list, void* pMsg)
{
  pMsg = (listHeader_t *)pMsg - 1;
  return ListAddHead(list, (listElementHandle_t)pMsg);
}

/*! *********************************************************************************
* \brief     Links the new element before a specified element. 
*
* \param[in] pMsg - current element from list
*            pNewMsg - new element to add
*
* \return listStatus_t.
*
* \pre Buffer must be allocated using MemManager.
*
* \post
*
* \remarks
*
********************************************************************************** */
listStatus_t ListAddPrevMsg( void* pMsg, void* pNewMsg )
{
  void *pElem = (listHeader_t*) pMsg - 1;
  void *pNewElem  = (listHeader_t*) pNewMsg - 1;
      
  return ListAddPrevElement((listElementHandle_t)pElem, (listElementHandle_t)pNewElem);            
}


/*! *********************************************************************************
* \brief     Unlinks element from the head of the list for the message system. 
*
* \param[in] list - pointer to the list to remove from.
*
* \return NULL if list is empty.
*         pointer to the data block if removal was successful.
*
* \pre Buffer must be allocated using MemManager.
*
* \post
*
* \remarks
*
********************************************************************************** */
void *ListRemoveHeadMsg(listHandle_t list)
{
  void *buffer;
  
  buffer = ListRemoveHead(list);
  return (buffer != NULL) ? (listHeader_t *)buffer+1 : buffer;
}

/*! *********************************************************************************
* \brief     Returns a pointer to the head of the list for the message system. 
*
* \param[in] list - pointer to the list
*
* \return NULL if list is empty.
*         pointer to the data block
*
* \pre
*
* \post
*
* \remarks
*
********************************************************************************** */
void *ListGetHeadMsg( listHandle_t list )
{
    listHeader_t *p;

    OSA_InterruptDisable();
    
    if( list->head != NULL )
    {
        p = (listHeader_t *)(void *)(list->head) + 1;
    }
    else
    {
        p = NULL;
    }
    
    OSA_InterruptEnable();

    return p;
}

/*! *********************************************************************************
* \brief     Returns a pointer to the data of the next message in the list.
*
* \param[in] pMsg - pointer to the data of the current message.
*
* \return NULL if buffer is the last element of the list.
*         pointer to the next data block if exists.
*
* \pre Buffer must be allocated using MemManager.
*
* \post
*
* \remarks
*
********************************************************************************** */
void *ListGetNextMsg( void* pMsg )
{
    listHeader_t *p;

    OSA_InterruptDisable();
    
    p = (listHeader_t*)pMsg - 1;

    if( p->link.next != NULL )
    {
        p = (listHeader_t*)(void *)(p->link.next) + 1;
    }
    else
    {
        p = NULL;
    }

    OSA_InterruptEnable();
    
    return p;
}

/*! *********************************************************************************
* \brief     Unlink the specified element from the list.
*
* \param[in] pMsg - pointer to the data of the current message.
*
* \return None.
*
* \pre Buffer must be allocated using MemManager.
*
* \post
*
* \remarks
*
********************************************************************************** */
listStatus_t ListRemoveMsg( void* pMsg )
{
    void *p = (listHeader_t*)pMsg - 1;

    return ListRemoveElement( (listElementHandle_t)p );
}

void* MSG_Alloc(uint32_t msgSize)
{
    uint32_t* buffer = NULL;

    buffer = MEM_BufferAllocWithId(msgSize + BLOCK_HDR_BYTES_OFFSET, 0, (uint32_t*) __get_LR());

    if( buffer != NULL )
    {
        buffer += BLOCK_HDR_WORD_OFFSET; /* pointing to payload */
    }
    return (void*)buffer;
}

uint16_t MSG_GetSize(void* buffer)
{
    uint16_t size = 0;
    if( buffer != NULL )
    {
        /* payload size if buffer size - message header size */
        size = MEM_BufferGetSize((uint32_t*)buffer - BLOCK_HDR_WORD_OFFSET);
        /* buffer size must be at least greater than message header size */
        assert(size > BLOCK_HDR_BYTES_OFFSET); 
        size -= (uint16_t)BLOCK_HDR_BYTES_OFFSET;
    }
    return size;
}

memStatus_t MSG_Free(void* buffer)
{
    memStatus_t ret = MEM_FREE_ERROR_c;

    if( buffer != NULL )
    {
        ret = MEM_BufferFree((uint32_t*)buffer - BLOCK_HDR_WORD_OFFSET);
    }
    return ret;
}
