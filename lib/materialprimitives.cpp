#include "materialprimitives.h"
#include "logging.h"
namespace stadic{
//PLASTIC
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
                }else{
                    ERROR("The R G B values for a plastic must be between 0 and 1.");
                }
                break;
            case 3:
                if (ok && dval >=0 and dval <=0.07){
                    return true;
                }else if (dval>.07&&dval<1){
                    WARNING("The specularity value for a plastic is suggested to be between 0 and 0.07.");
                    return true;
                }else if (dval>1){
                    ERROR("The specularity value for a plastic cannot be greater than 1.");
                }else if (dval<0){
                    ERROR("The specularity value for a plastic cannot be less than 0.");
                }
                break;
            case 4:
                if (ok && dval >=0 and dval <=0.02){
                    return true;
                }else if (dval<0){
                    ERROR("The roughness for a plastic cannot be less than 0.");
                }else if (dval>0.02 && dval<1){
                    WARNING("The roughness value for a plastic is suggested to be 0 and 0.02.");
                    return true;
                }else if (dval>1){
                    ERROR("The roughness for a plastic cannot be greater than 1.");
                }
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


//METAL
MetalMaterial::MetalMaterial(QObject *parent) : RadPrimitive(parent)
{
    RadPrimitive::setType("metal");
    std::vector<QString> arg3 = {"0","0","0","0","0"};
    initArg(3,arg3);
}

MetalMaterial::MetalMaterial(double red, double green, double blue, double spec, double rough, QObject *parent)
    : RadPrimitive(parent)
{
    RadPrimitive::setType("metal");
    setArg(3,QString().sprintf("%g",red),0);
    setArg(3,QString().sprintf("%g",green),1);
    setArg(3,QString().sprintf("%g",blue),2);
    setArg(3,QString().sprintf("%g",spec),3);
    setArg(3,QString().sprintf("%g",rough),4);
}

// Setters
bool MetalMaterial::setRed(double value)
{
    return setArg(3,QString().sprintf("%g",value),0);
}

bool MetalMaterial::setGreen(double value)
{
    return setArg(3,QString().sprintf("%g",value),1);
}

bool MetalMaterial::setBlue(double value)
{
    return setArg(3,QString().sprintf("%g",value),2);
}

bool MetalMaterial::setSpecularity(double value)
{
    return setArg(3,QString().sprintf("%g",value),3);
}

bool MetalMaterial::setRoughness(double value)
{
    return setArg(3,QString().sprintf("%g",value),4);
}

// Getters
double MetalMaterial::red() const
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

double MetalMaterial::green() const
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

double MetalMaterial::blue() const
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

double MetalMaterial::specularity() const
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

double MetalMaterial::roughness() const
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

bool MetalMaterial::validateArg(int number, QString value, int position) const
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
                }else{
                    ERROR("The R G B values for a metal must be between 0 and 1.");
                }
                break;
            case 3:
                if (ok && dval >=.5 and dval <=1){
                    return true;
                }else if (dval<.5&&dval>=0){
                    WARNING("The specularity value for a metal is suggested to be between 0.5 and 1.");
                    return true;
                }else if (dval>1){
                    ERROR("The specularity value for a metal cannot be greater than 1.");
                }else if (dval<0){
                    ERROR("The specularity value for a metal cannot be less than 0.");
                }
                break;
            case 4:
                if (ok && dval >=0 and dval <=0.5){
                    return true;
                }else if (dval<0){
                    ERROR("The roughness for a metal cannot be less than 0.");
                }else if (dval>0.5 && dval<1){
                    WARNING("The roughness value for a metal is suggested to be 0 and 0.5.");
                    return true;
                }else if (dval>1){
                    ERROR("The roughness for a metal cannot be greater than 1.");
                }
                break;
        }
    }
    return false;
}

bool MetalMaterial::validateArg(int number, std::vector<QString> arg) const
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
//TRANS
TransMaterial::TransMaterial(QObject *parent) : RadPrimitive(parent)
{
    RadPrimitive::setType("metal");
    std::vector<QString> arg3 = {"0","0","0","0","0","0","0"};
    initArg(3,arg3);
}

TransMaterial::TransMaterial(double red, double green, double blue, double spec, double rough,double trans, double transpec, QObject *parent)
    : RadPrimitive(parent)
{
    RadPrimitive::setType("metal");
    setArg(3,QString().sprintf("%g",red),0);
    setArg(3,QString().sprintf("%g",green),1);
    setArg(3,QString().sprintf("%g",blue),2);
    setArg(3,QString().sprintf("%g",spec),3);
    setArg(3,QString().sprintf("%g",rough),4);
    setArg(3,QString().sprintf("%g",trans),5);
    setArg(3,QString().sprintf("%g",transpec),6);

}

// Setters
bool TransMaterial::setRed(double value)
{
    return setArg(3,QString().sprintf("%g",value),0);
}

bool TransMaterial::setGreen(double value)
{
    return setArg(3,QString().sprintf("%g",value),1);
}

bool TransMaterial::setBlue(double value)
{
    return setArg(3,QString().sprintf("%g",value),2);
}

bool TransMaterial::setSpecularity(double value)
{
    return setArg(3,QString().sprintf("%g",value),3);
}

bool TransMaterial::setRoughness(double value)
{
    return setArg(3,QString().sprintf("%g",value),4);
}

bool TransMaterial::setTransmission(double value)
{
    //This needs to convert transmission into transmissivity
    return setArg(3,QString().sprintf("%g",value),5);
}

bool TransMaterial::setTransmissivity(double value)
{
    return setArg(3,QString().sprintf("%g",value),5);
}

bool TransMaterial::setTransSpecular(double value)
{
    return setArg(3,QString().sprintf("%g",value),6);
}

// Getters
double TransMaterial::red() const
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

double TransMaterial::green() const
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

double TransMaterial::blue() const
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

double TransMaterial::specularity() const
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

double TransMaterial::roughness() const
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

double TransMaterial::transmissivity() const
{
    bool ok;
    double value = getArg3(5).toDouble(&ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

double TransMaterial::transSpecular() const
{
    bool ok;
    double value = getArg3(6).toDouble(&ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

bool TransMaterial::validateArg(int number, QString value, int position) const
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
                }else{
                    ERROR("The R G B values for a trans must be between 0 and 1.");
                }
                break;
            case 3:
                if (ok && dval >=0 and dval <=0.07){
                    return true;
                }else if (dval>.07&&dval<1){
                    WARNING("The specularity value for a trans is suggested to be between 0 and 0.07.");
                    return true;
                }else if (dval>1){
                    ERROR("The specularity value for a trans cannot be greater than 1.");
                }else if (dval<0){
                    ERROR("The specularity value for a trans cannot be less than 0.");
                }
                break;
            case 4:
                if (ok && dval >=0 and dval <=0.02){
                    return true;
                }else if (dval<0){
                    ERROR("The roughness for a trans cannot be less than 0.");
                }else if (dval>0.02 && dval<1){
                    WARNING("The roughness value for a trans is suggested to be 0 and 0.02.");
                    return true;
                }else if (dval>1){
                    ERROR("The roughness for a trans cannot be greater than 1.");
                }
                break;
            case 5:
                if(ok && dval >= 0 and dval <= 1.0) {
                    return true;
                }else{
                    ERROR("The transmissivity value for a trans must be between 0 and 1.");
                }
                break;
            case 6:
                if(ok && dval >= 0 and dval <= 1.0) {
                    return true;
                }else{
                    ERROR("The transmitted specularity value for a trans must be between 0 and 1.");
                }
                break;
        }
    }
    return false;
}

bool TransMaterial::validateArg(int number, std::vector<QString> arg) const
{
    if(number==3) {
        if(arg.size() != 7) {
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
//GLASS
GlassMaterial::GlassMaterial(QObject *parent) : RadPrimitive(parent)
{
    RadPrimitive::setType("glass");
    std::vector<QString> arg3 = {"0","0","0"};
    initArg(3,arg3);
}

GlassMaterial::GlassMaterial(double redTrans, double greenTrans, double blueTrans, QObject *parent)
    : RadPrimitive(parent)
{
    RadPrimitive::setType("glass");
    setArg(3,QString().sprintf("%g",redTrans),0);
    setArg(3,QString().sprintf("%g",greenTrans),1);
    setArg(3,QString().sprintf("%g",blueTrans),2);
}

GlassMaterial::GlassMaterial(double redTrans, double greenTrans, double blueTrans, double refrac, QObject *parent)
    : RadPrimitive(parent)
{
    RadPrimitive::setType("glass");
    setArg(3,QString().sprintf("%g",redTrans),0);
    setArg(3,QString().sprintf("%g",greenTrans),1);
    setArg(3,QString().sprintf("%g",blueTrans),2);
    setArg(3, QString().sprintf("%g", refrac),3);
}

// Setters
bool GlassMaterial::setRedTrans(double value)
{
    return setArg(3,QString().sprintf("%g",value),0);
}

bool GlassMaterial::setGreenTrans(double value)
{
    return setArg(3,QString().sprintf("%g",value),1);
}

bool GlassMaterial::setBlueTrans(double value)
{
    return setArg(3,QString().sprintf("%g",value),2);
}

bool GlassMaterial::setRefraction(double value)
{
    return setArg(3,QString().sprintf("%g",value),3);
}

// Getters
double GlassMaterial::redTrans() const
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

double GlassMaterial::greenTrans() const
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

double GlassMaterial::blueTrans() const
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

double GlassMaterial::refraction() const
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

bool GlassMaterial::validateArg(int number, QString value, int position) const
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
                }else{
                    ERROR("The R G B transmissivity values for glass must be between 0 and 1.");
                }
                break;
            case 3:
                if (ok && dval >=0 and dval <=5){
                    return true;
                }else {
                    ERROR("The index of refraction value for glass must be between 0 and 5.");
                }
                break;
        }
    }
    return false;
}

bool GlassMaterial::validateArg(int number, std::vector<QString> arg) const
{
    if(number==3) {
        if(arg.size() != 4 || arg.size() !=3) {
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
