#include "geometryprimitives.h"
#include "logging.h"

namespace stadic {
//Polygon
PolygonGeometry::PolygonGeometry() : RadPrimitive()
{
    RadPrimitive::setType("polygon");
    std::vector<QString> arg3 = {};
    initArg(3,arg3);
}

PolygonGeometry::PolygonGeometry(std::vector<double> points) : RadPrimitive()
{
    RadPrimitive::setType("polygon");
    for (int i=0;i<points.size();i++){
        if (!setArg(3,QString().sprintf("%g",points[i]),i)){
            ERROR("The setting of the polygon arguments has failed.");
        }
    }
}

// Setters
bool PolygonGeometry::setPoints(std::vector<double> points)
{
    for (int i=0;i<points.size();i++){
        if (!setArg(3,QString().sprintf("%g",points[i]),i)){
            ERROR("The setting of the polygon arguments has failed.");
            return false;
        }
    }
    return true;
}

// Getters
std::vector<double> PolygonGeometry::points() const{
    std::vector<double> points;
    for (int i=0;i<arg3().size();i++){
        points.push_back(QString(arg3()[i]).toDouble());
    }
    return points;
}


//Private
bool PolygonGeometry::validateArg(int number, QString value, int position) const
{
    if(number==3) {
        bool ok;
        double dval = value.toDouble(&ok);
        if (ok){
            return true;
        }
    }
    return false;
}

bool PolygonGeometry::validateArg(int number, std::vector<QString> arg) const
{
    if(number==3) {
        if(arg.size()%3 != 0) {
            return false;
        }
        for(QString value : arg) {
            bool ok;
            double dval = value.toDouble(&ok);
            if(ok) {
                return true;
            }
        }
    }
    return false;
}



}
