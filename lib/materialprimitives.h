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

#ifndef MATERIALPRIMITIVES_H
#define MATERIALPRIMITIVES_H
#include "radprimitive.h"

#include "stadicapi.h"

namespace stadic{
//PLASTIC
class STADIC_API PlasticMaterial : public RadPrimitive
{
public:
    PlasticMaterial();
    PlasticMaterial(double red, double green, double blue, double spec, double rough);

    // Setters
    bool setType(std::string){return false;}
    bool setRed(double value);
    bool setGreen(double value);
    bool setBlue(double value);
    bool setSpecularity(double value);
    bool setRoughness(double value);
    // Getters
    double red() const;
    double green() const;
    double blue() const;
    double specularity() const;
    double roughness() const;

protected:
    bool validateArg3(const std::string &value, int position) const;
};

//METAL
class STADIC_API MetalMaterial : public RadPrimitive
{
public:
    MetalMaterial();
    MetalMaterial(double red, double green, double blue, double spec, double rough);

    // Setters
    bool setType(std::string){return false;}
    bool setRed(double value);
    bool setGreen(double value);
    bool setBlue(double value);
    bool setSpecularity(double value);
    bool setRoughness(double value);
    // Getters
    double red() const;
    double green() const;
    double blue() const;
    double specularity() const;
    double roughness() const;

protected:
    bool validateArg3(const std::string &value, int position) const;
};

//TRANS
class STADIC_API TransMaterial : public RadPrimitive
{
public:
    TransMaterial();
    TransMaterial(double red, double green, double blue, double spec, double rough, double trans, double transpec);

    // Setters
    bool setType(std::string){return false;}
    bool setRed(double value);
    bool setGreen(double value);
    bool setBlue(double value);
    bool setSpecularity(double value);
    bool setRoughness(double value);
    bool setTransmission(double value);
    bool setTransmissivity(double value);
    bool setTransSpecular(double value);

    // Getters
    double red() const;
    double green() const;
    double blue() const;
    double specularity() const;
    double roughness() const;
    double transmissivity() const;
    double transSpecular() const;

protected:
    bool validateArg3(const std::string &value, int position) const;
};

//GLASS
class STADIC_API GlassMaterial : public RadPrimitive
{
public:
    GlassMaterial();
    GlassMaterial(double redTrans, double greenTrans, double blueTrans);
    GlassMaterial(double redTrans, double greenTrans, double blueTrans, double refrac);
    // Setters
    bool setType(std::string){return false;}
    bool setRedTrans(double value);
    bool setGreenTrans(double value);
    bool setBlueTrans(double value);
    bool setRefraction(double value);

    // Getters
    double redTrans() const;
    double greenTrans() const;
    double blueTrans() const;
    double refraction() const;

protected:
    bool validateArg3(const std::string &value, int position) const;
    bool extendArg3() const { return true; }
};

//BSDF
class STADIC_API BSDFMaterial : public RadPrimitive
{
public:
    BSDFMaterial();
    BSDFMaterial(double thickness, std::string BSDFfile, double ux, double uy, double uz);

    // Setters
    bool setType(std::string){return false;}
    bool setThickness(double value);
    bool setBSDFfile(std::string name);
    bool setUX(double value);
    bool setUY(double value);
    bool setUZ(double value);
    // Getters
    double thickness() const;
    std::string bsdfFile() const;
    double ux() const;
    double uy() const;
    double uz() const;

protected:
    bool validateArg1(const std::string &value, int position) const;
};

}
#endif // MATERIALPRIMITIVES_H
