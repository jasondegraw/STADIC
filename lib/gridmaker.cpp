#include "gridmaker.h"
#include "radgeometry.h"
//#include "logging.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QString>

GridMaker::GridMaker(QObject *parent) :
    QObject(parent)
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
void GridMaker::setLayerNames(QString string){
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
std::vector<QString> GridMaker::layerNames(){
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
bool GridMaker::parseRad(QString file){
    QFile iFile;
    iFile.setFileName(file);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!iFile.exists()){
        std::cerr<<"The opening of the rad file failed."<<std::endl;
        return false;
    }

    QTextStream data(&iFile);
    //data<<iFile.readAll();
    while (!data.atEnd()){
        QString string;
        QString string2;
        data>>string;
        if (string.contains('#')){
            QString tempString;
            tempString=data.readLine();
        }else{
            if (string.contains("void")){
                data>>string;   //reads type
                data>>string;   //reads name
                data>>string;   //Reads number of arguments from first line
                if (string.toInt()>0){
                    for (int i=0;i<string.toInt();i++){
                        data>>string2;
                    }
                }
                data>>string;   //Reads number of arguments from second line
                if (string.toInt()>0){
                    for (int i=0;i<string.toInt();i++){
                        data>>string2;
                    }
                }
                data>>string;   //Reads number of arguments from third line
                if (string.toInt()>0){
                    for (int i=0;i<string.toInt();i++){
                        data>>string2;
                    }
                }
            }else{
                data>>string2;
                if (string2=="polygon"){
                    RadGeometry *rad=new RadGeometry(this);
                    rad->setModifier(string);
                    rad->setType(string2);
                    data>>string;
                    rad->setName(string);
                    data>>string;               //Read number of arguments from first line
                    if (string.toInt()>0){
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                            rad->setArg1(string2);
                        }
                    }
                    data>>string;               //Read number of arguments from second line
                    if (string.toInt()>0){
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                            rad->setArg2(string2);
                        }
                    }
                    data>>string;               //Read number of arguments from third line
                    if (string.toInt()>0){
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                            rad->setArg3(string2);
                        }
                    }
                    m_RadGeo.push_back(rad);
                }else{                          //If it isn't a polygon, the info still needs to be read in
                    data>>string;   //reads name
                    data>>string;   //Reads number of arguments from first line
                    if (string.toInt()>0){
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                        }
                    }
                    data>>string;   //Reads number of arguments from second line
                    if (string.toInt()>0){
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                        }
                    }
                    data>>string;   //Reads number of arguments from third line
                    if (string.toInt()>0){
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                        }
                    }
                }
            }
        }
    }
    iFile.close();
    //set polygons
    if (m_RadGeo.empty()){
        std::cerr<<"There are no polygons."<<std::endl;
        return false;
    }
    for (int i=0;i<m_RadGeo.size();i++){
        QPolygonF tempPolygon;
        for (int j=0;j<m_RadGeo.at(i)->arg3().size()/3;j++){
            tempPolygon.append(QPointF(m_RadGeo.at(i)->arg3()[j*3].toDouble(), m_RadGeo.at(i)->arg3()[j*3+1].toDouble()));
        }
        m_Polygons.push_back(tempPolygon);
    }

    return true;
}

bool GridMaker::makeGrid(){
    //unite polygons that are the right layer name
    bool firstPolygon=true;
    for (int i=0;i<m_RadGeo.size();i++){
        bool properName=false;
        for (int j=0;j<m_LayerNames.size();j++){
            if (m_RadGeo.at(i)->modifier()==m_LayerNames.at(j)){
                properName=true;
            }
        }
        if (firstPolygon==true && properName==true){
            firstPolygon=false;
            m_UnitedPolygon=m_Polygons.at(i);
        }else if (properName==true){
            m_UnitedPolygon.united(m_Polygons.at(i));
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
    while (x<maxX()){
        double y=minY()+offsetY();
        while (y<maxY()){
            if (m_UnitedPolygon.containsPoint(QPointF(x,y), Qt::OddEvenFill)){
                setTestPoints(x,y);
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

bool GridMaker::writePTS(QString file){
    QFile oFile;
    oFile.setFileName(file);
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        return false;
    }
    QTextStream out(&oFile);
    for (int i=0;i<m_TestPoints.size();i++){
        out<<m_TestPoints.at(i).rx()<<" "<<m_TestPoints.at(i).ry()<<" "<<m_ZHeight<<" 0 0 1"<<endl;
    }
    oFile.close();
    return true;
}
