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

//Polygon
class STADIC_API SphereGeometry : public RadPrimitive
{
public:
    SphereGeometry();
    SphereGeometry(std::vector<double> centerPoint, double radius);

    // Setters
    bool setType(std::string){return false;}
    bool setCenterPoint(std::vector<double> centerPoint);                                     //Function to set the points of a radiance polygon
    bool setRadius(double radius);

    // Getters
    std::vector<double> centerPoint() const;                                             //Function that returns the points of a radiance polygon as a vector
    double radius() const;

private:
   virtual bool validateArg(int number, std::string value, int position) const;
   virtual bool validateArg(int number, std::vector<std::string> value) const;
};

}

#endif // GEOMETRYPRIMITIVES_H
