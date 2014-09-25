#ifndef LEAKCHECK_H
#define LEAKCHECK_H

#include <QObject>
#include <QPolygonF>
#include <QPointF>
#include <vector>
#include <string>
#include "stadicapi.h"
#include "radfiledata.h"
#include "radprimitive.h"


namespace stadic{

class STADIC_API LeakCheck : public QObject
{
    Q_OBJECT
public:
    explicit LeakCheck(QObject *parent = 0);
    bool isEnclosed();                                                  //Main function call to determine if the model is enclosed


//Setters
    bool setRadFile(std::vector<std::string> files);                    //Function to set the rad files to be used for leak checking
    bool setFloorLayers(std::vector<std::string> layers);               //Function to set the floor layers to ensure the point is within the polygons
    bool setX(double x);                                                //Function to set the x coordinate of the analysis point
    bool setY(double y);                                                //Function to set the y coordinate of the analysis point
    bool setZ(double z);                                                //Function to set the z coordinate of the analysis point
    bool setPoint(std::vector<double> point);                           //Function to set the x, y, and z coordinates of the analysis point via a vector
    bool setReflectance(int ref);                                       //Function to set the reflectance which must be either 1 or 0

private:
    std::vector<std::string> m_RadFiles;                                //Vector holding the rad files
    std::vector<std::string> m_FloorLayers;                             //Vector holding the floor layers
    std::vector<double> m_TestPoint;                                    //Vector holding the analysis point
    RadFileData m_RadGeo;                                               //RadFileData for parsing the rad files
    std::vector<QPolygonF> m_Polygons;                                  //Vector of polygons
    QPolygonF m_UnitedPolygon;                                          //Variable holding the multipolygons
    double m_Reflectance;                                               //Variable holding the reflectance

    bool unitePolygons();                                               //Function to unite the polygons
    bool checkPoint();                                                  //Function to test whether the analysis point is within the polygon layers listed
    bool writePTS();                                                    //Function to write the pts file
    bool writeExtraRad();                                               //Function to write the sky and ground rad files
    bool xformModifiers();                                              //Function to xform the layers to black if necessary
    bool createOct();                                                   //Function to create the octree
    bool runCalc();                                                     //Function to run the calculation


signals:

public slots:

private slots:


};

}

#endif // LEAKCHECK_H
