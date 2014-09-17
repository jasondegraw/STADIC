#include "gridmaker.h"
#include "logging.h"
#include <vector>
#include <iostream>
#include <string>

void usage(){
    std::cerr<<"dxgridmaker generates a points file for use in RADIANCE analysis programs.  The program allows any number of polygons and any number of layer names to be used for the placement of points.  grid_maker will then join all of the polygons and create an array of points within the bounding rectangle and tests each point to determine whether it is within the joined polygon.  These points will then be written out to a given file name for use as input to other programs.  It is assumed that the units of the radiance geometry file are the same as the input units for the spacing, offset, and Z height."<<std::endl;
    std::cerr<<std::endl<<"-f name\tSet the input file to name. This file contains the radiance polygons that will be used for creating the analysis points.  This is a mandatory option."<<std::endl;
    std::cerr<<"-sx val\tSet the spacing in the x direction between the analysis points to val.  This is a mandatory option. "<<std::endl;
    std::cerr<<"-sy val\tSet the spacing in the y direction between the analysis points to val.  This is a mandatory option."<<std::endl;
    std::cerr<<"-ox val\tSet the offset in the x direction from the left side of the bounding rectangle to val."<<std::endl;
    std::cerr<<"-oy val\tSet the offset in the y direction from the lower side of the bounding rectangle to val."<<std::endl;
    std::cerr<<"-z val\tSet the z height of the analysis points using world coordinates to val."<<std::endl;
    std::cerr<<"-l name\tSet the layer name that will be used to find the polygons for use in creating the analysis grid to name.  Multiple layer names can be used, but each one must have a -l preceding it.  This is a mandatory option."<<std::endl;
    std::cerr<<"-r name\tSet the output file to name.  This file contains a space separated file in the following format per line:   x y z xd yd zd."<<std::endl;
    std::cerr<<"-p name\tSet the ouput for the joined polygon to name. This file contains the joined polygon in the radiance polygon format with a modifier of \"floor\" and an identifier of \"floor1\"."<<std::endl;
    std::cerr<<"-csv name\tSet the csv formatted output file to name.  This file contains the points file output in a csv format."<<std::endl;
}

int main (int argc, char *argv[])
{
    if(argc == 1) {
        usage();
    }
    std::string fileName;
    fileName.clear();
    std::string resultFile;
    resultFile.clear();
    std::string csvFile;
    csvFile.clear();
    std::string polyFile;
    polyFile.clear();
    std::vector<std::string> layerNames;
    double sx, sy, ox, oy, z;
    sx=0;
    sy=0;
    ox=0;
    oy=0;
    z=0;
    for (int i=1;i<argc;i++){
        if (argv[i]=="-f"){
            i++;
            fileName=argv[i];
        }else if (argv[i]=="-sx"){
            i++;
            sx=atof(argv[i]);
        }else if (argv[i]=="-sy"){
            i++;
            sy=atof(argv[i]);
        }else if (argv[i]=="-ox"){
            i++;
            ox=atof(argv[i]);
        }else if (argv[i]=="-oy"){
            i++;
            oy=atof(argv[i]);
        }else if (argv[i]=="-z"){
            i++;
            z=atof(argv[i]);
        }else if (argv[i]=="-r"){
            i++;
            resultFile=argv[i];
        }else if(argv[i]=="-l"){
            i++;
            layerNames.push_back(argv[i]);
        }else if(argv[i]=="-p"){
            i++;
            polyFile=argv[i];
        }else if(argv[i]=="-csv"){
            i++;
            csvFile=argv[i];
        }else{
            std::string temp=argv[i];
            STADIC_WARNING("The argument "+QString::fromStdString(temp)+" is an unkown argument.");
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
    stadic::GridMaker grid;
    for (int i=0;i<layerNames.size();i++){
        grid.setLayerNames(layerNames.at(i));
    }
    grid.setSpaceX(sx);
    grid.setSpaceY(sy);
    grid.setOffsetX(ox);
    grid.setOffsetY(oy);
    grid.setZHeight(z);
    if (!grid.parseRad(fileName)){
        STADIC_ERROR(std::string("The parsing of the rad file failed."));
        return EXIT_FAILURE;
    }
    if (!grid.makeGrid(fileName)){
        STADIC_ERROR(std::string("The creation of the grid failed."));
        EXIT_FAILURE;
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
        if (!grid.writeRadPoly(polyFile)){
            return EXIT_FAILURE;
        }
    }
    if (!csvFile.empty()){
        if (!grid.writePTScsv(csvFile)){
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
