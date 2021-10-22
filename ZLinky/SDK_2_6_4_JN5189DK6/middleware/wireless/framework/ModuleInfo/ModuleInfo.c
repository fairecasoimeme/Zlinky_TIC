/*! *********************************************************************************
* Copyright 2016-2017 NXP
* All rights reserved.
*
* \file
*
* Connectivity Framework Module Information
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************** */    

#include "ModuleInfo.h"

/*! *********************************************************************************
*************************************************************************************
* Private macros
*************************************************************************************
********************************************************************************** */

/* FWK build version  */
/*! \cond DOXY_SKIP_TAG */
#define gFwkVerMajor_c   5
#define gFwkVerMinor_c   7
#define gFwkVerPatch_c   3
#define gFwkBuildNo_c    0
#define gFwkModuleId_c   0xA3
/*! \endcond */

#define gFwkVerString_c "CONNECTIVITY FRAMEWORK v" \
                        QUH(gFwkVerMajor_c) "." \
                        QUH(gFwkVerMinor_c) "." \
                        QUH(gFwkVerPatch_c)

/*! *********************************************************************************
*************************************************************************************
* Private memory declarations
*************************************************************************************
********************************************************************************** */
/*!   
 * \brief The version string of the Connectivity Framework
 */
static ro_string_t const mFwkVersionString = gFwkVerString_c;


/*!   
 * \brief Registers the Connectivity Framework module info
 */
RegisterModuleInfo(FWK, /* DO NOT MODIFY */
                   mFwkVersionString, /* DO NOT MODIFY */
                   gFwkModuleId_c, /* DO NOT MODIFY */
                   gFwkVerMajor_c, /* DO NOT MODIFY */
                   gFwkVerMinor_c, /* DO NOT MODIFY */
                   gFwkVerPatch_c, /* DO NOT MODIFY */
                   gFwkBuildNo_c); /* DO NOT MODIFY */
