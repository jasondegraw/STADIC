#include "radgeometry.h"

namespace stadic {

RadPrimitive::RadPrimitive(QObject *parent) :
    QObject(parent)
{
    m_Arg1.clear();
    m_Arg2.clear();
    m_Arg3.clear();
}

//Setters
void RadPrimitive::setModifier(QString modifier){
    m_Modifier=modifier;
}
void RadPrimitive::setType(QString type){
    m_Type=type;
}
void RadPrimitive::setName(QString name){
    m_Name=name;
}
bool RadPrimitive::setArg1(std::vector<QString> vals){
    m_Arg1.clear();
    m_Arg1=vals;
    return true;
}
bool RadPrimitive::setArg1(QString arg, int position) {
    if(position<m_Arg1.size()) {
        if(!validateArg(1,arg,position)) {
            return false;
        }
        m_Arg1[position] = arg;
        return true;
    }
    return false;
}
bool RadPrimitive::setArg2(std::vector<QString> vals){
    m_Arg2.clear();
    m_Arg2=vals;
    return true;
}
bool RadPrimitive::setArg2(QString arg, int position) {
    if(position<m_Arg2.size()) {
        if(!validateArg(2,arg,position)) {
            return false;
        }
        m_Arg2[position] = arg;
        return true;
    }
    return false;
}
bool RadPrimitive::setArg3(std::vector<QString> vals){
    m_Arg3.clear();
    m_Arg3=vals;
    return true;
}
bool RadPrimitive::setArg3(QString arg, int position) {
    if(position<m_Arg3.size()) {
        if(!validateArg(3,arg,position)) {
            return false;
        }
        m_Arg3[position] = arg;
        return true;
    }
    return false;
}

//Getters
QString RadPrimitive::modifier(){
    return m_Modifier;
}
QString RadPrimitive::type(){
    return m_Type;
}
QString RadPrimitive::name(){
    return m_Name;
}
std::vector<QString> RadPrimitive::arg1(){
    return m_Arg1;
}
std::vector<QString> RadPrimitive::arg2(){
    return m_Arg2;
}
std::vector<QString> RadPrimitive::arg3(){
    return m_Arg3;
}

}
