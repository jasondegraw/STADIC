#include "gridmaker.h"
#include "radprimitive.h"
#include "logging.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <fstream>
#include <QString>
#include <radfiledata.h>

namespace stadic {

GridMaker::GridMaker()
{
    m_TestPoints.clear();
}

//Setters
//Points
void GridMaker::setTestPoints(double x, double y){
    QPointF tempPoint;
    tempPoint.setX(x);
    tempPoint.setY(y);
    m_TestPoints.push_back(tempPoint);
}
//InputData
void GridMaker::setLayerNames(std::string string){
    m_LayerNames.push_back(string);
}
void GridMaker::setSpaceX(double x){
    m_SpaceX=x;
}
void GridMaker::setSpaceY(double y){
    m_SpaceY=y;
}
void GridMaker::setOffsetX(double x){
    m_OffsetX=x;
}
void GridMaker::setOffsetY(double y){
    m_OffsetY=y;
}
void GridMaker::setZHeight(double z){
    m_ZHeight=z;
}
//Dimensional
void GridMaker::setMinX(double x){
    m_MinX=x;
}
void GridMaker::setMaxX(double x){
    m_MaxX=x;
}
void GridMaker::setMinY(double y){
    m_MinY=y;
}
void GridMaker::setMaxY(double y){
    m_MaxY=y;
}


//Getters
//Points
std::vector<QPointF> GridMaker::testPoints(){
    return m_TestPoints;
}
//InputData
std::vector<std::string> GridMaker::layerNames(){
    return m_LayerNames;
}
double GridMaker::spaceX(){
    return m_SpaceX;
}
double GridMaker::spaceY(){
    return m_SpaceY;
}
double GridMaker::offsetX(){
    return m_OffsetX;
}
double GridMaker::offsetY(){
    return m_OffsetY;
}
double GridMaker::zHeight(){
    return m_ZHeight;
}
//Dimensional
double GridMaker::minX(){
    return m_MinX;
}
double GridMaker::minY(){
    return m_MinY;
}
double GridMaker::maxX(){
    return m_MaxX;
}
double GridMaker::maxY(){
    return m_MaxY;
}


//Utilities
bool GridMaker::parseRad(std::string file){
    stadic::RadFileData radGeo;
    radGeo.addRad(file);
    //set polygons
    if (radGeo.geometry().empty()){
        std::cerr<<"There are no polygons."<<std::endl;
        return false;
    }
    for (int i=0;i<radGeo.geometry().size();i++){
        QPolygonF tempPolygon;
        for (int j=0;j<radGeo.geometry().at(i)->arg3().size()/3;j++){
            tempPolygon.append(QPointF(radGeo.geometry().at(i)->arg3()[j*3].toDouble(), radGeo.geometry().at(i)->arg3()[j*3+1].toDouble()));
        }
        m_Polygons.push_back(tempPolygon);
    }
    return true;
}

bool GridMaker::makeGrid(std::string file){
    stadic::RadFileData radGeo;
    radGeo.addRad(file);
    //unite polygons that are the right layer name
    bool firstPolygon=true;
    for (int i=0;i<radGeo.geometry().size();i++){
        bool properName=false;
        for (int j=0;j<m_LayerNames.size();j++){
            if (radGeo.geometry().at(i)->modifier()==QString::fromStdString(m_LayerNames.at(j))){
                properName=true;
            }
        }
        if (firstPolygon==true && properName==true){
            firstPolygon=false;
            m_UnitedPolygon=m_Polygons.at(i);
        }else if (properName==true){
            m_UnitedPolygon=m_UnitedPolygon.united(m_Polygons.at(i));
        }
    }

    //get min and max coordinates
    setMinX(m_UnitedPolygon.boundingRect().left());
    setMaxX(m_UnitedPolygon.boundingRect().right());
    setMinY(m_UnitedPolygon.boundingRect().bottom());
    setMaxY(m_UnitedPolygon.boundingRect().top());
    if (minY()>maxY()){
        //The previous two functions for the polygon bounding rectangle I think are backwards
        setMinY(m_UnitedPolygon.boundingRect().top());
        setMaxY(m_UnitedPolygon.boundingRect().bottom());
    }
    //Create vector of test points
    double x=minX()+offsetX();
    if (x==minX()){
        x=x+spaceX();
    }
    while (x<maxX()){
        double y=minY()+offsetY();
        if (y==minY()){
            y=y+spaceY();
        }
        while (y<maxY()){
            QPointF tempPoint;
            tempPoint.setX(x);
            tempPoint.setY(y);
            //QPointF interPoint;
            if (m_UnitedPolygon.containsPoint(tempPoint, Qt::OddEvenFill)){
                //This code uses 9 points surrounding the point in question.  If all of the surrounding points are inside then this point is also inside.
                bool surroundIn=true;
                for (int i=-1;i<2;i++){
                    double tempX=x+.01*i;
                    for (int j=-1;j<2;j++){
                        double tempY=y+.01*j;
                        if (!m_UnitedPolygon.containsPoint(QPointF(tempX, tempY), Qt::OddEvenFill)){
                            surroundIn=false;
                        }
                    }
                }
                if (surroundIn==true){
                    setTestPoints(x,y);
                }
            }
            y=y+spaceY();
        }
        x=x+spaceX();
    }
    if (m_TestPoints.empty()){
        std::cerr<<"The points array is empty."<<std::endl;
        return false;
    }
    return true;
}

bool GridMaker::writePTS(std::ostream& out){
    for (int i=0;i<m_TestPoints.size();i++){
        out<<m_TestPoints[i].rx()<<" "<<m_TestPoints[i].ry()<<" "<<m_ZHeight<<" 0 0 1"<<std::endl;
    }
    return true;
}

bool GridMaker::writePTS(){
    return writePTS(std::cout);
}

bool GridMaker::writePTS(std::string file){
    std::ofstream oFile;
    oFile.open(file);
    if (!oFile.is_open()){
        ERROR("The opening of the file "+file+" has failed.");
        return false;
    }
    return writePTS(oFile);
    oFile.close();
    return true;
}

bool GridMaker::writePTScsv(std::string file){
    std::ofstream oFile;
    oFile.open(file);
    if (!oFile.is_open()){
        ERROR("The opening of the file "+file+" has failed.");
        return false;
    }
    for (int i=0;i<m_TestPoints.size();i++){
        oFile<<m_TestPoints[i].rx()<<","<<m_TestPoints[i].ry()<<","<<m_ZHeight<<",0,0,1"<<std::endl;
    }
    /*
    QFile oFile;
    oFile.setFileName(QString::fromStdString(file));
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        return false;
    }
    QTextStream out(&oFile);
    for (int i=0;i<m_TestPoints.size();i++){
        out<<m_TestPoints.at(i).rx()<<","<<m_TestPoints.at(i).ry()<<","<<m_ZHeight<<",0,0,1"<<endl;
    }
    */
    oFile.close();
    return true;
}

bool GridMaker::writeRadPoly(std::string file){
    std::ofstream oFile;
    oFile.open(file);
    if (!oFile.is_open()){
        ERROR("The opening of the file "+file+" has failed.");
        return false;
    }
    oFile<<"floor\tpolygon\tfloor1"<<std::endl;
    oFile<<"0\t0\t"<<m_UnitedPolygon.size()*3;
    for (int i=0;i<m_UnitedPolygon.size();i++){
        oFile<<endl<<m_UnitedPolygon[i].x()<<"\t"<<m_UnitedPolygon[i].y()<<"\t0";
    }
    /*
    QFile oFile;
    oFile.setFileName(QString::fromStdString(file));
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        return false;
    }
    QTextStream out(&oFile);
    out<<"floor\tpolygon\tfloor1"<<endl;
    out<<"0\t0\t"<<m_UnitedPolygon.size()*3;
    for (int i=0;i<m_UnitedPolygon.size();i++){
        out<<endl<<m_UnitedPolygon.at(i).x()<<"\t"<<m_UnitedPolygon.at(i).y()<<"\t0";
    }
    */
    oFile.close();
    return true;
}

}
