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

#include "analemma.h"
#include "logging.h"
#include <iostream>
#include <string>

void usage(){
    std::cerr<<"dxgridmaker generates a points file for use in RADIANCE analysis programs.  The program allows any number of polygons and any number of layer names to be used for the placement of points.  grid_maker will then join all of the polygons and create an array of points within the bounding rectangle and tests each point to determine whether it is within the joined polygon.  These points will then be written out to a given file name for use as input to other programs.  It is assumed that the units of the radiance geometry file are the same as the input units for the spacing, offset, and Z height."<<std::endl;
    std::cerr<<std::endl<<"-f name\tSet the input file to name. This file contains the radiance polygons that will be used for creating the analysis points.  This is a mandatory option."<<std::endl;
}

int main (int argc, char *argv[])
{
    if(argc == 1) {
        usage();
    }
    std::string weaFile;
    weaFile.clear();
    std::string matFile;
    matFile.clear();
    std::string geoFile;
    geoFile.clear();
    std::string smxFile;
    smxFile.clear();
    double rotation=0;

    for (int i=1;i<argc;i++){
        if (std::string("-f")==argv[i]){
            i++;
            weaFile=argv[i];
        }else if (std::string("-m")==argv[i]){
            i++;
            matFile=argv[i];
        }else if (std::string("-g")==argv[i]){
            i++;
            geoFile=argv[i];
        }else if (std::string("-s")==argv[i]){
            i++;
            smxFile=argv[i];
        }else if (std::string("-r")==argv[i]){
            i++;
            rotation=atof(argv[i]);
        }else{
            std::string temp=argv[i];
            STADIC_WARNING("The argument "+temp+" is an unkown argument.");
        }
    }
    if (geoFile.empty()){
        STADIC_ERROR("The gometry file must be specified.");
        usage();
        return EXIT_FAILURE;
    }
    if (matFile.empty()){
        STADIC_ERROR("The material file must be specified.");
        usage();
        return EXIT_FAILURE;
    }
    if (smxFile.empty()){
        STADIC_ERROR("The smx file must be specified.");
        usage();
        return EXIT_FAILURE;
    }

    stadic::Analemma suns(weaFile);
    suns.setGeoFile(geoFile);
    suns.setMatFile(matFile);
    suns.setRotation(rotation);
    suns.setSMXFile(smxFile);
    if (!suns.genSun()){
        return EXIT_FAILURE;
    }



    return EXIT_SUCCESS;
}
