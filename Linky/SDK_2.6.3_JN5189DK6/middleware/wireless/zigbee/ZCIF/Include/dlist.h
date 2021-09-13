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
 * MODULE:             Smart Energy
 *
 * COMPONENT:          dlist.h
 *
 * DESCRIPTION:       Doubly Linked list definitions
 *
 ****************************************************************************/

#ifndef DLIST_H_INCLUDED
#define DLIST_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include Files                                                 ***/
/****************************************************************************/

#include "jendefs.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/* DLNODE - Doubly linked-list node */
typedef struct tagDNODE DNODE;

struct tagDNODE
{
  DNODE *psNext;
  DNODE *psPrevious;
};

/* DLIST - Doubly-linked-list header */
typedef struct tagDLIST 
{
  DNODE *psFirst;  /* First node in list (Points to NULL if none) */
  DNODE *psLast;   /* Last node in list (Points to NULL if none) */
} DLIST;

typedef bool_t (boDLISTtest)(void *pvSearchParam, void *psNodeUnderTest);

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
/* Linked-list initialisation */
PUBLIC void vDLISTinitialise( DLIST *psList );

/* Search operations */
PUBLIC void *psDLISTgetHead( DLIST *psList );
PUBLIC void *psDLISTgetTail( DLIST *psList );

PUBLIC void *psDLISTgetNext( DNODE *psCurrentNode );
PUBLIC void *psDLISTgetPrevious( DNODE *psCurrentNode );

/* Addition operations */
PUBLIC void vDLISTaddToHead( DLIST *psList, DNODE *psNewNode );
PUBLIC void vDLISTinsertBefore( DLIST  *psList,
                                DNODE *psCurrentNode,
                                DNODE *psNewNode);
PUBLIC void vDLISTinsertAfter( DLIST  *psList,
                               DNODE *psCurrentNode,
                               DNODE *psNewNode);
PUBLIC void vDLISTaddToTail( DLIST *psList, DNODE *psNewNode );

/* Removal operations */
PUBLIC DNODE *psDLISTremove( DLIST *psList, DNODE *psNode );
PUBLIC DNODE *psDLISTremoveFromTail( DLIST *psList );
PUBLIC DNODE *psDLISTremoveFromHead( DLIST *psList );

/* Searching operations */
extern PUBLIC void *psDLISTsearchFromHead( DLIST *psList,
                                            boDLISTtest *prTest,
                                            void *pvSearchElement);
extern PUBLIC void *psDLISTsearchForward( DNODE *psCurrentNode,
                                           boDLISTtest *prTest,
                                           void *pvSearchElement);
extern PUBLIC void *psDLISTsearchBackward( DNODE *psCurrentNode,
                                            boDLISTtest *prTest,
                                            void *pvSearchElement);
extern PUBLIC void *psDLISTsearchFromTail( DLIST *psList,
                                            boDLISTtest *prTest,
                                            void *pvSearchElement);

/* Status operations */
PUBLIC int iDLISTnumberOfNodes( DLIST *psList );
PUBLIC bool_t boDLISTisEmpty( DLIST *psList );

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* DLIST_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
