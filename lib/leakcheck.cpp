#include "leakcheck.h"
#include <QFile>
#include "logging.h"

namespace stadic{

LeakCheck::LeakCheck(QObject *parent) :
    QObject(parent)
{
}
bool LeakCheck::isEnclosed(){
    if (!checkPoint()){
        return false;
    }


}
//Setters
bool LeakCheck::setRadFile(QStringList files){
    for (int i=0;i<files.size();i++){
        QFile radFile(files[i]);
        if (!radFile.exists()){
            ERROR("The rad file named "+files[i]+" does not exist.");
            return false;
        }
        m_RadFiles.push_back(files[i]);
        if (!m_RadGeo.addRad(files[i])){
            return false;
        }
    }
    if (m_RadGeo.geometry().empty()){
        ERROR("There are no polygons in the rad files.");
        return false;
    }
    for (int i=0;i<m_RadGeo.geometry().size();i++){
        QPolygonF tempPolygon;
        for (int j=0;j<m_RadGeo.geometry()[i]->arg3().size()/3;j++){
            tempPolygon.append(QPointF(m_RadGeo.geometry()[i]->arg3()[j*3].toDouble(), m_RadGeo.geometry()[i]->arg3()[j*3+1].toDouble()));
        }
        m_Polygons.push_back(tempPolygon);
    }
    return true;
}

bool LeakCheck::setFloorLayers(QStringList layers){
    if (m_RadGeo.primitives().empty()){
        ERROR("No radiance geometry or materials have been specified.");
        return false;
    }
    for (int i=0;i<layers.size();i++){
        bool layerExists=false;
        for (int j=0;j<m_RadGeo.primitives().size();j++){
            if (layers[i]==m_RadGeo.primitives()[j]->modifier()){
                layerExists==true;
            }
        }
        if (!layerExists){
            ERROR("The layer "+layers[i]+" does not exist in the model.");
            return false;
        }
    }
    if (!unitePolygons()){
        return false;
    }
    return true;
}

bool LeakCheck::setX(double x){
    if (x<m_UnitedPolygon.boundingRect().left() || x>m_UnitedPolygon.boundingRect().right()){
        ERROR("The x coordinate is not within the min and max x coordinates.");
        return false;
    }
    if (m_TestPoint.size()<3){
        m_TestPoint.reserve(3);
    }
    m_TestPoint[0]=x;
    return true;
}

bool LeakCheck::setY(double y){
    double minY=m_UnitedPolygon.boundingRect().bottom();
    double maxY=m_UnitedPolygon.boundingRect().top();
    if (minY>maxY){
        minY=m_UnitedPolygon.boundingRect().top();
        maxY=m_UnitedPolygon.boundingRect().bottom();
    }
    if (y<minY|| y>maxY){
        ERROR("The x coordinate is not within the min and max x coordinates.");
        return false;
    }
    if (m_TestPoint.size()<3){
        m_TestPoint.reserve(3);
    }
    m_TestPoint[1]=y;
    return true;
}

bool LeakCheck::setZ(double z){
    if (m_TestPoint.size()<3){
        m_TestPoint.reserve(3);
    }
    m_TestPoint[2]=z;
    return true;
}

bool LeakCheck::setPoint(std::vector<double> point){
    if (point.size()!=3){
        ERROR("The vector of points does not contain 3 values.");
        return false;
    }
    if (point[0]<m_UnitedPolygon.boundingRect().left() || point[0]>m_UnitedPolygon.boundingRect().right()){
        ERROR("The x coordinate is not within the min and max x coordinates.");
        return false;
    }
    if (m_TestPoint.size()<3){
        m_TestPoint.reserve(3);
    }
    m_TestPoint[0]=point[0];

    double minY=m_UnitedPolygon.boundingRect().bottom();
    double maxY=m_UnitedPolygon.boundingRect().top();
    if (minY>maxY){
        minY=m_UnitedPolygon.boundingRect().top();
        maxY=m_UnitedPolygon.boundingRect().bottom();
    }
    if (point[1]<minY|| point[1]>maxY){
        ERROR("The x coordinate is not within the min and max x coordinates.");
        return false;
    }
    m_TestPoint[1]=point[1];
    m_TestPoint[2]=point[2];
    return true;
}

bool LeakCheck::unitePolygons(){
    //unite polygons that are the right layer name
    bool firstPolygon=true;
    for (int i=0;i<m_RadGeo.geometry().size();i++){
        bool properName=false;
        for (int j=0;j<m_FloorLayers.size();j++){
            if (m_RadGeo.geometry()[i]->modifier()==m_FloorLayers[j]){
                properName=true;
            }
        }
        if (firstPolygon==true && properName==true){
            firstPolygon=false;
            m_UnitedPolygon=m_Polygons[i];
        }else if (properName==true){
            m_UnitedPolygon=m_UnitedPolygon.united(m_Polygons[i]);
        }
    }
    return true;
}

bool LeakCheck::checkPoint(){
    QPointF tempPoint;
    tempPoint.setX(m_TestPoint[0]);
    tempPoint.setY(m_TestPoint[1]);
    if (m_UnitedPolygon.containsPoint(tempPoint,Qt::OddEvenFill)){
        bool surroundIn=true;
        for (int i=-1;i<2;i++){
            double tempX=m_TestPoint[0]+.01*i;
            for (int j=-1;j<2;j++){
                double tempY=m_TestPoint[1]+.01*j;
                if (!m_UnitedPolygon.containsPoint(QPointF(tempX, tempY), Qt::OddEvenFill)){
                    surroundIn=false;
                }
            }
        }
        if (surroundIn==true){
            return true;
        }else{
            WARNING("The point is on the boundary and may result in an incorrect analysis.");
            return true;
        }
    }
    ERROR("The point is not contained within the polygon.");
    return false;
}

}
