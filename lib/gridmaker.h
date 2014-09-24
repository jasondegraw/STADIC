#ifndef GRIDMAKER_H
#define GRIDMAKER_H

#include <vector>
#include <string>
#include "radprimitive.h"
#include "radfiledata.h"
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry/core/cs.hpp>
#include "stadicapi.h"

namespace stadic {

class STADIC_API GridMaker
{
public:
    GridMaker(std::vector<std::string> fileList);

    //Setters
    //Points

    //InputData  These functions must be used before the utilities can be used.
    void setLayerNames(std::vector<std::string> layerList);
    void addLayerName(std::string string);
    void setSpaceX(double x);
    void setSpaceY(double y);
    void setOffset(double val);
    void setOffsetX(double x);
    void setOffsetY(double y);
    void setOffsetZ(double z);
    void setZHeight(double z);



    //Getters
    //Points
    //InputData
    std::vector<std::string> layerNames();
    double spaceX();
    double spaceY();
    double offsetX();
    double offsetY();
    double zHeight();

    //Utilities
    bool makeGrid();
    bool writePTS(std::ostream& out);
    bool writePTS();
    bool writePTS(std::string file);
    bool writePTScsv(std::string file);
    bool viewPTS(std::string location);


private:
    //Points
    std::vector<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>> m_PointSet;
    std::vector<std::vector<double>> m_FinalPoints;
    //Polygons
    boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>, true, true>> m_UnitedPolygon;


    //InputData
    std::vector<std::string> m_LayerNames;
    RadFileData m_RadFile;
    double m_SpaceX;
    double m_SpaceY;
    double m_OffsetX;
    double m_OffsetY;
    double m_ZHeight;
    double m_OffsetZ;
    bool m_useZOffset;
    double m_Offset;
    bool m_UseOffset;
    std::string m_RadPolyFile;
    std::string m_RadPtsFile;
    std::string m_oconvFile;
    std::string m_picFile;

    //Dimensional
    double m_MinX;
    double m_MinY;
    double m_MaxX;
    double m_MaxY;
    double m_MaxXRad;
    double m_MinXRad;
    double m_MaxYRad;
    double m_MinYRad;

    //Functions
    bool parseRad();
    //Dimensional
    void setMinX(double x);
    void setMaxX(double x);
    void setMinY(double y);
    void setMaxY(double y);

    bool insetPolygons();
    void boundingBox(boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>, true, true>> polygonSet);
    bool testPoints();
    void addTestPoints(double x, double y);
    void zHeights();
    bool writeRadPoly(std::string file);
    bool writeRadPoints(std::string file);
    bool runoconv(std::string file);
    bool runrpict();

signals:

public slots:

};

}

#endif // GRIDMAKER_H
