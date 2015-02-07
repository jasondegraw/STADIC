/******************************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
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

#ifndef WINDOWGROUP_H
#define WINDOWGROUP_H

#include <string>
#include <vector>
#include "shadecontrol.h"
#include "stadicapi.h"
#include "jsonobjects.h"

namespace stadic {

class STADIC_API WindowGroup
{
public:
    WindowGroup();
    //bool parseJson(const QJsonObject &object);
    bool parseJson(const JsonObject &json);

    //  Setters
    void setName(const std::string &name);
    void setBSDF(bool isBSDF);
    void setBaseGeometry(const std::string &file);
    void setBaseCalculate(bool runBase);

    //  Getters
    std::string name() const;
    bool isBSDF() const;
    std::string baseGeometry() const;
    bool runBase() const;
    std::vector<std::string> bsdfBaseLayers() const;
    std::vector<std::string> glazingLayers() const;
    std::vector<std::string> shadeSettingGeometry() const;
    std::vector<bool> runSetting() const;
    std::vector<std::vector<std::string> > bsdfSettingLayers() const;
    ShadeControl *shadeControl();

private:
    std::string m_name;
    bool m_BSDF;
    std::string m_BaseGeometry;
    bool m_RunBase;
    std::vector<std::string> m_BSDFBaseLayers;
    std::vector<std::string> m_GlazingLayers;
    std::vector<std::string> m_ShadeSettingGeometry;
    std::vector<bool> m_RunSetting;
    std::vector<std::vector<std::string> > m_BSDFSettingLayers;
    ShadeControl m_ShadeControl;
    
};

}

#endif // WINDOWGROUPS_H
