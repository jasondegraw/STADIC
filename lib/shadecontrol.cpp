#include "shadecontrol.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

ShadeControl::ShadeControl(QObject *parent) :
    QObject(parent)
{
}


//Setters
bool ShadeControl::setMethod(QString method){
    if (method=="automated_profile_angle" || method=="automated_signal" || method=="automated_profile_angle_signal"){
        m_Method=method;
    }else{
        std::cerr<<"ERROR: The shade control method is not an approved name."<<std::endl;
        std::cerr<<"\tThe name must be one of the following:"<<std::endl;
        std::cerr<<"\t\t\"automated_profile_angle\", \"automated_signal\", \"automated_profile_angle_signal\""<<std::endl;
        return false;
    }
    return true;
}
bool ShadeControl::setElevationAzimuth(double value){
    if (value<360 && value>-360){
        m_ElevationAzimuth=value;
    }else{
        std::cerr<<"ERROR: The shade control elevation azimuth has to be between -360 and 360."<<std::endl;
        return false;
    }
    return true;
}
bool ShadeControl::setAngleSettings(double value){
    if (value>0){
        m_AngleSettings.push_back(value);
    }else{
        std::cerr<<"ERROR: The shade control angle setting has to be greater than 0."<<std::endl;
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
        std::cerr<<"ERROR: The key \"spin_ccw\" within location must be between -360 and 360."<<std::endl;
        return false;
    }
    m_Location.push_back(spin);
    return true;
}
bool ShadeControl::setSignalSettings(double value){
    if (value>0){
        m_SignalSettings.push_back(value);
    }else{
        std::cerr<<"ERROR: The shade control angle setting has to be greater than 0."<<std::endl;
        return false;
    }
    return true;
}


//Getters
QString ShadeControl::controlMethod(){
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


bool ShadeControl::parseJson(const QJsonObject &object){
    QJsonValue val=object.value(QString("method"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"method\" within shade_control does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if(val.isString()){
            if (!setMethod(val.toString())){
                return false;
            }else{
                if (controlMethod()=="automated_profile_angle"){
                    val=object.value(QString("elevation_azimuth"));
                    if (val.isUndefined()){
                        std::cerr<<"ERROR: The key \"elevation_azimuth\" was not found with control method \"automated_profile_angle\""<<std::endl;
                        return false;
                    }else{
                        if (val.isDouble()){
                            if (!setElevationAzimuth(val.toDouble())){
                                return false;
                            }
                        }else{
                            std::cerr<<"ERROR: The key \"elevation_azimuth\" does not contain a number."<<std::endl;
                            return false;
                        }
                    }

                    val=object.value("angle_settings");
                    if (val.isUndefined()){
                        std::cerr<<"ERROR: The key \"angle_settings\" was not found with control method \"automated_profile_angle\""<<std::endl;
                        return false;
                    }else{
                        if (val.isArray()){
                            QJsonArray array=val.toArray();
                            for (int i=0;i<array.size();i++){
                                if (array[i].isDouble()){
                                    if (!setAngleSettings(array[i].toDouble())){
                                        return false;
                                    }
                                }else{
                                    std::cerr<<"ERROR: The key \"angle_settings\" does not contain a number."<<std::endl;
                                    return false;
                                }
                            }
                        }else{
                            std::cerr<<"ERROR: The key \"angle_settings\" is not an array."<<std::endl;
                            return false;
                        }
                    }
                }else if (controlMethod()=="automated_signal"){
                    val=object.value("sensor");
                    if (val.isUndefined()){
                        std::cerr<<"ERROR: The key \"sensor\" was not found witin \"shade_control\""<<std::endl;
                        return false;
                    }else{
                        if (val.isObject()){
                            QJsonObject object1=val.toObject();
                            val=object1.value("location");
                            if (val.isUndefined()){
                                std::cerr<<"ERROR: The key \"location\" was not found with control method\"automated_signal\"."<<std::endl;
                                return false;
                            }else{
                                if (val.isObject()){
                                    QJsonObject tempObject=val.toObject();
                                    double x, y, z, xd, yd, zd, spin;
                                    val=tempObject.value("x");
                                    if (val.isDouble()){
                                        x=val.toDouble();
                                    }else{
                                        std::cerr<<"ERROR: The key \"x\" was not found with \"location\"."<<std::endl;
                                        return false;
                                    }
                                    val=tempObject.value("y");
                                    if (val.isDouble()){
                                        y=val.toDouble();
                                    }else{
                                        std::cerr<<"ERROR: The key \"y\" was not found with \"location\"."<<std::endl;
                                        return false;
                                    }
                                    val=tempObject.value("z");
                                    if (val.isDouble()){
                                        z=val.toDouble();
                                    }else{
                                        std::cerr<<"ERROR: The key \"z\" was not found with \"location\"."<<std::endl;
                                        return false;
                                    }
                                    val=tempObject.value("xd");
                                    if (val.isDouble()){
                                        xd=val.toDouble();
                                    }else{
                                        std::cerr<<"ERROR: The key \"xd\" was not found with \"location\"."<<std::endl;
                                        return false;
                                    }
                                    val=tempObject.value("yd");
                                    if (val.isDouble()){
                                        yd=val.toDouble();
                                    }else{
                                        std::cerr<<"ERROR: The key \"yd\" was not found with \"location\"."<<std::endl;
                                        return false;
                                    }
                                    val=tempObject.value("zd");
                                    if (val.isDouble()){
                                        zd=val.toDouble();
                                    }else{
                                        std::cerr<<"ERROR: The key \"zd\" was not found with \"location\"."<<std::endl;
                                        return false;
                                    }
                                    val=tempObject.value("spin_ccw");
                                    if (val.isDouble()){
                                        spin=val.toDouble();
                                    }else{
                                        std::cerr<<"ERROR: The key \"spin\" was not found with \"location\"."<<std::endl;
                                        return false;
                                    }
                                    if (!setLocation(x, y, z, xd, yd, zd, spin)){
                                        return false;
                                    }
                                }else{
                                    std::cerr<<"ERROR: The key\"location\" within \"sensor\" is not an object."<<std::endl;
                                    return false;
                                }
                            }
                        }else{
                            std::cerr<<"ERROR: The key \"sensor\" is not an object."<<std::endl;
                            return false;
                        }
                    }

                    val=object.value("signal_settings");
                    if (val.isUndefined()){
                        std::cerr<<"ERROR: The key \"signal_settings\" does not appear within \"control_method\"."<<std::endl;
                        return false;
                    }else{
                        if (val.isArray()){
                            QJsonArray array=val.toArray();
                            for (unsigned int j=0;j<array.size();j++){
                                if (array[j].isDouble()){
                                    if (!setSignalSettings(array[j].toDouble())){
                                        return false;
                                    }
                                }else{
                                    std::cerr<<"ERROR: The key\"signal_settings\" does not contain an array of numbers."<<std::endl;
                                    return false;
                                }
                            }
                        }else{
                            std::cerr<<"ERROR: The key \"signal_settings\" does not contain an array."<<std::endl;
                            return false;
                        }
                    }

                }else if (controlMethod()=="automated_profile_angle_signal"){

                }



            }
        }else{
            std::cerr<<"ERROR: The key \"method\" within window_groups is not a string."<<std::endl;
            return false;
        }
    }







    return true;
}
