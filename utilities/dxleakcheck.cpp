#include "leakcheck.h"
#include "logging.h"
#include <iostream>

void usage(){
    std::cout<<"dxleakcheck tests a radiance model for whether the space is or is not fully enclosed.  The program allows any number of polygons and any number of layer names to be used for the placement of the analysis point.  dxleakcheck will then test to ensure that point is within the bouding area of the polygons for the specified layers.  The program will then simulate the model with a uniform sky to determine if any light enters the model."<<std::endl;
    std::cout<<std::endl<<"-f name\tSet the rad file name. This file contains the radiance polygons that will be used for creating the analysis points.  Multiple files may be added with each preceded by a \"-f\".  This is a mandatory option."<<std::endl;
    std::cout<<"-X val\tSet the x position for the analysis point.  This is a mandatory option. "<<std::endl;
    std::cout<<"-Y val\tSet the y position for the analysis point.  This is a mandatory option."<<std::endl;
    std::cout<<"-Z val\tSet the z position for the analysis point.  This is a mandatory option."<<std::endl;
    std::cout<<"-l name\tSet the layer name that will be used to find the polygons for use in testint the analysis point location.  Multiple layer names can be added with each preceded by a  \"-l\".  This is a mandatory option."<<std::endl;
    std::cout<<"-D path\tSet the working directory location.  This is a mandatory option"<<std::endl;
    std::cout<<"-R val\tSet the reflectance value to use for the analysis.  This can either be 0 or 1."<<std::endl;

}

int main (int argc, char *argv[])
{
    if(argc == 1) {
        usage();
    }
    QStringList radFiles;                   //  Variable holding the radiance geometry/material file names
    radFiles.clear();
    QStringList floorLayers;                //  Variable holding the floor layer names for testing whether the given point is in the polygons
    floorLayers.clear();
    double xPt;                             //  Variable holding the x coordinate of the test point
    double yPt;                             //  Variable holding the y coordinate of the test point
    double zPt;                             //  Variable holding the z coordinate of the test point
    double reflectance=1;                   //  Variable holding the reflectance
    QString wDirectory;                     //  Variable holding the working directory for the creation of files
    wDirectory.clear();


    for (int i=1;i<argc;i++){
        if (QString(argv[i])=="-f"){
            i++;
            radFiles.append(argv[i]);
        }else if (QString(argv[i])=="-X"){
            i++;
            xPt=atof(argv[i]);
        }else if (QString(argv[i])=="-Y"){
            i++;
            yPt=atof(argv[i]);
        }else if (QString(argv[i])=="-Z"){
            i++;
            zPt=atof(argv[i]);
        }else if(QString(argv[i])=="-l"){
            i++;
            floorLayers.append(QString(argv[i]));
        }else if(QString(argv[i])=="-D"){
            i++;
            wDirectory=argv[i];
        }else if(QString(argv[i])=="-R"){
            i++;
            reflectance=atof(argv[i]);
        }else{
            QString temp=argv[i];
            WARNING("The argument "+temp+" is an unkown argument.");
        }
    }
    if (radFiles.isEmpty()){
        ERROR("At least one rad file name must be specified.\n\tSpecify with \"-f\".");
        return EXIT_FAILURE;
    }
    if (floorLayers.isEmpty()){
        ERROR("At least one floor layer name must be specified.\n\tSpecify with \"-t\".");
        return EXIT_FAILURE;
    }
    if (wDirectory.isEmpty()){
        ERROR("The name of the directory where the temporary files should be placed must be specified.\n\tSpecify with \"-D\".");
        return EXIT_FAILURE;
    }
    //Instantiate GridMaker Object
    stadic::LeakCheck leakChecker;
    if (!leakChecker.setRadFile(radFiles)){
        return EXIT_FAILURE;
    }
    if (!leakChecker.setFloorLayers(floorLayers)){
        return EXIT_FAILURE;
    }
    if(!leakChecker.setX(xPt)){
        return EXIT_FAILURE;
    }
    if(!leakChecker.setY(yPt)){
        return EXIT_FAILURE;
    }
    if(!leakChecker.setZ(zPt)){
        return EXIT_FAILURE;
    }
    if(!leakChecker.setReflectance(reflectance)){
        return EXIT_FAILURE;
    }
    if(!leakChecker.setWorkingDirectory(wDirectory)){
        return EXIT_FAILURE;
    }
    leakChecker.isEnclosed();

    return EXIT_SUCCESS;
}
