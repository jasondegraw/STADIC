#include "radprimitive.h"
#include <QTextStream>
#include "materialprimitives.h"
#include <iostream>

namespace stadic {

std::array<QString,51> RadPrimitive::s_typeStrings = {"source", "sphere", "bubble", "polygon", "cone", "cup",
                                                      "cylinder", "tube", "ring", "instance", "mesh", "light",
                                                      "illum", "glow", "spotlight", "mirror", "prism1", "prism2",
                                                      "plastic", "metal", "trans", "plastic2", "metal2", "trans2",
                                                      "mist", "dielectric", "interface", "glass", "plasfunc",
                                                      "metfunc", "transfunc", "brtdfunc", "plasdata", "metdata",
                                                      "transdata", "bsdf", "antimatter", "texfunc", "texdata",
                                                      "colorfunc", "brightfunc", "colordata", "brightdata",
                                                      "colorpict", "colortext", "brighttext", "mixfunc", "mixdata",
                                                      "mixpict", "mixtext", "unknown"};

RadPrimitive::RadPrimitive(QObject *parent) :
    QObject(parent)
{
}

bool RadPrimitive::isGeometry() const
{
    switch(type()) {
    case Source:
    case Sphere:
    case Bubble:
    case Polygon:
    case Cone:
    case Cup:
    case Cylinder:
    case Tube:
    case Ring:
    case Instance:
    case Mesh:
        return true;
    }
    return false;
}

bool RadPrimitive::isMaterial() const
{
    switch(type()) {
    case Light:
    case Illum:
    case Glow:
    case Spotlight:
    case Mirror:
    case Prism1:
    case Prism2:
    case Plastic:
    case Metal:
    case Trans:
    case Plastic2:
    case Metal2:
    case Trans2:
    case Mist:
    case Dielectric:
    case Interface:
    case Glass:
    case Plasfunc:
    case Metfunc:
    case Transfunc:
    case BRTDfunc:
    case Plasdata:
    case Metdata:
    case Transdata:
    case BSDF:
    case Antimatter:
        return true;
    }
    return false;
}

//Setters
void RadPrimitive::setModifier(QString modifier){
    m_Modifier=modifier;
}

bool RadPrimitive::setType(QString type){
    m_TypeString=type;
    return true;
}
void RadPrimitive::setName(QString name){
    m_Name=name;
}

bool RadPrimitive::setArg1(std::vector<QString> vals){
    if(validateArg(1,vals)) {
      m_Arg1=vals;
      return true;
    }
    return false;
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
    if(validateArg(2,vals)) {
      m_Arg2=vals;
      return true;
    }
    return false;
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
    if(validateArg(3,vals)) {
      m_Arg3=vals;
      return true;
    }
    return false;
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

bool RadPrimitive::setArg(int number, QString value, int position)
{
    std::vector<QString> *arg;
    switch(number) {
    case 1:
        arg = &m_Arg1;
        break;
    case 2:
        arg = &m_Arg2;
        break;
    case 3:
        arg = &m_Arg3;
        break;
    default:
        // Error/warning message?
        return false;
    }
    if(validateArg(number,value,position)) {
        if(position>=arg->size()) {
            arg->resize(position+1);
        }
        (*arg)[position] = value;
        return true;
    } else {
        // Error/warning message?
    }
    return false;
}

//Getters
QString RadPrimitive::modifier() const {
    return m_Modifier;
}
RadPrimitive::Type RadPrimitive::type() const{
    return typeFromString(m_TypeString);
}
QString RadPrimitive::typeString() const {
    return m_TypeString;
}
QString RadPrimitive::name() const {
    return m_Name;
}
std::vector<QString> RadPrimitive::arg1() const {
    return m_Arg1;
}
std::vector<QString> RadPrimitive::arg2() const {
    return m_Arg2;
}
std::vector<QString> RadPrimitive::arg3() const{
    return m_Arg3;
}

static QString nextNonComment(QTextStream &data)
{
  QString string;
  while(!data.atEnd()) {
    string = data.readLine().trimmed();
    //std::cout << "### " << string.toStdString() << std::endl;
    if(!string.isEmpty()) {
      if(!string.startsWith('#')) {
        return string;
      }
    }
  }
  return QString();
}

RadPrimitive* RadPrimitive::fromRad(QTextStream &data, QObject *parent)
{
    RadPrimitive *rad;

    QString string = nextNonComment(data);
    if(string.isEmpty()) {
        return nullptr;
    }
    QStringList list = string.split(QRegExp("\\s+")); // This should be "modifier type identifier"?
    if(list.size() != 3) {
        return nullptr;
    }

    switch(typeFromString(list[1])) {
    case Plastic:
        rad = new PlasticMaterial(parent);
        break;
    default:
        rad = new RadPrimitive(parent);
        rad->setType(list[1]);
        break;
    }
    rad->setModifier(list[0]);
    rad->setName(list[2]);

    int nargs;
    data>>string;   //Reads number of arguments from first line
    nargs = string.toInt();
    if (nargs>0){
        std::vector<QString> args;
        for (int i=0;i<nargs;i++){
            data>>string;
            args.push_back(string);
        }
        rad->setArg1(args);
    }

    data>>string;   //Reads number of arguments from second line
    nargs = string.toInt();
    if (nargs>0){
        std::vector<QString> args;
        for (int i=0;i<nargs;i++){
            data>>string;
            args.push_back(string);
        }
        rad->setArg2(args);
    }

    data>>string;   //Reads number of arguments from third line
    nargs = string.toInt();
    if (nargs>0){
        std::vector<QString> args;
        for (int i=0;i<nargs;i++){
            data>>string;
            args.push_back(string);
        }
        rad->setArg3(args);
    }
    return rad;
}

RadPrimitive::Type RadPrimitive::typeFromString(QString string)
{
    for(unsigned i=0;i<s_typeStrings.size();i++) {
        if(s_typeStrings[i] == string) {
            return (RadPrimitive::Type)i;
        }
    }
    return Unknown;
}

QString RadPrimitive::getArg1(int position) const
{
    if(position<0 || position>=m_Arg1.size()) {
        return QString();
    }
    return m_Arg1[position];
}

QString RadPrimitive::getArg2(int position) const
{
    if(position<0 || position>=m_Arg2.size()) {
        return QString();
    }
    return m_Arg2[position];
}

QString RadPrimitive::getArg3(int position) const
{
    if(position<0 || position>=m_Arg3.size()) {
        return QString();
    }
    return m_Arg3[position];
}

QString RadPrimitive::getArg(int number, int position) const
{
    std::vector<QString> *arg;
    switch(number) {
    case 1:
        return getArg1(position);
    case 2:
        return getArg2(position);
    case 3:
        return getArg3(position);
    }
    // Error/warning message?
    return QString();
}

void RadPrimitive::initArg(int number, std::vector<QString> arg)
{
    switch(number) {
    case 1:
        m_Arg1 = arg;
        break;
    case 2:
        m_Arg2 = arg;
        break;
    case 3:
        m_Arg3 = arg;
        break;
    default:
        // Error message?
        break;
    }
}



}
