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

#ifndef GEOMETRYPRIMITIVES_H
#define GEOMETRYPRIMITIVES_H
#include "radprimitive.h"

namespace stadic {
//Polygon
class STADIC_API PolygonGeometry : public RadPrimitive
{
public:
    PolygonGeometry();
    PolygonGeometry(std::vector<double> points);

    // Setters
    bool setType(std::string){return false;}
    bool setPoints(std::vector<double> points);                                     //Function to set the points of a radiance polygon

    // Getters
    std::vector<double> points() const;                                             //Function that returns the points of a radiance polygon as a vector


private:
   virtual bool validateArg(int number, std::string value, int position) const;
   virtual bool validateArg(int number, std::vector<std::string> value) const;
};

//Sphere
class STADIC_API SphereGeometry : public RadPrimitive
{
public:
    SphereGeometry();
    SphereGeometry(std::vector<double> centerPoint, double radius);

    // Setters
    bool setType(std::string){return false;}
    bool setCenterPoint(std::vector<double> centerPoint);                               //Function to set the center point of the sphere
    bool setRadius(double radius);                                                      //Function to set the radius of the sphere

    // Getters
    std::vector<double> centerPoint() const;                                            //Function that returns the center point of a sphere as a vector
    double radius() const;                                                              //Function that returns the radius of the sphere as a double

private:
   virtual bool validateArg(int number, std::string value, int position) const;
   virtual bool validateArg(int number, std::vector<std::string> value) const;
};

//Ring
class STADIC_API RingGeometry : public RadPrimitive
{
public:
    RingGeometry();
    RingGeometry(std::vector<double> centerPoint, std::vector<double> surfaceNormal, double innerRadius, double outerRadius);

    // Setters
    bool setType(std::string){return false;}
    bool setCenterPoint(std::vector<double> centerPoint);                               //Function to set the center point of the ring
    bool setSurfaceNormal(std::vector<double> surfaceNormal);                           //Function to set the surface normal of the ring
    bool setInnerRadius(double radius);                                                 //Function to set the inner radius of the ring
    bool setOuterRadius(double radius);                                                 //Function to set the outer radius of the ring

    // Getters
    std::vector<double> centerPoint() const;                                            //Function that returns the center point of a ring as a vector
    std::vector<double> surfaceNormal() const;                                          //Function that returns the surface normal of a ring as a vector
    double innerRadius() const;                                                         //Function that returns the inner radius of the ring as a double
    double outerRadius() const;                                                         //Function that returns the outer radius of the ring as a double

private:
   virtual bool validateArg(int number, std::string value, int position) const;
   virtual bool validateArg(int number, std::vector<std::string> value) const;
};

}

#endif // GEOMETRYPRIMITIVES_H
