/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

#include "EOappMeasuresConverter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOappMeasuresConverter_hid.h"


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
extern eOmeas_position_t eo_appMeasConv_jntPosition_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_position_t e_pos)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif
    
    return((e_pos / eo_appMeasConv_hid_GetEncConv_factor(p, jId)) - eo_appMeasConv_hid_GetEncConv_offset(p, jId));
}

extern eOicubCanProto_position_t eo_appMeasConv_jntPosition_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_position_t i_pos)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(eores_NOK_nodata);
    }
#endif
    return((i_pos + eo_appMeasConv_hid_GetEncConv_offset(p, jId)) * eo_appMeasConv_hid_GetEncConv_factor(p, jId)); 
}

extern eOmeas_velocity_t eo_appMeasConv_jntVelocity_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_velocity_t e_vel)
{
//    uint32_t vel_factor;
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(eores_NOK_nodata);
    }
#endif
// see CanBusMotionControl::getEncoderSpeedsRaw(double *v)
//     vel_factor = (1 << eo_appMeasConv_hid_GetVelEstimShift(p, jId));
//     
//     //*1000 : msec-->sec
//     return(((e_vel*1000)/vel_factor)/eo_appMeasConv_hid_GetEncConv_factor(p, jId));
    
    //*1000 : msec-->sec
    return(((e_vel*1000)>>eo_appMeasConv_hid_GetVelEstimShift(p, jId))/eo_appMeasConv_hid_GetEncConv_factor(p, jId));
}


extern eOicubCanProto_velocity_t eo_appMeasConv_jntVelocity_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
{
     uint32_t vel_factor;
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(eores_NOK_nodata);
    }
#endif
    
    
    return(((i_vel * eo_appMeasConv_hid_GetEncConv_factor(p, jId))/1000)<<eo_appMeasConv_hid_GetVelShift(p, jId));
}


extern eOicubCanProto_velocity_t eo_appMeasConv_jntVelocity_I2E_forSetVelRefMC4(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_velocity_t i_vel)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(0);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(0);
    }
#endif

    return((i_vel * eo_appMeasConv_hid_GetEncConv_factor(p, jId))/10);
}

extern eOmeas_acceleration_t eo_appMeasConv_jntAcceleration_E2I(EOappMeasConv *p, eOmc_jointId_t jId, eOicubCanProto_acceleration_t e_acc)
{
#ifdef _APPMEASCONV_SAFE_    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(eores_NOK_nodata);
    }
#endif
    
    return(((e_acc *1000000)>>(eo_appMeasConv_hid_GetVelEstimShift(p, jId)+eo_appMeasConv_hid_GetAccEstimShift(p, jId)))/eo_appMeasConv_hid_GetEncConv_factor(p, jId));
 
}

extern eOicubCanProto_acceleration_t eo_appMeasConv_jntAcceleration_I2E(EOappMeasConv *p, eOmc_jointId_t jId, eOmeas_acceleration_t i_acc)
{
#ifdef _APPMEASCONV_SAFE_
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= p->totalnumofjoint)
    {
        return(eores_NOK_nodata);
    }
#endif
        
    return((i_acc * eo_appMeasConv_hid_GetEncConv_factor(p, jId))<<eo_appMeasConv_hid_GetVelShift(p, jId));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




