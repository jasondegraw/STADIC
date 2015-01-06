/******************************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission of the
 *    respective copyright holder or contributor.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *****************************************************************************/

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
    setArg3(stadic::toString(red), 0);
    setArg3(stadic::toString(green), 1);
    setArg3(stadic::toString(blue), 2);
    setArg3(stadic::toString(spec), 3);
    setArg3(stadic::toString(rough), 4);
}

// Setters
bool PlasticMaterial::setRed(double value)
{
    return setArg3(stadic::toString(value), 0);
}

bool PlasticMaterial::setGreen(double value)
{
    return setArg3(stadic::toString(value), 1);
}

bool PlasticMaterial::setBlue(double value)
{
    return setArg3(stadic::toString(value), 2);
}

bool PlasticMaterial::setSpecularity(double value)
{
    return setArg3(stadic::toString(value), 3);
}

bool PlasticMaterial::setRoughness(double value)
{
    return setArg3(stadic::toString(value), 4);
}

// Getters
double PlasticMaterial::red() const
{
    return argToDouble(3, 0, "red");
}

double PlasticMaterial::green() const
{
    return argToDouble(3, 1, "green");
}

double PlasticMaterial::blue() const
{
    return argToDouble(3, 2, "blue");
}

double PlasticMaterial::specularity() const
{
    return argToDouble(3, 3, "specularity");
}

double PlasticMaterial::roughness() const
{
    return argToDouble(3, 4, "roughness");
}

bool PlasticMaterial::validateArg3(const std::string &value, int position) const
{
    switch(position){
    case 0:
        return checkDoubleValue(value, 0, 1, "red", red());
    case 1:
        return checkDoubleValue(value, 0, 1, "green", green());
    case 2:
        return checkDoubleValue(value, 0, 1, "blue", blue());
    case 3:
        return checkDoubleValue(value, 0, 1, 0, 0.07, "specularity",specularity());
    case 4:
        return checkDoubleValue(value, 0, 1, 0, 0.02, "roughness", roughness());
    default:
        STADIC_LOG(Severity::Warning, "A plastic primitive has no third argument in position " + stadic::toString(position));
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
    setArg3(stadic::toString(red), 0);
    setArg3(stadic::toString(green), 1);
    setArg3(stadic::toString(blue), 2);
    setArg3(stadic::toString(spec), 3);
    setArg3(stadic::toString(rough), 4);
}

// Setters
bool MetalMaterial::setRed(double value)
{
    return setArg3(stadic::toString(value), 0);
}

bool MetalMaterial::setGreen(double value)
{
    return setArg3(stadic::toString(value), 1);
}

bool MetalMaterial::setBlue(double value)
{
    return setArg3(stadic::toString(value), 2);
}

bool MetalMaterial::setSpecularity(double value)
{
    return setArg3(stadic::toString(value), 3);
}

bool MetalMaterial::setRoughness(double value)
{
    return setArg3(stadic::toString(value), 4);
}

// Getters
double MetalMaterial::red() const
{
    return argToDouble(3, 0, "red");
}

double MetalMaterial::green() const
{
    return argToDouble(3, 1, "green");
}

double MetalMaterial::blue() const
{
    return argToDouble(3, 2, "blue");
}

double MetalMaterial::specularity() const
{
    return argToDouble(3, 3, "specularity");
}

double MetalMaterial::roughness() const
{
    return argToDouble(3, 4, "roughness");
}

bool MetalMaterial::validateArg3(const std::string &value, int position) const
{
    switch(position) {
    case 0:
        return checkDoubleValue(value, 0, 1, "red", red());
    case 1:
        return checkDoubleValue(value, 0, 1, "green", green());
    case 2:
        return checkDoubleValue(value, 0, 1, "blue", blue());
    case 3:
        return checkDoubleValue(value, 0, 1, 0, 0.07, "specularity", specularity());
    case 4:
        return checkDoubleValue(value, 0, 1, 0, 0.02, "roughness", roughness());
    default:
        STADIC_LOG(Severity::Warning, "A metal primitive has no third argument in position " + stadic::toString(position));
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
    setArg3(stadic::toString(red), 0);
    setArg3(stadic::toString(green), 1);
    setArg3(stadic::toString(blue), 2);
    setArg3(stadic::toString(spec), 3);
    setArg3(stadic::toString(rough), 4);
    setArg3(stadic::toString(trans), 5);
    setArg3(stadic::toString(transpec), 6);

}

// Setters
bool TransMaterial::setRed(double value)
{
    return setArg3(stadic::toString(value), 0);
}

bool TransMaterial::setGreen(double value)
{
    return setArg3(stadic::toString(value), 1);
}

bool TransMaterial::setBlue(double value)
{
    return setArg3(stadic::toString(value), 2);
}

bool TransMaterial::setSpecularity(double value)
{
    return setArg3(stadic::toString(value), 3);
}

bool TransMaterial::setRoughness(double value)
{
    return setArg3(stadic::toString(value), 4);
}

bool TransMaterial::setTransmission(double value)
{
    //This needs to convert transmission into transmissivity
    STADIC_LOG(Severity::Fatal, "Transmission conversion is not yet available");
    return setArg3(stadic::toString(value), 5);
}

bool TransMaterial::setTransmissivity(double value)
{
    return setArg3(stadic::toString(value), 5);
}

bool TransMaterial::setTransSpecular(double value)
{
    return setArg3(stadic::toString(value), 6);
}

// Getters
double TransMaterial::red() const
{
    return argToDouble(3, 0, "red");
}

double TransMaterial::green() const
{
    return argToDouble(3, 1, "green");
}

double TransMaterial::blue() const
{
    return argToDouble(3, 2, "blue");
}

double TransMaterial::specularity() const
{
    return argToDouble(3, 3, "specularity");
}

double TransMaterial::roughness() const
{
    return argToDouble(3, 4, "roughness");
}

double TransMaterial::transmissivity() const
{
    return argToDouble(3, 5, "transmissivity");
}

double TransMaterial::transSpecular() const
{
    return argToDouble(3, 6, "transmitted specularity");
}

bool TransMaterial::validateArg3(const std::string &value, int position) const
{
    switch(position){
    case 0:
        return checkDoubleValue(value, 0, 1, "red", red());
    case 1:
        return checkDoubleValue(value, 0, 1, "green", green());
    case 2:
        return checkDoubleValue(value, 0, 1, "blue", blue());
    case 3:
        return checkDoubleValue(value, 0, 1, 0, 0.07, "specularity", specularity());
    case 4:
        return checkDoubleValue(value, 0, 1, 0, 0.02, "roughness", roughness());
    case 5:
        return checkDoubleValue(value, 0, 1, "transmissivity", transmissivity());
    case 6:
        return checkDoubleValue(value, 0, 1, "transmitted specularity", transSpecular());
    default:
        STADIC_LOG(Severity::Warning, "A trans primitive has no third argument in position " + stadic::toString(position));
    }
    return false;
}

//GLASS
GlassMaterial::GlassMaterial() : RadPrimitive()
{
    RadPrimitive::setType("glass");
    std::vector<std::string> arg3 = { "0", "0", "0" };
    initArg(3,arg3);
}

GlassMaterial::GlassMaterial(double redTrans, double greenTrans, double blueTrans)
    : RadPrimitive()
{
    RadPrimitive::setType("glass");
    std::vector<std::string> arg3 = { "0", "0", "0" };
    initArg(3, arg3);
    setArg3(stadic::toString(redTrans), 0);
    setArg3(stadic::toString(greenTrans), 1);
    setArg3(stadic::toString(blueTrans), 2);
}

GlassMaterial::GlassMaterial(double redTrans, double greenTrans, double blueTrans, double refrac)
    : RadPrimitive()
{
    RadPrimitive::setType("glass");
    std::vector<std::string> arg3 = { "0", "0", "0", "1.52" };
    initArg(3, arg3);
    setArg3(stadic::toString(redTrans),0);
    setArg3(stadic::toString(greenTrans),1);
    setArg3(stadic::toString(blueTrans), 2);
    setArg3(stadic::toString(refrac), 3);
}

// Setters
bool GlassMaterial::setRedTrans(double value)
{
    return setArg3(stadic::toString(value),0);
}

bool GlassMaterial::setGreenTrans(double value)
{
    return setArg3(stadic::toString(value),1);
}

bool GlassMaterial::setBlueTrans(double value)
{
    return setArg3(stadic::toString(value),2);
}

bool GlassMaterial::setRefraction(double value)
{
    return setArg3(stadic::toString(value),3);
}

// Getters
double GlassMaterial::redTrans() const
{
    return argToDouble(3, 0, "red transmissivity");
}

double GlassMaterial::greenTrans() const
{
    return argToDouble(3, 1, "green transmissivity");
}

double GlassMaterial::blueTrans() const
{
    return argToDouble(3, 2, "green transmissivity");
}

double GlassMaterial::refraction() const
{
    return argToDouble(3, 3, "index of refraction", 1.52);
}

bool GlassMaterial::validateArg3(const std::string &value, int position) const
{
    switch(position){
    case 0:
        return checkDoubleValue(value, 0, 1, "red transmissivity", redTrans());
    case 1:
        return checkDoubleValue(value, 0, 1, "green transmissivity", greenTrans());
    case 2:
        return checkDoubleValue(value, 0, 1, "blue transmissivity", blueTrans());
    case 3:
        return checkDoubleValue(value, 0, 5, "index of refraction", refraction());
    default:
        STADIC_LOG(Severity::Warning, "A glass primitive has no third argument in position " + stadic::toString(position));
    }
    return false;
}

//BSDF
BSDFMaterial::BSDFMaterial() : RadPrimitive()
{
    RadPrimitive::setType("bsdf");
    std::vector<std::string> arg1 = {"0","null","0","0","0","."};
    initArg(1,arg1);
}

BSDFMaterial::BSDFMaterial(double thickness, std::string BSDFfile, double ux, double uy, double uz)
    : RadPrimitive()
{
    RadPrimitive::setType("bsdf");
    std::vector<std::string> arg1 = { "0", "null", "0", "0", "0", "." };
    initArg(1, arg1);
    setArg1(stadic::toString(thickness),0);
    setArg1(BSDFfile, 1);
    setArg1(stadic::toString(ux), 2);
    setArg1(stadic::toString(uy), 3);
    setArg1(stadic::toString(uz), 4);
    setArg1(".", 5);
}

// Setters
bool BSDFMaterial::setThickness(double value)
{
    return setArg1(stadic::toString(value),0);
}

bool BSDFMaterial::setBSDFfile(std::string name)
{
    return setArg1(name, 1);
}

bool BSDFMaterial::setUX(double value)
{
    return setArg1(stadic::toString(value), 2);
}

bool BSDFMaterial::setUY(double value)
{
    return setArg1(stadic::toString(value), 3);
}

bool BSDFMaterial::setUZ(double value)
{
    return setArg1(stadic::toString(value), 4);
}

// Getters
double BSDFMaterial::thickness() const
{
    return argToDouble(1, 0, "thickness");
}

std::string BSDFMaterial::bsdfFile() const
{
    return getArg1(1);
}

double BSDFMaterial::ux() const
{
    return argToDouble(1, 2, "x vector");
}

double BSDFMaterial::uy() const
{
    return argToDouble(1, 3, "y vector");
}

double BSDFMaterial::uz() const
{
    return argToDouble(1, 4, "z vector");
}

bool BSDFMaterial::validateArg1(const std::string &value, int position) const
{
    switch(position){
    case 0:
        return checkDoubleValue(value, "thickness", thickness());
    case 1:
        //This should test to make sure the file exists.
        break;
    case 2:
        return checkDoubleValue(value, "x vector", ux());
    case 3:
        return checkDoubleValue(value, "y vector", uy());
    case 4:
        return checkDoubleValue(value, "z vector", uz());
    case 5:
        if(value!=".") {
            STADIC_LOG(Severity::Error, "The last argument on the first line of the BSDF material\n\tshould be a period \".\".");
        }
        break;
    default:
        STADIC_LOG(Severity::Warning, "A bsdf primitive has no first argument in position " + stadic::toString(position));
        return false;
    }
    return true;
}

}
