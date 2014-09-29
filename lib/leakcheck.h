#ifndef LEAKCHECK_H
#define LEAKCHECK_H

#include <vector>
#include <string>
#include "stadicapi.h"



namespace stadic{

class STADIC_API LeakCheck
{
public:
    explicit LeakCheck();
    bool isEnclosed();                                                  //Main function call to determine if the model is enclosed


//Setters
    bool setRadFile(std::vector<std::string> files);                    //Function to set the rad files to be used for leak checking
    void setFloorLayers(std::vector<std::string> layers);               //Function to set the floor layers to ensure the point is within the polygons
    bool setUnits(int unit);
    bool setReflectance(int ref);                                       //Function to set the reflectance which must be either 1 or 0

private:
    std::vector<std::string> m_RadFiles;                                //Vector holding the rad files
    std::vector<std::string> m_FloorLayers;                             //Vector holding the floor layers
    std::vector<std::vector<std::vector<double>>> m_Points;             //Vector holding the position of each of the analysis points
    int m_Units;                                                        //Variable holding the unit type 0=in, 1=ft, 2=mm, 3=m
    double m_Reflectance;                                               //Variable holding the reflectance

    bool makeGrid();                                                    //Function to make the analysis grid using GridMaker
    bool writeExtraRad();                                               //Function to write the sky and ground rad files
    bool xformModifiers();                                              //Function to xform the layers to black if necessary
    bool createOct();                                                   //Function to create the octree
    bool runCalc();                                                     //Function to run the calculation
};

}

#endif // LEAKCHECK_H
