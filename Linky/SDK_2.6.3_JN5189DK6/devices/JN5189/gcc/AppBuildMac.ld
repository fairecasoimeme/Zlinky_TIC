/****************************************************************************
 *
 * This software is owned by NXP B.V. and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on NXP products [NXP Microcontrollers such as JN5148, JN5142, JN5139]. 
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
 * Copyright NXP B.V. 2012. All rights reserved
 *
 ***************************************************************************/

INCLUDE AppBuildNone.ld

EXTERN(TSV_eOpen)
EXTERN(TSV_eStart)
EXTERN(TSV_eStop)
EXTERN(pvAppApiDefGetMacHandle)
EXTERN(u32AppApiDefInit)
EXTERN(vAppApiDefMlmeRequest)
EXTERN(vAppApiDefMcpsRequest)
EXTERN(eAppApiDefPlmeSet)
EXTERN(eAppApiDefPlmeGet)
EXTERN(pvAppApiDefGetMacAddrLocation)
EXTERN(vAppApiDefSaveMacSettings)
EXTERN(vAppApiDefRestoreMacSettings)

/* Following map patchable functions to their API calls, for TSV and MAC */
TSV_eOpenVector             = TSV_eOpen;
TSV_eStartVector            = TSV_eStart;
TSV_eStopVector             = TSV_eStop;
pvAppApiGetMacHandle        = pvAppApiDefGetMacHandle;
u32AppApiInit               = u32AppApiDefInit;
vAppApiMlmeRequest          = vAppApiDefMlmeRequest;
vAppApiMcpsRequest          = vAppApiDefMcpsRequest;
eAppApiPlmeSet              = eAppApiDefPlmeSet;
eAppApiPlmeGet              = eAppApiDefPlmeGet;
pvAppApiGetMacAddrLocation  = pvAppApiDefGetMacAddrLocation;
vAppApiSaveMacSettings      = vAppApiDefSaveMacSettings;
vAppApiRestoreMacSettings   = vAppApiDefRestoreMacSettings;
