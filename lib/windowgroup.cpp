#include "windowgroup.h"
#include "shadecontrol.h"
#include <vector>
#include <iostream>
#include <boost/optional.hpp>
#include "functions.h"
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

bool WindowGroup::parseJson(const boost::property_tree::ptree json){
    if (json.empty()){
        STADIC_LOG(Severity::Error, "The window group does not contain data.");
        return false;
    }
    boost::optional<std::string> sVal;
    boost::optional<bool> bVal;
    boost::optional<boost::property_tree::ptree> treeVal;

    sVal=getString(json, "name", "The key \"name\" within window_groups does not appear in the STADIC Control File.", "The key \"name\" within window_groups is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setName(sVal.get());
        sVal.reset();
    }
    if (json.find("BSDF")!=json.not_found()){
        bVal=getBool(json, "BSDF", "The key \"BSDF\" within window group " +name()+ " is missing.", "The key \"BSDF\" within window group " + name() + " is not a bool.", Severity::Info);
        if (!bVal){
            STADIC_LOG(Severity::Info, "It is assumed there are no BSDFs for window group "+name()+".");
            setBSDF(false);
        }else{
            setBSDF(bVal.get());
            bVal.reset();
        }
    }else{
        setBSDF(false);
    }

    sVal=getString(json, "base_geometry", "The key \"base_geometry\" within window group "+name()+" is missing.", "The key \"base_geometry\" within window group " + name() + " is not a string.", Severity::Info);
    if (!sVal){
        STADIC_LOG(Severity::Info, "It is assumed there are is no base geometry for window group " +name()+".");
        setBaseGeometry("empty.rad");
    }else{
        setBaseGeometry(sVal.get());
        sVal.reset();
    }
    if (isBSDF()){
        treeVal=getTree(json, "bsdf_base_layers", "The key \"bsdf_base_layers\" within window group " + name() + " is missing.", Severity::Info);
        if (!treeVal){
            STADIC_LOG(Severity::Info, "It is assumed that window group "+name()+" does not contain BSDFs in the base case.");
        }else{
            for(boost::property_tree::ptree::value_type &v : treeVal.get()){
                sVal = getString(v.second, "", "", "", Severity::Fatal);
                if (sVal){
                    m_BSDFBaseLayers.push_back(sVal.get());
                }else{
                    STADIC_LOG(Severity::Warning, "There was a problem reading the bsdf_base_layers key for window group "+name()+".");
                }
                sVal.reset();
            }
            treeVal.reset();
        }
    }

    treeVal=getTree(json, "glazing_layers", "The key \"glazing_layers\" within window group " + name() + " is missing.\n\tThese layers must be defined for the program to run.", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        for(boost::property_tree::ptree::value_type &v : treeVal.get()){
            sVal=getString(v.second, "", "", "", Severity::Fatal);
            if (sVal){
                m_GlazingLayers.push_back(sVal.get());
            }else{
                STADIC_LOG(Severity::Warning, "There was a problem reading the glazing_layers key for window group "+name()+".");
            }
            sVal.reset();
        }
        treeVal.reset();
    }

    treeVal=getTree(json, "shade_settings", "The key \"shade_settings\" within window group " + name() + " is missing.", Severity::Warning);
    if (!treeVal){
        STADIC_LOG(Severity::Info, "It is assumed there are no shade settings for window group "+name()+".");
    }else{
        for(boost::property_tree::ptree::value_type &v : treeVal.get()){
            sVal=getString(v.second, "", "", "", Severity::Fatal);
            if (sVal){
                m_ShadeSettingGeometry.push_back(sVal.get());
            }else{
                STADIC_LOG(Severity::Warning, "There was a problem reading the shade_settings key for window group "+name()+".");
            }
            sVal.reset();
        }
        treeVal.reset();
    }

    if (shadeSettingGeometry().size()>0){
        treeVal=getTree(json, "shade_control", "The key \"shade_control\" within window group " + name() + " is missing.", Severity::Warning);
        if (!treeVal){
            STADIC_LOG(Severity::Info, "It is assumed there is no shade control needed for windows within window group "+name()+".");
        }else{
            if (!m_ShadeControl.parseJson(treeVal.get())){
                return false;
            }
            treeVal.reset();
        }
    }

    if (isBSDF() && shadeSettingGeometry().size()>0){
        treeVal=getTree(json, "bsdf_setting_layers", "The key \"bsdf_setting_layers\" within window group " + name() + " is missing.", Severity::Info);
        if (!treeVal){
            STADIC_LOG(Severity::Info, "It is assumed that window group "+name()+" does not contain BSDFs in the setting layers.");
        }else{
            for(boost::property_tree::ptree::value_type &v : treeVal.get()){
                std::vector<std::string> tempVector;
                for (boost::property_tree::ptree::value_type &v2 : v.second){
                    sVal=getString(v2.second, "", "", "", Severity::Fatal);
                    if (sVal){
                        tempVector.push_back(sVal.get());
                    }else{
                        STADIC_LOG(Severity::Warning, "There was a problem reading the bsdf_setting_layers key for window group "+name()+".");
                    }
                    sVal.reset();
                }
                m_BSDFSettingLayers.push_back(tempVector);
            }
            treeVal.reset();
        }
    }
    return true;
}

}
