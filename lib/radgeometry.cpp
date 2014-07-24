#include "radgeometry.h"

std::array<QString,50> RadGeometry::s_typeStrings = {"Source", "Sphere", "Bubble", "Polygon", "Cone", "Cup", "Cylinder", "Tube", "Ring", "Instance", "Mesh", "Light", "Illum", "Glow", "Spotlight", "Mirror", "Prism1", "Prism2", "Plastic", "Metal", "Trans", "Plastic2", "Metal2", "Trans2", "Mist", "Dielectric", "Interface", "Glass", "Plasfunc", "Metfunc", "Transfunc", "BRTDfunc", "Plasdata", "Metdata", "Transdata", "BSDF", "Antimatter", "Texfunc", "Texdata", "Colorfunc", "Brightfunc", "Colordata", "Brightdata", "Colorpict", "Colortext", "Brighttext", "Mixfunc", "Mixdata", "Mixpict", "Mixtext"};

RadGeometry::RadGeometry(QObject *parent) :
    QObject(parent)
{
    m_Arg1.clear();
    m_Arg2.clear();
    m_Arg3.clear();
}

//Setters
void RadGeometry::setModifier(QString modifier){
    m_Modifier=modifier;
}
void RadGeometry::setType(QString type){
    m_Type=type;
}
void RadGeometry::setName(QString name){
    m_Name=name;
}
void RadGeometry::setArg1(QString arg){
    m_Arg1.push_back(arg);
}
void RadGeometry::setArg2(QString arg){
    m_Arg2.push_back(arg);
}
void RadGeometry::setArg3(QString arg){
    m_Arg3.push_back(arg);
}

//Getters
QString RadGeometry::modifier(){
    return m_Modifier;
}
QString RadGeometry::type(){
    return m_Type;
}
QString RadGeometry::name(){
    return m_Name;
}
std::vector<QString> RadGeometry::arg1(){
    return m_Arg1;
}
std::vector<QString> RadGeometry::arg2(){
    return m_Arg2;
}
std::vector<QString> RadGeometry::arg3(){
    return m_Arg3;
}

