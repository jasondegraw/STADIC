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

#include "controlzone.h"
#include <iostream>
#include "functions.h"
#include <boost/optional.hpp>
#include "logging.h"

namespace stadic {

ControlZone::ControlZone()
{
}

//Setters
void ControlZone::setName(const std::string &name)
{
    m_Name = name;
}

bool ControlZone::setOptimumMethod(const std::string &method)
{
    if (method=="on" || method=="off" || method=="dim_to_min" || method=="dim_to_off" || method=="switched"){
        m_OptimumMethod=method;
    }else{
        STADIC_ERROR("The \"optimum_control\" is not an acceptable string.\n\tThe following values are allowed: \"on\", \"off\", \"dim_to_min\", \"dim_to_off\", \"switched\".");
        return false;
    }
    return true;
}

bool ControlZone::setCPMethod(const std::string &method)
{
    if (method=="auto" || method=="user"){
        m_CPMethod=method;
    }else{
        STADIC_ERROR("The \"cp_method\" is not an acceptable string.\n\tThe following values are allowed: \"auto\" or \"user\"");
        return false;
    }
    return true;
}

void ControlZone::setNumCPs(int value)
{
    m_NumCPs=value;
}

void ControlZone::setTargetPercentage(double value)
{
    if (value>=0 && value<1){
        m_TargetPercentage=value;
    }else{
        STADIC_ERROR("The \"target_percentage\" must be between 0 and 1.  A value of 0.10 will be assumed.");
        m_TargetPercentage=0.1;
    }
}

void ControlZone::setExcludedPoints(const std::string &file)
{
    m_ExcludedPoints=file;
}

bool ControlZone::setCriticalPoints(int point)
{
    if (point>0){
        m_CriticalPoints.push_back(point);
    }else{
        STADIC_ERROR("The \"points\" listing for critical points must be greater than 0.");
        return false;
    }
    return true;
}

void ControlZone::setMaximumBFSignal(double value)
{
    m_MaximumBFSignal=value;
}

void ControlZone::setMinimumBFSignal(double value)
{
    m_MinimumBFSignal=value;
}

void ControlZone::setOffSignal(double value)
{
    m_OffSignal=value;
}

void ControlZone::setOnSignal(double value)
{
    m_OnSignal=value;
}

void ControlZone::setSetpointSignal(double value)
{
    m_SetpointSignal=value;
}

void ControlZone::setSignal(double value)
{
    m_Signal=value;
}

void ControlZone::setDimmingLevel(double value)
{
    m_DimmingLevel=value;
}

void ControlZone::setSignalToBFFile(const std::string &file)
{
    m_SignalToBFFile=file;
}

bool ControlZone::setSensorType(const std::string &type)
{
    if (type=="sensitivity_file"){
        m_SensorType=type;
    }else{
        STADIC_ERROR("The key \"sensor_type\" does not contain an acceptable type.\n\tThe acceptable type is as follows: \"sensitivity_file\"");
        return false;
    }
    return true;
}

void ControlZone::setSensorFile(const std::string &file)
{
    m_SensorFile=file;
}

bool ControlZone::setSensorLocation(double x, double y, double z, double xd, double yd, double zd, double spin)
{
    if (spin>-360 && spin<360){
        m_SensorLocation.clear();
        m_SensorLocation.push_back(x);
        m_SensorLocation.push_back(y);
        m_SensorLocation.push_back(z);
        m_SensorLocation.push_back(xd);
        m_SensorLocation.push_back(yd);
        m_SensorLocation.push_back(zd);
        m_SensorLocation.push_back(spin);
    }else{
        STADIC_ERROR("The key \"spin_ccw\" must contain a value between -360 and 360.");
        return false;
    }
    return true;
}

bool ControlZone::setAlgorithm(const std::string &algorithm)
{
    if (algorithm=="open_dimming" || algorithm=="closed_proportional" || algorithm=="constant_setpoint" || algorithm=="open_switching" || algorithm=="closed_switching" || algorithm=="user_defined_1"){
        m_Algorithm=algorithm;
    }else{
        STADIC_ERROR("The algorithm is not an acceptable string.\n\tThe following values are allowed:\n\t\t\"open_dimming\"\n\t\t\"closed_proportional\"\n\t\t\"constant_setpoint\"\n\t\t\"open_switching\"\n\t\t\"closed_switching\"\n\t\t\"user_defined_1\"");
        return false;
    }
    return true;
}

void ControlZone::setIESFile(const std::string &file)
{
    m_IESFile=file;
}

bool ControlZone::setLLF(double value)
{
    if (value>0){
        m_LLF=value;
    }else{
        STADIC_ERROR("The \"LLF\" must be greater than 0.");
        return false;
    }
    return true;
}

bool ControlZone::setLampLumens(int value)
{
    if (value>0){
        m_LampLumens=value;
    }else{
        STADIC_ERROR("The \"lamp_lumens\" must be greater than 0.");
        return false;
    }
    return true;
}

bool ControlZone::setBallastType(const std::string &type)
{
    if (type=="linear_dimming" || type=="non_dimming"){
        m_BallastType=type;
    }else{
        STADIC_ERROR("The \"ballast_type\" is not an approved type.\n\tThe approved types are as follows: \"linear_dimming\" and \"non_dimming\"");
        return false;
    }
    return true;
}

bool ControlZone::setBFMin(double value)
{
    if (value>=0 && value<3){
        m_BFMin=value;
    }else{
        STADIC_ERROR("The \"bf_min\" must be greater than or equal to 0.");
        return false;
    }
    return true;
}

bool ControlZone::setBFMax(double value)
{
    if (value>=0 && value<3){
        m_BFMax=value;
    }else{
        STADIC_ERROR("The \"bf_max\" must be between 0 and 3.");
        return false;
    }
    return true;
}

bool ControlZone::setWattsMax(double value)
{
    if (value>=0){
        m_WattsMax=value;
    }else{
        STADIC_ERROR("The \"watts_max\" must be greater than 0.");
        return false;
    }
    return true;
}

bool ControlZone::setWattsMin(double value)
{
    if (value>=0){
        m_WattsMin=value;
    }else{
        STADIC_ERROR("The \"watts_min\" must be greater than 0.");
        return false;
    }
    return true;
}

bool ControlZone::setBallastFactor(double value)
{
    if (value>=0 && value<3){
        m_BallastFactor=value;
    }else{
        STADIC_ERROR("The \"ballast_factor\" must be between 0 and 3.");
        return false;
    }
    return true;
}

bool ControlZone::setWatts(double value)
{
    if (value>0){
        m_Watts=value;
    }else{
        STADIC_ERROR("The \"watts\" must be greater than 0.");
        return false;
    }
    return true;
}

bool ControlZone::setLuminaireLayout(double x, double y, double z, double rotation, double tilt, double spin)
{
    std::vector<double> tempVector;
    tempVector.push_back(x);
    tempVector.push_back(y);
    tempVector.push_back(z);
    if (rotation>-360 && rotation<360){
        tempVector.push_back(rotation);
    }else{
        STADIC_ERROR("The key \"rotation\" must be between -360 and 360.");
        return false;
    }
    if (tilt>-360 && tilt<360){
        tempVector.push_back(tilt);
    }else{
        STADIC_ERROR("The key \"tilt\" must be between -360 and 360.");
        return false;
    }
    if (spin>-360 && spin<360){
        tempVector.push_back(spin);
    }else{
        STADIC_ERROR("The key \"spin\" must be between -360 and 360.");
        return false;
    }
    m_LuminaireLayout.push_back(tempVector);
    return true;
}

//Getters
std::string ControlZone::name()
{
    return m_Name;
}

std::string ControlZone::optimumMethod()
{
    return m_OptimumMethod;
}

std::string ControlZone::cpMethod()
{
    return m_CPMethod;
}

int ControlZone::numCPs()
{
    return m_NumCPs;
}

double ControlZone::targetPercentage()
{
    return m_TargetPercentage;
}

std::string ControlZone::excludedPoints()
{
    return m_ExcludedPoints;
}

std::vector<int> ControlZone::criticalPoints()
{
    return m_CriticalPoints;
}

double ControlZone::maximumBFSignal()
{
    return m_MaximumBFSignal;
}

double ControlZone::minimumBFSignal()
{
    return m_MinimumBFSignal;
}

double ControlZone::offSignal(){
    return m_OffSignal;
}

double ControlZone::setpointSignal(){
    return m_SetpointSignal;
}
double ControlZone::onSignal(){
    return m_OnSignal;
}
double ControlZone::signal(){
    return m_Signal;
}
double ControlZone::dimmingLevel(){
    return m_DimmingLevel;
}

std::string ControlZone::signalToBFFile()
{
    return m_SignalToBFFile;
}

std::string ControlZone::sensorType()
{
    return m_SensorType;
}

std::string ControlZone::sensorFile()
{
    return m_SensorFile;
}

std::vector<double> ControlZone::sensorLocation()
{
    return m_SensorLocation;
}

std::string ControlZone::algorithm()
{
    if (m_Algorithm.empty()){
        m_Algorithm="null";
    }
    return m_Algorithm;
}

std::string ControlZone::iesFile()
{
    return m_IESFile;
}

double ControlZone::llf()
{
    return m_LLF;
}

int ControlZone::lampLumens()
{
    return m_LampLumens;
}

std::string ControlZone::ballastType()
{
    return m_BallastType;
}

double ControlZone::bfMin()
{
    return m_BFMin;
}

double ControlZone::bfMax()
{
    return m_BFMax;
}

double ControlZone::wattsMax()
{
    return m_WattsMax;
}

double ControlZone::wattsMin()
{
    return m_WattsMin;
}

double ControlZone::ballastFactor()
{
    return m_BallastFactor;
}

double ControlZone::watts()
{
    return m_Watts;
}

std::vector<std::vector<double> > ControlZone::luminaireLayout()
{
    return m_LuminaireLayout;
}

bool ControlZone::parseJson (const JsonObject &json){
    boost::optional<JsonObject> treeVal;
    boost::optional<double> dVal;
    boost::optional<std::string> sVal;
    boost::optional<int> iVal;
    boost::optional<bool> bVal;

    sVal=getString(json, "name", "The key \"name\" within control_zones does not appear in the STADIC Control File.", "The key \"name\" within control_zones is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setName(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "optimum_control", "The key\"optimum_control\" is not found within the control zone "+name()+".", "The key \"optimum_control\" does not contain a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        if (!setOptimumMethod(sVal.get())){
            return false;
        }
        sVal.reset();
    }

    treeVal = getObject(json, "sensor");
    if (treeVal) {
        sVal = getString(treeVal.get(), "sensor_type", "The key \"sensor_type\" is not found within \"sensor\" in the control zone named "+name()+".", "The key \"sensor_type\" does not contain a string in one of the window groups.", Severity::Error);
        if(!sVal){
            return false;
        } else{
            if(!setSensorType(sVal.get())){
                return false;
            }
            sVal.reset();
        }

        sVal = getString(treeVal.get(), "sensor_file", "The key \"sensor_file\" is not found within \"sensor\" in one of the window groups.", "The key \"sensor_file\" does not contain a string in one of the window groups.", Severity::Error);
        if(!sVal){
            return false;
        } else{
            setSensorFile(sVal.get());
            sVal.reset();
        }
        boost::optional<JsonObject> treeVal2;
        treeVal2 = getObject(treeVal.get(), "location", "The key \"location\" was not found with control zone named "+name()+".", Severity::Error);
        if(!treeVal2){
            return false;
        } else{
            double x, y, z, xd, yd, zd, spin;
            dVal = getDouble(treeVal2.get(), "x", "The key \"x\" was not found within \"location\".", "The key \"x\" is not a double.", Severity::Error);
            if(!dVal){
                return false;
            } else{
                x = dVal.get();
                dVal.reset();
            }
            dVal = getDouble(treeVal2.get(), "y", "The key \"y\" was not found within \"location\".", "The key \"y\" is not a double.", Severity::Error);
            if(!dVal){
                return false;
            } else{
                y = dVal.get();
                dVal.reset();
            }
            dVal = getDouble(treeVal2.get(), "z", "The key \"z\" was not found within \"location\".", "The key \"z\" is not a double.", Severity::Error);
            if(!dVal){
                return false;
            } else{
                z = dVal.get();
                dVal.reset();
            }
            dVal = getDouble(treeVal2.get(), "xd", "The key \"xd\" was not found within \"location\".", "The key \"xd\" is not a double.", Severity::Error);
            if(!dVal){
                return false;
            } else{
                xd = dVal.get();
                dVal.reset();
            }
            dVal = getDouble(treeVal2.get(), "yd", "The key \"yd\" was not found within \"location\".", "The key \"yd\" is not a double.", Severity::Error);
            if(!dVal){
                return false;
            } else{
                yd = dVal.get();
                dVal.reset();
            }
            dVal = getDouble(treeVal2.get(), "zd", "The key \"zd\" was not found within \"location\".", "The key \"zd\" is not a double.", Severity::Error);
            if(!dVal){
                return false;
            } else{
                zd = dVal.get();
                dVal.reset();
            }
            dVal = getDouble(treeVal2.get(), "spin_ccw", "The key \"spin_ccw\" was not found within \"location\".", "The key \"spin_ccw\" is not a double.", Severity::Error);
            if(!dVal){
                return false;
            } else{
                spin = dVal.get();
                dVal.reset();
            }
            if(!setSensorLocation(x, y, z, xd, yd, zd, spin)){
                return false;
            }
        }
    }

    sVal = getString(json, "cp_method", "The key \"cp_method\" does not appear in the zone named \""+name()+"\".", "The key \"cp_method\" in the zone named \""+name()+"\" does not contain a string.", Severity::Info);
    if(sVal){
        if(!setCPMethod(sVal.get())){
            return false;
        }
        sVal.reset();
        if(cpMethod()=="auto"){
            iVal = getInt(json, "quantity", "The key \"quantity\" is needed with \"cp_method\" when auto is specified.", "The key \"quantity\" does not contain a number.", Severity::Error);
            if(!iVal){
                return false;
            } else{
                setNumCPs(iVal.get());
                iVal.reset();
            }
            dVal = getDouble(json, "target_percentage", "The key \"target_percentage\" is needed with \"cp_method\" when auto is specified.  A value of 0.10 will be assumed.", "The key \"target_percentage\" does not contain a number.  A value of 0.10 will be assumed.", Severity::Info);
            if(!dVal){
                setTargetPercentage(0.1);
            } else{
                setTargetPercentage(dVal.get());
                dVal.reset();
            }
            sVal = getString(json, "excluded_points", "No excluded points file is being used for the critical point analysis for control zone named \"" + name() + "\".", "The key \"excluded_points\" does not contain a string.", Severity::Info);
            if(sVal){
                setExcludedPoints(sVal.get());
                sVal.reset();
            }
        } else if(cpMethod()=="user"){
            treeVal = getObject(json, "points", "The key \"points\" is needed with \"cp_method\" when user is specified.", Severity::Error);
            if(!treeVal){
                return false;
            } else{
                for(boost::property_tree::ptree::value_type &v : treeVal.get()){
                    iVal = getInt(v.second, "", "", "", Severity::Fatal);
                    if(iVal){
                        if(!setCriticalPoints(iVal.get())){
                            return false;
                        }
                    } else{
                        STADIC_LOG(Severity::Warning, "The key \"points\" has an entry that is not a number.");
                    }
                    iVal.reset();
                }
            }
        }
    }

    treeVal = getObject(json, "open_dimming");
    if (treeVal){
        setAlgorithm("open_dimming");
        dVal = getDouble(treeVal.get(), "maximum_bf_signal", "The key\"maximum_bf_signal\" is needed with the \"open_dimming\" algorithm.", "The key \"maximum_bf_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setMaximumBFSignal(dVal.get());
            dVal.reset();
        }
        dVal = getDouble(treeVal.get(), "minimum_bf_signal", "The key\"minimum_bf_signal\" is needed with the \"open_dimming\" algorithm.", "The key \"minimum_bf_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setMinimumBFSignal(dVal.get());
            dVal.reset();
        }
        dVal = getDouble(treeVal.get(), "off_signal", "The key\"off_signal\" is needed with the \"open_dimming\" algorithm.", "The key \"off_signal\" does not contain a number.", Severity::Info);
        if(!dVal){
            STADIC_LOG(Severity::Info, "The off signal in zone "+name()+" is being set to the minimum ballast factor signal.");
            setOffSignal(minimumBFSignal());
        } else{
            if(dVal.get()>minimumBFSignal()){
                STADIC_LOG(Severity::Info, "The off signal in zone "+name()+" is being set to the minimum ballast factor signal.");
                setOffSignal(minimumBFSignal());
            } else{
                setOffSignal(dVal.get());
            }
            dVal.reset();
        }
    }

    treeVal = getObject(json, "closed_proportional");
    if (treeVal){
        setAlgorithm("closed_proportional");
        dVal = getDouble(treeVal.get(), "maximum_bf_signal", "The key\"maximum_bf_signal\" is needed with the \"open_dimming\" algorithm.", "The key \"maximum_bf_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setMaximumBFSignal(dVal.get());
            dVal.reset();
        }
        dVal = getDouble(treeVal.get(), "minimum_bf_signal", "The key\"minimum_bf_signal\" is needed with the \"open_dimming\" algorithm.", "The key \"minimum_bf_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setMinimumBFSignal(dVal.get());
            dVal.reset();
        }
        dVal = getDouble(treeVal.get(), "off_signal", "The key\"off_signal\" is needed with the \"open_dimming\" algorithm.", "The key \"off_signal\" does not contain a number.", Severity::Info);
        if(!dVal){
            STADIC_LOG(Severity::Info, "The off signal in zone "+name()+" is being set to the minimum ballast factor signal.");
            setOffSignal(minimumBFSignal());
        } else{
            if(dVal.get()>minimumBFSignal()){
                STADIC_LOG(Severity::Info, "The off signal in zone "+name()+" is being set to the minimum ballast factor signal.");
                setOffSignal(minimumBFSignal());
            } else{
                setOffSignal(dVal.get());
            }
            dVal.reset();
        }
    }
    
    treeVal = getObject(json, "constant_setpoint");
    if (json.find("constant_setpoint")!=json.not_found()){
        setAlgorithm("constant_setpoint");
        dVal = getDouble(treeVal.get(), "setpoint_signal", "The key \"setpoint_signal\" is needed with the \"constant_setpoint\" algorithm.", "The key \"setpoint_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setSetpointSignal(dVal.get());
            dVal.reset();
        }
        dVal = getDouble(treeVal.get(), "off_signal", "The key \"off_signal\" is needed with the \"constant_setpoint\" algorithm.", "The key \"off_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setOffSignal(dVal.get());
            dVal.reset();
        }
    }
        
    treeVal = getObject(json, "open_switching");
    if (treeVal){
        setAlgorithm("open_switching");
        dVal = getDouble(treeVal.get(), "off_signal", "The key \"off_signal\" is needed with the \"open_switching\" algorithm.", "The key \"off_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setOffSignal(dVal.get());
            dVal.reset();
        }
        dVal = getDouble(treeVal.get(), "on_signal", "The key \"on_signal\" is needed with the \"open_switching\" algorithm.", "The key \"on_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setOnSignal(dVal.get());
            dVal.reset();
        }
    }

    treeVal = getObject(json, "closed_switching");
    if (treeVal){
        setAlgorithm("closed_switching");
        dVal = getDouble(treeVal.get(), "off_signal", "The key \"off_signal\" is needed with the \"closed_switching\" algorithm.", "The key \"off_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setOffSignal(dVal.get());
            dVal.reset();
        }
        dVal = getDouble(treeVal.get(), "on_signal", "The key \"on_signal\" is needed with the \"closed_switching\" algorithm.", "The key \"on_signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setOnSignal(dVal.get());
            dVal.reset();
        }
    }
        
    treeVal = getObject(json, "user_defined_1");
    if (treeVal){
        setAlgorithm("user_defined_1");
        dVal = getDouble(treeVal.get(), "signal", "The key \"signal\" is needed with the \"user_defined_1\" algorithm.", "The key \"signal\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setSignal(dVal.get());
            dVal.reset();
        }
        dVal = getDouble(treeVal.get(), "dimming_level", "The key \"dimming_level\" is needed with the \"user_defined_1\" algorithm.", "The key \"dimming_level\" does not contain a number.", Severity::Error);
        if(!dVal){
            return false;
        } else{
            setDimmingLevel(dVal.get());
            dVal.reset();
        }
        sVal = getString(treeVal.get(), "signal_to_bf_file", "The key \"signal_to_bf_file\" is needed with the \"user_defined_1\" algorithm.", "The key \"signal_to_bf_file\" does not contain a string.", Severity::Error);
        if(!sVal){
            return false;
        } else{
            setSignalToBFFile(sVal.get());
            sVal.reset();
        }
    }
     
    //After parsing for the algorithm test to see if the algorithm has been set.  Alert user if it hasn't been set
    if (algorithm()=="null"){
        STADIC_LOG(Severity::Warning, "There is no algorithm specified for the control zone named \""+name()+"\".");
    }

    treeVal=getObject(json, "luminaire_information", "The key \"luminaire_information\" is missing for the control zone named "+name()+".", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        sVal=getString(treeVal.get(), "ies_file", "The key \"ies_file\" is needed within \"luminaire_information\".", "The key \"ies_file\" is not a string.", Severity::Error);
        if (!sVal){
            return false;
        }else{
            setIESFile(sVal.get());
            sVal.reset();
        }
        dVal=getDouble(treeVal.get(), "LLF", "The key \"LLF\" is needed within \"luminaire_information\".", "The key \"LLF\" is not a number.", Severity::Error);
        if (!dVal){
            return false;
        }else{
            if (!setLLF(dVal.get())){
                return false;
            }
            dVal.reset();
        }
        dVal=getDouble(treeVal.get(), "lamp_lumens", "The key \"lamp_lumens\" is needed within \"luminaire_information\".",  "The key \"lamp_lumens\" is not a number.", Severity::Error);
        if (!dVal){
            return false;
        }else{
            if (!setLampLumens(dVal.get())){
                return false;
            }
            dVal.reset();
        }
        treeVal.reset();
    }

    treeVal=getObject(json, "ballast_driver_information", "The key \"ballast_driver_information\" is missing for the control zone named "+name()+".", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        sVal=getString(treeVal.get(), "ballast_type", "The key \"ballast_type\" is missing for the control zone named "+name()+".", "The key \"ballast_type\" is not a string.", Severity::Error);
        if (!sVal){
            return false;
        }else{
            if (!setBallastType(sVal.get())){
                return false;
            }
            sVal.reset();
        }
        if (ballastType()=="linear_dimming"){
            dVal=getDouble(treeVal.get(), "bf_min", "The key \"bf_min\" is needed for ballast type \"linear_dimming\".", "The key \"bf_min\" is not a number.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                setBFMin(dVal.get());
                dVal.reset();
            }
            dVal=getDouble(treeVal.get(), "bf_max", "The key \"bf_max\" is needed for ballast type \"linear_dimming\".", "The key \"bf_max\" is not a number.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                setBFMax(dVal.get());
                dVal.reset();
            }
            dVal=getDouble(treeVal.get(), "watts_max", "The key \"watts_max\" is needed for ballast type \"linear_dimming\".", "The key \"watts_max\" is not a number.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                setWattsMax(dVal.get());
                dVal.reset();
            }
            dVal=getDouble(treeVal.get(), "watts_min", "The key \"watts_min\" is needed for ballast type \"linear_dimming\".", "The key \"watts_min\" is not a number.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                setWattsMin(dVal.get());
                dVal.reset();
            }
        }else if (ballastType()=="non_dimming"){
            dVal=getDouble(treeVal.get(), "ballast_factor", "The key \"ballast_factor\" is needed for ballast type \"non_dimming\".", "The key \"ballast_factor\" is not a number.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                setBallastFactor(dVal.get());
                dVal.reset();
            }
            dVal=getDouble(treeVal.get(), "watts", "The key \"watts\" is needed for ballast type \"non_dimming\".", "The key \"watts\" is not a number.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                setWatts(dVal.get());
                dVal.reset();
            }
        }
        treeVal.reset();
    }

    treeVal=getObject(json, "luminaire_layout", "The key \"luminaire_layout\" is needed for a control zone.", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        for(boost::property_tree::ptree::value_type &v : treeVal.get()){
            double x, y, z, rot, tilt, spin;
            dVal=getDouble(v.second, "x", "The key \"x\" must be found within the \"luminaire_layout\" entries.", "The key \"x\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                x=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(v.second, "y", "The key \"y\" was not found within \"luminaire_layout\".", "The key \"y\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                y=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(v.second, "z", "The key \"z\" was not found within \"luminaire_layout\".", "The key \"z\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                z=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(v.second, "rotation", "The key \"rotation\" was not found within \"luminaire_layout\".", "The key \"rotation\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                rot=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(v.second, "tilt", "The key \"tilt\" was not found within \"luminaire_layout\".", "The key \"tilt\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                tilt=dVal.get();
                dVal.reset();
            }
            dVal=getDouble(v.second, "spin_ccw", "The key \"spin_ccw\" was not found within \"location\".", "The key \"spin_ccw\" is not a double.", Severity::Error);
            if (!dVal){
                return false;
            }else{
                spin=dVal.get();
                dVal.reset();
            }
            if(!setLuminaireLayout(x, y, z, rot, tilt, spin)){
                return false;
            }
        }
    }
    treeVal.reset();
    return true;
}

}
