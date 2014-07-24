#include "gridmaker.h"
#include "radgeometry.h"
//#include "logging.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QString>
#include <QLineF>

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
    std::cout<<"Points:"<<std::endl;
    for (int i=0;i<m_UnitedPolygon.size();i++){
        std::cout<<"\t"<<m_UnitedPolygon.at(i).x()<<" "<<m_UnitedPolygon.at(i).y()<<std::endl;
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

                //Old code that tested to see if a point was on a line using QLineF::intersect
                /*
                bool onLine=false;
                for (int i=0;i<m_UnitedPolygon.size()-1;i++){
                    QLineF line;
                    QLineF line1;
                    line1.setP1(tempPoint);
                    line1.setP2(QPointF(0,0));
                    line.setP1(m_UnitedPolygon.at(i));
                    line.setP2(m_UnitedPolygon.at(i+1));
                    if (line.intersect(line1, &interPoint)==1){
                        if (qFuzzyCompare(interPoint.x(),tempPoint.x())&&qFuzzyCompare(interPoint.y(), tempPoint.y())){
                            onLine=true;
                        }
                    }
                }
                QLineF line;
                QLineF line1;
                line1.setP1(tempPoint);
                line1.setP2(QPointF(0,0));
                line.setP1(m_UnitedPolygon.at(m_UnitedPolygon.size()-1));
                line.setP2(m_UnitedPolygon.at(0));
                if (line.intersect(line1, &interPoint)==1){
                    if (qFuzzyCompare(interPoint.x(),tempPoint.x())&&qFuzzyCompare(interPoint.y(), tempPoint.y())){
                    //if (interPoint==tempPoint){
                        onLine=true;
                    }
                }
                if (!onLine){
                    setTestPoints(x,y);
                }*/
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

bool GridMaker::writePTScsv(QString file){
    QFile oFile;
    oFile.setFileName(file);
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        return false;
    }
    QTextStream out(&oFile);
    for (int i=0;i<m_TestPoints.size();i++){
        out<<m_TestPoints.at(i).rx()<<","<<m_TestPoints.at(i).ry()<<","<<m_ZHeight<<",0,0,1"<<endl;
    }
    oFile.close();
    return true;
}

bool GridMaker::writeRadPoly(QString file){
 QFile oFile;
    oFile.setFileName(file);
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
    oFile.close();
    return true;


}
