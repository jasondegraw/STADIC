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

#ifndef WINDOWGROUP_H
#define WINDOWGROUP_H

#include <string>
#include <vector>
#include "shadecontrol.h"
#include "stadicapi.h"
#include <boost/property_tree/ptree.hpp>

namespace stadic {

class STADIC_API WindowGroup
{
public:
    WindowGroup();
    //bool parseJson(const QJsonObject &object);
    bool parseJson(const boost::property_tree::ptree json);

    //  Setters
    void setName(const std::string &name);
    void setBSDF(bool isBSDF);
    void setBaseGeometry(const std::string &file);

    //  Getters
    std::string name() const;
    bool isBSDF() const;
    std::string baseGeometry() const;
    std::vector<std::string> bsdfBaseLayers() const;
    std::vector<std::string> glazingLayers() const;
    std::vector<std::string> shadeSettingGeometry() const;
    std::vector<std::vector<std::string> > bsdfSettingLayers() const;
    ShadeControl *shadeControl();

private:
    std::string m_name;
    bool m_BSDF;
    std::string m_BaseGeometry;
    std::vector<std::string> m_BSDFBaseLayers;
    std::vector<std::string> m_GlazingLayers;
    std::vector<std::string> m_ShadeSettingGeometry;
    std::vector<std::vector<std::string> > m_BSDFSettingLayers;
    ShadeControl m_ShadeControl;
    
};

}

#endif // WINDOWGROUPS_H
