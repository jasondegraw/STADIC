#include "materialprimitives.h"
#include "logging.h"
#include "functions.h"
namespace stadic{
//PLASTIC
PlasticMaterial::PlasticMaterial() : RadPrimitive()
{
    RadPrimitive::setType("plastic");
    std::vector<std::string> arg3 = {"0","0","0","0","0"};
    initArg(3,arg3);
}

PlasticMaterial::PlasticMaterial(double red, double green, double blue, double spec, double rough) : RadPrimitive()
{
    RadPrimitive::setType("plastic");
    setArg(3, stadic::toString(red), 0);
    setArg(3, stadic::toString(green), 1);
    setArg(3, stadic::toString(blue), 2);
    setArg(3, stadic::toString(spec), 3);
    setArg(3, stadic::toString(rough), 4);
}

// Setters
bool PlasticMaterial::setRed(double value)
{
    return setArg(3, stadic::toString(value), 0);
}

bool PlasticMaterial::setGreen(double value)
{
    return setArg(3, stadic::toString(value), 1);
}

bool PlasticMaterial::setBlue(double value)
{
    return setArg(3, stadic::toString(value), 2);
}

bool PlasticMaterial::setSpecularity(double value)
{
    return setArg(3, stadic::toString(value), 3);
}

bool PlasticMaterial::setRoughness(double value)
{
    return setArg(3, stadic::toString(value), 4);
}

// Getters
double PlasticMaterial::red() const
{
    bool ok;
    double value = stadic::toDouble(getArg3(0), &ok);
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
    double value = stadic::toDouble(getArg3(1), &ok);
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
    double value = stadic::toDouble(getArg3(2), &ok);
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
    double value = stadic::toDouble(getArg3(3), &ok);
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
    double value = stadic::toDouble(getArg3(4), &ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

bool PlasticMaterial::validateArg(int number, std::string value, int position) const
{
    if(number==3) {
        bool ok;
        double dval = stadic::toDouble(value, &ok);
        switch (position){
            case 0:
            case 1:
            case 2:
                if(ok && dval >= 0 && dval <= 1.0) {
                    return true;
                }else{
                    STADIC_ERROR("The R G B values for a plastic must be between 0 and 1.");
                }
                break;
            case 3:
                if (ok && dval >=0 && dval <=0.07){
                    return true;
                }else if (dval>.07&&dval<1){
                    STADIC_WARNING("The specularity value for a plastic is suggested to be between 0 and 0.07.");
                    return true;
                }else if (dval>1){
                    STADIC_ERROR("The specularity value for a plastic cannot be greater than 1.");
                }else if (dval<0){
                    STADIC_ERROR("The specularity value for a plastic cannot be less than 0.");
                }
                break;
            case 4:
                if (ok && dval >=0 && dval <=0.02){
                    return true;
                }else if (dval<0){
                    STADIC_ERROR("The roughness for a plastic cannot be less than 0.");
                }else if (dval>0.02 && dval<1){
                    STADIC_WARNING("The roughness value for a plastic is suggested to be between 0 and 0.02.");
                    return true;
                }else if (dval>1){
                    STADIC_ERROR("The roughness for a plastic cannot be greater than 1.");
                }
                break;
        }
    }
    return false;
}

bool PlasticMaterial::validateArg(int number, std::vector<std::string> arg) const
{
    if(number==3) {
        if(arg.size() != 5) {
            return false;
        }
        for(std::string value : arg) {
            bool ok;
            double dval = stadic::toDouble(value, &ok);
            if(ok && dval >= 0 && dval <= 1.0) {
                return true;
            }
        }
    }
    return false;
}


//METAL
MetalMaterial::MetalMaterial() : RadPrimitive()
{
    RadPrimitive::setType("metal");
    std::vector<std::string> arg3 = {"0","0","0","0","0"};
    initArg(3,arg3);
}

MetalMaterial::MetalMaterial(double red, double green, double blue, double spec, double rough)
    : RadPrimitive()
{
    RadPrimitive::setType("metal");
    setArg(3, stadic::toString(red), 0);
    setArg(3, stadic::toString(green), 1);
    setArg(3, stadic::toString(blue), 2);
    setArg(3, stadic::toString(spec), 3);
    setArg(3, stadic::toString(rough), 4);
}

// Setters
bool MetalMaterial::setRed(double value)
{
    return setArg(3, stadic::toString(value), 0);
}

bool MetalMaterial::setGreen(double value)
{
    return setArg(3, stadic::toString(value), 1);
}

bool MetalMaterial::setBlue(double value)
{
    return setArg(3, stadic::toString(value), 2);
}

bool MetalMaterial::setSpecularity(double value)
{
    return setArg(3, stadic::toString(value), 3);
}

bool MetalMaterial::setRoughness(double value)
{
    return setArg(3, stadic::toString(value), 4);
}

// Getters
double MetalMaterial::red() const
{
    bool ok;
    double value = stadic::toDouble(getArg3(0), &ok);
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
    double value = stadic::toDouble(getArg3(1), &ok);
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
    double value = stadic::toDouble(getArg3(2), &ok);
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
    double value = stadic::toDouble(getArg3(3), &ok);
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
    double value = stadic::toDouble(getArg3(4), &ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

bool MetalMaterial::validateArg(int number, std::string value, int position) const
{
    if(number==3) {
        bool ok;
        double dval = stadic::toDouble(value, &ok);
        switch (position){
            case 0:
            case 1:
            case 2:
                if(ok && dval >= 0 && dval <= 1.0) {
                    return true;
                }else{
                    STADIC_ERROR("The R G B values for a metal must be between 0 and 1.");
                }
                break;
            case 3:
                if (ok && dval >=.5 && dval <=1){
                    return true;
                }else if (dval<.5&&dval>=0){
                    STADIC_WARNING("The specularity value for a metal is suggested to be between 0.5 and 1.");
                    return true;
                }else if (dval>1){
                    STADIC_ERROR("The specularity value for a metal cannot be greater than 1.");
                }else if (dval<0){
                    STADIC_ERROR("The specularity value for a metal cannot be less than 0.");
                }
                break;
            case 4:
                if (ok && dval >=0 && dval <=0.5){
                    return true;
                }else if (dval<0){
                    STADIC_ERROR("The roughness for a metal cannot be less than 0.");
                }else if (dval>0.5 && dval<1){
                    STADIC_WARNING("The roughness value for a metal is suggested to be between 0 and 0.5.");
                    return true;
                }else if (dval>1){
                    STADIC_ERROR("The roughness for a metal cannot be greater than 1.");
                }
                break;
        }
    }
    return false;
}

bool MetalMaterial::validateArg(int number, std::vector<std::string> arg) const
{
    if(number==3) {
        if(arg.size() != 5) {
            return false;
        }
        for(std::string value : arg) {
            bool ok;
            double dval = stadic::toDouble(value, &ok);
            if(ok && dval >= 0 && dval <= 1.0) {
                return true;
            }
        }
    }
    return false;
}
//TRANS
TransMaterial::TransMaterial() : RadPrimitive()
{
    RadPrimitive::setType("trans");
    std::vector<std::string> arg3 = {"0","0","0","0","0","0","0"};
    initArg(3,arg3);
}

TransMaterial::TransMaterial(double red, double green, double blue, double spec, double rough, double trans, double transpec)
    : RadPrimitive()
{
    RadPrimitive::setType("trans");
    setArg(3, stadic::toString(red), 0);
    setArg(3, stadic::toString(green), 1);
    setArg(3, stadic::toString(blue), 2);
    setArg(3, stadic::toString(spec), 3);
    setArg(3, stadic::toString(rough), 4);
    setArg(3, stadic::toString(trans), 5);
    setArg(3, stadic::toString(transpec), 6);

}

// Setters
bool TransMaterial::setRed(double value)
{
    return setArg(3,std::to_string(value),0);
}

bool TransMaterial::setGreen(double value)
{
    return setArg(3,std::to_string(value),1);
}

bool TransMaterial::setBlue(double value)
{
    return setArg(3,std::to_string(value),2);
}

bool TransMaterial::setSpecularity(double value)
{
    return setArg(3,std::to_string(value),3);
}

bool TransMaterial::setRoughness(double value)
{
    return setArg(3,std::to_string(value),4);
}

bool TransMaterial::setTransmission(double value)
{
    //This needs to convert transmission into transmissivity
    return setArg(3,std::to_string(value),5);
}

bool TransMaterial::setTransmissivity(double value)
{
    return setArg(3,std::to_string(value),5);
}

bool TransMaterial::setTransSpecular(double value)
{
    return setArg(3,std::to_string(value),6);
}

// Getters
double TransMaterial::red() const
{
    bool ok;
    double value = stadic::toDouble(getArg3(0), &ok);
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
    double value = stadic::toDouble(getArg3(1), &ok);
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
    double value = stadic::toDouble(getArg3(2), &ok);
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
    double value = stadic::toDouble(getArg3(3), &ok);
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
    double value = stadic::toDouble(getArg3(4), &ok);
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
    double value = stadic::toDouble(getArg3(5), &ok);
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
    double value = stadic::toDouble(getArg3(6), &ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

bool TransMaterial::validateArg(int number, std::string value, int position) const
{
    if(number==3) {
        bool ok;
        double dval = stadic::toDouble(value, &ok);
        switch (position){
            case 0:
            case 1:
            case 2:
                if(ok && dval >= 0 && dval <= 1.0) {
                    return true;
                }else{
                    STADIC_ERROR("The R G B values for a trans must be between 0 and 1.");
                }
                break;
            case 3:
                if (ok && dval >=0 && dval <=0.07){
                    return true;
                }else if (dval>.07 && dval<1){
                    STADIC_WARNING("The specularity value for a trans is suggested to be between 0 and 0.07.");
                    return true;
                }else if (dval>1){
                    STADIC_ERROR("The specularity value for a trans cannot be greater than 1.");
                }else if (dval<0){
                    STADIC_ERROR("The specularity value for a trans cannot be less than 0.");
                }
                break;
            case 4:
                if (ok && dval >=0 && dval <=0.02){
                    return true;
                }else if (dval<0){
                    STADIC_ERROR("The roughness for a trans cannot be less than 0.");
                }else if (dval>0.02 && dval<1){
                    STADIC_WARNING("The roughness value for a trans is suggested to be 0 and 0.02.");
                    return true;
                }else if (dval>1){
                    STADIC_ERROR("The roughness for a trans cannot be greater than 1.");
                }
                break;
            case 5:
                if(ok && dval >= 0 && dval <= 1.0) {
                    return true;
                }else{
                    STADIC_ERROR("The transmissivity value for a trans must be between 0 and 1.");
                }
                break;
            case 6:
                if(ok && dval >= 0 && dval <= 1.0) {
                    return true;
                }else{
                    STADIC_ERROR("The transmitted specularity value for a trans must be between 0 and 1.");
                }
                break;
        }
    }
    return false;
}

bool TransMaterial::validateArg(int number, std::vector<std::string> arg) const
{
    if(number==3) {
        if(arg.size() != 7) {
            return false;
        }
        for(std::string value : arg) {
            bool ok;
            double dval = stadic::toDouble(value, &ok);
            if(ok && dval >= 0 && dval <= 1.0) {
                return true;
            }
        }
    }
    return false;
}
//GLASS
GlassMaterial::GlassMaterial() : RadPrimitive()
{
    RadPrimitive::setType("glass");
    std::vector<std::string> arg3 = {"0","0","0"};
    initArg(3,arg3);
}

GlassMaterial::GlassMaterial(double redTrans, double greenTrans, double blueTrans)
    : RadPrimitive()
{
    RadPrimitive::setType("glass");
    setArg(3,std::to_string(redTrans),0);
    setArg(3,std::to_string(greenTrans),1);
    setArg(3,std::to_string(blueTrans),2);
}

GlassMaterial::GlassMaterial(double redTrans, double greenTrans, double blueTrans, double refrac)
    : RadPrimitive()
{
    RadPrimitive::setType("glass");
    setArg(3,std::to_string(redTrans),0);
    setArg(3,std::to_string(greenTrans),1);
    setArg(3,std::to_string(blueTrans),2);
    setArg(3, std::to_string( refrac),3);
}

// Setters
bool GlassMaterial::setRedTrans(double value)
{
    return setArg(3,std::to_string(value),0);
}

bool GlassMaterial::setGreenTrans(double value)
{
    return setArg(3,std::to_string(value),1);
}

bool GlassMaterial::setBlueTrans(double value)
{
    return setArg(3,std::to_string(value),2);
}

bool GlassMaterial::setRefraction(double value)
{
    return setArg(3,std::to_string(value),3);
}

// Getters
double GlassMaterial::redTrans() const
{
    bool ok;
    double value = stadic::toDouble(getArg3(0), &ok);
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
    double value = stadic::toDouble(getArg3(1), &ok);
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
    double value = stadic::toDouble(getArg3(2), &ok);
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
    double value = stadic::toDouble(getArg3(3), &ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

bool GlassMaterial::validateArg(int number, std::string value, int position) const
{
    if(number==3) {
        bool ok;
        double dval = stadic::toDouble(value, &ok);
        switch (position){
            case 0:
            case 1:
            case 2:
                if(ok && dval >= 0 && dval <= 1.0) {
                    return true;
                }else{
                    STADIC_ERROR("The R G B transmissivity values for glass must be between 0 and 1.");
                }
                break;
            case 3:
                if (ok && dval >=0 && dval <=5){
                    return true;
                }else {
                    STADIC_ERROR("The index of refraction value for glass must be between 0 and 5.");
                }
                break;
        }
    }
    return false;
}

bool GlassMaterial::validateArg(int number, std::vector<std::string> arg) const
{
    if(number==3) {
        if(arg.size() != 4 || arg.size() !=3) {
            return false;
        }
        for(std::string value : arg) {
            bool ok;
            double dval = stadic::toDouble(value, &ok);
            if(ok && dval >= 0 && dval <= 1.0) {
                return true;
            }
        }
    }
    return false;
}
//BSDF
BSDFMaterial::BSDFMaterial() : RadPrimitive()
{
    RadPrimitive::setType("BSDF");
    std::vector<std::string> arg1 = {"0","null","0","0","0","."};
    initArg(1,arg1);
}

BSDFMaterial::BSDFMaterial(double thickness, std::string BSDFfile, double ux, double uy, double uz)
    : RadPrimitive()
{
    RadPrimitive::setType("BSDF");
    setArg(1,std::to_string(thickness),0);
    setArg(1,BSDFfile,1);
    setArg(1,std::to_string(ux),2);
    setArg(1,std::to_string(uy),3);
    setArg(1,std::to_string(uz),4);
    setArg(1,".",5);
}

// Setters
bool BSDFMaterial::setThickness(double value)
{
    return setArg(1,std::to_string(value),0);
}

bool BSDFMaterial::setBSDFfile(std::string name)
{
    return setArg(1,name,1);
}

bool BSDFMaterial::setUX(double value)
{
    return setArg(1,std::to_string(value),2);
}

bool BSDFMaterial::setUY(double value)
{
    return setArg(1,std::to_string(value),3);
}

bool BSDFMaterial::setUZ(double value)
{
    return setArg(1,std::to_string(value),4);
}

// Getters
double BSDFMaterial::thickness() const
{
    bool ok;
    double value = stadic::toDouble(getArg1(0), &ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

std::string BSDFMaterial::bsdfFile() const
{
    return getArg1(1);
}

double BSDFMaterial::ux() const
{
    bool ok;
    double value = stadic::toDouble(getArg1(2), &ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

double BSDFMaterial::uy() const
{
    bool ok;
    double value = stadic::toDouble(getArg1(3), &ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

double BSDFMaterial::uz() const
{
    bool ok;
    double value = stadic::toDouble(getArg1(4), &ok);
    if(!ok) {
        // This is bad and should *never* happen
        // Probably need to issue a panicky error message
        return 0;
    }
    return value;
}

bool BSDFMaterial::validateArg(int number, std::string value, int position) const
{
    if(number==1) {
        bool ok;
        double dval = 0;
        if (position!=1){
            dval=stadic::toDouble(value, &ok);
        }
        switch (position){
            case 0:
                if (ok && dval>=0){
                    return true;
                }
                break;
            case 1:
                //This should test to make sure the file exists.
                return true;
                break;
            case 2:
                if(ok) {
                    return true;
                }else{
                    STADIC_ERROR("There was an error in the x vector for the BSDF material.");
                }
                break;
            case 3:
                if (ok){
                    return true;
                }else{
                    STADIC_ERROR("There was an error in the y vector for the BSDF material.");
                }
                break;
            case 4:
                if (ok){
                    return true;
                }else{
                    STADIC_ERROR("There was an error in the z vector for the BSDF material.");
                }
                break;
            case 5:
                if (value=="."){
                    return true;
                }else{
                    STADIC_ERROR("The last argument on the first line of the BSDF material\n\tshould be a period \".\".");
                }
                break;
        }
    }
    return false;
}

bool BSDFMaterial::validateArg(int number, std::vector<std::string> arg) const
{
    if(number==1) {
        if(arg.size() != 6) {
            return false;
        }
        for(std::string value : arg) {
            bool ok;
            double dval = stadic::toDouble(value, &ok);
            if(ok && dval >= 0 && dval <= 1.0) {
                return true;
            }
        }
    }
    return false;
}

}
