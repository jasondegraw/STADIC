/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission of the
 *    respective copyright holder or contributor.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *****************************************************************************/

#include "photosensor.h"
#include "logging.h"
#include <fstream>
#include "functions.h"

namespace stadic {

Photosensor::Photosensor()
{
}
bool Photosensor::setType(std::string type){
    if (type=="cosine"){
        m_Type=type;
        return true;
    }else if (type=="cosine_squared"){
        m_Type=type;
        return true;
    }
    STADIC_LOG(Severity::Error, "The photosensor type of "+type+" is not recognized.");
    return false;
}


//utilities
bool Photosensor::writeSensorFile(std::string file){
    if (m_Type=="cosine"){
        if (!writeCosine(file)){
            return false;
        }
    }else if (m_Type=="cosine_squared"){
        if (!writeCosineSquared(file)){
            return false;
        }
    }

    return true;
}


//private
bool Photosensor::writeCosine(std::string file){
    std::ofstream sensorOut;
    sensorOut.open(file);
    if (!sensorOut.is_open()){
        STADIC_LOG(Severity::Warning, "The opening of the file "+file+" for writing the photosensor sensitivity has failed.");
        return false;
    }


    sensorOut.close();
    return true;
}

bool Photosensor::writeCosineSquared(std::string file){
    std::ofstream sensorOut;
    sensorOut.open(file);
    if (!sensorOut.is_open()){
        STADIC_LOG(Severity::Warning, "The opening of the file "+file+" for writing the photosensor sensitivity has failed.");
        return false;
    }


    sensorOut.close();
    return true;
}

}