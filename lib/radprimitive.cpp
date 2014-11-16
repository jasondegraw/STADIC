/****************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted for
 * personal and commercial purposes provided that the
 * following conditions are met:
 *
 * 1. Redistribution of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PENNSYLVANIA STATE UNIVERSITY
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************/

#include "radprimitive.h"
#include "geometryprimitives.h"
#include "materialprimitives.h"
#include <iostream>
#include "functions.h"
#include <sstream>

namespace stadic {

std::array<std::string,51> RadPrimitive::s_typeStrings = {"source", "sphere", "bubble", "polygon", "cone", "cup",
                                                          "cylinder", "tube", "ring", "instance", "mesh", "light",
                                                          "illum", "glow", "spotlight", "mirror", "prism1", "prism2",
                                                          "plastic", "metal", "trans", "plastic2", "metal2", "trans2",
                                                          "mist", "dielectric", "interface", "glass", "plasfunc",
                                                          "metfunc", "transfunc", "brtdfunc", "plasdata", "metdata",
                                                          "transdata", "bsdf", "antimatter", "texfunc", "texdata",
                                                          "colorfunc", "brightfunc", "colordata", "brightdata",
                                                          "colorpict", "colortext", "brighttext", "mixfunc", "mixdata",
                                                          "mixpict", "mixtext", "unknown"};

RadPrimitive::RadPrimitive()
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
void RadPrimitive::setModifier(const std::string &modifier)
{
    m_Modifier=modifier;
}

bool RadPrimitive::setType(const std::string &type)
{
    m_TypeString=type;
    return true;
}

void RadPrimitive::setName(const std::string &name)
{
    m_Name=name;
}

bool RadPrimitive::setArg1(std::vector<std::string> vals)
{
    if(validateArg1(vals)) {
        m_Arg1 = vals;
        return true;
    }
    return false;
}

bool RadPrimitive::setArg1(const std::string &arg, int position)
{
    if(position<m_Arg1.size()) {
        if(!validateArg1(arg,position)) {
            return false;
        }
        m_Arg1[position] = arg;
        return true;
    }
    return false;
}

bool RadPrimitive::setArg2(std::vector<std::string> vals)
{
    if(validateArg2(vals)) {
      m_Arg2=vals;
      return true;
    }
    return false;
}

bool RadPrimitive::setArg2(const std::string &arg, int position)
{
    if(position<m_Arg2.size()) {
        if(!validateArg2(arg,position)) {
            return false;
        }
        m_Arg2[position] = arg;
        return true;
    }
    return false;
}

bool RadPrimitive::setArg3(std::vector<std::string> vals)
{
    if(validateArg3(vals)) {
      m_Arg3=vals;
      return true;
    }
    return false;
}

bool RadPrimitive::setArg3(const std::string &arg, int position)
{
    if(position<m_Arg3.size()) {
        if(!validateArg3(arg,position)) {
            return false;
        }
        m_Arg3[position] = arg;
        return true;
    } else if(position == m_Arg3.size() && extendArg3()) { // Allow for possible extension of the vector by one
        if(!validateArg3(arg, position)) {
            return false;
        }
        m_Arg3.push_back(arg);
        return true;
    } else {
        STADIC_LOG(Severity::Error, "Argument 3, position \'" + toString(position) + "\' is out of range.");
    }
    return false;
}

//Getters
std::string RadPrimitive::modifier() const {
    return m_Modifier;
}
RadPrimitive::Type RadPrimitive::type() const{
    return typeFromString(m_TypeString);
}
std::string RadPrimitive::typeString() const {
    return m_TypeString;
}
std::string RadPrimitive::name() const {
    return m_Name;
}
std::vector<std::string> RadPrimitive::arg1() const {
    return m_Arg1;
}
std::vector<std::string> RadPrimitive::arg2() const {
    return m_Arg2;
}
std::vector<std::string> RadPrimitive::arg3() const{
    return m_Arg3;
}

static std::string nextNonComment(std::stringstream &data)
{
    std::string string;
    while(!data.eof()) {
        std::getline(data, string);
        string = trim(string);
        if(!string.empty()) {
            if(string.find('#')==string.npos){
                return string;
            }
        }
    }
    return std::string();
}

std::string RadPrimitive::toRad() const
{
    std::stringstream stream;
    stream << modifier() << " " << typeString() << " " << name() << std::endl;
    if(m_Arg1.size()>0) {
        stream << m_Arg1.size();
        for(const std::string &value : m_Arg1) {
            stream <<" "<<value;
        }
        stream << std::endl;
    } else {
        stream << 0 << std::endl;
    }
    if(m_Arg2.size()>0) {
        stream << m_Arg2.size();
        for(const std::string &value : m_Arg2) {
            stream <<" "<<value;
        }
        stream << std::endl;
    } else {
        stream << 0 << std::endl;
    }
    if(m_Arg3.size()>0) {
        stream << m_Arg3.size();
        for(const std::string &value : m_Arg3) {
            stream <<" "<<value;
        }
        stream << std::endl;
    } else {
        stream << 0 << std::endl;
    }
    return stream.str();
}

RadPrimitive* RadPrimitive::fromRad(std::stringstream &data)
{
    RadPrimitive *rad;

    std::string string = nextNonComment(data);
    if (string.empty()){
        return nullptr;
    }
    std::vector<std::string> list = trimmedSplit(string,' ');    // string.split(QRegExp("\\s+")); // This should be "modifier type identifier"?
    if(list.size() != 3) {
        return nullptr;
    }

    switch(typeFromString(list[1])) {
    case Polygon:
        rad = new PolygonGeometry();
        break;
    case Plastic:
        rad = new PlasticMaterial();
        break;
    case Metal:
        rad = new MetalMaterial();
        break;
    case Trans:
        rad = new TransMaterial();
        break;
    case Glass:
        rad = new GlassMaterial();
        break;
    case BSDF:
        rad = new BSDFMaterial();
        break;
    default:
        rad = new RadPrimitive();
        rad->setType(list[1]);
        break;
    }
    rad->setModifier(list[0]);
    rad->setName(list[2]);

    int nargs;
    data>>string;   //Reads number of arguments from first line
    nargs = toInteger(string);
    if (nargs>0){
        std::vector<std::string> args;
        for (int i=0;i<nargs;i++){
            data>>string;
            args.push_back(string);
        }
        if(!rad->setArg1(args)) {
            delete rad;
            STADIC_LOG(Severity::Error, "Incorrect input in first argument line for " + list[1] + " primitive, identifier "
                + list[2]);
            return nullptr;
        }
    }

    data>>string;   //Reads number of arguments from second line
    nargs = toInteger(string);
    if (nargs>0){
        std::vector<std::string> args;
        for (int i=0;i<nargs;i++){
            data>>string;
            args.push_back(string);
        }
        if(!rad->setArg2(args)) {
            delete rad;
            STADIC_LOG(Severity::Error, "Incorrect input in second argument line for " + list[1] + " primitive, identifier "
                + list[2]);
            return nullptr;
        }
    }

    data>>string;   //Reads number of arguments from third line
    nargs = toInteger(string);
    if (nargs>0){
        std::vector<std::string> args;
        for (int i=0;i<nargs;i++){
            data>>string;
            args.push_back(string);
        }
        if(!rad->setArg3(args)) {
            delete rad;
            STADIC_LOG(Severity::Error, "Incorrect input in third argument line for " + list[1] + " primitive, identifier "
                + list[2]);
            return nullptr;
        }
    }
    return rad;
}

RadPrimitive::Type RadPrimitive::typeFromString(const std::string &string)
{
    for(unsigned i=0;i<s_typeStrings.size();i++) {
        if(s_typeStrings[i] == string) {
            return (RadPrimitive::Type)i;
        }
    }
    return Unknown;
}

std::string RadPrimitive::getArg1(int position) const
{
    if(position<0 || position>=m_Arg1.size()) {
        STADIC_LOG(Severity::Error, "Argument 1, position \'" + toString(position) + "\' is out of range.");
    }
    return m_Arg1[position];
}

std::string RadPrimitive::getArg2(int position) const
{
    if(position<0 || position>=m_Arg2.size()) {
        STADIC_LOG(Severity::Error, "Argument 2, position \'" + toString(position) + "\' is out of range.");
    }
    return m_Arg2[position];
}

std::string RadPrimitive::getArg3(int position) const
{
    if(position<0 || position>=m_Arg3.size()) {
        STADIC_LOG(Severity::Error, "Argument 3, position \'" + toString(position) + "\' is out of range.");
    }
    return m_Arg3[position];
}

std::string RadPrimitive::getArg1(int position, const std::string &defaultValue) const
{
    if(position<0 || position>=m_Arg1.size()) {
        return defaultValue;
    }
    return m_Arg1[position];
}

std::string RadPrimitive::getArg2(int position, const std::string &defaultValue) const
{
    if(position<0 || position>=m_Arg2.size()) {
        return defaultValue;
    }
    return m_Arg2[position];
}

std::string RadPrimitive::getArg3(int position, const std::string &defaultValue) const
{
    if(position<0 || position>=m_Arg3.size()) {
        return defaultValue;
    }
    return m_Arg3[position];
}

void RadPrimitive::initArg(int number, std::vector<std::string> arg)
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
        STADIC_LOG(Severity::Error, "Argument number \'" + toString(number) + "\' is out of range.");
        break;
    }
}

bool RadPrimitive::validateArg1(const std::vector<std::string> &arg) const 
{
    int i = 0;
    for(const std::string &value : arg) {
        if(!validateArg1(value, i)) {
            return false;
        }
        i++;
    }
    return true;
}

bool RadPrimitive::validateArg2(const std::vector<std::string> &arg) const
{
    int i = 0;
    for(const std::string &value : arg) {
        if(!validateArg2(value, i)) {
            return false;
        }
        i++;
    }
    return true;
}

bool RadPrimitive::validateArg3(const std::vector<std::string> &arg) const
{
    int i = 0;
    for(const std::string &value : arg) {
        if(!validateArg3(value, i)) {
            return false;
        }
        i++;
    }
    return true;
}

bool RadPrimitive::checkDoubleValue(const std::string &value, double min, double max, const std::string &variable, double current) const
{
    bool ok;
    double dval = stadic::toDouble(value, &ok);
    if(!ok) {
        STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
            + " primitive must be a floating point number, value remains " + stadic::toString(current) + ".");
        return false;
    }
    if(dval < min) {
        STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
            + " primitive cannot be less than " + stadic::toString(min) + ", value remains " + stadic::toString(current) + ".");
        return false;
    } else if(dval > max) {
        STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
            + " primitive cannot be greater than " + stadic::toString(max) + ", value remains " + stadic::toString(current) + ".");
        return false;
    }
    return true;
}

bool RadPrimitive::checkDoubleValue(const std::string &value, double min, double max,  double recMin, double recMax, 
    const std::string &variable, double current) const
{
    bool ok;
    double dval = stadic::toDouble(value, &ok);
    if(!ok) {
        STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
            + " primitive must be a floating point number, value remains " + stadic::toString(current) + ".");
        return false;
    }
    if(dval < min) {
        STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
            + " primitive cannot be less than " + stadic::toString(min) + ", value remains " + stadic::toString(current) + ".");
    } else if(dval > max) {
        STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
            + " primitive cannot be greater than " + stadic::toString(max) + ", value remains " + stadic::toString(current) + ".");
    } else {
        if(dval < recMin || dval > recMax) {
            STADIC_LOG(Severity::Warning, "The suggested range for the " + variable 
                + " value for a " + typeString() + " primitive is [" + stadic::toString(recMin) + "," + stadic::toString(recMax) + "].");
        }
        return true;
    }
    return false;
}

bool RadPrimitive::checkDoubleValueMin(const std::string &value, double min, const std::string &variable, double current, bool inclusive) const
{
    bool ok;
    double dval = stadic::toDouble(value, &ok);
    if(!ok) {
        STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
            + " primitive must be a floating point number, value remains " + stadic::toString(current) + ".");
        return false;
    }
    if(inclusive) {
        if(dval < min) {
            STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
                + " primitive cannot be less than " + stadic::toString(min) + ", value remains " + stadic::toString(current) + ".");
            return false;
        }
    } else {
        if(dval <= min) {
            STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
                + " primitive must be greater than " + stadic::toString(min) + ", value remains " + stadic::toString(current) + ".");
            return false;
        }
    }
    return true;
}

bool RadPrimitive::checkDoubleValue(const std::string &value, const std::string &variable, double current) const
{
    bool ok;
    double dval = stadic::toDouble(value, &ok);
    if(!ok) {
        STADIC_LOG(Severity::Warning, "The " + variable + " value for a " + typeString()
            + " primitive must be a floating point number, value remains " + stadic::toString(current) + ".");
        return false;
    }
    return true;
}

double RadPrimitive::argToDouble(int number, int position, const std::string &variable) const
{
    bool ok;
    std::string string;
    switch(number) {
    case 1:
        string = getArg1(position);
        break;
    case 2:
        string = getArg2(position);
        break;
    case 3:
        string = getArg3(position);
        break;
    default:
        STADIC_LOG(Severity::Fatal, "Argument number " + stadic::toString(number) + " out of range for in " + typeString() + " primitive.");
    }
        
    double value = stadic::toDouble(string, &ok);
    if(!ok) {
        STADIC_LOG(Severity::Fatal, "Corrupted " + variable + " value (\"" + getArg3(position) + "\" in argument list " + stadic::toString(number) + ", position "
            + toString(position) + ") in " + typeString() + " primitive.");
    }
    return value;
}

double RadPrimitive::argToDouble(int number, int position, const std::string &variable, double defaultValue) const
{
    bool ok;
    std::string string;
    switch(number) {
    case 1:
        string = getArg1(position);
        break;
    case 2:
        string = getArg2(position);
        break;
    case 3:
        string = getArg3(position, stadic::toString(defaultValue));
        break;
    default:
        STADIC_LOG(Severity::Fatal, "Argument number " + stadic::toString(number) + " out of range for in " + typeString() + " primitive.");
    }
    double value = stadic::toDouble(getArg3(position, stadic::toString(defaultValue)), &ok);
    if(!ok) {
        STADIC_LOG(Severity::Fatal, "Corrupted " + variable + " value (\"" + getArg3(position) + "\" in argument list 3, position "
            + toString(position) + ") in " + typeString() + " primitive.");
    }
    return value;
}

}
