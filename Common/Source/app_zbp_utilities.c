/*****************************************************************************
 *
 * MODULE:             JN-AN-1220 ZLO Sensor Demo
 *
 * COMPONENT:          app_zpb_utilities.c
 *
 * DESCRIPTION:        Stack Development Utilities (Implementation)
 *
 ****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5169,
 * JN5179, JN5189].
 * You, and any third parties must reproduce the copyright and warranty notice
 * and any other legend of ownership on each copy or partial copy of the
 * software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Copyright NXP B.V. 2016-2018. All rights reserved
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

/* Stack Includes */
#include <jendefs.h>
#include <string.h>
#include "ZTimer.h"
#include "zps_apl_af.h"
#include "zps_apl_aib.h"
#include "zps_nwk_sap.h"
#include "zps_nwk_nib.h"
#include "zps_nwk_pub.h"

#include "pdum_gen.h"
#include "dbg.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef DEBUG_TIMERS
    #define TRACE_TIMERS    FALSE
#else
    #define TRACE_TIMERS    TRUE
#endif

#ifndef DEBUG_ZBP_UTILS
    #define TRACE_ZBP_UTILS FALSE
#else
    #define TRACE_ZBP_UTILS TRUE
#endif

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        External Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Tasks                                                         ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************
 *
 * NAME: vDisplayTableSizes
 *
 * DESCRIPTION:
 * Displays the sizes of the various tables within the PRO stack
 *
 ****************************************************************************/
PUBLIC void vDisplayTableSizes(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    DBG_vPrintf(TRACE_ZBP_UTILS, "Address Map:Size: %d: Record %d: %d: Total: %d ",
                                thisNib->sTblSize.u16AddrMap,
                                10,
                                (thisNib->sTblSize.u16AddrMap * 10)
                                );


    DBG_vPrintf(TRACE_ZBP_UTILS, "NT:Size: %d: Record %d: %d: Total: %d ",
                                thisNib->sTblSize.u16NtActv,
                                sizeof(ZPS_tsNwkActvNtEntry),
                                (thisNib->sTblSize.u16NtActv * sizeof(ZPS_tsNwkActvNtEntry))
                                );



    DBG_vPrintf(TRACE_ZBP_UTILS, "Routing Table:Size: %d: Record %d: %d: Total: %d ",
                                thisNib->sTblSize.u16Rt,
                                sizeof(ZPS_tsNwkRtEntry),
                                (thisNib->sTblSize.u16Rt * sizeof(ZPS_tsNwkRtEntry))
                                );


    DBG_vPrintf(TRACE_ZBP_UTILS, "Route Record:Size: %d: Record %d: %d: Total: %d ",
                                thisNib->sTblSize.u16Rct,
                                sizeof(ZPS_tsNwkRctEntry),
                                (thisNib->sTblSize.u16Rct * sizeof(ZPS_tsNwkRctEntry))
                                );
}



/****************************************************************************
 *
 * NAME: vDisplayAddressMapTable
 *
 * DESCRIPTION:
 * Displays the address map table within the PRO stack
 *
 ****************************************************************************/
PUBLIC void vDisplayAddressMapTable(void)
{
    ZPS_tsNwkNib * thisNib;
    void * thisNet = ZPS_pvAplZdoGetNwkHandle();

    thisNib = ZPS_psNwkNibGetHandle(thisNet);

    uint8 i = 0;

    DBG_vPrintf(TRACE_ZBP_UTILS,"\r\nAddress Map Size: %d", thisNib->sTblSize.u16AddrMap);

    for( i=0;i<thisNib->sTblSize.u16AddrMap;i++)
    {
        DBG_vPrintf(TRACE_ZBP_UTILS,"\nShort Addr: %04x, Ext Addr: %016llx,", thisNib->sTbl.pu16AddrMapNwk[i], ZPS_u64NwkNibGetMappedIeeeAddr(thisNet,thisNib->sTbl.psNtActv[i].u16Lookup));
    }
}



/****************************************************************************
 *
 * NAME: vDisplayNT
 *
 * DESCRIPTION:
 * Displays the neighbour table within the PRO stack
 *
 ****************************************************************************/
PUBLIC void vDisplayNT( void )
{
    void * thisNet = ZPS_pvAplZdoGetNwkHandle();
    ZPS_tsNwkNib * thisNib = ZPS_psNwkNibGetHandle(thisNet);
    uint8 i;

    DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nNT Size: %d\n", thisNib->sTblSize.u16NtActv);

    for( i = 0 ; i < thisNib->sTblSize.u16NtActv ; i++ )
    {

        DBG_vPrintf(TRACE_ZBP_UTILS, "SAddr: 0x%04x - ExtAddr: 0x%016llx - LQI: %i - Failed TX's: %i - Auth: %i - %i %i %i %i %i %i - Active: %i - %i %i %i\n",
                    thisNib->sTbl.psNtActv[i].u16NwkAddr,
                    ZPS_u64NwkNibGetMappedIeeeAddr(thisNet,thisNib->sTbl.psNtActv[i].u16Lookup),
                    thisNib->sTbl.psNtActv[i].u8LinkQuality,
                    thisNib->sTbl.psNtActv[i].u8TxFailed,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1Authenticated,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1DeviceType,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1ExpectAnnc,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1LinkStatusDone,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1PowerSource,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1RxOnWhenIdle,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1SecurityMode,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u1Used,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u2Relationship,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u3Age,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u3OutgoingCost
                    );

    }
}



/****************************************************************************
 *
 * NAME: vClearNTEntry
 *
 * DESCRIPTION:
 * Clears the NT entry specified by the address parameter
 *
 ****************************************************************************/
PUBLIC void vClearNTEntry( uint64 u64AddressToRemove )
{

    void * thisNet = ZPS_pvAplZdoGetNwkHandle();
    ZPS_tsNwkNib * thisNib = ZPS_psNwkNibGetHandle(thisNet);

    uint8 i;

    DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nNT Size: %d\n", thisNib->sTblSize.u16NtActv);

    for( i = 0 ; i < thisNib->sTblSize.u16NtActv ; i++ )
    {

        if(ZPS_u64NwkNibGetMappedIeeeAddr(thisNet,thisNib->sTbl.psNtActv[i].u16Lookup) == u64AddressToRemove)
        {
             memset( &thisNib->sTbl.psNtActv[i], 0, sizeof(ZPS_tsNwkActvNtEntry) );
             thisNib->sTbl.psNtActv[i].u16NwkAddr = ZPS_NWK_INVALID_NWK_ADDR;
        }
    }
}

/****************************************************************************
 *
 * NAME: vDisplayAPSTable
 *
 * DESCRIPTION:
 * Display the APS key table
 *
 ****************************************************************************/
PUBLIC void vDisplayAPSTable(void)
{
#if 0
    // TODO table restructured
    uint8 i;
    int8 j;

    ZPS_tsAplAib * psAplAib;

    psAplAib = ZPS_psAplAibGetAib();

    for ( i = 0 ; i < (psAplAib->psAplDeviceKeyPairTable->u16SizeOfKeyDescriptorTable + 1) ; i++ )
    {
        DBG_vPrintf(TRACE_ZBP_UTILS, "MAC: %016llx \n", ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),psAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].u16ExtAddrLkup));
        DBG_vPrintf(TRACE_ZBP_UTILS, "KEY: " );

        for(j=0; j<16;j++)
        {
            DBG_vPrintf(TRACE_ZBP_UTILS, "%02x, ", psAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].au8LinkKey[j]);
        }
        DBG_vPrintf(TRACE_ZBP_UTILS, "\r\n");
        DBG_vPrintf(TRACE_ZBP_UTILS, "Incoming FC: %d\n", psAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].u32IncomingFrameCounter);
        DBG_vPrintf(TRACE_ZBP_UTILS, "Outgoing FC: %d\n", psAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].u32OutgoingFrameCounter);
    }
#endif
}


/****************************************************************************
 *
 * NAME: vClearDiscNT
 *
 * DESCRIPTION:
 * Resets the discovery NT
 *
 ****************************************************************************/
PUBLIC void vClearDiscNT(void)
{
    ZPS_tsNwkNib * thisNib;

    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    memset((uint8*)thisNib->sTbl.psNtDisc, 0, sizeof(ZPS_tsNwkDiscNtEntry) * thisNib->sTblSize.u8NtDisc);

}


/****************************************************************************
 *
 * NAME: vRemoveCoordParents
 *
 * DESCRIPTION:
 * Removes Coordinator parents from the scan results to test joining through
 * a router, and key establishment.
 *
 ****************************************************************************/
PUBLIC void vRemoveCoordParents(void)
{
    ZPS_tsNwkNib * thisNib;
    uint8 i;

    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    for( i = 0; i < thisNib->sTblSize.u8NtDisc; i++)
    {
        if(thisNib->sTbl.psNtDisc[i].u16NwkAddr == 0x0000 )
        {
            thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1JoinPermit = 0;
        }
    }

}


/****************************************************************************
 *
 * NAME: vDisplayDiscNT
 *
 * DESCRIPTION:
 * Display the discovered nodes after a scan
 *
 ****************************************************************************/
PUBLIC void vDisplayDiscNT(void)
{
#if(TRACE_ZBP_UTILS == TRUE)
    ZPS_tsNwkNib * thisNib;
    uint8 i;

    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    for( i = 0; i < thisNib->sTblSize.u8NtDisc; i++)
    {
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nIndex: %d", i );

        DBG_vPrintf(TRACE_ZBP_UTILS, " EPID: %016llx", thisNib->sTbl.psNtDisc[i].u64ExtPanId);

        DBG_vPrintf(TRACE_ZBP_UTILS, " PAN: %04x", thisNib->sTbl.psNtDisc[i].u16PanId);

        DBG_vPrintf(TRACE_ZBP_UTILS, " SAddr: %04x", thisNib->sTbl.psNtDisc[i].u16NwkAddr);

        DBG_vPrintf(TRACE_ZBP_UTILS, " LQI %d\n", thisNib->sTbl.psNtDisc[i].u8LinkQuality);

        DBG_vPrintf(TRACE_ZBP_UTILS, " CH: %d", thisNib->sTbl.psNtDisc[i].u8LogicalChan);

        DBG_vPrintf(TRACE_ZBP_UTILS, " PJ: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1JoinPermit);

        DBG_vPrintf(TRACE_ZBP_UTILS, " Coord: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1PanCoord);

        DBG_vPrintf(TRACE_ZBP_UTILS, " RT Cap: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1ZrCapacity);

        DBG_vPrintf(TRACE_ZBP_UTILS, " ED Cap: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1ZedCapacity);

        DBG_vPrintf(TRACE_ZBP_UTILS, " Depth: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u4Depth);

        DBG_vPrintf(TRACE_ZBP_UTILS, " StPro: %d", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u4StackProfile);

        DBG_vPrintf(TRACE_ZBP_UTILS, " PP: %d\r\n", thisNib->sTbl.psNtDisc[i].uAncAttrs.bfBitfields.u1PotentialParent);
    }
#endif
}

/****************************************************************************
 *
 * NAME: bInRoutingTable
 *
 * DESCRIPTION:
 * Checks if a node is in the routing table, passed in as a parameter
 *
 ****************************************************************************/
PUBLIC bool bInRoutingTable( uint16 u16ShortAddress )
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i = 0;

    for( i=0;i<thisNib->sTblSize.u16Rt;i++)
    {
        if(thisNib->sTbl.psRt[i].u16NwkDstAddr == u16ShortAddress )
        {
            DBG_vPrintf(TRACE_ZBP_UTILS,"\nGot Short Address: %02x", thisNib->sTbl.psRt[i].u16NwkDstAddr);
            return TRUE;
        }
    }

    return 0;
}



/****************************************************************************
 *
 * NAME: vDisplayRoutingTable
 *
 * DESCRIPTION:
 * Displays the contents of the routing table
 *
 ****************************************************************************/
PUBLIC void vDisplayRoutingTable(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i = 0;

    DBG_vPrintf(TRACE_ZBP_UTILS,"\r\nRouting Table Size %d\n", thisNib->sTblSize.u16Rt);

    for( i=0;i<thisNib->sTblSize.u16Rt;i++)
    {
        DBG_vPrintf(TRACE_ZBP_UTILS,"Status: %d, Short Address: %02x, Next Hop: %02x\n",
                thisNib->sTbl.psRt[i].uAncAttrs.bfBitfields.u3Status,
                thisNib->sTbl.psRt[i].u16NwkDstAddr,
                thisNib->sTbl.psRt[i].u16NwkNxtHopAddr
                );
    }

}

/****************************************************************************
 *
 * NAME: vClearRoutingTable
 *
 * DESCRIPTION:
 * Clears the contents of the routing table
 *
 ****************************************************************************/
PUBLIC void vClearRoutingTable(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i = 0;

    DBG_vPrintf(TRACE_ZBP_UTILS,"\nRouting Table Size %d", thisNib->sTblSize.u16Rt);

    for( i=0;i<thisNib->sTblSize.u16Rt;i++)
    {
        thisNib->sTbl.psRt[i].uAncAttrs.bfBitfields.u3Status = 3;
        thisNib->sTbl.psRt[i].u16NwkDstAddr = 0;
        thisNib->sTbl.psRt[i].u16NwkNxtHopAddr = 0;
    }
}



/****************************************************************************
 *
 * NAME: vDisplayRouteRecordTable
 *
 * DESCRIPTION:
 * Displays the contents of the route record table
 *
 ****************************************************************************/
PUBLIC void vDisplayRouteRecordTable(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i, j = 0;

    for( i=0;i<thisNib->sTblSize.u16Rct;i++)
    {
        DBG_vPrintf(TRACE_ZBP_UTILS,"\nRelay Count: %i NwkdstAddr: 0x%04x", thisNib->sTbl.psRct[i].u8RelayCount, thisNib->sTbl.psRct[i].u16NwkDstAddr);
        for ( j = 0 ; j < ZPS_NWK_NIB_MAX_DEPTH_DEF * 2 ; j++)
        {
            DBG_vPrintf(TRACE_ZBP_UTILS,"\nPath[%i]: %i", j, thisNib->sTbl.psRct[i].au16Path[j]);
        }
    }
}




/****************************************************************************
 *
 * NAME: vClearRouteRecordTable
 *
 * DESCRIPTION:
 * Clears the contents of the route record table
 *
 ****************************************************************************/
PUBLIC void vClearRouteRecordTable(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i, j = 0;

    DBG_vPrintf(TRACE_ZBP_UTILS,"\nClearing Record Table");

    for( i=0;i<thisNib->sTblSize.u16Rct;i++)
    {
        thisNib->sTbl.psRct[i].u8RelayCount = 0;
        thisNib->sTbl.psRct[i].u16NwkDstAddr = 0xFFFE;
        for ( j = 0 ; j < ZPS_NWK_NIB_MAX_DEPTH_DEF * 2 ; j++)
        {
            thisNib->sTbl.psRct[i].au16Path[j] = 0;
        }
    }
}



/****************************************************************************
 *
 * NAME: vDisplayNWKKey
 *
 * DESCRIPTION:
 * Displays the current NWK key
 *
 ****************************************************************************/
PUBLIC void vDisplayNWKKey(void)
{
#if (TRACE_ZBP_UTILS == TRUE)
    uint8 i = 0;
    uint8 j = 0;

    ZPS_tsNwkNib * thisNib;

    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    for(j=0;j<thisNib->sTblSize.u8SecMatSet;j++)
    {

        DBG_vPrintf(TRACE_ZBP_UTILS, "APP: Key");

        for(i = 0;i<16;i++)
        {
            DBG_vPrintf(TRACE_ZBP_UTILS, "%x", thisNib->sTbl.psSecMatSet[j].au8Key[i]);
        }

        DBG_vPrintf(TRACE_ZBP_UTILS, "\r\n");
    }
#endif
}


/****************************************************************************
 *
 * NAME: vDisplayNWKTransmitTable
 *
 * DESCRIPTION:
 * Displays the tables required for a tx packet
 *
 ****************************************************************************/
PUBLIC void vDisplayNWKTransmitTable(void)
{
    vDisplayNT();
    vDisplayRoutingTable();
    vDisplayAddressMapTable();
    vDisplayAPSTable();
}


/****************************************************************************
*
* NAME: vDisplayBindingTable
*
* DESCRIPTION:
* Display the binding table to the UART
*
****************************************************************************/
PUBLIC void vDisplayBindingTable( void )
{
#if (TRACE_ZBP_UTILS == TRUE)
    uint32   j = 0;

    ZPS_tsAplAib * tsAplAib  = ZPS_psAplAibGetAib();

    DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nBind Size %d",  tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable );

    for( j = 0 ; j < tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable ; j++ )
    {
        DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nMAC Dest %016llx", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].uDstAddress.u64Addr);
        DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nEP Dest %d", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u8DestinationEndPoint);
        DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nCluster Dest %d", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16ClusterId);
    }
#endif
}

/****************************************************************************
 *
 * NAME: vDisplayStackEvent
 *
 * DESCRIPTION:
 * Display function only, display the current stack event before each state
 * consumes
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vDisplayStackEvent( ZPS_tsAfEvent sStackEvent )
{

    DBG_vPrintf(TRACE_ZBP_UTILS, "\nAPP Stack event:%d",sStackEvent.eType);

    switch (sStackEvent.eType)
    {

    case ZPS_EVENT_APS_DATA_INDICATION:
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nData Ind: Profile :%x Cluster :%x EP:%x",
            sStackEvent.uEvent.sApsDataIndEvent.u16ProfileId,
            sStackEvent.uEvent.sApsDataIndEvent.u16ClusterId,
            sStackEvent.uEvent.sApsDataIndEvent.u8DstEndpoint);
        break;

    case ZPS_EVENT_NWK_STATUS_INDICATION:
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nNwkStat: Addr:%x Status:%x",
            sStackEvent.uEvent.sNwkStatusIndicationEvent.u16NwkAddr,
            sStackEvent.uEvent.sNwkStatusIndicationEvent.u8Status);
        break;

    case ZPS_EVENT_NWK_STARTED:
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nZPS_EVENT_NWK_STARTED\n");
        ZPS_eAplZdoPermitJoining(0xff);
        break;

    case ZPS_EVENT_NWK_FAILED_TO_START:
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nZPS_EVENT_NWK_FAILED_TO_START\n");
        break;

    case ZPS_EVENT_NWK_NEW_NODE_HAS_JOINED:
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nZPS_EVENT_NWK_NEW_NODE_HAS_JOINED\n");
        break;

    case ZPS_EVENT_NWK_FAILED_TO_JOIN:
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nZPS_EVENT_NWK_FAILED_TO_JOIN - %x \n",sStackEvent.uEvent.sNwkJoinFailedEvent.u8Status);
        break;

    case ZPS_EVENT_ERROR:
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nZPS_EVENT_ERROR\n");
        ZPS_tsAfErrorEvent *psErrEvt = &sStackEvent.uEvent.sAfErrorEvent;
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nStack Err: %d", psErrEvt->eError);
        break;

    default:
        DBG_vPrintf(TRACE_ZBP_UTILS, "\nUnhandled Stack Event\n");
        break;

    }

}

/****************************************************************************
 *
 * NAME: vStartStopTimer
 *
 * DESCRIPTION:
 * Stops and starts a timer with given number of ticks and changes the state.
 *
 * PARAMETERS:
 * uint8 u8Timer           : The handle to the timer
 * uint32 u32Ticks         : Ticks
 * uint8 eNextState        : State during this time
 * uint8 * pu8State        : pointer to the state variable.
 *
 * RETURNS:
 *
 *
 ****************************************************************************/
PUBLIC void vStartStopTimer(uint8 u8Timer, uint32 u32Ticks,uint8* pu8State, uint8 eNextState)
{
    ZTIMER_eStop(u8Timer);
    ZTIMER_eStart(u8Timer, u32Ticks);
    *pu8State = eNextState;
}

/****************************************************************************
 *
 * NAME: vEnablePermitJoin
 *
 * DESCRIPTION:
 * should be called to enable permit joining and to broad cast mgmt
 * permit joining request
 *
 * PARAMETERS:  Name                            Usage
 *  None
 *
 * RETURNS:
 * None
 *
 ****************************************************************************/
PUBLIC void vEnablePermitJoin(uint8 u8Time)
{
    uint8 u8TransactionSequenceNumber;
    ZPS_teStatus eStatus;
    ZPS_tuAddress uDestinationAddress;

    ZPS_tsAplZdpMgmtPermitJoiningReq sZdpMgmtPermitJoiningReq;
    PDUM_thAPduInstance hAPduInst;

    /* TBD permit joining timer for more than 254 sec*/
    u8Status = ZPS_eAplZdoPermitJoining(u8Time);
    DBG_vPrintf(TRACE_ZBP_UTILS, "ZPS_eAplZdoPermitJoining status = %d  \n",u8Status);
    /* Broadcast mgmt permit joining */
    hAPduInst = PDUM_hAPduAllocateAPduInstance(apduZDP);
    if (hAPduInst == PDUM_INVALID_HANDLE)
    {
        DBG_vPrintf(TRACE_ZBP_UTILS, "ZPS_eAplZdpMgmtPermitJoiningRequest - PDUM_INVALID_HANDLE\n");
    }
    else
    {
        sZdpMgmtPermitJoiningReq.u8PermitDuration = u8Time;
        sZdpMgmtPermitJoiningReq.bTcSignificance = FALSE;

        uDestinationAddress.u16Addr = ZPS_E_BROADCAST_ZC_ZR;

        eStatus = ZPS_eAplZdpMgmtPermitJoiningRequest(hAPduInst,
                uDestinationAddress,
                FALSE,
                &u8TransactionSequenceNumber,
                &sZdpMgmtPermitJoiningReq);
        DBG_vPrintf(TRACE_ZBP_UTILS, "ZPS_eAplZdpMgmtPermitJoiningRequest  status = %d  \n",u8Status);
        if (eStatus)
        {
            PDUM_eAPduFreeAPduInstance(hAPduInst);
        }
    }
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
