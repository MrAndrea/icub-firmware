/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"

#include "EOtheErrorManager.h"

#include "EOVmutex_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOSmutex.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOSmutex_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOSmutex";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOSmutex* eos_mutex_New(eOres_fp_voidp_uint32_t hal_sys_criticalsection_take, eOres_fp_voidp_t hal_sys_criticalsection_release) 
{
    EOSmutex *retptr = NULL;   
    
    
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != hal_sys_criticalsection_take), s_eobj_ownname, "_take is NULL", NULL);
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != hal_sys_criticalsection_release), s_eobj_ownname, "_release is NULL", NULL);     

    // i get the memory for the mutex object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOSmutex), 1);
    
    // i get the base mutex
    retptr->mutex = eov_mutex_hid_New();

    // init its vtable
    eov_mutex_hid_SetVTABLE(retptr->mutex, hal_sys_criticalsection_take, hal_sys_criticalsection_release); 
    
    return(retptr);    
}


extern eOresult_t eos_mutex_Take(EOSmutex *m, eOreltime_t tout)
{
    if(NULL == m)
    {
        return(eores_NOK_nullpointer);
    }
    
    return(eov_mutex_Take(m, tout));
}


extern eOresult_t eos_mutex_Release(EOSmutex *m)
{
    if(NULL == m)
    {
        return(eores_NOK_nullpointer);
    }
    
    return(eov_mutex_Release(m));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



