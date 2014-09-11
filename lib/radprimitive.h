#ifndef RADPRIMITIVE_H
#define RADPRIMITIVE_H

#include <QFile>
#include <QTextStream>
#include<array>
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

    bool isGeometry() const;
    bool isMaterial() const;

    //Setters
    void setModifier(std::string modifier);
    virtual bool setType(std::string type);
    void setName(std::string name);
    bool setArg1(std::vector<std::string> vals);
    bool setArg1(std::string arg, int position);
    bool setArg2(std::vector<std::string> vals);
    bool setArg2(std::string arg, int position);
    bool setArg3(std::vector<std::string> vals);
    bool setArg3(std::string arg, int position);

    virtual bool setArg(int number, std::string value, int position);

    //Getters
    std::string modifier() const;
    Type type() const;
    std::string typeString() const;
    std::string name() const;
    std::vector<std::string> arg1() const;
    std::vector<std::string> arg2() const;
    std::vector<std::string> arg3() const;

    virtual std::string getArg1(int position) const;
    virtual std::string getArg2(int position) const;
    virtual std::string getArg3(int position) const;
    virtual std::string getArg(int number, int position) const;

    static RadPrimitive *fromRad(std::stringstream &data);

protected:
    void initArg(int number, std::vector<std::string> arg);

private:
    virtual bool validateArg(int number, std::string value, int position) const {return true;}
    virtual bool validateArg(int number, std::vector<std::string> value) const {return true;}
    static std::array<std::string,51> s_typeStrings;
    static Type typeFromString(std::string string);

    std::string m_Modifier;
    std::string m_TypeString;
    std::string m_Name;
    std::vector<std::string> m_Arg1;
    std::vector<std::string> m_Arg2;
    std::vector<std::string> m_Arg3;

};

}

#endif // RADPRIMITIVE_H
