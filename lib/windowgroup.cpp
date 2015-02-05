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

#include "windowgroup.h"
#include "shadecontrol.h"
#include <vector>
#include <iostream>
#include <boost/optional.hpp>
#include "functions.h"
#include "jsonobjects.h"
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
void WindowGroup::setBaseCalculate(bool runBase){
    m_RunBase=runBase;
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
std::vector<bool> WindowGroup::runSetting() const
{
    return m_RunSetting;
}
std::vector<std::vector<std::string> > WindowGroup::bsdfSettingLayers() const
{
    return m_BSDFSettingLayers;
}
ShadeControl* WindowGroup::shadeControl()
{
    return &m_ShadeControl;
}

bool WindowGroup::parseJson(const JsonObject &json){
    if (json.empty()){
        STADIC_LOG(Severity::Error, "The window group does not contain data.");
        return false;
    }
    boost::optional<std::string> sVal;
    boost::optional<bool> bVal;
    boost::optional<JsonObject> treeVal;

    sVal=getString(json, "name", "The key \"name\" within window_groups does not appear in the STADIC Control File.", "The key \"name\" within window_groups is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setName(sVal.get());
    }

    bVal = getBool(json, "BSDF", false, "The key \"BSDF\" within window group " + name() + " is not a bool, assuming no BSDFs for window group " + name() + ".", Severity::Info);
    setBSDF(bVal.get());

    sVal=getString(json, "base_geometry", "The key \"base_geometry\" within window group "+name()+" is missing.", "The key \"base_geometry\" within window group " + name() + " is not a string.", Severity::Info);
    if(!sVal) {
        STADIC_LOG(Severity::Info, "It is assumed there are is no base geometry for window group " +name()+".");
        setBaseGeometry("empty.rad");
    } else {
        setBaseGeometry(sVal.get());
    }

    bVal=getBool(json, "calculate_base", "The key \"calculate_base\" within window group " + name() + " is not a bool.  This will not be calculated.", "The key \"calculate_base\" within window group " + name() + " is not a bool.  This will not be calculated.", Severity::Info);
    if (bVal){
        setBaseCalculate(bVal.get());
        bVal.reset();
    }else{
        setBaseCalculate(false);
    }
    if (isBSDF()){
        treeVal=getArray(json, "bsdf_base_materials");
        if(!treeVal) {
            STADIC_LOG(Severity::Info, "It is assumed that window group "+name()+" does not contain BSDFs in the base case.");
        } else {
            for(auto &v : treeVal.get()){
                sVal = asString(v, "There was a problem reading the bsdf_base_materials key for window group "+name()+".", Severity::Fatal);
                if (sVal){
                    m_BSDFBaseLayers.push_back(sVal.get());
                }
            }
        }
        treeVal.reset();
    }

    treeVal=getArray(json, "glazing_materials", "The key \"glazing_materials\" within window group " + name() + " is missing.\n\tThese materials must be defined for the program to run.", Severity::Error);
    if(!treeVal) {
        return false;
    } else {
        for(auto &v : treeVal.get()){
            sVal = asString(v, "There was a problem reading the glazing_materials key for window group "+name()+".", Severity::Fatal);
            if (sVal){
                m_GlazingLayers.push_back(sVal.get());
            }
        }
        treeVal.reset();
    }

    treeVal=getArray(json, "shade_settings", "The key \"shade_settings\" within window group " + name() + " is missing.", Severity::Warning);
    if(!treeVal){
        STADIC_LOG(Severity::Info, "It is assumed there are no shade settings for window group "+name()+".");
    }else{
        for(auto &v : treeVal.get()){
            sVal = asString(v, "There was a problem reading the shade_settings key for window group "+name()+".", Severity::Fatal);
            if (sVal){
                m_ShadeSettingGeometry.push_back(sVal.get());
                sVal.reset();
            }
        }
        treeVal.reset();
    }

    treeVal=getArray(json, "calculate_setting", "The key \"calculate_setting\" within window group "+ name() + " is missing.", Severity::Warning);
    if (!treeVal){
        STADIC_LOG(Severity::Info, "It is assumed there are no shade settings for window group "+name()+".");
    }else{
        for (auto &v : treeVal.get()){
            bVal=asBool(v, "There was a problem reading the calculate settings for window group "+name()+".", Severity::Warning);
            if (bVal){
                m_RunSetting.push_back(bVal.get());
                bVal.get();
            }
        }
        treeVal.reset();
    }

    if (shadeSettingGeometry().size()>0){
        treeVal=getObject(json, "shade_control", "The key \"shade_control\" within window group " + name() + " is missing.", Severity::Warning);
        if (!treeVal){
            STADIC_LOG(Severity::Info, "It is assumed there is no shade control needed for windows within window group "+name()+".");
        }else{
            if (!m_ShadeControl.parseJson(treeVal.get())){
                return false;
            }
        }
    }

    if (isBSDF() && shadeSettingGeometry().size()>0){
        treeVal=getArray(json, "bsdf_setting_materials");
        if (!treeVal){
            STADIC_LOG(Severity::Info, "It is assumed that window group "+name()+" does not contain BSDFs in the setting layers.");
        }else{            
            for(int index1=0;index1<treeVal.get().size();index1++){
                std::vector<std::string> tempVector;
                for (int index2=0;index2<treeVal.get()[index1].size();index2++){
                    tempVector.push_back(treeVal.get()[index1][index2].asString());
                }
                m_BSDFSettingLayers.push_back(tempVector);
            }
        }
    }
    return true;
}

}
