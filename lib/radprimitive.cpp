/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
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

#include "radprimitive.h"
#include "geometryprimitives.h"
#include "materialprimitives.h"
#include <iostream>
#include "functions.h"
#include <sstream>

namespace stadic {

std::shared_ptr<RadPrimitive> RadPrimitive::s_void;

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
bool RadPrimitive::setModifierName(const std::string &name)
{
    if(!m_modifier) {
        m_modifierName = name;
        return true;
    }
    return false;
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

void RadPrimitive::setModifier(const std::shared_ptr<RadPrimitive> &modifier)
{
    m_modifierName = boost::none;
    m_modifier = modifier;
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
std::string RadPrimitive::modifierName() const {
    if(m_modifierName) {
        return m_modifierName.get();
    } else if(m_modifier) {
        return m_modifier->name();
    }
    return std::string();
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

static std::string nextNonComment(std::istream &data)
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
    stream << modifierName() << " " << typeString() << " " << name() << std::endl;
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

std::shared_ptr<RadPrimitive> RadPrimitive::fromRad(RadParser &data)
{
    RadPrimitive *rad;
    boost::optional<std::string> input;

    // Currently look for "modifier type identifier"
    input = data.next();
    if(!input) {
        STADIC_LOG(Severity::Warning, "Failed to read a modifier from line " + std::to_string(data.linenumber()) + " of input file");
        return nullptr;
    }
    std::string modifier = input.get();

    input = data.next();
    if(!input) {
        STADIC_LOG(Severity::Warning, "Failed to read a primitive type from line " + std::to_string(data.linenumber()) + " of input file");
        return nullptr;
    }
    std::string type = input.get();

    input = data.next();
    if(!input) {
        STADIC_LOG(Severity::Warning, "Failed to read an identifier from line " + std::to_string(data.linenumber()) + " of input file");
        return nullptr;
    }
    std::string id = input.get();

    switch(typeFromString(type)) {
    case Polygon:
        rad = new PolygonGeometry();
        break;
        //case Ring:
        //    rad = new RingGeometry();
        //    break;
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
        STADIC_LOG(Severity::Warning, "Unknown primitive \"" + type + "\" in input.");
        rad = new UnknownPrimitive();
        rad->setType(type);
        break;
    }
    rad->setModifierName(modifier);
    rad->setName(id);

    int nargs;

    // Read the first argument line
    input = data.next();
    if(!input) {
        STADIC_LOG(Severity::Warning, "Failed to read a first argument count from line " + std::to_string(data.linenumber()) + " of input file");
        delete rad;
        return nullptr;
    }

    nargs = toInteger(input.get());
    if(nargs>0){
        std::vector<std::string> args;
        for(int i = 0; i<nargs; i++){
            input = data.next();
            if(!input) {
                STADIC_LOG(Severity::Warning, "Insufficient first argument data on line " + std::to_string(data.linenumber()) + " of input file");
                delete rad;
                return nullptr;
            }
            args.push_back(input.get());
        }
        if(!rad->setArg1(args)) {
            STADIC_LOG(Severity::Error, "Incorrect input in first argument set for " + type + " primitive, identifier "
                + id);
            delete rad;
            return nullptr;
        }
    }

    // Read the first argument line
    input = data.next();
    if(!input) {
        STADIC_LOG(Severity::Warning, "Failed to read a second argument count from line " + std::to_string(data.linenumber()) + " of input file");
        delete rad;
        return nullptr;
    }

    nargs = toInteger(input.get());
    if(nargs>0){
        std::vector<std::string> args;
        for(int i = 0; i<nargs; i++){
            input = data.next();
            if(!input) {
                STADIC_LOG(Severity::Warning, "Insufficient second argument data on line " + std::to_string(data.linenumber()) + " of input file");
                delete rad;
                return nullptr;
            }
            args.push_back(input.get());
        }
        if(!rad->setArg2(args)) {
            delete rad;
            STADIC_LOG(Severity::Error, "Incorrect input in second argument set for " + type + " primitive, identifier "
                + id);
            return nullptr;
        }
    }

    // Read the first argument line
    input = data.next();
    if(!input) {
        STADIC_LOG(Severity::Warning, "Failed to read a third argument count from line " + std::to_string(data.linenumber()) + " of input file");
        delete rad;
        return nullptr;
    }

    nargs = toInteger(input.get());
    if(nargs>0){
        std::vector<std::string> args;
        for(int i = 0; i<nargs; i++){
            input = data.next();
            if(!input) {
                STADIC_LOG(Severity::Warning, "Insufficient third argument data on line " + std::to_string(data.linenumber()) + " of input file");
                delete rad;
                return nullptr;
            }
            args.push_back(input.get());
        }
        if(!rad->setArg3(args)) {
            STADIC_LOG(Severity::Error, "Incorrect input in third argument set for " + type + " primitive, identifier "
                + id);
            delete rad;
            return nullptr;
        }
    }
    
    return std::shared_ptr<RadPrimitive>(rad);
}

bool RadPrimitive::buildModifierTree(shared_vector<RadPrimitive> &primitives)
{
    // The first primitive has to have a void modifier
    if(primitives.size() > 0) {
        auto current = primitives[0];
        if(current->m_modifierName) {
            if(current->m_modifierName.get() != "void") {
                STADIC_LOG(Severity::Warning, "Primitive \'" + current->name()
                    + "\' is first primitive in primitives vector, but has non-void modifier \'" + current->m_modifierName.get() + "\'");
                return false;
            }
        } else if(current->m_modifier) {
            if(current->m_modifier->m_Name != "void") {
                STADIC_LOG(Severity::Warning, "Primitive \'" + current->name()
                    + "\' is first primitive in primitives vector, but has non-void modifier \'" + current->m_modifier->m_Name + "\'");
                return false;
            }
        } else {
            if(!current->m_modifier) {
                STADIC_LOG(Severity::Warning, "Primitive \'" + current->name() + "\' has no modifier");
                return false;
            }
        }
        current->m_modifier = RadPrimitive::sharedVoid();
        current->m_modifierName = boost::none;
    }
    for(unsigned i = 1; i < primitives.size(); i++) {
        auto current = primitives[i];
        auto name = current->m_modifierName;
        if(!name) {
            if(!current->m_modifier) {
                STADIC_LOG(Severity::Warning, "Primitive \'" + current->name() + "\' has no modifier");
                return false;
            }
            // Loop and find the modifier
            for(unsigned j = i - 1; j >= 0; j++) {
                if(current->m_modifier == primitives[j]) {
                    break;
                }
                if(j == i){
                    STADIC_LOG(Severity::Warning, "Failed to find modifier \'" + current->m_modifier->name() + "\' for primitive \'"
                        + current->name() + "\'");
                    return false;
                }
            }
        } else {
            if(name.get() == "void") {
                current->m_modifier = RadPrimitive::sharedVoid();
                current->m_modifierName = boost::none;
                continue;
            }
            // Loop and find the modifier by name
            for(unsigned j = i - 1; j >= 0; j--) {
                if(name.get() == primitives[j]->name()) {
                    current->m_modifier = primitives[j];
                    current->m_modifierName = boost::none;
                    break;
                }
                if(j == i){
                    STADIC_LOG(Severity::Warning, "Failed to find modifier \'" + name.get() + "\' for primitive \'"
                        + current->name() + "\'");
                    return false;
                }
            }
        }
    }
    return true;
}

bool RadPrimitive::checkModifierTree(shared_vector<RadPrimitive> &primitives)
{
    // The first primitive has to have a void modifier
    if(primitives.size() > 0) {
        auto current = primitives[0];
        if(current->m_modifierName) {
            if(current->m_modifierName.get() != "void") {
                STADIC_LOG(Severity::Warning, "Primitive \'" + current->name()
                + "\' is first primitive in primitives vector, but has non-void modifier \'" + current->m_modifierName.get() + "\'");
                return false;
            }
        } else if(current->m_modifier) {
            if(current->m_modifier->m_Name != "void") {
                STADIC_LOG(Severity::Warning, "Primitive \'" + current->name()
                    + "\' is first primitive in primitives vector, but has non-void modifier \'" + current->m_modifier->m_Name + "\'");
                return false;
            }
        } else {
            if(!current->m_modifier) {
                STADIC_LOG(Severity::Warning, "Primitive \'" + current->name() + "\' has no modifier");
                return false;
            }
        }
    }
    for(unsigned i = 1; i < primitives.size(); i++) {
        auto current = primitives[i];
        auto name = current->m_modifierName;
        //std::cout << i << " " << current->name() << std::endl;
        if(!name) {
            if(!current->m_modifier) {
                STADIC_LOG(Severity::Warning, "Primitive \'" + current->name() + "\' has no modifier");
                return false;
            } else if(current->m_modifier->name() == "void") {
                continue;
            }
            // Loop and find the modifier
            for(unsigned j = i - 1; j >= 0; j--) {
                //std::cout << current->m_modifier->name() << " " << primitives[j]->name() << std::endl;
                if(current->m_modifier == primitives[j]) {
                    break;
                }
                if(j == i){
                    STADIC_LOG(Severity::Warning, "Failed to find modifier \'" + current->m_modifier->name() + "\' for primitive \'"
                        + current->name() + "\'");
                    return false;
                }
            }
        } else {
            if(name.get() == "void") {
                continue;
            }
            // Loop and find the modifier by name
            for(unsigned j = i - 1; j >= 0; j--) {
                //std::cout << "\t" << j << " " << primitives[j]->name() << std::endl;
                if(name.get() == primitives[j]->name()) {
                    break;
                }
                if(j == i){
                    STADIC_LOG(Severity::Warning, "Failed to find modifier \'" + name.get() + "\' for primitive \'"
                        + current->name() + "\'");
                    return false;
                }
            }
        }
    }
    return true;
}

std::shared_ptr<RadPrimitive> RadPrimitive::sharedVoid()
{
    if(!s_void) {
        s_void = std::make_shared<VoidPrimitive>();
    }
    return s_void;
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
    stadic::toDouble(value, &ok);
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

std::shared_ptr<RadPrimitive> RadPrimitive::findModifier(const std::string &name, const shared_vector<RadPrimitive> &knownPrimitives)
{
    if(name == "void") {
        return sharedVoid();
    }
    for(auto iter = knownPrimitives.end(); iter >= knownPrimitives.begin(); iter--) {
        if((*iter)->name() == name) {
            return (*iter);
        }
    }
    return std::shared_ptr<RadPrimitive>();
}

}
