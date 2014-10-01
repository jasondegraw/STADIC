#ifndef WINDOWGROUP_H
#define WINDOWGROUP_H

#include <string>
#include <vector>
#include "shadecontrol.h"
#include <boost/property_tree/ptree.hpp>

#include "stadicapi.h"

namespace stadic {

class STADIC_API WindowGroup
{
public:
    WindowGroup();
    bool parseJson(const QJsonObject &object);
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
