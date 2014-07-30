#include "materialprimitives.h"
namespace stadic{
PlasticMaterial::PlasticMaterial(QObject *parent) : RadPrimitive(parent)
{
    RadPrimitive::setType("plastic");
    std::vector<QString> arg3 = {"0","0","0","0","0"};
    initArg(3,arg3);
}

PlasticMaterial::PlasticMaterial(double red, double green, double blue, double spec, double rough, QObject *parent)
    : RadPrimitive(parent)
{
    RadPrimitive::setType("plastic");
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
        bool ok;
        double dval = value.toDouble(&ok);
        switch (position){
            case 0:
            case 1:
            case 2:
                if(ok && dval >= 0 and dval <= 1.0) {
                    return true;
                }
                break;
            case 3:
                if (ok && dval >=0 and dval <=0.07){
                    return true;
                }
                break;
            case 4:
                if (ok && dval >=0 and dval <=0.02){
                    return true;
                }
                return true;
                break;
        }
    }
    return false;
}

bool PlasticMaterial::validateArg(int number, std::vector<QString> arg) const
{
    if(number==3) {
        if(arg.size() != 5) {
            return false;
        }
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
