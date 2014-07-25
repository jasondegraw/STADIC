#include "radgeometry.h"

std::array<QString,51> RadGeometry::s_typeStrings = {"source", "sphere", "bubble", "polygon", "cone", "cup",
                                                     "cylinder", "tube", "ring", "instance", "mesh", "light",
                                                     "illum", "glow", "spotlight", "mirror", "prism1", "prism2",
                                                     "plastic", "metal", "trans", "plastic2", "metal2", "trans2",
                                                     "mist", "dielectric", "interface", "glass", "plasfunc",
                                                     "metfunc", "transfunc", "brtdfunc", "plasdata", "metdata",
                                                     "transdata", "bsdf", "antimatter", "texfunc", "texdata",
                                                     "colorfunc", "brightfunc", "colordata", "brightdata",
                                                     "colorpict", "colortext", "brighttext", "mixfunc", "mixdata",
                                                     "mixpict", "mixtext", "unknown"};

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
    m_TypeString=type;
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
RadGeometry::Type RadGeometry::type(){
    return typeFromString(m_TypeString);
}
QString RadGeometry::typeString(){
    return m_TypeString;
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

RadGeometry* RadGeometry::fromRad(QFile file, QObject *parent)
{
    RadGeometry *obj;
    QString type;
    switch(typeFromString(type)) {
    case RadGeometry::Plastic:
        obj = new ::Plastic(parent);
        break;
    default:
        obj = new RadGeometry(parent);
        break;
    }
    return obj;
}

RadGeometry::Type RadGeometry::typeFromString(QString string)
{
    for(unsigned i=0;i<s_typeStrings.size();i++) {
        if(s_typeStrings[i] == string) {
            return (RadGeometry::Type)i;
        }
    }
    return Unknown;
}

QString RadGeometry::getArg1(int position) const
{
    if(position<0 || position>=m_Arg1.size()) {
        return QString();
    }
    return m_Arg1[position];
}

QString RadGeometry::getArg2(int position) const
{
    if(position<0 || position>=m_Arg2.size()) {
        return QString();
    }
    return m_Arg2[position];
}

QString RadGeometry::getArg3(int position) const
{
    if(position<0 || position>=m_Arg3.size()) {
        return QString();
    }
    return m_Arg3[position];
}

QString RadGeometry::getArg(int number, int position) const
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

bool RadGeometry::setArg(int number, QString value, int position)
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

void RadGeometry::initArg(int number, std::vector<QString> arg)
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

Plastic::Plastic(QObject *parent) : RadGeometry(parent)
{
    std::vector<QString> arg3 = {"0","0","0","0","0"};
    initArg(3,arg3);
}

Plastic::Plastic(double red, double green, double blue, double spec, double rough, QObject *parent)
    : RadGeometry(parent)
{
    setArg(3,QString().sprintf("%g",red),0);
    setArg(3,QString().sprintf("%g",green),1);
    setArg(3,QString().sprintf("%g",blue),2);
    setArg(3,QString().sprintf("%g",spec),3);
    setArg(3,QString().sprintf("%g",rough),4);
}

// Setters
bool Plastic::setRed(double value)
{
    return setArg(3,QString().sprintf("%g",value),0);
}

bool Plastic::setGreen(double value)
{
    return setArg(3,QString().sprintf("%g",value),1);
}

bool Plastic::setBlue(double value)
{
    return setArg(3,QString().sprintf("%g",value),2);
}

bool Plastic::setSpecularity(double value)
{
    return setArg(3,QString().sprintf("%g",value),3);
}

bool Plastic::setRoughness(double value)
{
    return setArg(3,QString().sprintf("%g",value),4);
}

// Getters
double Plastic::red() const
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

double Plastic::green() const
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

double Plastic::blue() const
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

double Plastic::specularity() const
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

double Plastic::roughness() const
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

bool Plastic::validateArg(int number, QString value, int position)
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
