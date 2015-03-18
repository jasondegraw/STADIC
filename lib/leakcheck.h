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
    std::vector<std::vector<std::vector<double> > > m_Points;             //Vector holding the position of each of the analysis points
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
