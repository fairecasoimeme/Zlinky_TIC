############################################################################
#
# MODULE              JN-AN-1243
#
# DESCRIPTION         All Build Configurations Makefile
#
############################################################################
#
# This software is owned by NXP B.V. and/or its supplier and is protected
# under applicable copyright laws. All rights are reserved. We grant You,
# and any third parties, a license to use this software solely and
# exclusively on NXP products [NXP Microcontrollers such as JN5168, JN5164]. 
# You, and any third parties must reproduce the copyright and warranty notice
# and any other legend of ownership on each copy or partial copy of the 
# software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Copyright NXP B.V. 2016-2018. All rights reserved
#
############################################################################
# Build each target in turn
all:
	rm -f ../../Doc/size.txt
	$(MAKE) -C ../../ZLinky/Build/mcux clean -r
	$(MAKE) -C ../../ZLinky/Build/mcux all -r
	$(MAKE) -C ../../ZLinky/Build/mcux APP_NTAG_NWK=1 APP_NTAG_OTA=1 OTA=1 OTA_ENCRYPTED=0 clean -r
	$(MAKE) -C ../../ZLinky/Build/mcux APP_NTAG_NWK=1 APP_NTAG_OTA=1 OTA=1 OTA_ENCRYPTED=0 all -r
	$(MAKE) -C ../../ZLinky/Build/mcux APP_NTAG_NWK=1 DUAL_BOOT=1 clean -r
	$(MAKE) -C ../../ZLinky/Build/mcux APP_NTAG_NWK=1 DUAL_BOOT=1 all -r
