#include "logging.h"
#include "gridmaker.h"
#include "radprimitive.h"
#include "functions.h"
#include <iostream>
#include <fstream>
#include "process.h"
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/strategies/agnostic/buffer_distance_asymmetric.hpp>
#include <boost/geometry/strategies/cartesian/buffer_join_miter.hpp>
#include <boost/geometry/strategies/cartesian/buffer_end_flat.hpp>
#include <boost/geometry/strategies/cartesian/buffer_point_square.hpp>
#include <boost/geometry/strategies/cartesian/buffer_point_square.hpp>
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
    m_UseOffset=false;
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
    m_Offset=-val;
    m_UseOffset=true;
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

    if (!testPoints()){
        return false;
    }

    if (m_useZOffset){
        zHeights();
    }else{
        for (int p=0;p<m_PointSet.size();p++){
            std::vector<double> tempPoint;
            tempPoint.push_back(m_PointSet[p].get<0>());
            tempPoint.push_back(m_PointSet[p].get<1>());
            tempPoint.push_back(m_ZHeight);
            m_FinalPoints.push_back(tempPoint);
        }
    }
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
        STADIC_ERROR("The opening of the file "+file+" has failed.");
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
        STADIC_ERROR("The opening of the file "+file+" has failed.");
        return false;
    }
    for (int i=0;i<m_FinalPoints.size();i++){
        oFile<<m_FinalPoints[i][0]<<","<<m_FinalPoints[i][1]<<","<<m_FinalPoints[i][2]<<",0,0,1"<<std::endl;
    }
    oFile.close();
    return true;
}
bool GridMaker::viewPTS(std::string location){
    //The file names need to be adjusted
    m_RadPolyFile=location+"poly.rad";
    if (!writeRadPoly(m_RadPolyFile)){
        return false;
    }

    m_RadPtsFile=location+"points.rad";
    if (!writeRadPoints(m_RadPtsFile)){
        return false;
    }

    m_oconvFile=location+"points.oct";
    if (!runoconv(m_oconvFile)){
        return false;
    }

    m_picFile=location;
    if (!runrpict()){
        return false;
    }
    return true;
}



//Private
//Functions
bool GridMaker::parseRad(){
    //set polygons
    bool firstPolygon=false;
    if (m_RadFile.geometry().empty()){
        std::cerr<<"There are no polygons."<<std::endl;
        return false;
    }
    for (int i=0;i<m_RadFile.geometry().size();i++){
        boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> tempPolygon;
        for (int j=0;j<m_RadFile.geometry().at(i)->arg3().size()/3;j++){
            boost::geometry::append(tempPolygon,boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>(toDouble(m_RadFile.geometry().at(i)->arg3()[j*3]), toDouble(m_RadFile.geometry().at(i)->arg3()[j*3+1])));
        }
        boost::geometry::correct(tempPolygon);
        if (boost::geometry::is_valid(tempPolygon)){
            //unite polygons that are the right layer name
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
                m_UnitedPolygon.push_back(tempPolygon);
                std::clog<<"The first polygon has been added to the multi-polygon."<<std::endl;
            }else if (properName==true){
                boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true>> tempMultiPolygon;
                boost::geometry::union_(m_UnitedPolygon,tempPolygon,tempMultiPolygon);
                m_UnitedPolygon=tempMultiPolygon;
            }
        }
    }
    if (boost::geometry::is_valid(m_UnitedPolygon)){
        return true;
    }else{
        STADIC_ERROR("There was a problem uniting the polygons.");
        return false;
    }
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
bool GridMaker::insetPolygons(){
    boost::geometry::strategy::buffer::distance_asymmetric<double> distance_strategy((m_Offset -m_Offset*0.10), (m_Offset-m_Offset*0.10));
    boost::geometry::strategy::buffer::join_miter join_strategy;
    boost::geometry::strategy::buffer::end_flat end_strategy;
    boost::geometry::strategy::buffer::point_square point_strategy;
    boost::geometry::strategy::buffer::side_straight side_strategy;
    boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true>> tempPolygon;
    boost::geometry::buffer(m_UnitedPolygon,tempPolygon,distance_strategy,side_strategy,join_strategy,end_strategy,point_strategy);
    m_UnitedPolygon=tempPolygon;
    if (!boost::geometry::is_valid(m_UnitedPolygon)){
        STADIC_ERROR("Creating the offset failed to create a valid polygon.");
        return false;
    }
    return true;
}


void GridMaker::boundingBox(boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>, true, true>> polygonSet){
    boost::geometry::model::box<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>> box;
    boost::geometry::envelope(polygonSet,box);
    //std::clog<<"minX="<<box.min_corner().get<0>()<<std::endl;
    //std::clog<<"minY="<<box.min_corner().get<1>()<<std::endl;
    //std::clog<<"maxX="<<box.max_corner().get<0>()<<std::endl;
    //std::clog<<"maxY="<<box.max_corner().get<1>()<<std::endl;
    //std::clog<<"offset="<<m_Offset<<std::endl;
    setMinX(box.min_corner().get<0>()-m_Offset*.1);
    setMinY(box.min_corner().get<1>()-m_Offset*.1);
    setMaxX(box.max_corner().get<0>()+m_Offset*.1);
    setMaxY(box.max_corner().get<1>()+m_Offset*.1);
    //std::clog<<"minX="<<m_MinX<<std::endl;
    //std::clog<<"minY="<<m_MinY<<std::endl;
    //std::clog<<"maxX="<<m_MaxX<<std::endl;
    //std::clog<<"maxY="<<m_MaxY<<std::endl;

}

bool GridMaker::testPoints(){
    if (m_UseOffset){
        if (!insetPolygons()){
            return false;
        }
        boundingBox(m_UnitedPolygon);
    }else if (m_OffsetX>0 || m_OffsetY>0){
        //Offset x and y from bounding rectangle given m_OffsetX and m_OffsetY
        //reset min and max x and y
        boundingBox(m_UnitedPolygon);
        setMinX(m_MinX+m_OffsetX);
        setMaxX(m_MaxX-m_OffsetX);
        setMinY(m_MinY+m_OffsetY);
        setMaxY(m_MaxY-m_OffsetY);
    }else if (m_SpaceX>0 && m_SpaceY>0){
        //Get min and max of bounding rectangle and divide by spacing for both x and y
        //If the result is an integer then the offset should be equal to the spacing
        //if the result is not an integer multiply the remainder by the spacing and divide by two for the offset
        //reset min and max x and y
        boundingBox(m_UnitedPolygon);
        if (remainder((m_MaxX-m_MinX),m_SpaceX)){
            setMinX(m_MinX+m_SpaceX);
            setMaxX(m_MaxX-m_SpaceX);
        }else{
            double tempNum=remainder((m_MaxX-m_MinX),m_SpaceX)*m_SpaceX;
            setMinX(m_MinX+tempNum);
            setMaxX(m_MaxX-tempNum);
        }
        if (remainder((m_MaxY-m_MinY),m_SpaceY)){
            setMinX(m_MinY+m_SpaceY);
            setMaxX(m_MaxY-m_SpaceY);
        }else{
            double tempNum=remainder((m_MaxY-m_MinY),m_SpaceY)*m_SpaceY;
            setMinX(m_MinY+tempNum);
            setMaxX(m_MaxY-tempNum);
        }
    }else{
        STADIC_ERROR("The offsets cannot be determined, because the spacing and offset values are all 0.");
        return false;
    }
        //Create vector of test points
    double x=m_MinX;
    while (x<=m_MaxX){
        double y=m_MinY;
        while (y<=m_MaxY){
            if (m_UseOffset){
                if (boost::geometry::covered_by(boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>(x,y),m_UnitedPolygon)){
                    addTestPoints(x,y);
                }
            }else{
                if (boost::geometry::within(boost::geometry::model::point<double,2,boost::geometry::cs::cartesian>(x,y),m_UnitedPolygon)){
                    addTestPoints(x,y);
                }
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

bool GridMaker::writeRadPoly(std::string file){
    std::ofstream oFile;
    oFile.open(file);
    if (!oFile.is_open()){
        STADIC_ERROR("The opening of the file "+file+" has failed.");
        return false;
    }
    oFile<<"void plastic floor\n0\n0\n5\t.5\t.5\t.5\t0\t0"<<std::endl<<std::endl;
    m_MaxXRad=-1000;
    m_MinXRad=1000;
    m_MaxYRad=-1000;
    m_MinYRad=1000;
    for (int i=0;i<m_RadFile.geometry().size();i++){
        for (int j=0;j<m_LayerNames.size();j++){
            if (m_RadFile.geometry().at(i)->modifier()==m_LayerNames.at(j)){
                oFile<<"floor\tpolygon\tfloor"<<i<<std::endl;
                oFile<<"0\t0\t"<<m_RadFile.geometry().at(i)->arg3().size()<<std::endl;
                int coordinate=0;
                for (int k=0;k<m_RadFile.geometry().at(i)->arg3().size();k++){
                    oFile<<"\t"<<m_RadFile.geometry().at(i)->arg3()[k];
                    if (coordinate==0){
                        if (toDouble(m_RadFile.geometry().at(i)->arg3()[k])>m_MaxXRad){
                            m_MaxXRad=toDouble(m_RadFile.geometry().at(i)->arg3()[k]);
                        }
                        if (toDouble(m_RadFile.geometry().at(i)->arg3()[k])<m_MinXRad){
                            m_MinXRad=toDouble(m_RadFile.geometry().at(i)->arg3()[k]);
                        }
                        coordinate++;
                    }else if (coordinate==1){
                        if (toDouble(m_RadFile.geometry().at(i)->arg3()[k])>m_MaxYRad){
                            m_MaxYRad=toDouble(m_RadFile.geometry().at(i)->arg3()[k]);
                        }
                        if (toDouble(m_RadFile.geometry().at(i)->arg3()[k])<m_MinYRad){
                            m_MinYRad=toDouble(m_RadFile.geometry().at(i)->arg3()[k]);
                        }
                        coordinate++;
                    }else{
                        coordinate=0;
                    }
                }
                oFile<<std::endl;
            }
        }
    }

    /*
    for (int i=0;i<m_UnitedPolygon.size();i++){
        oFile<<"floor\tpolygon\tfloor"<<i<<std::endl;
        oFile<<"0\t0\t"<<(m_UnitedPolygon[i].outer().size()-1)*3<<std::endl;
        for (int j=0;j<m_UnitedPolygon[i].outer().size()-1;j++){
            oFile<<"\t"<<"\t"<<m_UnitedPolygon[i].outer()[j].get<0>()<<"\t"<<m_UnitedPolygon[i].outer()[j].get<1>()<<"\t0"<<std::endl;
        }
        oFile<<std::endl;
    }
    */
    oFile.close();
    return true;
}

bool GridMaker::writeRadPoints(std::string file){
    std::ofstream oFile;
    oFile.open(file);
    if (!oFile.is_open()){
        STADIC_ERROR("The opening of the file "+file+" has failed.");
        return false;
    }
    oFile<<"void plastic pts\n0\n0\n5\t.75\t.25\t.25\t0\t0"<<std::endl<<std::endl;
    for (int i=0;i<m_FinalPoints.size();i++){
        oFile<<"pts ring pts"<<i<<"\n0\n0\n8\t"<<m_FinalPoints[i][0]<<"\t"<<m_FinalPoints[i][1]<<"\t"<<m_FinalPoints[i][2]<<std::endl;
        oFile<<"\t0\t0\t1"<<std::endl;
        oFile<<"\t0\t"<<m_SpaceX/8<<std::endl<<std::endl;
    }
    oFile.close();
    return true;
}

bool GridMaker::runoconv(std::string file){
    std::string oconvProgram="oconv";
    std::vector<std::string> args;
    args.clear();
    args.push_back(m_RadPolyFile);
    args.push_back(m_RadPtsFile);
    Process oconv(oconvProgram,args);
    oconv.setStandardOutputFile(file);
    oconv.start();
    if (!oconv.wait()){
        return false;
    }
    return true;
}

bool GridMaker::runrpict(){
    std::string picFile=m_picFile+"grid.pic";
    std::string bmpFile=m_picFile+"grid.bmp";
    std::vector<std::string> args;
    args.push_back("-vtl");
    args.push_back("-w");
    args.push_back("-vp");
    args.push_back(std::to_string((m_MaxXRad-m_MinXRad)/2+m_MinXRad));
    args.push_back(std::to_string((m_MaxYRad-m_MinYRad)/2+m_MinYRad));
    double tempZmax=0;
    for (int i=0;i<m_FinalPoints.size();i++){
        if (m_FinalPoints[i][2]>tempZmax){
            tempZmax=m_FinalPoints[i][2];
        }
    }
    args.push_back(std::to_string(2+tempZmax));
    args.push_back("-vd");
    args.push_back("0");
    args.push_back("0.000000001");
    args.push_back("-1");
    args.push_back("-vh");
    args.push_back(std::to_string(1.1*(m_MaxXRad-m_MinXRad)));
    args.push_back("-vv");
    args.push_back(std::to_string(1.1*(m_MaxYRad-m_MinYRad)));
    args.push_back("-vo");
    args.push_back("0");
    args.push_back("-va");
    args.push_back("0");
    args.push_back("-vs");
    args.push_back("0");
    args.push_back("-vl");
    args.push_back("0");
    args.push_back("-w");
    args.push_back("-x");
    args.push_back("500");
    args.push_back("-y");
    args.push_back("500");
    args.push_back("-pj");
    args.push_back("0");
    args.push_back("-av");
    args.push_back("0.9");
    args.push_back("0.9");
    args.push_back("0.9");
    args.push_back("-ad");
    args.push_back("15000");
    args.push_back(m_oconvFile);

    std::string rpictProgram="rpict";
    Process rpict(rpictProgram,args);
    rpict.setStandardOutputFile(picFile);
    rpict.start();
    if (!rpict.wait()){
        return false;
    }

    std::string bmpProgram="ra_bmp";
    args.clear();
    args.push_back(picFile);
    Process raBMP(bmpProgram,args);
    raBMP.setStandardOutputFile(bmpFile);
    raBMP.start();
    if(!raBMP.wait()){
        return false;
    }
    return true;
}


}
