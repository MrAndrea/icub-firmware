/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Davide Pollarolo
 * email:   valentina.gaggero@iit.it, davide.pollarolo@iit.it
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


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EOAPPENCODERSREADER_HID_H_
#define _EOAPPENCODERSREADER_HID_H_


// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOappEncodersReader.h
    @brief     This file provides hidden interface to encodersReader obj.
    @author    valentina.gaggero@iit.it, davide.pollarolo@iit.it
    @date       02/17/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------


#include "hal_spiencoder.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOappEncodersReader.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------



/** @typedef    struct eOappEncReader_stream_t
    @brief      contains representation of an SPI stream of hal encoders. They all must be of the same type.
 **/
typedef struct 
{
    hal_spiencoder_type_t   type;                               /**< the type of SPI encoders. it must be only one */
    uint8_t                 numberof;                           /**< their number inside the stream */
    volatile eObool_t       isacquiring;                        // we shoudl use it in the future to see if acquisition is still ongoing. infact also the incremental are mapped into streams
} eOappEncReader_stream_t;


// the application is able to read until a maximum of 6 encoders (EMS board): 3 connected with one SPI (SPI2) and others 3 connected with another SPI bus (SPI3).
//

typedef struct
{
    hal_spiencoder_t first;                                         /**< indicates the first encoder to read */
    hal_spiencoder_t list[hal_spiencoder_maxnumber_in_stream+1];    /**< for the encoder i-th: in list[i] the encoder i-th finds the number of encoder it must start when it has finish its onw read. */
} EOappEncReader_streamedSPIsequence_t;


typedef enum
{
    encreader_spistatus_idle      = 0,
    encreader_spistatus_reading   = 1,
    encreader_spistatus_finished  = 2  
} eOappEncReader_SPIstatus_t;


typedef struct
{
    volatile eOappEncReader_SPIstatus_t     spiStatus;              /**< contains the status of reading */
    eo_appEncReader_encoder_type_t          encType;                /**< the type of the encoder to be read (AEA and AMO supported) */
    uint8_t                                 numberofencoders;       // the number of encoders associated to stream
    uint8_t                                 maxsupportedencoders;
    EOappEncReader_streamedSPIsequence_t    sequence;               /**< contains the sequence of reading of encoders connected to a stream */ 
} EOappEncReader_SPIstream_config_t;


typedef struct
{
    volatile eOappEncReader_SPIstatus_t     spiStatus;          /**< contains the status of reading  */
    eo_appEncReader_encoder_type_t          encType;            /**< the type of the encoder to be read  */
    uint8_t                                 numberofencoders;   // the number of encoders associated to stream
    uint8_t                                 maxsupportedencoders;
    hal_spiencoder_t                        singleport;
} EOappEncReader_SPIchained_config_t;


// in future: instead of the above two struct use the following
// 
//typedef struct
//{
//    volatile eOappEncReader_SPIstatus_t     spiStatus;          /**< contains the status of reading  */
//    eo_appEncReader_encoder_type_t          encType;            /**< the type of the encoder to be read  */
//    uint8_t                                 numberofencoders;   // the number of encoders associated to stream
//    uint8_t                                 maxsupportedencoders;
//} EOappEncReader_SPIstreamConfig_t;
// 
//typedef union
//{
//    hal_spiencoder_t                        singleport;
//    EOappEncReader_streamedSPIsequence_t    sequence;
//} EOappEncReader_SPIstreamports_t;
// 
//typedef struct
//{
//    EOappEncReader_SPIstreamConfig_t        streamconfig;
//    EOappEncReader_SPIstreamports_t         streamports;
//} EOappEncReader_SPIstream_t;

//enum { encoder_readingtype_spichained = 0, encoder_readingtype_spistreamed = 1, encoder_readingtype_other = 2, encoder_readingtypes_numberof = 3 };

struct EOappEncReader_hid
{
    eObool_t                                initted;
    eObool_t                                active;
    uint8_t                                 totalnumberofencoders; // it cannot be higher than eOappEncReader_encoders_maxnumberof
    const hal_spiencoder_stream_map_t*      stream_map;
    eOappEncReader_cfg_t                    config;       
    eOappEncReader_stream_t                 SPI_streams[hal_spiencoder_streams_number];  // SPI streams; must be coherent with what inside cfg
    EOappEncReader_SPIstream_config_t       configofSPIstream0;      /* encoders of type aea or amo configured on the first  SPI stream */
    EOappEncReader_SPIstream_config_t       configofSPIstream1;      /* encoders of type aea or amo configured on the second SPI stream */
    EOappEncReader_SPIchained_config_t      configofSPIChained[hal_spiencoder_streams_number];  // at most 
    eo_appEncReader_encoder_type_t          encodertype[eOappEncReader_encoders_maxnumberof];
    //volatile uint8_t                        maskofacquiringspichained;
    uint64_t                                times[2][4];
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



