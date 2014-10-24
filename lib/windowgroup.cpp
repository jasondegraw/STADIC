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
 *    and the following disclaimer
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University
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
