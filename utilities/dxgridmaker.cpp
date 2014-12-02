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

#include "gridmaker.h"
#include "logging.h"
#include "functions.h"
#include <vector>
#include <iostream>
#include <string>

void usage()
{
    std::cerr << "Usage: dxgridmaker [OPTIONS]" << std::endl;
    std::cerr << stadic::wrapAtN("Generate a points file for use in Radiance analysis programs.  The program"
        " allows any number of polygons and any number of layer names to be used for the placement of points.  dxgridmaker"
        " joins all of the polygons and creates an array of points within the bounding rectangle and tests each"
        " point to determine whether it is within the joined polygon.  These points will then be written out"
        " for use as input to other programs.  It is assumed that the units of the Radiance geometry file are"
        " the same as the input units for the spacing, offset, and Z height.") << std::endl;

    std::cerr << std::endl;
    std::cerr << stadic::wrapAtN("-f name        Set the input file to name. This file contains the radiance polygons"
        " that will be used for creating the analysis points.  This is a mandatory option.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-sx val        Set the spacing in the x direction between the analysis points to"
        " val.  This is a mandatory option.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-sy val        Set the spacing in the y direction between the analysis points to"
        " val.  This is a mandatory option.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-ox val        Set the offset in the x direction from the left side of the"
        " bounding rectangle to val.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-oy val        Set the offset in the y direction from the lower side of the bounding"
        " rectangle to val.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-oz val        Set the offset in the z direction from the average height of the floor"
        " polygons to val.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-o val         Set the offset for both the x and y direction from the perimeter of the"
        " floor polygons to val.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-z val         Set the z height of the analysis points using world coordinates to val.",
        72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-l name        Set the layer name that will be used to find the polygons for use in"
        " creating the analysis grid to name.  Multiple layer names can be used, but each one must have a -l preceding"
        " it.  This is a mandatory option.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-r name        Set the output file to name.  This file contains a space separated file"
        " in the following format per line:   x y z xd yd zd.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-t dist        Set the threshold distance.  Should the polygons be too small after the"
        " offset, and the length as well as width of the polygon are less than the threshold, those polygons will be"
        " ignored.  If this value is not used, a centerline of points will be placed down the polygon.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-vp location   Set the location that the files should be placed for creating the"
        " parallel projection .bmp.  This should end in the directory separator.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-vse location  Set the location that the files should be placed for creating the South"
        " East isometric .bmp.  This should end in the directory separator.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-vsw location  Set the location that the files should be placed for creating the South"
        " West isometric .bmp.  This should end in the directory separator.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-vne location  Set the location that the files should be placed for creating the North"
        " East isometric .bmp.  This should end in the directory separator.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-vnw location  Set the location that the files should be placed for creating the North"
        " West isometric .bmp.  This should end in the directory separator.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-p name        Set the ouput for the joined polygon to name. This file contains the"
        " joined polygon in the radiance polygon format with a modifier of \"floor\" and an identifier of \"floor1\".", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-csv name      Set the csv formatted output file to name.  This file contains the points"
        " file output in a csv format.", 72, 15, true) << std::endl;
}

int main (int argc, char *argv[])
{
    if(argc == 1) {
        usage();
    }
    std::vector<std::string> fileName;
    std::string resultFile;
    std::string csvFile;
    std::string polyFile;
    std::string viewLocation;
    std::string vType;
    std::vector<std::string> layerNames;
    bool useZOffset=false;
    bool useOffset=false;
    bool useThreshold=false;
    double sx, sy, ox, oy, oz, offset,z, threshold;
    sx=0;
    sy=0;
    ox=0;
    oy=0;
    oz=0;
    offset=0;
    z=0;
    for (int i=1;i<argc;i++){
        if (std::strcmp(argv[i],"-f")==0){
            i++;
            fileName.push_back(argv[i]);
        }else if (std::strcmp(argv[i],"-sx")==0){
            i++;
            sx=atof(argv[i]);
        }else if (std::strcmp(argv[i],"-sy")==0){
            i++;
            sy=atof(argv[i]);
        }else if (std::strcmp(argv[i],"-ox")==0){
            i++;
            ox=atof(argv[i]);
        }else if (std::strcmp(argv[i],"-oy")==0){
            i++;
            oy=atof(argv[i]);
        }else if (std::strcmp(argv[i],"-oz")==0){
            i++;
            useZOffset=true;
            oz=atof(argv[i]);
        }else if (std::strcmp(argv[i],"-o")==0){
            i++;
            useOffset=true;
            offset=atof(argv[i]);
        }else if (std::strcmp(argv[i],"-z")==0){
            i++;
            z=atof(argv[i]);
        }else if (std::strcmp(argv[i],"-r")==0){
            i++;
            resultFile=argv[i];
        }else if(std::strcmp(argv[i],"-l")==0){
            i++;
            layerNames.push_back(argv[i]);
        }else if(std::strcmp(argv[i],"-vp")==0){
            i++;
            viewLocation=argv[i];
            vType="p";
        }else if(std::strcmp(argv[i],"-vse")==0){
            i++;
            viewLocation=argv[i];
            vType="se";
        }else if(std::strcmp(argv[i],"-vne")==0){
            i++;
            viewLocation=argv[i];
            vType="ne";
        }else if(std::strcmp(argv[i],"-vsw")==0){
            i++;
            viewLocation=argv[i];
            vType="sw";
        }else if(std::strcmp(argv[i],"-vnw")==0){
            i++;
            viewLocation=argv[i];
            vType="nw";
        }else if(std::strcmp(argv[i],"-p")==0){
            i++;
            polyFile=argv[i];
        }else if(std::strcmp(argv[i],"-csv")==0){
            i++;
            csvFile=argv[i];
        }else if(std::strcmp(argv[i],"-t")==0){
            i++;
            threshold=atof(argv[i]);
            useThreshold=true;
        }else{
            std::string temp=argv[i];
            STADIC_WARNING("The argument "+temp+" is an unkown argument.\n\tTry running with no arguments to get usage.");
        }
    }
    if (sx==0 ||sy==0){
        STADIC_ERROR(std::string("The x and y spacing are needed to complete the calculation.\n\tSpecify with \"-sx\" and \"-sy\"."));
        return EXIT_FAILURE;
    }
    if (fileName.empty()){
        STADIC_ERROR(std::string("The rad file name must be specified.\n\tSpecify with \"-f\"."));
        return EXIT_FAILURE;
    }

    //Instantiate GridMaker Object
    stadic::GridMaker grid(fileName);
    grid.setLayerNames(layerNames);
    grid.setSpaceX(sx);
    grid.setSpaceY(sy);
    if (useOffset){
        grid.setOffset(offset);
    }else{
    grid.setOffsetX(ox);
    grid.setOffsetY(oy);
    }
    if (useZOffset){
        grid.setOffsetZ(oz);
    }else{
    grid.setZHeight(z);
    }
    if (useThreshold){
        grid.setThreshold(threshold);
    }
    if (!grid.makeGrid()){
        return EXIT_FAILURE;
    }
    if (resultFile.empty()){
        if(!grid.writePTS()){
            STADIC_ERROR(std::string("The writing of the points file to the standard output has failed."));
            return EXIT_FAILURE;
        }
    }else{
        if (!grid.writePTS(resultFile)){
            STADIC_ERROR(std::string("The writing of the points file failed."));
            return EXIT_FAILURE;
        }
    }
    /*
    if (!polyFile.empty()){
        if (!grid.writeRadPoly(polyFile)){
            return EXIT_FAILURE;
        }
    }
    */
    if (!csvFile.empty()){
        if (!grid.writePTScsv(csvFile)){
            return EXIT_FAILURE;
        }
    }
    if (!viewLocation.empty()){
        if (!grid.viewPTS(viewLocation, vType)){
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
