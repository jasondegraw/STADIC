#include "windowgroup.h"
#include "shadecontrol.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <vector>
#include <iostream>

#include "logging.h"

namespace stadic {

WindowGroup::WindowGroup() 
{
}

//  Setters
void WindowGroup::setName(const std::string &name){
    m_name = name;
}
void WindowGroup::setBSDF(bool isBSDF){
    m_BSDF=isBSDF;
}
void WindowGroup::setBaseGeometry(const std::string &file){
    m_BaseGeometry=file;
}

//  Getters
std::string WindowGroup::name() const
{
    return m_name;
}
bool WindowGroup::isBSDF() const
{
    return m_BSDF;
}
std::string WindowGroup::baseGeometry() const
{
    return m_BaseGeometry;
}
std::vector<std::string> WindowGroup::bsdfBaseLayers() const
{
    return m_BSDFBaseLayers;
}
std::vector<std::string> WindowGroup::glazingLayers() const
{
    return m_GlazingLayers;
}
std::vector<std::string> WindowGroup::shadeSettingGeometry() const
{
    return m_ShadeSettingGeometry;
}
std::vector<std::vector<std::string> > WindowGroup::bsdfSettingLayers() const
{
    return m_BSDFSettingLayers;
}
ShadeControl* WindowGroup::shadeControl()
{
    return &m_ShadeControl;
}


bool WindowGroup::parseJson(const QJsonObject &object){
    QJsonValue val=object.value(QString("name"));
    if (val.isUndefined()){
        STADIC_ERROR("The key \"name\" within window_groups does not appear in the STADIC Control File.");
        return false;
    }else{
        if(val.isString()){
            setName(val.toString().toStdString());
        }else{
            STADIC_ERROR("The key \"name\" within window_groups is not a string.");
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
            STADIC_ERROR("The key \"BSDF\" within window group " + name() + " is not a bool.");
            return false;
        }
    }

    val=object.value(QString("base_geometry"));
    if (val.isUndefined()){
        setBaseGeometry("emptry.rad");
    }else{
        if (val.isString()){
            setBaseGeometry(val.toString().toStdString());
        }else{
            STADIC_ERROR("The key \"base_geometry\" within window group " + name() + " is not a bool.");
            return false;
        }
    }

    val=object.value(QString("bsdf_base_layers"));
    if (!val.isUndefined()){
        if (val.isArray()){
            QJsonArray array=val.toArray();
            for (int i=0;i<array.size();i++){
                if (array[i].isString()){
                    m_BSDFBaseLayers.push_back(array[i].toString().toStdString());
                }else{
                    STADIC_ERROR("The key \"bsdf_base_layers\" within window group " + name() + " does not contain a string.");
                    return false;
                }
            }
        }else{
            STADIC_ERROR("The key \"bsdf_base_layers\" within window group " + name() + " is not an array.");
            return false;
        }
    }

    val=object.value(QString("glazing_layers"));
    if (!val.isUndefined()){
        if (val.isArray()){
            QJsonArray array=val.toArray();
            for (int i=0;i<array.size();i++){
                if (array[i].isString()){
                    m_GlazingLayers.push_back(array[i].toString().toStdString());
                }else{
                    STADIC_ERROR("The key \"glazing_layers\" withinwindow group " + name() + " does not contain a string.");
                    return false;
                }
            }
        }else{
            STADIC_ERROR("The key \"glazing_layers\" within window group " + name() + " is not an array.");
            return false;
        }
    }

    val=object.value(QString("shade_control"));
    if (!val.isUndefined()){
        if (val.isObject()){
            if (!m_ShadeControl.parseJson(val.toObject())){
                return false;
            }
        }else{
            STADIC_ERROR("The key \"shade_control\" within window group " + name() + " is not an object.");
            return false;
        }
    }else{
        STADIC_WARNING("There are no \"shade_controls\" listed for window group " + name());
    }

    val=object.value(QString("shade_settings"));
    if (!val.isUndefined()){
        if (val.isArray()){
            QJsonArray array=val.toArray();
            for (int i=0;i<array.size();i++){
                if (array[i].isString()){
                    m_ShadeSettingGeometry.push_back(array[i].toString().toStdString());
                }else{
                    STADIC_ERROR("The key \"shade_settings\" within window group " + name() + " does not contain a string.");
                    return false;
                }
            }
        }else{
            STADIC_ERROR("The key \"shade_settings\" within window group " + name() + " is not an array.");
            return false;
        }
    }else{
        STADIC_WARNING("There are no \"shade_settings\" listed for window group " + name());
    }

    val=object.value(QString("bsdf_setting_layers"));
    if (!val.isUndefined()){
        if(val.isArray()){
            QJsonArray tempArray=val.toArray();
            for (unsigned int j=0;j<tempArray.size();j++){
                if (val.isArray()){
                    QJsonArray array=tempArray.at(j).toArray();
                    std::vector<std::string> tempVector;
                    for (int i=0;i<array.size();i++){
                        if (array[i].isString()){
                            tempVector.push_back(array[i].toString().toStdString());
                        }else{
                            STADIC_ERROR("The key \"bsdf_setting_layers\" within window group " + name() + " does not contain a string.");
                            return false;
                        }
                    }
                    m_BSDFSettingLayers.push_back(tempVector);
                }else{
                    STADIC_ERROR("The key \"bsdf_setting_layers\" within window group " + name() + " is not an array.");
                    return false;
                }
            }
        }else{
            STADIC_ERROR("The key \"bsdf_setting_layers\" within window group " + name() + " is not an array.");
        }
    }


    return true;
}

bool WindowGroup::parseJson(const boost::property_tree::ptree json){

    return true;
}

}
