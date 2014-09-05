#ifndef GRIDMAKER_H
#define GRIDMAKER_H

#include <QObject>
#include <QPointF>
#include <QPolygonF>
#include <vector>
#include <string>
#include "radprimitive.h"

#include "stadicapi.h"

namespace stadic {

class STADIC_API GridMaker : public QObject
{
    Q_OBJECT
public:
    explicit GridMaker(QObject *parent = 0);

    //Setters
    //Points
    void setTestPoints(double x, double y);
    //InputData  These functions must be used before the utilities can be used.
    void setLayerNames(std::string string);
    void setSpaceX(double x);
    void setSpaceY(double y);
    void setOffsetX(double x);
    void setOffsetY(double y);
    void setZHeight(double z);
    //Dimensional
    void setMinX(double x);
    void setMaxX(double x);
    void setMinY(double y);
    void setMaxY(double y);


    //Getters
    //Points
    std::vector<QPointF> testPoints();
    //InputData
    std::vector<std::string> layerNames();
    double spaceX();
    double spaceY();
    double offsetX();
    double offsetY();
    double zHeight();
    //Dimensional
    double minX();
    double minY();
    double maxX();
    double maxY();

    //Utilities
    bool parseRad(std::string file);
    bool makeGrid(std::string file);
    bool writePTS();
    bool writePTS(std::string file);
    bool writePTScsv(std::string file);
    bool writeRadPoly(std::string file);

private:
    //Points
    std::vector<QPointF> m_TestPoints;

    //Polygons
    std::vector<QPolygonF> m_Polygons;
    QPolygonF m_UnitedPolygon;

    //InputData
    std::vector<std::string> m_LayerNames;
    std::vector<RadPrimitive *> m_RadGeo;
    double m_SpaceX;
    double m_SpaceY;
    double m_OffsetX;
    double m_OffsetY;
    double m_ZHeight;

    //Dimensional
    double m_MinX;
    double m_MinY;
    double m_MaxX;
    double m_MaxY;




signals:

public slots:

};

}

#endif // GRIDMAKER_H
