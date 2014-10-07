#include "materialprimitives.h"
#include "logging.h"
#include "functions.h"
namespace stadic{
//PLASTIC
PlasticMaterial::PlasticMaterial() : RadPrimitive()
{
    RadPrimitive::setType("plastic");
    std::vector<std::string> arg3 = { "0", "0", "0", "0", "0" };
    initArg(3, arg3);
}

PlasticMaterial::PlasticMaterial(double red, double green, double blue, double spec, double rough) : RadPrimitive()
{
    RadPrimitive::setType("plastic");
    std::vector<std::string> arg3 = { "0", "0", "0", "0", "0" };
    initArg(3, arg3);
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
    if(number == 3) {
        switch(position){
        case 0:
            return checkValue(value, 0, 0, 1, "red", "plastic");
        case 1:
            return checkValue(value, 1, 0, 1, "blue", "plastic");
        case 2:
            return checkValue(value, 2, 0, 1, "green", "plastic");
        case 3:
            return checkValue(value, 3, 0, 1, 0, 0.07, "specularity", "plastic");
        case 4:
            return checkValue(value, 4, 0, 1, 0, 0.02, "roughness", "plastic");
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
    std::vector<std::string> arg3 = { "0", "0", "0", "0", "0" };
    initArg(3, arg3);
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
        switch (position) {
        case 0:
            return checkValue(value, 0, 0, 1, "red", "metal");
        case 1:
            return checkValue(value, 1, 0, 1, "blue", "metal");
        case 2:
            return checkValue(value, 2, 0, 1, "green", "metal");
        case 3:
            return checkValue(value, 3, 0, 1, 0, 0.07, "specularity", "metal");
        case 4:
            return checkValue(value, 4, 0, 1, 0, 0.02, "roughness", "metal");
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
    std::vector<std::string> arg3 = { "0", "0", "0", "0", "0", "0", "0" };
    initArg(3, arg3);
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
    return setArg(3,stadic::toString(value),0);
}

bool TransMaterial::setGreen(double value)
{
    return setArg(3,stadic::toString(value),1);
}

bool TransMaterial::setBlue(double value)
{
    return setArg(3,stadic::toString(value),2);
}

bool TransMaterial::setSpecularity(double value)
{
    return setArg(3,stadic::toString(value),3);
}

bool TransMaterial::setRoughness(double value)
{
    return setArg(3,stadic::toString(value),4);
}

bool TransMaterial::setTransmission(double value)
{
    //This needs to convert transmission into transmissivity
    return setArg(3,stadic::toString(value),5);
}

bool TransMaterial::setTransmissivity(double value)
{
    return setArg(3,stadic::toString(value),5);
}

bool TransMaterial::setTransSpecular(double value)
{
    return setArg(3,stadic::toString(value),6);
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
        switch (position){
        case 0:
            return checkValue(value, 0, 0, 1, "red", "trans");
        case 1:
            return checkValue(value, 1, 0, 1, "blue", "trans");
        case 2:
            return checkValue(value, 2, 0, 1, "green", "trans");
        case 3:
            return checkValue(value, 3, 0, 1, 0, 0.07, "specularity", "trans");
        case 4:
            return checkValue(value, 4, 0, 1, 0, 0.02, "roughness", "trans");
        case 5:
            return checkValue(value, 5, 0, 1, "transmissivity", "trans");
        case 6:
            return checkValue(value, 6, 0, 1, "transmitted specularity", "trans");
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
    std::vector<std::string> arg3 = { "0", "0", "0", "0" };
    initArg(3,arg3);
}

GlassMaterial::GlassMaterial(double redTrans, double greenTrans, double blueTrans, double refrac)
    : RadPrimitive()
{
    RadPrimitive::setType("glass");
    std::vector<std::string> arg3 = { "0", "0", "0", "0" };
    initArg(3, arg3);
    setArg(3,stadic::toString(redTrans),0);
    setArg(3,stadic::toString(greenTrans),1);
    setArg(3,stadic::toString(blueTrans),2);
    setArg(3,stadic::toString(refrac),3);
}

// Setters
bool GlassMaterial::setRedTrans(double value)
{
    return setArg(3,stadic::toString(value),0);
}

bool GlassMaterial::setGreenTrans(double value)
{
    return setArg(3,stadic::toString(value),1);
}

bool GlassMaterial::setBlueTrans(double value)
{
    return setArg(3,stadic::toString(value),2);
}

bool GlassMaterial::setRefraction(double value)
{
    return setArg(3,stadic::toString(value),3);
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
            return checkValue(value, 0, 0, 1, "red transmissivity", "glass");
        case 1:
            return checkValue(value, 1, 0, 1, "blue transmissivity", "glass");
        case 2:
            return checkValue(value, 2, 0, 1, "green transmissivity", "glass");
        case 3:
            return checkValue(value, 3, 0, 5, "index of refraction", "glass");
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
    setArg(1,stadic::toString(thickness),0);
    setArg(1,BSDFfile,1);
    setArg(1,stadic::toString(ux),2);
    setArg(1,stadic::toString(uy),3);
    setArg(1,stadic::toString(uz),4);
    setArg(1,".",5);
}

// Setters
bool BSDFMaterial::setThickness(double value)
{
    return setArg(1,stadic::toString(value),0);
}

bool BSDFMaterial::setBSDFfile(std::string name)
{
    return setArg(1,name,1);
}

bool BSDFMaterial::setUX(double value)
{
    return setArg(1,stadic::toString(value),2);
}

bool BSDFMaterial::setUY(double value)
{
    return setArg(1,stadic::toString(value),3);
}

bool BSDFMaterial::setUZ(double value)
{
    return setArg(1,stadic::toString(value),4);
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
