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
 * MODULE:             JN-AN-1135 (LC-IPD)
 *
 * COMPONENT:          appZpsExtendedDebug.c
 *
 * DESCRIPTION:        ZBP functions to aid development
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

/* Stack Includes */
#include <jendefs.h>
#include <string.h>
#include "zps_apl_af.h"
#include "zps_apl_aib.h"
#include "zps_nwk_sap.h"
#include "zps_nwk_nib.h"
#include "zps_nwk_pub.h"
#include "dbg.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#ifndef TRACE_TIMERS
#define TRACE_TIMERS    TRUE
#endif

#ifndef TRACE_ZBP_UTILS
#define TRACE_ZBP_UTILS    TRUE
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
/***        Tasks                               ***/
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
 * PARAMETERS: None
 *
 * RETURNS:
 * None
 *
 ****************************************************************************/
PUBLIC void vDisplayTableSizes(void)
{
    ZPS_tsNwkNib * thisNib;

    void * thisNet = ZPS_pvAplZdoGetNwkHandle();
    thisNib = ZPS_psNwkNibGetHandle(thisNet);

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
 * Displays the address map table
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * None
 *
 ****************************************************************************/
PUBLIC void vDisplayAddressMapTable(void)
{
    ZPS_tsNwkNib * thisNib;
    thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());

    uint8 i = 0;

    DBG_vPrintf(TRACE_ZBP_UTILS,"\r\nAddress Map Size: %d", thisNib->sTblSize.u16AddrMap);

    for( i=0;i<thisNib->sTblSize.u16AddrMap;i++)
    {
        DBG_vPrintf(TRACE_ZBP_UTILS,"\nShort Addr: %04x, Ext Addr: %016llx,", thisNib->sTbl.pu16AddrMapNwk[i], ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),thisNib->sTbl.pu16AddrLookup[i]));
    }
	
	DBG_vPrintf(TRACE_ZBP_UTILS,"\r\n MAC table Size: %d", thisNib->sTblSize.u16MacAddTableSize);
	for( i=0;i<thisNib->sTblSize.u16MacAddTableSize;i++)
    {
        DBG_vPrintf(TRACE_ZBP_UTILS,"\nLocation: %04x, Ext Addr: %016llx,", i, thisNib->sTbl.pu64AddrExtAddrMap[i]);
    }
}

/****************************************************************************
 *
 * NAME: vDisplayNT
 *
 * DESCRIPTION:
 * Displays the neighbor table
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * None
 *
 ****************************************************************************/
PUBLIC void vDisplayNT( void )
{
    ZPS_tsNwkNib * thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());
    uint8 i;

    DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nNT Size: %d\n", thisNib->sTblSize.u16NtActv);

    for( i = 0 ; i < thisNib->sTblSize.u16NtActv ; i++ )
    {
        DBG_vPrintf(TRACE_ZBP_UTILS, "SAddr: 0x%04x - ExtAddr: 0x%016llx - LQI: %i - Failed TX's: %i - Auth: %i - %i %i %i %i %i %i - Active: %i - %i %i %i\n",
                    thisNib->sTbl.psNtActv[i].u16NwkAddr,
                    ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),thisNib->sTbl.psNtActv[i].u16Lookup),
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
                    thisNib->sTbl.psNtActv[i].u8Age,
                    thisNib->sTbl.psNtActv[i].uAncAttrs.bfBitfields.u3OutgoingCost
                    );
    }
}

/****************************************************************************
 *
 * NAME: vClearNTEntry
 *
 * DESCRIPTION:
 * clears the neighbor table entries
 *
 * PARAMETERS: None
 *
 * RETURNS:
 * None
 *
 ****************************************************************************/
PUBLIC void vClearNTEntry( uint64 u64AddressToRemove )
{

    ZPS_tsNwkNib * thisNib = ZPS_psNwkNibGetHandle(ZPS_pvAplZdoGetNwkHandle());
    uint8 i;

    DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nNT Size: %d\n", thisNib->sTblSize.u16NtActv);

    for( i = 0 ; i < thisNib->sTblSize.u16NtActv ; i++ )
    {

        if(ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),thisNib->sTbl.psNtActv[i].u16Lookup) == u64AddressToRemove)
        {
             memset( &thisNib->sTbl.psNtActv[i], 0, sizeof(ZPS_tsNwkActvNtEntry) );
             thisNib->sTbl.psNtActv[i].u16NwkAddr = ZPS_NWK_INVALID_NWK_ADDR;
             thisNib->sTbl.psNtActv[i].u16Lookup = 0xFFFF;
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
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vDisplayAPSTable(void)
{
    uint8 i;
    uint8 j;

    ZPS_tsAplAib * tsAplAib;

    tsAplAib = ZPS_psAplAibGetAib();

    for ( i = 0 ; i < (tsAplAib->psAplDeviceKeyPairTable->u16SizeOfKeyDescriptorTable + 1) ; i++ )
    {
        DBG_vPrintf(TRACE_ZBP_UTILS, "MAC: %016llx \n", ZPS_u64NwkNibGetMappedIeeeAddr(ZPS_pvAplZdoGetNwkHandle(),tsAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].u16ExtAddrLkup));
        DBG_vPrintf(TRACE_ZBP_UTILS, "KEY: " );

        for(j=0; j<16;j++)
        {
            DBG_vPrintf(TRACE_ZBP_UTILS, "%02x, ", tsAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].au8LinkKey[j]);
        }
        DBG_vPrintf(TRACE_ZBP_UTILS, "\r\n");
        DBG_vPrintf(TRACE_ZBP_UTILS, "Incoming FC: %d\n", tsAplAib->pu32IncomingFrameCounter[i]);
        DBG_vPrintf(TRACE_ZBP_UTILS, "Outgoing FC: %d\n", tsAplAib->psAplDeviceKeyPairTable->psAplApsKeyDescriptorEntry[i].u32OutgoingFrameCounter);
    }
}


/****************************************************************************
 *
 * NAME: vClearDiscNT
 *
 * DESCRIPTION:
 * Resets the discovery NT
 *
 * RETURNS:
 * void
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
 * RETURNS:
 * void
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
 * Display the discovery network table
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PUBLIC void vDisplayDiscNT(void)
{
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

}


/****************************************************************************
 *
 * NAME: bInRoutingTable
 *
 * DESCRIPTION:
 *  find an entry in the routing table with the requested short address
 *
 * RETURNS:
 * void
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
 *  Display the routing table
 *
 * RETURNS:
 * void
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
                thisNib->sTbl.psRt[i].u16NwkNxtHopAddr);
    }

}

/****************************************************************************
 *
 * NAME: vClearRoutingTable
 *
 * DESCRIPTION:
 *  Clear routing table
 *
 * RETURNS:
 * void
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
        thisNib->sTbl.psRt[i].uAncAttrs.bfBitfields.u3Status = ZPS_NWK_RT_INACTIVE;
        thisNib->sTbl.psRt[i].u16NwkDstAddr = 0xfffe;
        thisNib->sTbl.psRt[i].u16NwkNxtHopAddr = 0;
     }
}

/****************************************************************************
 *
 * NAME: vDisplayRouteRecordTable
 *
 * DESCRIPTION:
 *  Display route record table
 *
 * RETURNS:
 * void
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
        for ( j = 0 ; j < thisNib->u8MaxSourceRoute  ; j++)
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
 *  Clear route record table
 *
 * RETURNS:
 * void
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
        for ( j = 0 ; j < thisNib->u8MaxSourceRoute ; j++)
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
 *  Display the network key
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
void vDisplayNWKKey(void)
{
    uint8 i = 0;
    uint8 j = 0;

    ZPS_tsNwkNib * thisNib;

    void * thisNet = ZPS_pvAplZdoGetNwkHandle();
    thisNib = ZPS_psNwkNibGetHandle(thisNet);

    for(j=0;j<thisNib->sTblSize.u8SecMatSet;j++)
    {
        DBG_vPrintf(TRACE_ZBP_UTILS, "APP: Nwk Key ");
        for(i = 0;i<16;i++)
        {
             DBG_vPrintf(TRACE_ZBP_UTILS, "%02x", thisNib->sTbl.psSecMatSet[j].au8Key[i]);
        } 
        DBG_vPrintf(TRACE_ZBP_UTILS, "\r\n");
     }
}

/****************************************************************************
 *
 * NAME: vDisplayNWKTransmitTable
 *
 * DESCRIPTION:
 *  prints out various network tables
 *
 * RETURNS:
 * void
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
* PARAMETERS: None
*
*
* RETURNS:
* None
*
****************************************************************************/
PUBLIC void vDisplayBindingTable( void )
{
    uint32   j = 0;
    uint64 u64Addr;

    ZPS_tsAplAib * tsAplAib  = ZPS_psAplAibGetAib();

    DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nBind Size %d",  tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable );

    for( j = 0 ; j < tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].u32SizeOfBindingTable ; j++ )
    {
        if (tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u8DstAddrMode == ZPS_E_ADDR_MODE_GROUP)
        {
            // Group
            DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nGroup Addr 0x%x, ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16AddrOrLkUp);
            DBG_vPrintf(TRACE_ZBP_UTILS, "Dest Ep %d, ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u8DestinationEndPoint);
            DBG_vPrintf(TRACE_ZBP_UTILS, "Cluster Id 0x%x ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16ClusterId);

        }
        else
        {
            
            u64Addr = ZPS_u64NwkNibGetMappedIeeeAddr( ZPS_pvAplZdoGetNwkHandle(), tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16AddrOrLkUp);
            DBG_vPrintf(TRACE_ZBP_UTILS, "\r\nMAC addr 0x%x %x, ", (uint32)(u64Addr>>32), (uint32)(u64Addr&0xffffffff));
            DBG_vPrintf(TRACE_ZBP_UTILS, " Dest EP %d, ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u8DestinationEndPoint);
            DBG_vPrintf(TRACE_ZBP_UTILS, "Cluster Id 0x%x ", tsAplAib->psAplApsmeAibBindingTable->psAplApsmeBindingTable[0].pvAplApsmeBindingTableEntryForSpSrcAddr[j].u16ClusterId);
        }
    }
    DBG_vPrintf(TRACE_ZBP_UTILS, "\r\n");
}


/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

