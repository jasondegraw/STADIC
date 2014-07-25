#include "radprimitive.h"

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

//Setters
void RadPrimitive::setModifier(QString modifier){
    m_Modifier=modifier;
}

void RadPrimitive::setType(QString type){
    m_TypeString=type;
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

RadPrimitive* RadPrimitive::fromRad(QFile file, QObject *parent)
{
    RadPrimitive *obj;
    QString type;
    switch(typeFromString(type)) {
    case Plastic:
        obj = new PlasticMaterial(parent);
        break;
    default:
        obj = new RadPrimitive(parent);
        break;
    }
    return obj;
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

PlasticMaterial::PlasticMaterial(QObject *parent) : RadPrimitive(parent)
{
    std::vector<QString> arg3 = {"0","0","0","0","0"};
    initArg(3,arg3);
}

PlasticMaterial::PlasticMaterial(double red, double green, double blue, double spec, double rough, QObject *parent)
    : RadPrimitive(parent)
{
    setArg(3,QString().sprintf("%g",red),0);
    setArg(3,QString().sprintf("%g",green),1);
    setArg(3,QString().sprintf("%g",blue),2);
    setArg(3,QString().sprintf("%g",spec),3);
    setArg(3,QString().sprintf("%g",rough),4);
}

// Setters
bool PlasticMaterial::setRed(double value)
{
    return setArg(3,QString().sprintf("%g",value),0);
}

bool PlasticMaterial::setGreen(double value)
{
    return setArg(3,QString().sprintf("%g",value),1);
}

bool PlasticMaterial::setBlue(double value)
{
    return setArg(3,QString().sprintf("%g",value),2);
}

bool PlasticMaterial::setSpecularity(double value)
{
    return setArg(3,QString().sprintf("%g",value),3);
}

bool PlasticMaterial::setRoughness(double value)
{
    return setArg(3,QString().sprintf("%g",value),4);
}

// Getters
double PlasticMaterial::red() const
{
    bool ok;
    double value = getArg3(0).toDouble(&ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

double PlasticMaterial::green() const
{
    bool ok;
    double value = getArg3(1).toDouble(&ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

double PlasticMaterial::blue() const
{
    bool ok;
    double value = getArg3(2).toDouble(&ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

double PlasticMaterial::specularity() const
{
    bool ok;
    double value = getArg3(3).toDouble(&ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

double PlasticMaterial::roughness() const
{
    bool ok;
    double value = getArg3(4).toDouble(&ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

bool PlasticMaterial::validateArg(int number, QString value, int position) const
{
    if(number==3) {
        if(position>0 && position<5) {
            bool ok;
            double dval = value.toDouble(&ok);
            if(ok && dval >= 0 and dval <= 1.0) {
                return true;
            }
        }
    }
    return false;
}

bool PlasticMaterial::validateArg(int number, std::vector<QString> arg) const
{
    if(number==3) {
        for(QString value : arg) {
            bool ok;
            double dval = value.toDouble(&ok);
            if(ok && dval >= 0 and dval <= 1.0) {
                return true;
            }
        }
    }
    return false;
}

}
