#include "radgeometry.h"

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

