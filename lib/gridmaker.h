#ifndef GRIDMAKER_H
#define GRIDMAKER_H

#include <QObject>
#include <QPointF>
#include <QPolygonF>
#include <vector>
#include "radgeometry.h"

class GridMaker : public QObject
{
    Q_OBJECT
public:
    explicit GridMaker(QObject *parent = 0);

    //Setters
    //Points
    void setTestPoints(double x, double y);
    //InputData  These functions must be used before the utilities can be used.
    void setLayerNames(QString string);
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
    std::vector<QString> layerNames();
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
    bool parseRad(QString file);
    bool makeGrid();
    bool writePTS(QString file);
    bool writePTScsv(QString file);
    bool writeRadPoly(QString file);

private:
    //Points
    std::vector<QPointF> m_TestPoints;

    //Polygons
    std::vector<QPolygonF> m_Polygons;
    QPolygonF m_UnitedPolygon;

    //InputData
    std::vector<QString> m_LayerNames;
    std::vector<RadGeometry *> m_RadGeo;
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

#endif // GRIDMAKER_H
