#include "gridmaker.h"
#include "logging.h"
#include <vector>
#include <iostream>

void usage(){
    std::cout<<"dxgridmaker generates a points file for use in RADIANCE analysis programs.  The program allows any number of polygons and any number of layer names to be used for the placement of points.  grid_maker will then join all of the polygons and create an array of points within the bounding rectangle and tests each point to determine whether it is within the joined polygon.  These points will then be written out to a given file name for use as input to other programs.  It is assumed that the units of the radiance geometry file are the same as the input units for the spacing, offset, and Z height."<<std::endl;
    std::cout<<std::endl<<"-f name\tSet the input file to name. This file contains the radiance polygons that will be used for creating the analysis points.  This is a mandatory option."<<std::endl;
    std::cout<<"-Sx val\tSet the spacing in the x direction between the analysis points to val.  This is a mandatory option. "<<std::endl;
    std::cout<<"-Sy val\tSet the spacing in the y direction between the analysis points to val.  This is a mandatory option."<<std::endl;
    std::cout<<"-Ox val\tSet the offset in the x direction from the left side of the bounding rectangle to val."<<std::endl;
    std::cout<<"-Oy val\tSet the offset in the y direction from the lower side of the bounding rectangle to val."<<std::endl;
    std::cout<<"-Z val\tSet the z height of the analysis points using world coordinates to val."<<std::endl;
    std::cout<<"-l name\tSet the layer name that will be used to find the polygons for use in creating the analysis grid to name.  Multiple layer names can be used, but each one must have a -l preceding it.  This is a mandatory option."<<std::endl;
    std::cout<<"-r name\tSet the output file to name.  This file contains a space separated file in the following format per line:   x y z xd yd zd./n/t/tThis is a mandatory option."<<std::endl;
    std::cout<<"-p name\tSet the ouput for the joined polygon to name. This file contains the joined polygon in the radiance polygon format with a modifier of \"floor\" and an identifier of \"floor1\"."<<std::endl;
    std::cout<<"-csv name\tSet the csv formatted output file to name.  This file contains the points file output in a csv format."<<std::endl;
}

int main (int argc, char *argv[])
{
    if(argc == 1) {
        usage();
    }
    QString fileName;
    fileName.clear();
    QString resultFile;
    resultFile.clear();
    QString csvFile;
    csvFile.clear();
    QString polyFile;
    polyFile.clear();
    std::vector<QString> layerNames;
    double Sx, Sy, Ox, Oy, Z;
    Sx=0;
    Sy=0;
    Ox=0;
    Oy=0;
    Z=0;
    for (int i=1;i<argc;i++){
        if (QString(argv[i])=="-f"){
            i++;
            fileName=argv[i];
        }else if (QString(argv[i])=="-Sx"){
            i++;
            Sx=atof(argv[i]);
        }else if (QString(argv[i])=="-Sy"){
            i++;
            Sy=atof(argv[i]);
        }else if (QString(argv[i])=="-Ox"){
            i++;
            Ox=atof(argv[i]);
        }else if (QString(argv[i])=="-Oy"){
            i++;
            Oy=atof(argv[i]);
        }else if (QString(argv[i])=="-Z"){
            i++;
            Z=atof(argv[i]);
        }else if (QString(argv[i])=="-r"){
            i++;
            resultFile=argv[i];
        }else if(QString(argv[i])=="-l"){
            i++;
            layerNames.push_back(QString(argv[i]));
        }else if(QString(argv[i])=="-p"){
            i++;
            polyFile=argv[i];
        }else if(QString(argv[i])=="-csv"){
            i++;
            csvFile=argv[i];
        }else{
            QString temp=argv[i];
            WARNING("The argument "+temp+" is an unkown argument.");
        }
    }
    if (Sx==0 ||Sy==0){
        ERROR("The x and y spacing are needed to complete the calculation.\n\tSpecify with \"-Sx\" and \"-Sy\".");
        return EXIT_FAILURE;
    }
    if (fileName.isEmpty()){
        ERROR("The rad file name must be specified.\n\tSpecify with \"-f\".");
        return EXIT_FAILURE;
    }
    if (resultFile.isEmpty()){
        ERROR("The result file name must be specified.\n\tSpecify with \"-r\".");
        return EXIT_FAILURE;
    }
    //Instantiate GridMaker Object
    stadic::GridMaker grid;
    for (int i=0;i<layerNames.size();i++){
        grid.setLayerNames(layerNames.at(i));
    }
    grid.setSpaceX(Sx);
    grid.setSpaceY(Sy);
    grid.setOffsetX(Ox);
    grid.setOffsetY(Oy);
    grid.setZHeight(Z);
    if (!grid.parseRad(fileName)){
        ERROR("The parsing of the rad file failed.");
        return EXIT_FAILURE;
    }
    if (!grid.makeGrid(fileName)){
        ERROR(QString("The creation of the grid failed."));
        EXIT_FAILURE;
    }
    if (!grid.writePTS(resultFile)){
        ERROR("The writing of the points file failed.");
        return EXIT_FAILURE;
    }
    if (!polyFile.isEmpty()){
        if (!grid.writeRadPoly(polyFile)){
            return EXIT_FAILURE;
        }
    }
    if (!csvFile.isEmpty()){
        if (!grid.writePTScsv(csvFile)){
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
