/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
 * Copyright (c) 2015, Jason W. DeGraw
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

#ifndef RADPRIMITIVE_H
#define RADPRIMITIVE_H

#include <array>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include "stadicapi.h"
#include "functions.h"
#include "logging.h"
#include "sharedvector.h"
#include <boost/optional.hpp>

namespace stadic {

class STADIC_API RadPrimitive
{
public:
    enum Type {Source, Sphere, Bubble, Polygon, Cone, Cup, Cylinder, Tube, Ring, Instance,
               Mesh, Light, Illum, Glow, Spotlight, Mirror, Prism1, Prism2, Plastic, Metal,
               Trans, Plastic2, Metal2, Trans2, Mist, Dielectric, Interface, Glass, Plasfunc,
               Metfunc, Transfunc, BRTDfunc, Plasdata, Metdata, Transdata, BSDF, Antimatter,
               Texfunc, Texdata, Colorfunc, Brightfunc, Colordata, Brightdata, Colorpict, Colortext,
               Brighttext, Mixfunc, Mixdata, Mixpict, Mixtext, Unknown};
    RadPrimitive();
    virtual ~RadPrimitive()
    {}

    bool isGeometry() const;  //!< Returns true if the primitive is of a geometry type
    bool isMaterial() const;  //!< Returns true if the primitive is of a material type
    virtual bool isVoid() const  //!< Returns true if the primitive is a void primitive
    {
        return false;
    }

    //Setters
    virtual bool setType(const std::string &type);  //!< Set the primitive type, returns true on success. This function is overloaded by most subclasses
    void setName(const std::string &name);  //!< Set the name of the primitive
    virtual bool setArg1(std::vector<std::string> vals);  //!< Set the arguments on line one from a vector of strings, returns true on success
    virtual bool setArg1(const std::string &arg, int position);  //!< Set an argument on line one given the position of the argument, returns true on success
    virtual bool setArg2(std::vector<std::string> vals);  //!< Set the arguments on line two from a vector of strings, returns true on success
    virtual bool setArg2(const std::string &arg, int position);  //!< Set an argument on line two given the position of the argument, returns true on success
    virtual bool setArg3(std::vector<std::string> vals);  //!< Set the arguments on line three from a vector of strings, returns true on success
    virtual bool setArg3(const std::string &arg, int position);  //!< Set an argument on line three given the position of the argument, returns true on success

    //Getters
    std::string modifierName() const;  //!< Returns the modifier name associated with the primitive
    boost::optional<std::string>  modifierOverride() const; //!< Returns the modifier name that will be used, if empty then the modifier pointer is used
    Type type() const;  //!< Returns the primitive type as an enumeration
    std::string typeString() const;  //!< Returns the type as a string
    virtual std::string name() const;  //!< Returns the identifier of the primitive
    std::vector<std::string> arg1() const;  //!< Returns the first line of arguments as a vector
    std::vector<std::string> arg2() const;  //!< Returns the second line of arguments as a vector
    std::vector<std::string> arg3() const;  //! Returns the third line of arguments as a vector

    virtual std::string getArg1(int position) const; //!< Returns a given argument from the first line as a string, throws for out of range values
    virtual std::string getArg2(int position) const; //!< Returns a given argument from the second line as a string, throws for out of range values
    virtual std::string getArg3(int position) const; //!< Returns a given argument from the third line as a string, throws for out of range values
    virtual std::string getArg1(int position, const std::string &defaultValue) const; //!< Returns a given argument from the first line as a string or a default if the position is out of range
    virtual std::string getArg2(int position, const std::string &defaultValue) const; //!< Returns a given argument from the second line as a string or a default if the position is out of range
    virtual std::string getArg3(int position, const std::string &defaultValue) const; //!< Returns a given argument from the third line as a string or a default if the position is out of range

    std::string toRad() const;  //!< Returns the primitive in Radiance's primitive file format
    static RadPrimitive *fromRad(std::istream &data);
    static std::shared_ptr<RadPrimitive> fromRad(std::stringstream &data, const shared_vector<RadPrimitive> &knownPrimitives);
    static bool buildModifierTree(shared_vector<RadPrimitive> &primitives);  //!< Attempts to build connections between primitives and modifier primitives, returns true on success
    static bool checkModifierTree(shared_vector<RadPrimitive> &primitives);  //!< Returns true if the input list of primitives represents a complete Radiance primitive input file
    static std::shared_ptr<RadPrimitive> sharedVoid();  //!< Returns a shared pointer to a void primitive - use this to limit the number of void primitives in memory

protected:
    void initArg(int number, std::vector<std::string> arg);
    bool checkDoubleValue(const std::string &value, double min, double max, const std::string &variable, 
        double current) const;
    bool checkDoubleValue(const std::string &value, double min, double max, double recMin, double recMax,
        const std::string &variable, double current) const;
    bool checkDoubleValueMin(const std::string &value, double min, const std::string &variable,
        double current, bool inclusive) const;
    bool checkDoubleValue(const std::string &value, const std::string &variable, double current) const;
    double argToDouble(int number, int position, const std::string &variable) const;
    double argToDouble(int number, int position, const std::string &variable, double defaultValue) const;
    virtual bool setModifierName(const std::string &name);  //!< Set the modifier name if possible
    virtual bool validateArg1(const std::string &value, int position) const 
    {
        STADIC_LOG(Severity::Warning, "A " + typeString() + " primitive has no first argument in position "
            + stadic::toString(position));
        return false;
    }
    virtual bool validateArg1(const std::vector<std::string> &arg) const;
    virtual bool validateArg2(const std::string &value, int position) const
    {
        STADIC_LOG(Severity::Warning, "A " + typeString() + " primitive has no second argument in position "
            + stadic::toString(position));
        return false;
    }
    virtual bool validateArg2(const std::vector<std::string> &arg) const;
    virtual bool validateArg3(const std::string &value, int position) const
    {
        STADIC_LOG(Severity::Warning, "A " + typeString() + " primitive has no third argument in position "
            + stadic::toString(position));
        return false;
    }
    virtual bool validateArg3(const std::vector<std::string> &arg) const;
    virtual bool extendArg3() const { return false; }
    
private:
    static std::shared_ptr<RadPrimitive> s_void;
    static std::array<std::string,51> s_typeStrings;

    static Type typeFromString(const std::string &string);
    std::shared_ptr<RadPrimitive> findModifier(const std::string &name, const shared_vector<RadPrimitive> &knownPrimitives);

    boost::optional<std::string> m_modifierName;     // Optional storage of the modifier name - should only hold the string before building the tree
    std::shared_ptr<RadPrimitive> m_modifier;        // Shared pointer to the modifier primitive
    std::string m_TypeString;                        // String type
    std::string m_Name;                              // String name
    std::vector<std::string> m_Arg1;                 // Vector holding arguments on line 1
    std::vector<std::string> m_Arg2;                 // Vector holding arguments on line 2
    std::vector<std::string> m_Arg3;                 // Vector holding arguments on line 3

};

class STADIC_API UnknownPrimitive : public RadPrimitive
{
public:
    UnknownPrimitive()
    {}

protected:
    virtual bool validateArg1(const std::string &, int) const
    {
        return true;
    }

    virtual bool validateArg2(const std::string &, int) const
    {
        return true;
    }

    virtual bool validateArg3(const std::string &, int) const
    {
        return true;
    }
};

class STADIC_API VoidPrimitive : public RadPrimitive
{
public:
    VoidPrimitive() 
    {
        setName("void");
    }

    bool isVoid() const
    {
        return true;
    }

protected:
    virtual bool validateArg1(const std::string &, int) const
    {
        return false;
    }

    virtual bool validateArg2(const std::string &, int) const
    {
        return false;
    }

    virtual bool validateArg3(const std::string &, int) const
    {
        return false;
    }

};

}

#endif // RADPRIMITIVE_H
