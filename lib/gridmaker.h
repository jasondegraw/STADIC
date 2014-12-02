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

#ifndef GRIDMAKER_H
#define GRIDMAKER_H

#include <vector>
#include <string>
#include "radprimitive.h"
#include "radfiledata.h"
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/multi_polygon.hpp>
#include <boost/geometry/core/cs.hpp>
#include "stadicapi.h"

namespace stadic {

class STADIC_API GridMaker
{
public:
    GridMaker(std::vector<std::string> fileList);                                       //Constructor that takes a list of radiance geometry files

    //Setters
    //Points

    //InputData  These functions must be used before the utilities can be used.
    void setLayerNames(std::vector<std::string> layerList);                             //Function that sets the layer names via a list on which to place the grid
    void addLayerName(std::string string);                                              //Function that adds a layer name on which to place the grid
    void setSpaceX(double x);                                                           //Function that sets the spacing in the x direction between points
    void setSpaceY(double y);                                                           //Function that sets the spacing in the y direction between points
    void setOffset(double val);                                                         //Function that sets a uniform inset from the polygon boundaries
    void setOffsetX(double x);                                                          //Function that sets an offset from the min and max x values
    void setOffsetY(double y);                                                          //Function that sets an offset from the min and max y values
    void setOffsetZ(double z);                                                          //Function that sets a z offset from the average height of each polygon
    void setZHeight(double z);                                                          //Function that sets an absolute z height using world coordinates
    void setThreshold(double val);
    void setRotation(double rot);                                                       //Function that sets the rotation angle of the building (or space)
    
    //Getters
    //Points
    //InputData
    std::vector<std::string> layerNames();                                              //Function that returns the layer names in a vector
    double spaceX();                                                                    //Function that returns the spacing in the x direction as a double
    double spaceY();                                                                    //Function that returns the spacing in the y direction as a double
    double offsetX();                                                                   //Function that returns the x offset as a double
    double offsetY();                                                                   //Function that returns the y offset as a double
    double zHeight();                                                                   //Function that returns the absolute z height as a double

    std::vector<std::vector<std::vector<double> > > points();                           //Function that returns the points that are used for analysis

    //Utilities
    bool makeGrid();                                                                    //Main function that makes the grid
    bool writePTS(std::ostream& out);                                                   //Function to write the points file to the given stream
    bool writePTS();                                                                    //Function to write the points file via the standard output
    bool writePTS(std::string file);                                                    //Function to write the points file to a file
    bool writePTScsv(std::string file);                                                 //Function to write the pts file to a file in a csv format
    bool viewPTS(std::string location, std::string vType);                              //Function to render a bmp of the points file and the layers chosen for the grid


private:
    //Points
    std::vector<std::vector<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> > > m_PointSet;       //The x,y points in boost format that are within the polygons
    std::vector<std::vector<std::vector<double> > > m_FinalPoints;                                                         //The x,y,z points as doubles that are the final output set
    //Polygons
    std::vector<boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>, true, true> > > m_UnitedPolygon;  //Vector containing a multipolygon at each elevation that holds each of the joined polygons


    //InputData
    std::vector<std::string> m_LayerNames;                      //Vector hlding the layer names on which to place the grid
    RadFileData m_RadFile;                                      //RadFileData object for handling the .rad files
    double m_SpaceX;                                            //Variable holding the spacing in the x direction
    double m_SpaceY;                                            //Variable holding the spacing in the y direction
    double m_OffsetX;                                           //Variable holding the x offset
    double m_OffsetY;                                           //Variable holding the y offset
    double m_ZHeight;                                           //Variable holding the absolute z height
    double m_OffsetZ;                                           //Variable holding the z offset from each polygon height
    bool m_useZOffset;                                          //Boolean for testing whether a z offset should be applied
    double m_Offset;                                            //Variable holding the uniform inset from the boundaries of the polygons described by the layer list
    bool m_UseOffset;                                           //Boolean for testing whether a unifrom offset should be applied
    double m_Threshold;                                         //Variable holding the threshold distance for a polygon to be included in the points generation
    bool m_UseThreshold;                                        //Boolean for testing whether to use the threshold distance
    std::string m_RadPolyFile;                                  //Variable holding the radiance polygon filename
    std::string m_RadPtsFile;                                   //Variable holding the radiance point-spheres filename
    std::string m_oconvFile;                                    //Variable holding the octree filename
    std::string m_picFile;                                      //Variable holding the pic file location
    double m_rotation;                                          //Variable holding the building rotation angle (ccw is positive).
    bool m_UseRotation;                                         //Boolean for testing whether to use rotation
    
    //Dimensional
    std::vector<double> m_MinX;                                 //Vector holding the minimum x value per multipolygon set
    std::vector<double> m_MinY;                                 //Vector holding the minimum y value per multipolygon set
    std::vector<double> m_MaxX;                                 //Vector holding the maximum x value per multipolygon set
    std::vector<double> m_MaxY;                                 //Vector holding the maximum y value per multipolygon set
    double m_MaxXRad;                                           //Variable holding the maximum x coordinate in the radpoly file
    double m_MinXRad;                                           //Variable holding the minimum x coordinate in the radpoly file
    double m_MaxYRad;                                           //Variable holding the maximum y coordinate in the radpoly file
    double m_MinYRad;                                           //Variable holding the minimum y coordinate in the radpoly file
    double m_MaxZRad;                                           //Variable holding the maximum z coordinate in the radpoly file
    double m_MinZRad;                                           //Variable holding the minimum z coordinate in the radpoly file
    std::vector<double> m_PolySetHeight;                        //Vector holding the average heights of the polygons per multipolygon set

    //Functions
    bool parseRad();                                            //Function that parses the rad file with the RadFileData object
    //Dimensional
    void setMinX(double x, int set);                            //Function to set the minimum x coordinate for a given multipolygon
    void setMaxX(double x, int set);                            //Function to set the maximum x coordinate for a given multipolygon
    void setMinY(double y, int set);                            //Function to set the minimum y coordinate for a given multipolygon
    void setMaxY(double y, int set);                            //Function to set the maximum y coordinate for a given multipolygon

    bool insetPolygons();                                       //Function to inset the multipolygons
    void boundingBox(boost::geometry::model::multi_polygon<boost::geometry::model::polygon<boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>, true, true>> polygonSet, int set);     //Function to get the bounding box of a multipolygon
    bool testPoints();                                          //Function for determining if points are within or on a polygon
    void addTestPoints(double x, double y, int set);            //Function for adding the points to the final point vector if they are on one of the multipolygons
    bool writeRadPoly(std::string file);                        //Function for writing the radiance polygon of the listed layers
    bool writeRadPoints(std::string file);                      //Function for writing the points file as spheres
    bool runoconv(std::string file);                            //Function for running oconv
    bool runrpict(std::string vType);                           //Function for running rpict and ra_bmp

};

}

#endif // GRIDMAKER_H
