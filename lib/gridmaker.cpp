/****************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted for
 * personal and commercial purposes provided that the
 * following conditions are met:
 *
 * 1. Redistribution of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PENNSYLVANIA STATE UNIVERSITY
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************/

#include "logging.h"
#include "gridmaker.h"
#include "radprimitive.h"
#include "functions.h"
#include "stadicprocess.h"
#include <iostream>
#include <fstream>
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/strategies/agnostic/buffer_distance_asymmetric.hpp>
#include <boost/geometry/strategies/cartesian/buffer_join_miter.hpp>
#include <boost/geometry/strategies/cartesian/buffer_end_flat.hpp>
#include <boost/geometry/strategies/cartesian/buffer_point_square.hpp>
#include <boost/geometry/strategies/cartesian/buffer_side_straight.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/algorithms/within.hpp>
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
    m_PolySetHeight.clear();
    m_UseThreshold=false;
    m_UseRotation=false;
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
void GridMaker::setThreshold(double val){
    m_Threshold=val;
    m_UseThreshold=true;
}
void GridMaker::setRotation(double rot){
    if (rot!=0){
        m_rotation=rot;
        m_UseRotation=true;
    }
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
std::vector<std::vector<std::vector<double> > > GridMaker::points(){
    return m_FinalPoints;
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
        for (int p=0;p<m_PointSet.size();p++){
            std::vector<std::vector<double> > tempVect;
            for (int i=0;i<m_PointSet[p].size();i++){
                std::vector<double> tempPoint;
                tempPoint.push_back(m_PointSet[p][i].get<0>());
                tempPoint.push_back(m_PointSet[p][i].get<1>());
                tempPoint.push_back(m_PolySetHeight[p]+m_OffsetZ);
                tempVect.push_back(tempPoint);
            }
            m_FinalPoints.push_back(tempVect);
        }
    }else{
        for (int p=0;p<m_PointSet.size();p++){
            std::vector<std::vector<double> > tempVect;
            for (int i=0;i<m_PointSet[p].size();i++){
                std::vector<double> tempPoint;
                tempPoint.push_back(m_PointSet[p][i].get<0>());
                tempPoint.push_back(m_PointSet[p][i].get<1>());
                tempPoint.push_back(m_ZHeight);
                tempVect.push_back(tempPoint);
            }
            m_FinalPoints.push_back(tempVect);
        }
    }
    return true;
}

bool GridMaker::writePTS(std::ostream& out){
    for (int i=0;i<m_FinalPoints.size();i++){
        for (int p=0;p<m_FinalPoints[i].size();p++){
            out<<m_FinalPoints[i][p][0]<<" "<<m_FinalPoints[i][p][1]<<" "<<m_FinalPoints[i][p][2]<<" 0 0 1"<<std::endl;
        }
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
        for (int p=0;p<m_FinalPoints[i].size();p++){
            oFile<<m_FinalPoints[i][p][0]<<","<<m_FinalPoints[i][p][1]<<","<<m_FinalPoints[i][p][2]<<",0,0,1"<<std::endl;
        }
    }
    oFile.close();
    return true;
}
bool GridMaker::viewPTS(std::string location, std::string vType){
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
    if (!runrpict(vType)){
        return false;
    }

    return true;
}



//Private
//Functions
bool GridMaker::parseRad(){
    //set polygons
    std::vector<bool> firstPolygon;

    if (m_RadFile.geometry().empty()){
        std::cerr<<"There are no polygons."<<std::endl;
        return false;
    }
    for (int i=0;i<m_RadFile.geometry().size();i++){
        boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> tempPolygon;
        double tempZ=0;
        for (int j=0;j<m_RadFile.geometry().at(i)->arg3().size()/3;j++){
            boost::geometry::append(tempPolygon,boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>(toDouble(m_RadFile.geometry().at(i)->arg3()[j*3]), toDouble(m_RadFile.geometry().at(i)->arg3()[j*3+1])));
            tempZ=tempZ+toDouble(m_RadFile.geometry().at(i)->arg3()[j*3+2]);
        }
        tempZ=tempZ/(m_RadFile.geometry().at(i)->arg3().size()/3.0);
        boost::geometry::correct(tempPolygon);
        if (boost::geometry::is_valid(tempPolygon)){
            //unite polygons that are the right layer name
            bool properName=false;
            for (int j=0;j<m_LayerNames.size();j++){
                if (m_RadFile.geometry().at(i)->modifier()==m_LayerNames.at(j)){
                    properName=true;
                }
            }
            int setPos;
            if (properName==true){
                if (m_PolySetHeight.empty()){
                    m_PolySetHeight.push_back(tempZ);
                    firstPolygon.push_back(true);
                    setPos=0;
                }else{
                    bool heightFound=false;
                    for (int j=0;j<m_PolySetHeight.size();j++){
                        if (tempZ>(m_PolySetHeight[j]*.99)&&tempZ<(m_PolySetHeight[j]*1.01)){
                            heightFound=true;
                            setPos=j;
                        }
                    }
                    if (!heightFound){
                            m_PolySetHeight.push_back(tempZ);
                            firstPolygon.push_back(true);
                            setPos=m_PolySetHeight.size()-1;
                    }
                }
                if (firstPolygon[setPos]){
                    firstPolygon[setPos]=false;
                    boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> > tempMultiPolygon;
                    tempMultiPolygon.push_back(tempPolygon);
                    m_UnitedPolygon.push_back(tempMultiPolygon);
                }else{
                    boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> > tempMultiPolygon;
                    boost::geometry::union_(m_UnitedPolygon[setPos],tempPolygon,tempMultiPolygon);
                    m_UnitedPolygon[setPos]=tempMultiPolygon;
                }
            }
        }
    }
    for (int i=0;i<m_UnitedPolygon.size();i++){
        if (!boost::geometry::is_valid(m_UnitedPolygon[i])){
            STADIC_ERROR("There was a problem uniting the polygons.");
            return false;
        }
    }
    return true;
}
//Dimensional
void GridMaker::setMinX(double x, int set){
    m_MinX[set]=x;
}
void GridMaker::setMaxX(double x, int set){
    m_MaxX[set]=x;
}
void GridMaker::setMinY(double y, int set){
    m_MinY[set]=y;
}
void GridMaker::setMaxY(double y, int set){
    m_MaxY[set]=y;
}
bool GridMaker::insetPolygons(){
    boost::geometry::strategy::buffer::distance_asymmetric<double> distance_strategy((m_Offset -m_Offset*0.10), (m_Offset-m_Offset*0.10));
    boost::geometry::strategy::buffer::join_miter join_strategy;
    boost::geometry::strategy::buffer::end_flat end_strategy;
    boost::geometry::strategy::buffer::point_square point_strategy;
    boost::geometry::strategy::buffer::side_straight side_strategy;
    std::vector<bool> keepPolygon;
    for (int i=0;i<m_PolySetHeight.size();i++){
        boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> > tempPolygon;
        boost::geometry::buffer(m_UnitedPolygon[i],tempPolygon,distance_strategy,side_strategy,join_strategy,end_strategy,point_strategy);
        if (!boost::geometry::is_valid(tempPolygon)){
            //Since this isn't a valid polygon, try to center the points on the polygon unless a threshold is
            //  set by the user and both dimensions are smaller than the threshold
            boost::geometry::model::box<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> > box;
            boost::geometry::envelope(m_UnitedPolygon[i],box);
            double minX=box.min_corner().get<0>();
            double minY=box.min_corner().get<1>();
            double maxX=box.max_corner().get<0>();
            double maxY=box.max_corner().get<1>();
            double deltaY=maxY-minY;
            double deltaX=maxX-minX;
            double minDist=std::min(deltaY, deltaX);

            if (m_UseThreshold){
                if (deltaY>m_Threshold ||deltaX>m_Threshold){
                    //The following line has had the negatives introduced for the distance strategy.  These are not needed in the main buffer but are allowing this to work properly now.
                    boost::geometry::strategy::buffer::distance_asymmetric<double> distance_strategy2(-(minDist/2-0.01), -(minDist/2-0.01));
                    boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> > tempPolygon2;
                    boost::geometry::buffer(m_UnitedPolygon[i],tempPolygon2,distance_strategy2,side_strategy,join_strategy,end_strategy,point_strategy);
                    if (!boost::geometry::is_valid(tempPolygon2)){
                        keepPolygon.push_back(false);
                    }else{
                        keepPolygon.push_back(true);
                        m_UnitedPolygon[i]=tempPolygon2;
                    }
                }else{
                    keepPolygon.push_back(false);
                }
            }else{
                //The following line has had the negatives introduced for the distance strategy.  These are not needed in the main buffer but are allowing this to work properly now.
                boost::geometry::strategy::buffer::distance_asymmetric<double> distance_strategy2(-(minDist/2-0.01), -(minDist/2-0.01));
                boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> > tempPolygon2;
                boost::geometry::buffer(m_UnitedPolygon[i],tempPolygon2,distance_strategy2,side_strategy,join_strategy,end_strategy,point_strategy);
                if (!boost::geometry::is_valid(tempPolygon2)){
                    keepPolygon.push_back(false);
                }else{
                    keepPolygon.push_back(true);
                    m_UnitedPolygon[i]=tempPolygon2;
                }

            }

        }else{
            keepPolygon.push_back(true);
            m_UnitedPolygon[i]=tempPolygon;
        }
        boost::geometry::correct(m_UnitedPolygon[i]);
    }
    for (int i=m_PolySetHeight.size()-1;i>=0;i--){
        if (keepPolygon[i]){
            if (!boost::geometry::is_valid(m_UnitedPolygon[i])){
                STADIC_ERROR("Creating the offset failed to create a valid polygon.");
                return false;
            }
        }else{
            m_UnitedPolygon.erase(m_UnitedPolygon.begin()+ i);
            m_PolySetHeight.erase(m_PolySetHeight.begin()+ i);
        }
    }
    if (std::find(keepPolygon.begin(), keepPolygon.end(), false)!=keepPolygon.end()){
        STADIC_LOG(Severity::Info, "Some surfaces were too small at certain heights to place valid points.\n\tThese surfaces were removed from the grid making process.");
    }

    /*
    boost::geometry::strategy::buffer::distance_asymmetric<double> distance_strategy((m_Offset -m_Offset*0.10), (m_Offset-m_Offset*0.10));
    boost::geometry::strategy::buffer::join_miter join_strategy;
    boost::geometry::strategy::buffer::end_flat end_strategy;
    boost::geometry::strategy::buffer::point_square point_strategy;
    boost::geometry::strategy::buffer::side_straight side_strategy;
    for (int i=0;i<m_PolySetHeight.size();i++){
        boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>,true,true> > tempPolygon;
        boost::geometry::buffer(m_UnitedPolygon[i],tempPolygon,distance_strategy,side_strategy,join_strategy,end_strategy,point_strategy);
        m_UnitedPolygon[i]=tempPolygon;
        boost::geometry::correct(m_UnitedPolygon[i]);
        if (!boost::geometry::is_valid(m_UnitedPolygon[i])){
            STADIC_ERROR("Creating the offset failed to create a valid polygon.");
            return false;
        }
    }
    */
    return true;
}


void GridMaker::boundingBox(boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>, true, true> > polygonSet, int set){
    m_MinX.resize(m_PolySetHeight.size());
    m_MinY.resize(m_PolySetHeight.size());
    m_MaxX.resize(m_PolySetHeight.size());
    m_MaxY.resize(m_PolySetHeight.size());
    boost::geometry::model::box<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> > box;
    boost::geometry::envelope(polygonSet,box);
    //std::clog<<"minX="<<box.min_corner().get<0>()<<std::endl;
    //std::clog<<"minY="<<box.min_corner().get<1>()<<std::endl;
    //std::clog<<"maxX="<<box.max_corner().get<0>()<<std::endl;
    //std::clog<<"maxY="<<box.max_corner().get<1>()<<std::endl;
    //std::clog<<"offset="<<m_Offset<<std::endl;
    double minX=box.min_corner().get<0>();
    double minY=box.min_corner().get<1>();
    double maxX=box.max_corner().get<0>();
    double maxY=box.max_corner().get<1>();
    if ((maxX-minX)<std::abs(m_Offset) || (maxY-minY)<std::abs(m_Offset)){
        setMinX(minX+.005, set);
        setMinY(minY+.005, set);
        setMaxX(maxX-.005, set);
        setMaxY(maxY-.005, set);
    }else{
        setMinX(minX-m_Offset*.1, set);
        setMinY(minY-m_Offset*.1, set);
        setMaxX(maxX+m_Offset*.1, set);
        setMaxY(maxY+m_Offset*.1, set);
    }
    //std::clog<<"minX="<<m_MinX<<std::endl;
    //std::clog<<"minY="<<m_MinY<<std::endl;
    //std::clog<<"maxX="<<m_MaxX<<std::endl;
    //std::clog<<"maxY="<<m_MaxY<<std::endl;

}

bool GridMaker::testPoints(){
    if (m_UseRotation){
        boost::geometry::strategy::transform::rotate_transformer<boost::geometry::degree, double, 2, 2> rotate(m_rotation);
        for (int i=0;i<m_PolySetHeight.size();i++){
            boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>, true, true> > rotatedSet;
            boost::geometry::transform(m_UnitedPolygon[i],rotatedSet, rotate);
            m_UnitedPolygon[i]=rotatedSet;
        }
    }
    
    if (m_UseOffset){
        if (!insetPolygons()){
            return false;
        }
        for (int i=0;i<m_PolySetHeight.size();i++){
            boundingBox(m_UnitedPolygon[i],i);
        }

    }else if (m_OffsetX>0 || m_OffsetY>0){
        //Offset x and y from bounding rectangle given m_OffsetX and m_OffsetY
        //reset min and max x and y

        for (int i=0;i<m_PolySetHeight.size();i++){
            boundingBox(m_UnitedPolygon[i],i);
            setMinX(m_MinX[i]+m_OffsetX, i);
            setMaxX(m_MaxX[i]-m_OffsetX, i);
            setMinY(m_MinY[i]+m_OffsetY, i);
            setMaxY(m_MaxY[i]-m_OffsetY, i);
        }
    }else if (m_SpaceX>0 && m_SpaceY>0){
        //Get min and max of bounding rectangle and divide by spacing for both x and y
        //If the result is an integer then the offset should be equal to the spacing
        //if the result is not an integer multiply the remainder by the spacing and divide by two for the offset
        //reset min and max x and y
        for (int i=0;i<m_PolySetHeight.size();i++){
            boundingBox(m_UnitedPolygon[i],i);
            if (remainder((m_MaxX[i]-m_MinX[i]),m_SpaceX)){
                setMinX((m_MinX[i]+m_SpaceX),i);
                setMaxX((m_MaxX[i]-m_SpaceX),i);
            }else{
                double tempNum=remainder((m_MaxX[i]-m_MinX[i]),m_SpaceX)*m_SpaceX;
                setMinX((m_MinX[i]+tempNum),i);
                setMaxX((m_MaxX[i]-tempNum),i);
            }
            if (remainder((m_MaxY[i]-m_MinY[i]),m_SpaceY)){
                setMinX((m_MinY[i]+m_SpaceY),i);
                setMaxX((m_MaxY[i]-m_SpaceY),i);
            }else{
                double tempNum=remainder((m_MaxY[i]-m_MinY[i]),m_SpaceY)*m_SpaceY;
                setMinX((m_MinY[i]+tempNum),i);
                setMaxX((m_MaxY[i]-tempNum),i);
            }
        }
    }else{
        STADIC_ERROR("The offsets cannot be determined, because the spacing and offset values are all 0.");
        return false;
    }
    for (int i=0;i<m_PolySetHeight.size();i++){      
        //Create vector of test points
        double x=m_MinX[i];
        while (x<=m_MaxX[i]){
            double y=m_MinY[i];
            while (y<=m_MaxY[i]){
                if (m_UseOffset){
                    if (boost::geometry::covered_by(boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>(x,y),m_UnitedPolygon[i])){
                        addTestPoints(x,y,i);
                    }
                }else{
                    if (boost::geometry::within(boost::geometry::model::point<double,2,boost::geometry::cs::cartesian>(x,y),m_UnitedPolygon[i])){
                        addTestPoints(x,y,i);
                    }
                }
                y=y+spaceY();
            }
            x=x+spaceX();
        }
        if(m_PointSet.empty()){
            STADIC_LOG(stadic::Severity::Error, "The points array has no pointsets.");
            return false;
        }
        if (m_PointSet[i].empty()){
            std::cerr<<"The points array is empty."<<std::endl;
            return false;
        }
    }
    return true;
}

void GridMaker::addTestPoints(double x, double y, int set){
    if (m_PointSet.empty()){
        m_PointSet.resize(1);
    }
    if (m_PointSet.size()<(set+1)){
        m_PointSet.resize(set+1);
    }
    std::vector<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> > tempVec;
    tempVec=m_PointSet[set];
    if(m_UseRotation){
        boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> unRotated;
        boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> rotated(x,y);
        boost::geometry::strategy::transform::rotate_transformer<boost::geometry::degree, double, 2, 2> rotate(-m_rotation);
        boost::geometry::transform(rotated,unRotated, rotate);
        tempVec.push_back(unRotated);
        
    }else{
        tempVec.push_back(boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>(x,y));
    }    
    m_PointSet[set]=tempVec;
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
    m_MaxZRad=-1000;
    m_MinZRad=1000;
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
                        if (toDouble(m_RadFile.geometry().at(i)->arg3()[k])>m_MaxZRad){
                            m_MaxZRad=toDouble(m_RadFile.geometry().at(i)->arg3()[k]);
                        }
                        if (toDouble(m_RadFile.geometry().at(i)->arg3()[k])<m_MinZRad){
                            m_MinZRad=toDouble(m_RadFile.geometry().at(i)->arg3()[k]);
                        }
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
        for (int p=0;p<m_FinalPoints[i].size();p++){
            oFile<<"pts sphere pts"<<i<<"\n0\n0\n4\t"<<m_FinalPoints[i][p][0]<<"\t"<<m_FinalPoints[i][p][1]<<"\t"<<m_FinalPoints[i][p][2]<<std::endl;
            oFile<<"\t"<<m_SpaceX/8<<std::endl<<std::endl;
        }
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
        if (oconv.state()==Process::BadProgram){
            STADIC_LOG(Severity::Fatal, "The oconv program could not be found.");
        }
        STADIC_LOG(Severity::Fatal, "The creation of the octree has failed.");
    }
    return true;
}

bool GridMaker::runrpict(std::string vType){
    std::string picFile=m_picFile+"grid.pic";
    std::string pfiltFile=m_picFile+"gridfiltered.pic";
    std::string bmpFile=m_picFile+"grid.bmp";
    std::vector<std::string> args;
    double deltaX=m_MaxXRad-m_MinXRad;
    double deltaY=m_MaxYRad-m_MinYRad;
    double deltaZ=m_MaxZRad-m_MinZRad;
    double tempZmax=0;
    for (int i=0;i<m_FinalPoints.size();i++){
        for (int p=0;p<m_FinalPoints[i].size();p++){
            if (m_FinalPoints[i][p][2]>tempZmax){
                tempZmax=m_FinalPoints[i][p][2];
            }
        }
    }
    if (deltaZ==0){
        deltaZ=m_MaxZRad+2*tempZmax;
    }
    args.push_back("-w");
    if (vType=="p"){
        args.push_back("-vtl");
        args.push_back("-vp");
        args.push_back(std::to_string(deltaX/2+m_MinXRad));
        args.push_back(std::to_string(deltaY/2+m_MinYRad));

        args.push_back(std::to_string(2+tempZmax));
        args.push_back("-vd");
        args.push_back("0");
        args.push_back("0.000000001");
        args.push_back("-1");
        args.push_back("-vh");
        args.push_back(std::to_string(1.1*(deltaX)));
        args.push_back("-vv");
        args.push_back(std::to_string(1.1*(deltaY)));
    }else{
        args.push_back("-vtv");
        args.push_back("-vp");
        if (vType=="ne"){
            args.push_back(toString(m_MaxXRad+deltaX));
            args.push_back(toString(m_MaxYRad+deltaY));
            args.push_back(toString(m_MaxZRad+4*deltaZ));
            args.push_back("-vd");
            args.push_back(toString(-1.5*deltaX));
            args.push_back(toString(-1.5*deltaY));
            args.push_back(toString(-4.5*deltaZ));
        }else if (vType=="nw"){
            args.push_back(toString(m_MinXRad-deltaX));
            args.push_back(toString(m_MaxYRad+deltaY));
            args.push_back(toString(m_MaxZRad+4*deltaZ));
            args.push_back("-vd");
            args.push_back(toString(1.5*deltaX));
            args.push_back(toString(-1.5*deltaY));
            args.push_back(toString(-4.5*deltaZ));
        }else if (vType=="sw"){
            args.push_back(toString(m_MinXRad-deltaX));
            args.push_back(toString(m_MinYRad-deltaY));
            args.push_back(toString(m_MaxZRad+4*deltaZ));
            args.push_back("-vd");
            args.push_back(toString(1.5*deltaX));
            args.push_back(toString(1.5*deltaY));
            args.push_back(toString(-4.5*deltaZ));
        }else if (vType=="se"){
            args.push_back(toString(m_MaxXRad+deltaX));
            args.push_back(toString(m_MinYRad-deltaY));
            args.push_back(toString(m_MaxZRad+4*deltaZ));
            args.push_back("-vd");
            args.push_back(toString(-1.5*deltaX));
            args.push_back(toString(1.5*deltaY));
            args.push_back(toString(-4.5*deltaZ));
        }else{
            STADIC_ERROR("The view type chosen is not an appropriate type.");
            return false;
        }
        args.push_back("-vu");
        args.push_back("0");
        args.push_back("0");
        args.push_back("1");
        args.push_back("-vh");
        args.push_back("45");
        args.push_back("-vv");
        args.push_back("45");
    }
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
    args.push_back("3000");
    args.push_back("-y");
    args.push_back("3000");
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
        if (rpict.state()==Process::BadProgram){
            STADIC_LOG(Severity::Fatal, "The rpict program could not be found.");
        }
        STADIC_LOG(Severity::Fatal, "The creation of the picture has failed.");
    }

    std::string pfiltProgram="pfilt";
    args.clear();
    args.push_back("-x/2");
    args.push_back("-y/2");
    args.push_back(picFile);
    Process pFilt(pfiltProgram, args);
    pFilt.setStandardOutputFile(pfiltFile);
    pFilt.start();
    if (!pFilt.wait()){
        if (pFilt.state()==Process::BadProgram){
            STADIC_LOG(Severity::Fatal, "The pfilt program could not be found.");
        }
        STADIC_LOG(Severity::Fatal, "The filtering of the picture has failed.");
    }

    std::string bmpProgram="ra_bmp";
    args.clear();
    args.push_back(pfiltFile);
    Process raBMP(bmpProgram,args);
    raBMP.setStandardOutputFile(bmpFile);
    raBMP.start();
    if(!raBMP.wait()){
        if (raBMP.state()==Process::BadProgram){
            STADIC_LOG(Severity::Fatal, "The ra_bmp program could not be found.");
        }
        STADIC_LOG(Severity::Fatal, "The creation of the bitmap has failed.");
    }
    return true;
}


}
