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
    std::string matFile;
    std::string geoFile;
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
        }else if (std::string("-r")==argv[i]){
            i++;
            rotation=atof(argv[i]);
        }else{
            std::string temp=argv[i];
            STADIC_WARNING("The argument "+temp+" is an unkown argument.");
        }
    }
    stadic::Analemma suns(weaFile);
    suns.setGeoFile(geoFile);
    suns.setMatFile(matFile);
    suns.setRotation(rotation);
    if (!suns.genSun()){
        return false;
    }



    return EXIT_SUCCESS;
}
