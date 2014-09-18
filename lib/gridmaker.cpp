#include "gridmaker.h"
#include "radprimitive.h"
#include "logging.h"
#include <iostream>
#include <fstream>
#include <QString>
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/strategies/agnostic/buffer_distance_symmetric.hpp>
#include <boost/geometry/strategies/cartesian/buffer_join_round.hpp>
#include <boost/geometry/strategies/cartesian/buffer_end_round.hpp>
#include <boost/geometry/strategies/cartesian/buffer_point_circle.hpp>
#include <boost/geometry/strategies/cartesian/buffer_side_straight.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/algorithms/is_valid.hpp>
#include <boost/geometry/algorithms/union.hpp>

namespace stadic {

GridMaker::GridMaker(std::vector<std::string> fileList)
{
    for (unsigned int i=0;i<fileList.size();i++){
        m_RadFile.addRad(fileList[i]);
    }

    m_PointSet.clear();
    m_useZOffset=false;
    m_FinalPoints.clear();
}

//Setters
//Points

//InputData
void GridMaker::setLayerNames(std::vector<std::string> layerList){
    m_LayerNames=layerList;
}
void GridMaker::addLayerName(std::string string){
    m_LayerNames.push_back(string);
}
void GridMaker::setSpaceX(double x){
    m_SpaceX=x;
}
void GridMaker::setSpaceY(double y){
    m_SpaceY=y;
}
void GridMaker::setOffset(double val){
    m_Offset=val;
}

void GridMaker::setOffsetX(double x){
    m_OffsetX=x;
}
void GridMaker::setOffsetY(double y){
    m_OffsetY=y;
}
void GridMaker::setOffsetZ(double z){
    m_OffsetZ=z;
    m_useZOffset=true;
}
void GridMaker::setZHeight(double z){
    m_ZHeight=z;
}



//Getters
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



//Utilities
bool GridMaker::makeGrid(){
    if (!parseRad()){
        return false;
    }

    std::clog<<"About to unite the polygons."<<std::endl;
    if (!unitePolygons()){
        return false;
    }

    //If doing an offset all the way around
    if (m_Offset>0){
        std::clog<<"Insetting the polygons."<<std::endl;
        if (!insetPolygons()){
            return false;
        }
        std::clog<<"Creating the bounding box."<<std::endl;
        boundingBox(m_UnitedPolygon);
    }else if (m_OffsetX>0 || m_OffsetY>0){
        //Offset x and y from bounding rectangle given m_OffsetX and m_OffsetY
        //reset min and max x and y

    }else{
        //Get min and max of bounding rectangle and divide by spacing for both x and y
        //If the result is an integer then the offset should be equal to the spacing
        //if the result is not an integer multiply the remainder by the spacing and divide by two for the offset
        //reset min and max x and y
    }
    std::clog<<"Getting the points in the polygons."<<std::endl;
    if (!testPoints()){
        return false;
    }

    if (m_useZOffset){
        std::clog<<"Getting the z heights from the polygons."<<std::endl;
        zHeights();
    }
    //Finish this code for when the program is ran with absolute z


    return true;
}

bool GridMaker::writePTS(std::ostream& out){
    for (int i=0;i<m_FinalPoints.size();i++){
        out<<m_FinalPoints[i][0]<<" "<<m_FinalPoints[i][1]<<" "<<m_FinalPoints[i][2]<<" 0 0 1"<<std::endl;
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
    for (int i=0;i<m_FinalPoints.size();i++){
        oFile<<m_FinalPoints[i][0]<<","<<m_FinalPoints[i][1]<<","<<m_FinalPoints[i][2]<<",0,0,1"<<std::endl;
    }
    oFile.close();
    return true;
}
/*
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
    oFile.close();
    return true;
}
*/
//Private
//Functions
bool GridMaker::parseRad(){
    //set polygons
    if (m_RadFile.geometry().empty()){
        std::cerr<<"There are no polygons."<<std::endl;
        return false;
    }
    for (int i=0;i<m_RadFile.geometry().size();i++){
        boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> tempPolygon;
        for (int j=0;j<m_RadFile.geometry().at(i)->arg3().size()/3;j++){
            boost::geometry::append(tempPolygon,boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>(atof(m_RadFile.geometry().at(i)->arg3()[j*3].c_str()), atof(m_RadFile.geometry().at(i)->arg3()[j*3+1].c_str())));
        }
        boost::geometry::correct(tempPolygon);
        if (!boost::geometry::is_valid(tempPolygon)){
            ERROR("The creation of the boost polygon has failed.");
            return false;
        }
        m_Polygons.push_back(tempPolygon);
    }
    return true;
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
bool GridMaker::unitePolygons(){
    //unite polygons that are the right layer name
    bool firstPolygon=true;
    for (int i=0;i<m_RadFile.geometry().size();i++){
        bool properName=false;
        for (int j=0;j<m_LayerNames.size();j++){
            if (m_RadFile.geometry().at(i)->modifier()==m_LayerNames.at(j)){
                properName=true;
            }
        }

        if (firstPolygon==true && properName==true){
            std::clog<<"The first polygon has been found."<<std::endl;
            firstPolygon=false;
            //the next line of code is failing
            m_UnitedPolygon.push_back(m_Polygons[i]);
            std::clog<<"The first polygon has been added to the multi-polygon."<<std::endl;
            //boost::geometry::append(m_UnitedPolygon,m_Polygons[i]);
        }else if (properName==true){
            boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true>> tempPolygon;
            boost::geometry::union_(m_UnitedPolygon,m_Polygons[i],tempPolygon);
            m_UnitedPolygon=tempPolygon;
        }
    }
    if (boost::geometry::is_valid(m_UnitedPolygon)){
        return true;
    }else{
        ERROR("There was a problem uniting the polygons.");
        return false;
    }
}

bool GridMaker::insetPolygons(){
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(m_OffsetX);
    boost::geometry::strategy::buffer::join_round join_strategy(36);
    boost::geometry::strategy::buffer::end_round end_strategy(36);
    boost::geometry::strategy::buffer::point_circle circle_strategy(36);
    boost::geometry::strategy::buffer::side_straight side_strategy;
    boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true>> tempPolygon;
    boost::geometry::buffer(m_UnitedPolygon,tempPolygon,distance_strategy,side_strategy,join_strategy,end_strategy,circle_strategy);
    m_UnitedPolygon=tempPolygon;
    if (!boost::geometry::is_valid(m_UnitedPolygon)){
        ERROR("Creating the offset failed to create a valid polygon.");
        return false;
    }
    return true;
}

void GridMaker::boundingBox(boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>, true, true>> polygonSet){
    boost::geometry::model::box<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>> box;
    boost::geometry::envelope(polygonSet,box);
    setMinX(box.min_corner().get<0>());
    setMinY(box.min_corner().get<1>());
    setMaxX(box.max_corner().get<0>());
    setMaxY(box.max_corner().get<1>());
}

bool GridMaker::testPoints(){
    //Create vector of test points
    double x=m_MinX;
    while (x<m_MaxX){
        double y=m_MinY;
        while (y<m_MaxY){
            if (boost::geometry::covered_by(boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>(x,y),m_UnitedPolygon)){
                addTestPoints(x,y);
            }
            y=y+spaceY();
        }
        x=x+spaceX();
    }
    if (m_PointSet.empty()){
        std::cerr<<"The points array is empty."<<std::endl;
        return false;
    }
    return true;
}

void GridMaker::addTestPoints(double x, double y){
    m_PointSet.push_back(boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>(x,y));
}

void GridMaker::zHeights(){
    for (int p=0;p<m_PointSet.size();p++){
        double tempZ=-10000;
        for (int i=0;i<m_RadFile.geometry().size();i++){
            for (int j=0;j<m_LayerNames.size();j++){
                if (m_RadFile.geometry().at(i)->modifier()==m_LayerNames.at(j)){
                    boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> tempPolygon;
                    for (int k=0;k<m_RadFile.geometry().at(i)->arg3().size()/3;k++){
                        boost::geometry::append(tempPolygon,boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>(atof(m_RadFile.geometry().at(i)->arg3()[k*3].c_str()), atof(m_RadFile.geometry().at(i)->arg3()[k*3+1].c_str())));
                    }
                    boost::geometry::correct(tempPolygon);
                    if (boost::geometry::covered_by((m_PointSet[p]),tempPolygon)){
                        double averageZ=0;
                        for (int k=0;k<m_RadFile.geometry().at(i)->arg3().size()/3;k++){
                            averageZ=averageZ+atof(m_RadFile.geometry().at(i)->arg3()[k*3+2].c_str());
                        }
                        if (averageZ/(m_RadFile.geometry().at(i)->arg3().size()/3)>tempZ){
                            tempZ=averageZ/(m_RadFile.geometry().at(i)->arg3().size()/3);
                        }
                    }
                }
            }
        }
        std::vector<double> tempPoint;
        tempPoint.push_back(m_PointSet[p].get<0>());
        tempPoint.push_back(m_PointSet[p].get<1>());
        tempPoint.push_back(tempZ+m_OffsetZ);
        m_FinalPoints.push_back(tempPoint);
    }


}

}
