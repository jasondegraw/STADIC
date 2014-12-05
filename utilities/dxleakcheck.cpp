/****************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted for
 * personal and commercial purposes provided that the
 * following conditions are met:
 *
 * 1. Redistribution of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PENNSYLVANIA STATE UNIVERSITY
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************/

#include "leakcheck.h"
#include "logging.h"
#include <iostream>
#include <string>

void usage(){
    std::cout<<"dxleakcheck tests a radiance model for whether the space is or is not fully enclosed.  The program allows any number of polygons and any number of layer names to be used for the placement of the analysis point.  dxleakcheck will then test to ensure that point is within the bouding area of the polygons for the specified layers.  The program will then simulate the model with a uniform sky to determine if any light enters the model."<<std::endl;
    std::cout<<std::endl<<"-f name\tSet the rad file name. This file contains the radiance polygons that will be used for creating the analysis points.  Multiple files may be added with each preceded by a \"-f\".  This is a mandatory option."<<std::endl;
    std::cout<<"-u unit\tSet the unit type (in, ft, mm, or m) to unit for determining the point spacing."<<std::endl;
    std::cout<<"-l name\tSet the layer name that will be used to find the polygons for use in testint the analysis point location.  Multiple layer names can be added with each preceded by a  \"-l\".  This is a mandatory option."<<std::endl;
    std::cout<<"-r val\tSet the reflectance value to use for the analysis.  This can either be 0 or 1."<<std::endl;

}

int main (int argc, char *argv[])
{
    if(argc == 1) {
        usage();
    }
    std::vector<std::string> radFiles;                   //  Variable holding the radiance geometry/material file names
    std::vector<std::string> floorLayers;                //  Variable holding the floor layer names for testing whether the given point is in the polygons
    int unit;
    double reflectance=1;                   //  Variable holding the reflectance

    for (int i=1;i<argc;i++){
        if (std::string("-f")==argv[i]){
            i++;
            radFiles.push_back(argv[i]);
        }else if (std::string("-u")==argv[i]){
            i++;
            if (std::string("in")==argv[i]){
                unit=0;
            }else if (std::string("ft")==argv[i]){
                unit=1;
            }else if (std::string("mm")==argv[i]){
                unit=2;
            }else if (std::string("m")==argv[i]){
                unit=3;
            }else{
                usage();
                EXIT_FAILURE;
            }
        }else if(std::string("-l")==argv[i]){
            i++;
            floorLayers.push_back(argv[i]);
        }else if(std::string("-r")==argv[i]){
            i++;
            reflectance=atof(argv[i]);
        }else{
            std::string temp=argv[i];
            STADIC_WARNING("The argument "+temp+" is an unkown argument.");
        }
    }
    if (radFiles.empty()){
        STADIC_ERROR(std::string("At least one rad file name must be specified.\n\tSpecify with \"-f\"."));
        return EXIT_FAILURE;
    }
    if (floorLayers.empty()){
        STADIC_ERROR(std::string("At least one floor layer name must be specified.\n\tSpecify with \"-t\"."));
        return EXIT_FAILURE;
    }
    //Instantiate GridMaker Object
    stadic::LeakCheck leakChecker;
    if (!leakChecker.setRadFile(radFiles)){
        return EXIT_FAILURE;
    }
    leakChecker.setFloorLayers(floorLayers);
    if (!leakChecker.setUnits(unit)){
        return EXIT_FAILURE;
    }
    if(!leakChecker.setReflectance(reflectance)){
        return EXIT_FAILURE;
    }
    leakChecker.isEnclosed();

    return EXIT_SUCCESS;
}
