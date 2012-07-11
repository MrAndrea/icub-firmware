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
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file       EOMappDataTransmitter.c
    @brief      This file contains internal implementation for EOMappDataTransmitter object
    @author     valentina.gaggero@iit.it
    @date       02/20/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"     // to see NULL, calloc etc.
#include "string.h"     //memcpy

// abstraction layers
#include "hal.h"

//embobj
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
//nvs
#include "EOtransceiver.h"
#include "EOtheBOARDtransceiver.h"

//only for debug purpose
#include "EOpacket_hid.h"

// application
#include "EoMotionControl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOMappDataTransmitter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOMappDataTransmitter_hid.h" 




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define EVT_CHECK(var, EVTMASK)             (EVTMASK == (var&EVTMASK))



#define TASK_DATATRANSMITTER_PRIO          60   // task piority

//************************ system controller task events **************************************************
#define EVT_START          (1 << 0)

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
extern  uint32_t ena_tx_onrx;
extern  int16_t pwm_out;
extern int32_t encoder_can;
extern int32_t posref_can;
extern uint8_t can_msg[8];

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
//function used to configure eOMtask obj
void s_eom_appDataTransmitter_taskInit(void *p);
static void s_eom_appDataTransmitter_taskStartup(EOMtask *p, uint32_t t);
static void s_eom_appDataTransmitter_taskRun(EOMtask *tsk, uint32_t evtmsgper); 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static const char s_eobj_ownname[] = "EOMappDataTransmitter";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOMappDataTransmitter* eom_appDataTransmitter_New(EOMappDataTransmitter_cfg_t *cfg)
{
    EOMappDataTransmitter *retptr = NULL;


    if(NULL == cfg)
    {
        return(retptr);
    }


    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOMappDataTransmitter), 1);

    //save obj's configuration
    memcpy(&retptr->cfg, cfg, sizeof(EOMappDataTransmitter_cfg_t));
    retptr->mytask = eom_task_New(eom_mtask_EventDriven,
                              TASK_DATATRANSMITTER_PRIO ,
                              2*1024,           //stacksize: da rivedere
                              s_eom_appDataTransmitter_taskStartup, 
                              s_eom_appDataTransmitter_taskRun,  
                              0,                //message queue size. the task is evt based 
                              eok_reltimeINFINITE,
                              retptr, 
                              s_eom_appDataTransmitter_taskInit, 
                              "dataTransmitter");

    retptr->st = eOm_appDataTransmitter_st__idle; 
    
    return(retptr);
}


extern eOresult_t eom_appDataTransmitter_Activate(EOMappDataTransmitter *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    p->st = eOm_appDataTransmitter_st__active;
    ena_tx_onrx = 0;
    return(eores_OK);
}


extern eOresult_t eom_appDataTransmitter_Deactivate(EOMappDataTransmitter *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    p->st = eOm_appDataTransmitter_st__idle;

    return(eores_OK);
}

extern eOresult_t eom_appDataTransmitter_SendData(EOMappDataTransmitter *p)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eOm_appDataTransmitter_st__idle == p->st)
    {
        return(eores_NOK_generic);    
    }

    //in i'm here i'm in activ state
    p->st = eOm_appDataTransmitter_st__transmitting;
    eom_task_SetEvent(p->mytask, EVT_START);
    return(eores_OK);
}


    


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
void s_eom_appDataTransmitter_taskInit(void *p)
{
    eom_task_Start(p);
}


static void s_eom_appDataTransmitter_taskStartup(EOMtask *tsk, uint32_t t)
{
    EOMappDataTransmitter *p = (EOMappDataTransmitter*)eom_task_GetExternalData(tsk);

    eo_errman_Assert(eo_errman_GetHandle(), NULL != p, s_eobj_ownname, "extdata() is NULL");

}

static void s_eom_appDataTransmitter_taskRun(EOMtask *tsk, uint32_t evtmsgper)
{
    eOevent_t evt;
    EOpacket *pkt_ptr;
    uint16_t numberofrops;
    eOresult_t res;
    
    eOmc_setpoint_t     mySetPoint_current = 
    {
        EO_INIT(.type)       eomc_setpoint_current,
        EO_INIT(.to)
        {
            EO_INIT(.current)
            {
                EO_INIT(.value)     0
            }   
        }
    };

    mySetPoint_current.to.current.value = pwm_out;



    EOMappDataTransmitter *p = (EOMappDataTransmitter*)eom_task_GetExternalData(tsk);

    evt = (eOevent_t)evtmsgper;
    uint8_t payload[8];   
   

    if(EVT_CHECK(evt, EVT_START))
    {
        //this set point should be fill by motor controller: used mySetPoint_current only for test porpouse
        res = eo_transceiver_Transmit(eo_boardtransceiver_GetHandle(), &pkt_ptr, &numberofrops);
        if(eores_OK != res)
        {
            return;
        }
//        if(0 == ena_tx_onrx)
//        {
//            return;
//        }

        ena_tx_onrx = 0;

        res = eo_ethBaseModule_TransmitPacket(p->cfg.eth_mod, pkt_ptr);
        if(eores_OK != res)
        {
            return;
        }
        eo_appCanSP_SendSetPoint(p->cfg.appCanSP_ptr, 0, &mySetPoint_current);
    
        ((int32_t*)payload)[0]=encoder_can;
        ((int32_t*)payload)[1]=posref_can;
        
        eo_appCanSP_SendMessage_TEST(p->cfg.appCanSP_ptr, NULL, payload);
        p->st = eOm_appDataTransmitter_st__active;
    }

}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



