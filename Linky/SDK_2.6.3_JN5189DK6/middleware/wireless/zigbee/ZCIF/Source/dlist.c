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
 * COMPONENT:          dlist.c
 *
 * DESCRIPTION:        Routines for creation, deletion and manipulation of arbitrary doubly-linked
 * lists
 *
 ****************************************************************************/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include "jendefs.h"
#include "dlist.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum {FORWARD, BACKWARD} eSearchDirection;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE bool_t boIsHead(DLIST *psList, DNODE *psNode);
PRIVATE bool_t boIsTail(DLIST *psList, DNODE *psNode);
PRIVATE DNODE *psSearchList( DNODE *psCurrentNode,
                             boDLISTtest *prTest,
                             void *pvSearchElement,
                             eSearchDirection eDirection);
/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:            vDLISTinitialise
 *
 * DESCRIPTION:
 * Initialise a doubly-linked list header
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psNewList       Pointer to the doubly-linked list header
 *
 * RETURNS:
 * Nothing
 *
 *****************************************************************************/
PUBLIC void vDLISTinitialise( DLIST *psNewList )
{
    psNewList->psFirst = NULL;
    psNewList->psLast = NULL;
}


/****************************************************************************
 *
 * NAME:             psDLISTgetHead
 *
 * DESCRIPTION:
 * Obtains a pointer to the first node in the list.
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList      R   The doubly-linked list we are working with
 *
 * RETURNS:
 * The first node in the relavant list, or NULL is there is no node in the list
 *
 *****************************************************************************/
PUBLIC void *psDLISTgetHead( DLIST *psList )
{
    return psList->psFirst;
}


/****************************************************************************
 *
 * NAME:             psDLISTgetTail
 *
 * DESCRIPTION:
 * Obtains a pointer to the last node in the list.
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList      R   The doubly-linked list we are working with
 *
 * RETURNS:
 * The last node in the relevant list, or NULL is there is no node in the list
 *
 *****************************************************************************/
PUBLIC void *psDLISTgetTail( DLIST *psList )
{
    return psList->psLast;
}


/****************************************************************************
 *
 * NAME:        psDLISTgetNext
 *
 * DESCRIPTION:
 * Obtains a pointer to the node nearer the tail of the list after the current
 * one.
 *
 * PARAMETERS:       Name        RW  Usage
 * DNODE *           psCurrentNode   Pointer to the current node position
 *
 * RETURNS:
 * Pointer to the next node in the list or NULL if the list is empty or
 * Current node is at the end of the list
 *
 ****************************************************************************/
PUBLIC void *psDLISTgetNext( DNODE *psCurrentNode )
{
    return psCurrentNode->psNext;
}


/****************************************************************************
 *
 * NAME:        psUTILgetPrevious
 *
 * DESCRIPTION:
 * Obtains a pointer to the node nearer the head of the list than the current
 * one.
 *
 * PARAMETERS:       Name        RW  Usage
 * DNODE *           psCurrentNode   Pointer to the current node position
 *
 * RETURNS:
 * Pointer to the previous node in the list or NULL if the list is empty or
 * CurrentNode is at the head of the list
 *
 ****************************************************************************/
PUBLIC void *psDLISTgetPrevious( DNODE *psCurrentNode )
{
    return psCurrentNode->psPrevious;
}


/****************************************************************************
 *
 * NAME:       vDLISTaddToHead
 *
 * DESCRIPTION:
 * Adds the new node onto the head of the specified list, and links the rest of
 * the list to it
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST  *          psList          The list to which the node is to be added
 * DNODE *           psNewNode       Node to be added to the list
 *
 * RETURNS:
 * Nothing
 *
 ****************************************************************************/
PUBLIC void vDLISTaddToHead( DLIST *psList, DNODE *psNewNode )
{
    if (boDLISTisEmpty(psList) == TRUE)
    {
        /* Put the first entry on the list, and make sure that the First and
         * Last list ptrs point to it.  Also need to point the previous and
         * next ptrs of the node to the list structure
         */
        psNewNode->psPrevious = (DNODE *)NULL;
        psNewNode->psNext = (DNODE *)NULL;
        psList->psFirst = psNewNode;
        psList->psLast = psNewNode;
    }
    else
    {
        psNewNode->psNext = psList->psFirst;
        psNewNode->psPrevious = (DNODE *)NULL;
        psList->psFirst->psPrevious = psNewNode;
        psList->psFirst = psNewNode;
    }
}


/****************************************************************************
 *
 * NAME:        vDLISTinsertBefore
 *
 * DESCRIPTION:
 * Insert the new node into a list before (ie nearer the head than) the
 * current specified node on the list
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST  *          psList          The list to which the node is to be added
 * DNODE *           psCurrentNode   Pointer to current list position
 * DNODE *           psNewNode       Pointer to node to be added to the list
 *
 * RETURNS:
 * Nothing
 *
 ****************************************************************************/
PUBLIC void vDLISTinsertBefore( DLIST  *psList,
                                DNODE *psCurrentNode,
                                DNODE *psNewNode )
{
    if (boIsHead(psList, psCurrentNode) == TRUE)
    {
        /* Adding to the head of the list...
         * Need to update the head pointer of the list structure...
         */
        psList->psFirst = psNewNode;
        /* ...and point the new node previous ptr to NULL */
        psNewNode->psPrevious = (DNODE *)NULL;
    }
    else
    {
        /* Not trying to insert at the head of the list, therefore we link the
         * new node next ptr to the current node and the new node previous ptr
         * to the node originally before the current node
         */
        psCurrentNode->psPrevious->psNext = psNewNode;
        psNewNode->psPrevious = psCurrentNode->psPrevious;
    }

    psNewNode->psNext = psCurrentNode;
    psCurrentNode->psPrevious = psNewNode;

}


/****************************************************************************
 *
 * NAME:        vDLISTinsertAfter
 *
 * DESCRIPTION:
 * Insert the new node into a list after (ie nearer the tail than) the
 * current specified node on the list
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList          Pointer to list to be added to
 * DNODE *           psCurrentNode   Pointer to current list position
 * DNODE *           psNewNode       Pointer to node to be added to the list
 *
 * RETURNS:
 * Nothing
 *
 ****************************************************************************/
PUBLIC void vDLISTinsertAfter( DLIST *psList,
                               DNODE *psCurrentNode,
                               DNODE *psNewNode)
{
    if (boIsTail(psList, psCurrentNode) == TRUE)
    {
        /* Adding to the tail of the list...
         * Make the new node next ptr point to NULL...
         */
        psNewNode->psNext = (DNODE *)NULL;
        /*...and the list last ptr link to the new node */
        psList->psLast = psNewNode;
    }
    else
    {
        /* Not trying to insert after the tail of the list, therefore we link
         * the new node next ptr to the node following the current one, and the
         * previous ptr of the node following the current node to the new node
         */
        psNewNode->psNext = psCurrentNode->psNext;
        psCurrentNode->psNext->psPrevious = psNewNode;
    }
    psCurrentNode->psNext = psNewNode;
    psNewNode->psPrevious = psCurrentNode;

}


/****************************************************************************
 *
 * NAME:        vDLISTaddToTail
 *
 * DESCRIPTION:
 * Insert the new node onto the tail of the specified list
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList          Pointer to list to be added to
 * DNODE *           psNewNode       Pointer to node to be added to the list
 *
 * RETURNS:
 * Nothing
 *
 ****************************************************************************/
PUBLIC void vDLISTaddToTail( DLIST *psList, DNODE *psNewNode )
{
    if (boDLISTisEmpty(psList) == TRUE)
    {
        /* If list is empty, adding to tail or head is the same */
        vDLISTaddToHead( psList, psNewNode );
    }
    else
    {
        psNewNode->psPrevious = psList->psLast;
        psNewNode->psNext = (DNODE *)NULL;
        psList->psLast->psNext = psNewNode;
        psList->psLast = psNewNode;
    }
}


/****************************************************************************
 *
 * NAME:        vDLISTremove
 *
 * DESCRIPTION:
 * Unlinks the selected node from the list it is currently linked into
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList          Pointer to list to be removed from
 * DNODE *           psNode          Pointer to node to be removed
 *
 * RETURNS:
 * Pointer to node removed from list
 *
 ****************************************************************************/
PUBLIC DNODE *psDLISTremove( DLIST *psList, DNODE *psNode )
{
    if (boIsTail(psList, psNode) == TRUE)
    {
        if (boIsHead(psList, psNode) == TRUE)
        {
            /* Last item on the list - need to set first and last ptrs to NULL */
            psList->psFirst = NULL;
            psList->psLast = NULL;
            return psNode;
        }
        else
        {
            /* This is the tail node of a list containing two or more nodes */
            return psDLISTremoveFromTail( psList );
        }
    }
    else
    {
        if (boIsHead(psList, psNode) == TRUE)
        {
            /* This is the head node of a list containing two or more nodes */
            return psDLISTremoveFromHead( psList );
        }
        else
        {
            /* This is an intermediate node on a list containing 3 or more nodes */
            psNode->psNext->psPrevious = psNode->psPrevious;
            psNode->psPrevious->psNext = psNode->psNext;
            return psNode;
        }
    }
}


/****************************************************************************
 *
 * NAME:        vDLISTremoveFromTail
 *
 * DESCRIPTION:
 * Unlinks the tail node from the specified list
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList          Pointer to list to remove node from tail
 *
 * RETURNS:
 * Pointer to node removed from list, or NULL if the list is empty
 *
 ****************************************************************************/
PUBLIC DNODE *psDLISTremoveFromTail( DLIST *psList )
{
    DNODE *psTailNode;

    if (boDLISTisEmpty(psList) == TRUE)
        return NULL;

    psTailNode = psList->psLast;
    if ( psList->psLast->psPrevious != (DNODE *)NULL )
    {
        /* More than one item on the list */
        psTailNode->psPrevious->psNext = (DNODE *)NULL;
        psList->psLast = psTailNode->psPrevious;
    }
    else
    {
        /* Last node on the list - set the list first and last ptrs to NULL
         * to show it is now empty
         */
        psList->psFirst = NULL;
        psList->psLast = NULL;
    }
    return psTailNode;
}


/****************************************************************************
 *
 * NAME:        vDLISTremoveFromHead
 *
 * DESCRIPTION:
 * Unlinks the head node from the specified list
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList          Pointer to list to remove node from head
 *
 * RETURNS:
 * Pointer to node removed from list, or NULL if the list is empty
 *
 ****************************************************************************/
PUBLIC DNODE *psDLISTremoveFromHead( DLIST *psList )
{
    DNODE *psHeadNode;

    if (boDLISTisEmpty(psList) == TRUE)
        return NULL;

    psHeadNode = psList->psFirst;
    if ( psList->psFirst->psNext != (DNODE *)NULL )
    {
        /* More than one item on the list */
        psHeadNode->psNext->psPrevious = (DNODE *)NULL;
        psList->psFirst = psHeadNode->psNext;
    }
    else
    {
        /* Last node on the list - set the list first and last ptrs to NULL
         * to show it is now empty
         */
        psList->psFirst = NULL;
        psList->psLast = NULL;
    }
    return psHeadNode;
}


/****************************************************************************
 *
 * NAME:        psDLISTsearchForward
 *
 * DESCRIPTION:
 * Search a list for a node containing a match for the comparison routine,
 * starting at the current position and moving to the tail of the list
 *
 * PARAMETERS:       Name        RW Usage
 * DNODE *           psCurrentNode  Pointer to current position on the list
 * DLISTtest *       prTest         Pointer to the test to be applied to
 *                                  each node
 * void *            pvSearchElement Pointer to element that test will use
 *                                  in match
 *
 * RETURNS:
 * Pointer to node for which the test returns TRUE, or NULL if there is no
 * match
 *
 ****************************************************************************/
PUBLIC void *psDLISTsearchForward( DNODE *psCurrentNode,
                                    boDLISTtest *prTest,
                                    void *pvSearchElement)
{
    return psSearchList( psCurrentNode, prTest, pvSearchElement, FORWARD);
}


/****************************************************************************
 *
 * NAME:        psDLISTsearchBackward
 *
 * DESCRIPTION:
 * Search a list for a node containing a match for the comparison routine,
 * starting at the current position and moving to the head of the list
 *
 * PARAMETERS:       Name        RW Usage
 * DNODE *           psCurrentNode  Pointer to current position on the list
 * DLISTtest *       prTest         Pointer to the test to be applied to
 *                                  each node
 * void *            pvSearchElement Pointer to element that test will use
 *                                  in match
 *
 * RETURNS:
 * Pointer to node for which the test returns TRUE, or NULL if there is no
 * match
 *
 ****************************************************************************/
PUBLIC void *psDLISTsearchBackward( DNODE *psCurrentNode,
                                     boDLISTtest *prTest,
                                     void *pvSearchElement)
{
    return psSearchList( psCurrentNode, prTest, pvSearchElement, BACKWARD);
}


/****************************************************************************
 *
 * NAME:        psDLISTsearchFromHead
 *
 * DESCRIPTION:
 * Search a list for a node containing a match for the comparison routine,
 * starting at the head of the list and moving to the tail
 *
 * PARAMETERS:       Name        RW Usage
 * DLIST *           psList         The list to be searched
 * DLISTtest *       prTest         Pointer to the test to be applied to
 *                                  each node
 * void *            pvSearchElement Pointer to element that test will use
 *                                  in match
 *
 * RETURNS:
 * Pointer to node for which the test returns TRUE, or NULL if there is no
 * match
 *
 ****************************************************************************/
PUBLIC void *psDLISTsearchFromHead( DLIST *psList,
                                     boDLISTtest *prTest,
                                     void *pvSearchElement)
{
    return psSearchList( psList->psFirst, prTest, pvSearchElement, FORWARD );
}


/****************************************************************************
 *
 * NAME:        psDLISTsearchFromTail
 *
 * DESCRIPTION:
 * Search a list for a node containing a match for the comparison routine,
 * starting at the tail of the list and moving to the head
 *
 * PARAMETERS:       Name        RW Usage
 * DLIST *           psList         The list to be searched
 * DLISTtest *       prTest         Pointer to the test to be applied to
 *                                  each node
 * void *            pvSearchElement Pointer to element that test will use
 *                                  in match
 *
 * RETURNS:
 * Pointer to node for which the test returns TRUE, or NULL if there is no
 * match
 *
 ****************************************************************************/
PUBLIC void *psDLISTsearchFromTail( DLIST *psList,
                                     boDLISTtest *prTest,
                                     void *pvSearchElement)
{
    return psSearchList( psList->psLast, prTest, pvSearchElement, BACKWARD );
}


/****************************************************************************
 *
 * NAME:        boDLISTisEmpty
 *
 * DESCRIPTION:
 * Indicates the state of a list
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList          Pointer to list being tested
 *
 * RETURNS:
 * TRUE if there are no entries on the list, FALSE otherwise
 *
 ****************************************************************************/
PUBLIC bool_t boDLISTisEmpty( DLIST *psList )
{
    return ((psList->psFirst == NULL) && (psList->psLast == NULL));
}


/****************************************************************************
 *
 * NAME:        iDLISTnumberOfNodes
 *
 * DESCRIPTION:
 * Gives the number of nodes on the list
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList          Pointer to list being checked
 *
 * RETURNS:
 * The number of elements on the list - 0 if empty
 *
 ****************************************************************************/
PUBLIC int iDLISTnumberOfNodes( DLIST *psList )
{
    int iCount = 0;
    DNODE *pPtr = psList->psFirst;

    while (pPtr != NULL)
    {
        pPtr = psDLISTgetNext( pPtr );
        iCount++;
    }

    return (iCount);
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME:        boIsHead
 *
 * DESCRIPTION:
 * Indicates if the node is on the head of the specified list
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList          Pointer to list being tested
 * DNODE *           psNode          Pointer to node on list
 *
 * RETURNS:
 * TRUE if the node is at the head of the list, FALSE otherwise
 *
 ****************************************************************************/
PRIVATE bool_t boIsHead(DLIST *psList, DNODE *psNode)
{
    return (psNode == (DNODE *)psList->psFirst);
}


/****************************************************************************
 *
 * NAME:        boIsTail
 *
 * DESCRIPTION:
 * Indicates if the node is on the tail of the specified list
 *
 * PARAMETERS:       Name        RW  Usage
 * DLIST *           psList          Pointer to list being tested
 * DNODE *           psNode          Pointer to node on list
 *
 * RETURNS:
 * TRUE if the node is at the tail of the list, FALSE otherwise
 *
 ****************************************************************************/
PRIVATE bool_t boIsTail(DLIST *psList, DNODE *psNode)
{
    return (psNode == (DNODE *)psList->psLast);
}


/****************************************************************************
 *
 * NAME:        psSearchList
 *
 * DESCRIPTION:
 * Search a list for a node containing a match for the comparison routine,
 * starting at the current position and moving in the requested direction
 *
 * PARAMETERS:       Name        RW Usage
 * DNODE *           psCurrentNode  Pointer to current position on the list
 * DLISTtest *       prTest         Pointer to the test to be applied to
 *                                  each node
 * void *            pvSearchElement Pointer to element that test will use
 *                                  in match
 * eSearchDirection  eDirection     Direction of search - FORWARD is from head
 *                                  to tail
 *
 * RETURNS:
 * Pointer to node for which the test returns TRUE, or NULL if there is no
 * match
 *
 ****************************************************************************/
PRIVATE DNODE *psSearchList( DNODE *psCurrentNode,
                             boDLISTtest *prTest,
                             void *pvSearchElement,
                             eSearchDirection eDirection)
{
    DNODE *psSearchPtr = psCurrentNode;

    if (psSearchPtr != NULL)
    {
        do
        {
            if (prTest(pvSearchElement, psSearchPtr) == TRUE)
                return (psSearchPtr);
            else
            {
                if (eDirection == FORWARD)
                    psSearchPtr = psSearchPtr->psNext;
                else
                    psSearchPtr = psSearchPtr->psPrevious;
            }
        }
        while (psSearchPtr != NULL);
    }

    return psSearchPtr;
}


/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
