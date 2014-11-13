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

#include "shadecontrol.h"
#include "logging.h"
#include <iostream>
#include "functions.h"
#include "jsonobjects.h"
#include <boost/optional.hpp>

namespace stadic {

ShadeControl::ShadeControl()
{
}


//Setters
bool ShadeControl::setMethod(std::string method)
{
    if (method=="automated_profile_angle" || method=="automated_signal" || method=="automated_profile_angle_signal"){
        m_Method=method;
    }else{
        STADIC_ERROR(std::string("The shade control method is not an approved name.")+"\n"
            + "\tThe name must be one of the following:"+"\n"
            + "\t\t\"automated_profile_angle\", \"automated_signal\", \"automated_profile_angle_signal\"");
        return false;
    }
    return true;
}

bool ShadeControl::setElevationAzimuth(double value){
    if (value<360 && value>-360){
        m_ElevationAzimuth=value;
    }else{
        STADIC_ERROR("The shade control elevation azimuth has to be between -360 and 360.");
        return false;
    }
    return true;
}

bool ShadeControl::setAngleSettings(double value){
    if (value>0){
        m_AngleSettings.push_back(value);
    }else{
        STADIC_ERROR("The shade control angle setting has to be greater than 0.");
        return false;
    }
    return true;
}

bool ShadeControl::setLocation(double x, double y, double z, double xd, double yd, double zd, double spin){
    m_Location.push_back(x);
    m_Location.push_back(y);
    m_Location.push_back(z);
    m_Location.push_back(xd);
    m_Location.push_back(yd);
    m_Location.push_back(zd);
    if (spin>360 || spin<-360){
        STADIC_ERROR("The key \"spin_ccw\" within location must be between -360 and 360.");
        return false;
    }
    m_Location.push_back(spin);
    return true;
}

bool ShadeControl::setSignalSettings(double value){
    if (value>0){
        m_SignalSettings.push_back(value);
    }else{
        STADIC_ERROR("The shade control angle setting has to be greater than 0.");
        return false;
    }
    return true;
}

bool ShadeControl::setSensorType(std::string type){
    if (type=="sensitivity_file"){
        m_SensorType=type;
    }else{
        STADIC_ERROR(std::string("The key \"sensor_type\" does not contain an acceptable type.")+"\n"
            + "\tThe acceptable type is as follows: \"sensitivity_file\"");
        return false;
    }
    return true;
}
void ShadeControl::setSensorFile(std::string file){
    m_SensorFile=file;
}


//Getters
std::string ShadeControl::controlMethod(){
    return m_Method;
}
double ShadeControl::elevationAzimuth(){
    return m_ElevationAzimuth;
}
std::vector<double> ShadeControl::angleSettings(){
    return m_AngleSettings;
}
std::vector<double> ShadeControl::location(){
    return m_Location;
}
std::vector<double> ShadeControl::signalSettings(){
    return m_SignalSettings;
}
std::string ShadeControl::sensorType(){
    return m_SensorType;
}
std::string ShadeControl::sensorFile(){
    return m_SensorFile;
}

bool ShadeControl::readAutoProf(const JsonObject &json, std::string method){
    boost::optional<JsonObject> treeVal;
    boost::optional<double> dVal;
    dVal=getDouble(json, "elevation_azimuth", "The key \"elevation_azimuth\" was not found with control method \""+method+"\".","The key \"elevation_azimuth\" does not contain a number.", Severity::Error);
    if (!dVal){
        return false;
    }else{
        if (!setElevationAzimuth(dVal.get())){
            return false;
        }
    }
    treeVal=getObject(json,"angle_settings", "The key \"angle_settings\" was not found with control method \""+method+"\".", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        for(auto &v : treeVal.get()){
            dVal = asDouble(v.second, "There was a problem reading the angle_settings key.", Severity::Fatal);
            if (dVal){
                if (!setAngleSettings(dVal.get())){
                    return false;
                }
            }
        }
    }
    return true;
}

bool ShadeControl::readAutoSign(const JsonObject &json, std::string method){
    boost::optional<boost::property_tree::ptree> treeVal;
    boost::optional<double> dVal;
    boost::optional<std::string> sVal;
    treeVal=getObject(json, "sensor", "The key \"sensor\" was not found with control method \""+method+"\".", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        sVal=getString(treeVal.get(), "sensor_type", "The key \"sensor_type\" is not found within \"sensor\" in one of the window groups.","The key \"sensor_type\" does not contain a string in one of the window groups.", Severity::Error);
        if (!sVal){
            return false;
        }else{
            if (!setSensorType(sVal.get())){
                return false;
            }
            sVal.reset();
        }

        sVal=getString(treeVal.get(), "sensor_file", "The key \"sensor_file\" is not found within \"sensor\" in one of the window groups.","The key \"sensor_file\" does not contain a string in one of the window groups.", Severity::Error);
        if (!sVal){
            return false;
        }else{
            setSensorFile(sVal.get());
            sVal.reset();
        }
        boost::optional<boost::property_tree::ptree> treeVal2;
        treeVal2=getObject(treeVal.get(), "location", "The key \"location\" was not found with control method \""+method+"\".", Severity::Error);
        if (!treeVal2){
            return false;
        }else{
            double x, y, z, xd, yd, zd, spin;
            dVal=getDouble(treeVal2.get(), "x", "The key \"x\" was not found within \"location\".", "The key \"x\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                x=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(treeVal2.get(), "y", "The key \"y\" was not found within \"location\".", "The key \"y\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                y=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(treeVal2.get(), "z", "The key \"z\" was not found within \"location\".", "The key \"z\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                z=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(treeVal2.get(), "xd", "The key \"xd\" was not found within \"location\".", "The key \"xd\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                xd=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(treeVal2.get(), "yd", "The key \"yd\" was not found within \"location\".", "The key \"yd\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                yd=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(treeVal2.get(), "zd", "The key \"zd\" was not found within \"location\".", "The key \"zd\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                zd=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(treeVal2.get(), "spin_ccw", "The key \"spin_ccw\" was not found within \"location\".", "The key \"spin_ccw\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                spin=dVal.get();
                dVal.reset();
            }
            if (!setLocation(x, y, z, xd, yd, zd, spin)){
                return false;
            }
        }
    }
    treeVal.reset();
    treeVal=getObject(json,"signal_settings", "The key \"signal_settings\" was not found with control method \""+method+"\".", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        for(auto &v : treeVal.get()){
            dVal=getDouble(v.second, "", "", "", Severity::Fatal);
            if (dVal){
                if (!setSignalSettings(dVal.get())){
                    return false;
                }
            }else{
                STADIC_LOG(Severity::Warning, "There was a problem reading the signal_settings key.");
            }
            dVal.reset();
        }
        treeVal.reset();
    }
    return true;
}

bool ShadeControl::parseJson(const JsonObject &json){
    if (json.empty()){
        STADIC_LOG(Severity::Error, "The window group does not contain data.");
        return false;
    }
    boost::optional<std::string> sVal;
    boost::optional<JsonObject> treeVal;
    sVal=getString(json, "method", "The key \"method\" does not appear in the STADIC Control File for shade control.", "The key \"method\" is not a string." ,Severity::Info);
    if (!sVal){
        STADIC_LOG(Severity::Error, "There was a proble with the keyword \"method\".");
        return false;
    }else{
        if (!setMethod(sVal.get())){
            return false;
        }else{
            sVal.reset();
            if (controlMethod()=="automated_profile_angle"){
                if (!readAutoProf(json, "automated_profile_angle")){
                    return false;
                }
            }else if (controlMethod()=="automated_signal"){
                if (!readAutoSign(json, "automated_signal")){
                    return false;
                }
            }else if("automated_profile_angle_signal"){
                if (!readAutoProf(json, "automated_profile_angle_signal")){
                    return false;
                }
                if (!readAutoSign(json, "automated_profile_angle_signal")){
                    return false;
                }
            }
        }
    }
    treeVal.reset();

    return true;
}


}
