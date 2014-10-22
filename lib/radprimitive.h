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
 *    and the following Disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University
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

#ifndef RADPRIMITIVE_H
#define RADPRIMITIVE_H


#include <array>
#include <vector>
#include <string>
#include <sstream>

#include "stadicapi.h"

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

    bool isGeometry() const;                                            //Boolean that determines whether the primitive is of a geometry type
    bool isMaterial() const;                                            //Boolean that determines whether the primitive is of a material type

    //Setters
    void setModifier(std::string modifier);                             //Function to set the modifier
    virtual bool setType(std::string type);                             //Function to set the type
    void setName(std::string name);                                     //Function to set the name
    virtual bool setArg1(std::vector<std::string> vals);                //Function to set the argumens on line one from a vector of strings
    virtual bool setArg1(std::string arg, int position);                //Function to set an argument on line one given the position of the argument
    virtual bool setArg2(std::vector<std::string> vals);                //Function to set the arguments on line two from a vector of strings
    virtual bool setArg2(std::string arg, int position);                //Function to set an argument on line two given the position of the argument
    virtual bool setArg3(std::vector<std::string> vals);                //Function to set the arguments on line three from a vector of strings
    virtual bool setArg3(std::string arg, int position);                //Function to set an argument on line three given the position of the argument

    virtual bool setArg(int number, std::string value, int position);

    //Getters
    std::string modifier() const;                                       //Function that returns the modifier as a string
    Type type() const;                                                  //Function that returns the type as a type
    std::string typeString() const;                                     //Function that returns the type as a string
    std::string name() const;                                           //Function that returns the name as a string
    std::vector<std::string> arg1() const;                              //Function that returns the first line of arguments as a vector
    std::vector<std::string> arg2() const;                              //Function that returns the second line of arguments as a vector
    std::vector<std::string> arg3() const;                              //Function that returns the third line of arguments as a vector

    virtual std::string getArg1(int position) const; //!< Returns a given argument from the first line as a string, throws for out of range values
    virtual std::string getArg2(int position) const; //!< Returns a given argument from the second line as a string, throws for out of range values
    virtual std::string getArg3(int position) const; //!< Returns a given argument from the third line as a string, throws for out of range values
    virtual std::string getArg1(int position, const std::string &defaultValue) const; //!< Returns a given argument from the first line as a string or a default if the position is out of range
    virtual std::string getArg2(int position, const std::string &defaultValue) const; //!< Returns a given argument from the second line as a string or a default if the position is out of range
    virtual std::string getArg3(int position, const std::string &defaultValue) const; //!< Returns a given argument from the third line as a string or a default if the position is out of range
    virtual std::string getArg(int number, int position) const; //!< Returns a given argument from a given line, throws for out of range values
    virtual std::string getArg(int number, int position, const std::string &defaultValue) const; //!< Returns a given argument from a given line or a default if the number or position is out of range

    static RadPrimitive *fromRad(std::stringstream &data);

protected:
    void initArg(int number, std::vector<std::string> arg);
    bool checkValue(const std::string &value, int index, double min, double max, const std::string &variable, 
        const std::string &object) const;
    bool checkValue(const std::string &value, int index, double min, double max, double recMin, double recMax, 
        const std::string &variable, const std::string &object) const;
    double arg3ToDouble(int position, const std::string &variable, const std::string &object) const;
    double arg3ToDouble(int position, const std::string &variable, const std::string &object,
        double defaultValue) const;
    virtual bool validateArg1(const std::string &value, int position) const { return false; }
    virtual bool validateArg1(std::vector<std::string> value) const { return false; }
    virtual bool validateArg2(const std::string &value, int position) const { return false; }
    virtual bool validateArg2(std::vector<std::string> value) const { return false; }
    virtual bool validateArg3(const std::string &value, int position) const { return false; }
    virtual bool validateArg3(std::vector<std::string> value) const { return false; }
    virtual bool extendArg3() const { return false; }
    
private:
    virtual bool validateArg(int number, std::string value, int position) const {return true;}
    virtual bool validateArg(int number, std::vector<std::string> value) const {return true;}
    static std::array<std::string,51> s_typeStrings;
    static Type typeFromString(std::string string);

    std::string m_Modifier;                                             //Variable holding the modifier
    std::string m_TypeString;                                           //Variable holding the type
    std::string m_Name;                                                 //Variable holding the name
    std::vector<std::string> m_Arg1;                                    //Vector holding arguments on line 1
    std::vector<std::string> m_Arg2;                                    //Vector holding arguments on line 2
    std::vector<std::string> m_Arg3;                                    //Vector holding arguments on line 3

};

}

#endif // RADPRIMITIVE_H
