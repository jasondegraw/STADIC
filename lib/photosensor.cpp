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

#include "photosensor.h"
#include "logging.h"
#include <fstream>
#include <cmath>
#include <iostream>

namespace stadic {

Photosensor::Photosensor()
{
}
bool Photosensor::setType(std::string type){
    if (type=="cosine"){
        m_Type=type;
        return true;
    }else if (type=="cosine_squared"){
        m_Type=type;
        return true;
    }
    STADIC_LOG(Severity::Error, "The photosensor type of "+type+" is not recognized.");
    return false;
}

//getters
std::vector<double> Photosensor::getVup(double xd, double yd, double zd, double spin_ccw){
    double PI=3.1415926535897932;
    std::vector<double> vUp;
    if (xd==0 && yd==0){
        //vUp rotates about the z axis only starting pointing east with 0 rotation
        //rotation is done around nadir of the photosensor using the right hand rule
        if (spin_ccw==0||spin_ccw==360||spin_ccw==-360){
            vUp.push_back(1);
            vUp.push_back(0);
            vUp.push_back(0);
        }else if (spin_ccw==90 ||spin_ccw==-270){
            vUp.push_back(0);
            vUp.push_back(1);
            vUp.push_back(0);
        }else if (spin_ccw==180 || spin_ccw==-180){
            vUp.push_back(-1);
            vUp.push_back(0);
            vUp.push_back(0);
        }else if (spin_ccw==270 || spin_ccw==-90){
            vUp.push_back(0);
            vUp.push_back(-1);
            vUp.push_back(0);
        }else{
            double xTemp;
            double yTemp;
            double zTemp;
            xTemp=cos(spin_ccw*PI/180);
            yTemp=sin(spin_ccw*PI/180);
            zTemp=0;
            if ((spin_ccw>0 && spin_ccw<90) || (spin_ccw>-360 && spin_ccw<-270)){
                //First quadrant (x is positive, y is positive)
                //Nothing needs to happen
            }else if ((spin_ccw>90 && spin_ccw<180)|| (spin_ccw>-270&&spin_ccw<-180)){
                //Second quadrant (x is negative, y is positive)
                xTemp=xTemp*-1;
            }else if ((spin_ccw>180 && spin_ccw<270)||(spin_ccw>-180&&spin_ccw<-90)){
                //Third quadrant (x is negative, y is negative)
                xTemp=xTemp*-1;
                yTemp=yTemp*-1;
            }else if ((spin_ccw>270 && spin_ccw<360)||(spin_ccw>-90 &&spin_ccw<0)){
                //Fourth quadrant (x is positive, y is negative)
                yTemp=yTemp*-1;
            }else{
                STADIC_LOG(Severity::Error, "The spin angle for the photosensor is outside the range of -360 to 360.  The vector will be set to 1,0,0.");
                vUp.push_back(1);
                vUp.push_back(0);
                vUp.push_back(0);
            }
            vUp.push_back(xTemp);
            vUp.push_back(yTemp);
            vUp.push_back(zTemp);
        }
    }else{
        //vUp starts out pointing vertically from 90 degrees from Nadir.
        double xRotation;
        double yRotation;
        //#1 determine rotation to get nadir vector aligned with -z
        double length;
        //yRotation=
        //xRotation=atan2(yd,zd);

        //#2 rotate about nadir

        //#3 reverse rotation from step #1

    }

    return vUp;
}


//utilities
bool Photosensor::writeSensorFile(std::string file){
    std::ofstream sensorOut;
    sensorOut.open(file);
    if (!sensorOut.is_open()){
        STADIC_LOG(Severity::Warning, "The opening of the file "+file+" for writing the photosensor sensitivity has failed.");
        return false;
    }
    if (!writeSensorFile(sensorOut)){
        STADIC_LOG(Severity::Error, "The writing of the file "+file+" has failed.");
        return false;
    }
    sensorOut.close();
    return true;
}
bool Photosensor::writeSensorFile(){
    return writeSensorFile(std::cout);
}

//private
bool Photosensor::writeCosine(std::ostream& out){
    double PI=3.1415926535897932;
    out<<"degrees\t0\t90\t180\t270"<<std::endl;
    double sensitivity;
    for (int i=0;i<=90;i++){
        sensitivity=std::cos((i)*PI/180);
        out<<i<<"\t"<<sensitivity<<"\t"<<sensitivity<<"\t"<<sensitivity<<"\t"<<sensitivity<<std::endl;
    }

    return true;
}

bool Photosensor::writeCosineSquared(std::ostream& out){
    double PI=3.1415926535897932;
    out<<"degrees\t0\t90\t180\t270"<<std::endl;
    double sensitivity;
    for (int i=0;i<=90;i++){
        sensitivity=std::cos((i)*PI/180)*std::cos((i)*PI/180);
        out<<i<<"\t"<<sensitivity<<"\t"<<sensitivity<<"\t"<<sensitivity<<"\t"<<sensitivity<<std::endl;
    }
    return true;
}
bool Photosensor::writeSensorFile(std::ostream& out){
    if (m_Type=="cosine"){
        if (!writeCosine(out)){
            return false;
        }
    }else if (m_Type=="cosine_squared"){
        if (!writeCosineSquared(out)){
            return false;
        }
    }
    return true;
}
}
