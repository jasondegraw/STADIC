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

#include "gridmaker.h"
#include "logging.h"
#include "functions.h"
#include <vector>
#include <iostream>
#include <string>

void usage()
{
    std::cerr << "Usage: dxgridmaker [OPTIONS]" << std::endl;
    std::cerr << "Analysis point generator version " << VERSIONMAJOR << "." << VERSIONMINOR << "." << VERSIONPATCH << std::endl;
    std::cerr << stadic::wrapAtN("Generate a points file for use in Radiance analysis programs.  The program"
        " allows any number of polygons and any number of layer names to be used for the placement of points.  dxgridmaker"
        " joins all of the polygons and creates an array of points within the bounding rectangle and tests each"
        " point to determine whether it is within the joined polygon.  These points will then be written out"
        " for use as input to other programs.  It is assumed that the units of the Radiance geometry file are"
        " the same as the input units for the spacing, offset, and Z height.") << std::endl;

    std::cerr << std::endl;
    std::cerr << stadic::wrapAtN("-f name        Set the input file to name. This file contains the radiance polygons"
        " that will be used for creating the analysis points.  This is a mandatory option.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-i name        Set the identifier sub-string that will be used to find the polygons"
        " for use in creating the analysis grid to name.  Multiple identifier sub-strings can be used, but each one"
        " must have a -i preceding it.", 72, 15, true) << std::endl;
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
        " it.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-r name        Set the output file to name.  This file contains a space separated file"
        " in the following format per line:   x y z xd yd zd.", 72, 15, true) << std::endl;
    std::cerr << stadic::wrapAtN("-rz val        Set the angle of rotation to val.  A positive value should be used when the"
        " building (or space) is rotated ccw.  Setting the rotation allows the points to be aligned with the space.", 72, 15, true) << std::endl;
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
    std::vector<std::string> identifiers;
    bool useZOffset=false;
    bool useOffset=false;
    bool useThreshold=false;
    double sx, sy, ox, oy, oz, offset,z, threshold, rotation;
    sx=0;
    sy=0;
    ox=0;
    oy=0;
    oz=0;
    offset=0;
    z=0;
    rotation=0;
    for (int i=1;i<argc;i++){
        if (std::string("-f")==argv[i]){
            i++;
            fileName.push_back(argv[i]);
        }else if (std::string("-sx")==argv[i]){
            i++;
            sx=atof(argv[i]);
        }else if (std::string("-sy")==argv[i]){
            i++;
            sy=atof(argv[i]);
        }else if (std::string("-ox")==argv[i]){
            i++;
            ox=atof(argv[i]);
        }else if (std::string("-oy")==argv[i]){
            i++;
            oy=atof(argv[i]);
        }else if (std::string("-oz")==argv[i]){
            i++;
            useZOffset=true;
            oz=atof(argv[i]);
        }else if (std::string("-o")==argv[i]){
            i++;
            useOffset=true;
            offset=atof(argv[i]);
        }else if (std::string("-z")==argv[i]){
            i++;
            z=atof(argv[i]);
        }else if (std::string("-r")==argv[i]){
            i++;
            resultFile=argv[i];
        }else if(std::string("-l")==argv[i]){
            i++;
            layerNames.push_back(argv[i]);
        }else if (std::string("-i")==argv[i]){
            i++;
            identifiers.push_back(argv[i]);
        }else if(std::string("-vp")==argv[i]){
            i++;
            viewLocation=argv[i];
            vType="p";
        }else if(std::string("-vse")==argv[i]){
            i++;
            viewLocation=argv[i];
            vType="se";
        }else if(std::string("-vne")==argv[i]){
            i++;
            viewLocation=argv[i];
            vType="ne";
        }else if(std::string("-vsw")==argv[i]){
            i++;
            viewLocation=argv[i];
            vType="sw";
        }else if(std::string("-vnw")==argv[i]){
            i++;
            viewLocation=argv[i];
            vType="nw";
        }else if(std::string("-p")==argv[i]){
            i++;
            polyFile=argv[i];
        }else if(std::string("-csv")==argv[i]){
            i++;
            csvFile=argv[i];
        }else if(std::string("-t")==argv[i]){
            i++;
            threshold=atof(argv[i]);
            useThreshold=true;
        }else if(std::string("-rz")==argv[i]){
            i++;
            rotation=atof(argv[i]);
        }else{
            std::string temp=argv[i];
            STADIC_ERROR("Invalid option \""+temp+"\".  Run with no arguments to get usage.");
            return EXIT_FAILURE;
        }
    }
    if (sx==0 ||sy==0){
        STADIC_ERROR("The x and y spacing are needed to complete the calculation.  Specify with \"-sx\" and \"-sy\".");
        return EXIT_FAILURE;
    }
    if (fileName.empty()){
        STADIC_ERROR("The rad file name must be specified.  Specify with \"-f\".");
        return EXIT_FAILURE;
    }

    //Instantiate GridMaker Object
    stadic::GridMaker grid(fileName);
    bool geometryNamed=false;
    if (layerNames.size()>0){
        grid.setLayerNames(layerNames);
        geometryNamed=true;
    }
    if (identifiers.size()>0){
        grid.setIdentifiers(identifiers);
        geometryNamed=true;
    }
    if (geometryNamed==false){
        STADIC_LOG(stadic::Severity::Warning,"All geometry will be used to generate points.");
    }
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
    if (rotation!=0){
        grid.setRotation(rotation);
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
    if (!polyFile.empty()){
        if (!grid.writeUnitedRadPoly(polyFile)){
            return EXIT_FAILURE;
        }
    }
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
