#include "windowgroup.h"
#include "shadecontrol.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <vector>
#include <iostream>

namespace stadic {

WindowGroup::WindowGroup(QObject *parent) :
    QObject(parent)
{
}

//  Setters
void WindowGroup::setBSDF(bool isBSDF){
    m_BSDF=isBSDF;
}
void WindowGroup::setBaseGeometry(QString file){
    m_BaseGeometry=file;
}

//  Getters
bool WindowGroup::isBSDF(){
    return m_BSDF;
}
QString WindowGroup::baseGeometry(){
    return m_BaseGeometry;
}
std::vector<QString> WindowGroup::bsdfBaseLayers(){
    return m_BSDFBaseLayers;
}
std::vector<QString> WindowGroup::glazingLayers(){
    return m_GlazingLayers;
}
std::vector<QString> WindowGroup::shadeSettingGeometry(){
    return m_ShadeSettingGeometry;
}
std::vector<std::vector<QString> > WindowGroup::bsdfSettingLayers(){
    return m_BSDFSettingLayers;
}
ShadeControl* WindowGroup::shadeControl(){
    return m_ShadeControl;
}


bool WindowGroup::parseJson(const QJsonObject &object){
    QJsonValue val=object.value(QString("name"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"name\" within window_groups does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if(val.isString()){
            setObjectName(val.toString());
        }else{
            std::cerr<<"ERROR: The key \"name\" within window_groups is not a string."<<std::endl;
            return false;
        }
    }

    val=object.value(QString("BSDF"));
    if (val.isUndefined()){
        setBSDF(false);
    }else{
        if (val.isBool()){
            setBSDF(val.toBool());
        }else{
            std::cerr<<"ERROR: The key \"BSDF\" within window group "<<objectName().toStdString()<<" is not a bool."<<std::endl;
            return false;
        }
    }

    val=object.value(QString("base_geometry"));
    if (val.isUndefined()){
        setBaseGeometry(QString("emptry.rad"));
    }else{
        if (val.isString()){
            setBaseGeometry(val.toString());
        }else{
            std::cerr<<"ERROR: The key \"base_geometry\" within window group "<<objectName().toStdString()<<" is not a bool."<<std::endl;
            return false;
        }
    }

    val=object.value(QString("bsdf_base_layers"));
    if (!val.isUndefined()){
        if (val.isArray()){
            QJsonArray array=val.toArray();
            for (int i=0;i<array.size();i++){
                if (array[i].isString()){
                    m_BSDFBaseLayers.push_back(array[i].toString());
                }else{
                    std::cerr<<"ERROR: The key \"bsdf_base_layers\" within window group "<<objectName().toStdString()<<" does not contain a string."<<std::endl;
                    return false;
                }
            }
        }else{
            std::cerr<<"ERROR: The key \"bsdf_base_layers\" within window group "<<objectName().toStdString()<<" is not an array."<<std::endl;
            return false;
        }
    }

    val=object.value(QString("glazing_layers"));
    if (!val.isUndefined()){
        if (val.isArray()){
            QJsonArray array=val.toArray();
            for (int i=0;i<array.size();i++){
                if (array[i].isString()){
                    m_GlazingLayers.push_back(array[i].toString());
                }else{
                    std::cerr<<"ERROR: The key \"glazing_layers\" withinwindow group "<<objectName().toStdString()<<" does not contain a string."<<std::endl;
                    return false;
                }
            }
        }else{
            std::cerr<<"ERROR: The key \"glazing_layers\" within window group "<<objectName().toStdString()<<" is not an array."<<std::endl;
            return false;
        }
    }

    val=object.value(QString("shade_control"));
    if (!val.isUndefined()){
        if (val.isObject()){
            m_ShadeControl=new ShadeControl(this);
            if (!m_ShadeControl->parseJson(val.toObject())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The key \"shade_control\" within window group "<<objectName().toStdString()<<" is not an object."<<std::endl;
            return false;
        }
    }else{
        std::cerr<<"WARNING: There are no \"shade_controls\" listed for window group "<<objectName().toStdString()<<std::endl;
    }

    val=object.value(QString("shade_settings"));
    if (!val.isUndefined()){
        if (val.isArray()){
            QJsonArray array=val.toArray();
            for (int i=0;i<array.size();i++){
                if (array[i].isString()){
                    m_ShadeSettingGeometry.push_back(array[i].toString());
                }else{
                    std::cerr<<"ERROR: The key \"shade_settings\" within window group "<<objectName().toStdString()<<" does not contain a string."<<std::endl;
                    return false;
                }
            }
        }else{
            std::cerr<<"ERROR: The key \"shade_settings\" within window group "<<objectName().toStdString()<<" is not an array."<<std::endl;
            return false;
        }
    }else{
        std::cerr<<"WARNING: There are no \"shade_settings\" listed for window group "<<objectName().toStdString()<<std::endl;
    }

    val=object.value(QString("bsdf_setting_layers"));
    if (!val.isUndefined()){
        if(val.isArray()){
            QJsonArray tempArray=val.toArray();
            for (unsigned int j=0;j<tempArray.size();j++){
                if (val.isArray()){
                    QJsonArray array=tempArray.at(j).toArray();
                    std::vector<QString> tempVector;
                    for (int i=0;i<array.size();i++){
                        if (array[i].isString()){
                            tempVector.push_back(array[i].toString());
                        }else{
                            std::cerr<<"ERROR: The key \"bsdf_setting_layers\" within window group "<<objectName().toStdString()<<" does not contain a string."<<std::endl;
                            return false;
                        }
                    }
                    m_BSDFSettingLayers.push_back(tempVector);
                }else{
                    std::cerr<<"ERROR: The key \"bsdf_setting_layers\" within window group "<<objectName().toStdString()<<" is not an array."<<std::endl;
                    return false;
                }
            }
        }else{
            std::cerr<<"ERROR: The key \"bsdf_setting_layers\" within window group "<<objectName().toStdString()<<" is not an array."<<std::endl;
        }
    }


    return true;
}

}
