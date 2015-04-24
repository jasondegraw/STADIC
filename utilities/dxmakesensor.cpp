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
#include "functions.h"
#include <iostream>
#include <string>

void usage(){
    std::cerr << "dxmakesensor" << std::endl;
    std::cerr << stadic::wrapAtN("Create a photosensor distibution for input to rsensor.", 72) << std::endl;
    std::cerr << std::endl;
    std::cerr << stadic::wrapAtN("-f name  Set the sensor file name. This file contains the radiance polygons that will"
        " be used for creating the analysis points.  Multiple files may be added with each preceded by a \"-f\".  This"
        " is a mandatory option.", 72, 9, true) << std::endl;
    std::cerr << stadic::wrapAtN("-t type  Set the sensor type (\"cosine\", \"cosine_squared\") for the sensitivity.", 72, 9, true) << std::endl;

}

int main (int argc, char *argv[])
{
    if(argc == 1) {
        usage();
    }
    std::string fileName;
    std::string type;
    for (int i=1;i<argc;i++){
        if (std::string("-f")==argv[i]){
            i++;
            fileName=argv[i];
        }else if(std::string("-t")==argv[i]){
            i++;
            type=argv[i];
        }else{
            std::string temp=argv[i];
            STADIC_ERROR("Invalid option \""+temp+"\".  Run with no arguments to get usage.");
            return EXIT_FAILURE;
        }
    }
    stadic::Photosensor sensor;
    if (!sensor.setType(type)){
        return EXIT_FAILURE;
    }
    if (fileName.empty()){
        if(!sensor.writeSensorFile()){
            return EXIT_FAILURE;
        }
    }else{
        if (!sensor.writeSensorFile(fileName)){
            return EXIT_FAILURE;
        }
    }


    return EXIT_SUCCESS;
}
