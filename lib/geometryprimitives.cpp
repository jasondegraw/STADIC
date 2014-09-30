#include "geometryprimitives.h"
#include "logging.h"
#include "functions.h"

namespace stadic {
//Polygon
PolygonGeometry::PolygonGeometry() : RadPrimitive()
{
    RadPrimitive::setType("polygon");
}

PolygonGeometry::PolygonGeometry(std::vector<double> points) : RadPrimitive()
{
    RadPrimitive::setType("polygon");
    setPoints(points);
}

// Setters
bool PolygonGeometry::setPoints(std::vector<double> points)
{
    if(points.size()%3 != 0) {
        STADIC_ERROR("The setting of the polygon points has failed - argument size must be a multiple of 3.");
        return false;
    } else {
        std::vector<std::string> args;
        for(double value : points) {
            args.push_back(stadic::toString(value));
            initArg(3, args);
        }
    }
    return true;
}

// Getters
std::vector<double> PolygonGeometry::points() const
{
    std::vector<double> points;
    for (int i=0;i<arg3().size();i++){
        points.push_back(stadic::toDouble(arg3()[i]));
    }
    return points;
}


//Private
bool PolygonGeometry::validateArg(int number, std::string value, int position) const
{
    if(number==3) {
        bool ok;
        double dval = stadic::toDouble(value, &ok);
        if (ok){
            return true;
        }
    }
    return false;
}

bool PolygonGeometry::validateArg(int number, std::vector<std::string> arg) const
{
    if(number==3) {
        if(arg.size()%3 != 0) {
            return false;
        }
        for(std::string value : arg) {
            bool ok;
            double dval = stadic::toDouble(value, &ok);
            if(ok) {
                return true;
            }
        }
    }
    return false;
}

}
