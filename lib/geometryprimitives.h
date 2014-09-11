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
    bool setPoints(std::vector<double> points);

    // Getters
    std::vector<double> points() const;


private:
   virtual bool validateArg(int number, std::string value, int position) const;
   virtual bool validateArg(int number, std::vector<std::string> value) const;
};


}

#endif // GEOMETRYPRIMITIVES_H
