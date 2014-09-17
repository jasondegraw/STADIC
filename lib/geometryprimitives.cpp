#include "geometryprimitives.h"
#include "logging.h"

namespace stadic {
//Polygon
PolygonGeometry::PolygonGeometry() : RadPrimitive()
{
    RadPrimitive::setType("polygon");
    std::vector<std::string> arg3 = {};
    initArg(3,arg3);
}

PolygonGeometry::PolygonGeometry(std::vector<double> points) : RadPrimitive()
{
    RadPrimitive::setType("polygon");
    for (int i=0;i<points.size();i++){
        if (!setArg(3,std::to_string(points[i]),i)){
            STADIC_ERROR("The setting of the polygon arguments has failed.");
        }
    }
}

// Setters
bool PolygonGeometry::setPoints(std::vector<double> points)
{
    for (int i=0;i<points.size();i++){
        if (!setArg(3,std::to_string(points[i]),i)){
            STADIC_ERROR("The setting of the polygon arguments has failed.");
            return false;
        }
    }
    return true;
}

// Getters
std::vector<double> PolygonGeometry::points() const{
    std::vector<double> points;
    for (int i=0;i<arg3().size();i++){
        points.push_back(atof(arg3()[i].c_str()));
    }
    return points;
}


//Private
bool PolygonGeometry::validateArg(int number, std::string value, int position) const
{
    if(number==3) {
        //bool ok;
        double dval = atof(value.c_str());
        return true;
        /*
        if (ok){
            return true;
        }
        */
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
            //bool ok;
            double dval = atof(value.c_str());
            return true;
            /*
            if(ok) {
                return true;
            }
            */
        }
    }
    return false;
}



}
