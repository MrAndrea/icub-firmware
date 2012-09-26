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

/* @file       eOcfg_nvsEP_mc_usrcbk_ebx.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint mc
    @author     valentina.gaggero@iit.it
    @date       05/04/2012
**/





// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"
#include "EOnv_hid.h"

#include "EOMotionControl.h"
#include "eOcfg_nvsEP_mc.h"

#include "eOcfg_nvsEP_mc_any_con_jxxdefault.h"
#include "eOcfg_nvsEP_mc_any_con_mxxdefault.h"


//application
#include "EOtheEMSapplBody.h"
#include "EOappTheDataBase.h"
#include "EOicubCanProto_specifications.h"
#include "EOappMeasuresConverter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "eOcfg_nvsEP_mngmnt_usr_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
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
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
/* TAG_ALE*/

//joint-init
extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eOmc_joint_config_t             *cfg = (eOmc_joint_config_t*)nv->loc;
    memcpy(cfg, &eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jconfig, sizeof(eOmc_joint_config_t));
}

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv)
{
    eOmc_joint_status_t             *cfg = (eOmc_joint_status_t*)nv->loc;
    memcpy(cfg, &eo_cfg_nvsEP_mc_any_con_jxxdefault_defaultvalue.jstatus, sizeof(eOmc_joint_status_t));
}


//joint-update
extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    float                   rescaler_pos;
    float                   rescaler_trq;
    eOmc_joint_config_t     *cfg = (eOmc_joint_config_t*)nv->loc;

    //currently no joint config param must be sent to 2foc board. (for us called 1foc :) )
    //(currently no pid velocity is sent to 2foc)
    //use joint config to configure ems motor controller.

    // 1) set pid position 
    rescaler_pos = 1.0f/(float)(1<<cfg->pidposition.scale);
    eo_emsController_SetOffset(jxx, cfg->pidposition.offset);
    eo_emsController_SetPosPidLimits(jxx, cfg->pidposition.limitonoutput, cfg->pidposition.limitonintegral);
    eo_emsController_SetPosPid(jxx, cfg->pidposition.kp*rescaler_pos, cfg->pidposition.kd*rescaler_pos, cfg->pidposition.ki*rescaler_pos);

    // 2) set torque pid    
    rescaler_trq = 1.0f/(float)(1<<cfg->pidtorque.scale);
    eo_emsController_SetOffset(jxx, cfg->pidtorque.offset);
    eo_emsController_SetTrqPidLimits(jxx, cfg->pidtorque.limitonoutput, cfg->pidtorque.limitonintegral);
    eo_emsController_SetTrqPid(jxx, cfg->pidtorque.kp*rescaler_trq, cfg->pidtorque.kd*rescaler_trq, cfg->pidtorque.ki*rescaler_trq);

    // 3) set velocity pid:    to be implemented
   
   // 4) set min position    
    eo_emsController_SetPosMin(jxx, cfg->minpositionofjoint);
        
    // 5) set max position
    eo_emsController_SetPosMax(jxx, cfg->maxpositionofjoint);

    // 6) set vel timeout        
    eo_emsController_SetVelTimeout(jxx, cfg->velocitysetpointtimeout);
        
    // 7) set impedance : to be implemented  
}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOmc_PID_t      *pid_ptr = (eOmc_PID_t*)nv->loc;
    float           rescaler = 1.0f/(float)(1<<pid_ptr->scale);

    eo_emsController_SetOffset(jxx, pid_ptr->offset);
    eo_emsController_SetPosPidLimits(jxx, pid_ptr->limitonoutput, pid_ptr->limitonintegral);
    eo_emsController_SetPosPid(jxx, pid_ptr->kp*rescaler, pid_ptr->kd*rescaler, pid_ptr->ki*rescaler);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOmc_PID_t      *pid_ptr = (eOmc_PID_t*)nv->loc;
    float           rescaler = 1.0f/(float)(1<<pid_ptr->scale);

    eo_emsController_SetOffset(jxx, pid_ptr->offset);
    eo_emsController_SetTrqPidLimits(jxx, pid_ptr->limitonoutput, pid_ptr->limitonintegral);
    eo_emsController_SetTrqPid(jxx, pid_ptr->kp*rescaler, pid_ptr->kd*rescaler, pid_ptr->ki*rescaler);

}




extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__impedance(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    // to be implemented 
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOmeas_position_t       *pos_ptr = (eOmeas_position_t*)nv->loc;

    eo_emsController_SetPosMin(jxx, *pos_ptr);
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOmeas_position_t   *pos_ptr = (eOmeas_position_t*)nv->loc;

    eo_emsController_SetPosMax(jxx, *pos_ptr);
}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOmeas_time_t                           *time_ptr = (eOmeas_time_t*)nv->loc;

    eo_emsController_SetVelTimeout(jxx, *time_ptr);
}




extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__motionmonitormode(eOcfg_nvsEP_mc_jointNumber_t jxx,  const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    /*NOTE: this function is equal to mc4 fucntion.*/
    eOresult_t              res;
    eOmc_joint_status_t     *jstatus_ptr;
    
    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jstatus_ptr);
    if(eores_OK != res)
    {
        return;
    }

    if(eomc_motionmonitormode_dontmonitor == *((eOenum08_t*)nv->loc))
    {
        jstatus_ptr->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  
    }
    else
    {
        jstatus_ptr->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
    }
}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eOmc_setpoint_t                         *setPoint = (eOmc_setpoint_t*)nv->loc;
    eOmc_joint_config_t                     *jconfig_ptr;
    eOmc_joint_status_t                     *jstatus_ptr;


    //set monitor status = notreachedyet if monitormode is forever
    res = eo_appTheDB_GetJointConfigPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jconfig_ptr);
    if(eores_OK != res)
    {
        return; //i should never be here
    }
    
    if(eomc_motionmonitormode_forever == jconfig_ptr->motionmonitormode)
    {
        res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx,  &jstatus_ptr);
        if(eores_OK != res)
        {
            return; //i should never be here
        }
        
        /* if monitorstatus values setpointreached means this is a new set point, 
        so i need to start to check is set point is reached because i'm in monitormode = forever */
        if(eomc_motionmonitorstatus_setpointisreached == jstatus_ptr->basic.motionmonitorstatus)
        {
            jstatus_ptr->basic.motionmonitorstatus = eomc_motionmonitorstatus_setpointnotreachedyet;
        }
    }
    

    switch (setPoint->type)
    {
        case eomc_setpoint_position:
        {
            eo_emsController_SetPosRef(jxx, setPoint->to.position.value, setPoint->to.position.withvelocity);
        }break;
        
        case eomc_setpoint_velocity:
        {
            eo_emsController_SetVelRef(jxx, setPoint->to.velocity.value, setPoint->to.velocity.withacceleration);    
        }break;

        case eomc_setpoint_torque:
        {
            eo_emsController_SetTrqRef(jxx, setPoint->to.torque.value);
        }break;

        case eomc_setpoint_current:
        {             
            eo_emsController_SetOffset(jxx, setPoint->to.current.value);
        }break;

        default:
        {
            return;
        }
    }
}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__stoptrajectory(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eo_emsController_Stop(jxx);
}


extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOmc_calibrator_t                       *calibrator = (eOmc_calibrator_t*)nv->loc;

    eo_emsController_StartCalibration(jxx, 
                                      calibrator->params.type3.position, 
                                      calibrator->params.type3.velocity,
                                      calibrator->params.type3.offset); 

}

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__controlmode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                              res;
    eOmc_joint_status_t                     *jstatus_ptr;
    eOappTheDB_jointOrMotorCanLocation_t    canLoc;
    eOmc_controlmode_t                      *controlmode_ptr = (eOmc_controlmode_t*)nv->loc;
    eOicubCanProto_msgDestination_t         msgdest;
    eOicubCanProto_msgCommand_t            msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingMotorBoard,
        EO_INIT(.cmdId) 0
    };

    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());


    // 1) set control mode in status nv (a mirror nv)
    res = eo_appTheDB_GetJointStatusPtr(eo_appTheDB_GetHandle(), (eOmc_jointId_t)jxx, &jstatus_ptr);
    if(eores_OK != res)
    {
        return;
    }
    jstatus_ptr->basic.controlmodestatus = *controlmode_ptr;



    // 2) send control mode value to 2foc
    res = eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), jxx,  &canLoc, NULL);
    if(eores_OK != res)
    {
        return;
    }

    //set destination of all messages 
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);

    switch(*controlmode_ptr)
    {
        case eomc_controlmode_switch_everything_off:
        {
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__DISABLE_PWM_PAD;
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__CONTROLLER_IDLE;
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
        }break;
        
        case eomc_controlmode_idle:
        {
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__ENABLE_PWM_PAD;
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__CONTROLLER_IDLE;
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
        }break;
        
        default: // pos-controlmode, vel-controlmode, etc (see enum type eOmc_controlmode_t) 
        {
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__SET_CONTROL_MODE;
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, controlmode_ptr);
            
            msgCmd.cmdId = ICUBCANPROTO_POL_MB_CMD__CONTROLLER_RUN;
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
        }
        
    }
    
    // 3) set control mode to ems controller
    eo_emsController_SetControlMode(jxx, (control_mode_t)(*controlmode_ptr));       

}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



