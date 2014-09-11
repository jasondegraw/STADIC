#include "leakcheck.h"
#include "logging.h"
#include <iostream>
#include <fstream>
#include "objects.h"

namespace stadic{

LeakCheck::LeakCheck(QObject *parent) :
    QObject(parent)
{
}
bool LeakCheck::isEnclosed(){
    if (!checkPoint()){
        return false;
    }

    if (!writePTS()){
        return false;
    }

    if (!writeExtraRad()){
        return false;
    }

    if (!xformModifiers()){
        return false;
    }

    if (!createOct()){
        return false;
    }

    if (!runCalc()){
        return false;
    }
    std::ifstream iFile;
    iFile.open("Final.res");
    if (!iFile.is_open()){
        ERROR("The opening of the results file failed.");
        return false;
    }
    std::string val;
    if (!getline(iFile,val)){
        ERROR("The results file is empty.");
        return false;
    }
    iFile.close();

    if (atof(val.c_str())>0&&atof(val.c_str())<0.5){
        WARNING("The illuminance value is greater than 0 at the analysis point, but less than 0.5.\n\tIt will be assumed that there is no light leak.");
    }else if (atof(val.c_str())>=0.5){
        ERROR("The provided model either contains a leak or the provided point is outside the space.");
        return false;
    }else{
        std::cout<<"The model is fully enclosed."<<std::endl;
    }

    return true;
}
//Setters
bool LeakCheck::setRadFile(std::vector<std::string> files){
    for (int i=0;i<files.size();i++){
        FilePath radFile(files[i]);
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
            tempPolygon.append(QPointF(atof(m_RadGeo.geometry()[i]->arg3()[j*3].c_str()), atof(m_RadGeo.geometry()[i]->arg3()[j*3+1].c_str())));
        }
        m_Polygons.push_back(tempPolygon);
    }
    return true;
}

bool LeakCheck::setFloorLayers(std::vector<std::string> layers){
    if (m_RadGeo.primitives().empty()){
        ERROR("No radiance geometry or materials have been specified.");
        return false;
    }
    for (int i=0;i<layers.size();i++){
        bool layerExists=false;
        for (int j=0;j<m_RadGeo.primitives().size();j++){
            if (layers[i]==m_RadGeo.primitives()[j]->modifier()){
                layerExists=true;
            }
        }
        if (!layerExists){
            ERROR("The layer "+layers[i]+" does not exist in the model.");
            return false;
        }
        m_FloorLayers.push_back(layers[i]);
    }
    if (!unitePolygons()){
        return false;
    }
    return true;
}

bool LeakCheck::setX(double x){
    if (m_UnitedPolygon.isEmpty()){
        if (!unitePolygons()){
            return false;
        }
    }
    if (m_UnitedPolygon.isEmpty()){
        ERROR("The uniting of the polygons has failed.");
        return false;
    }

    double minX=m_UnitedPolygon.boundingRect().left();
    double maxX=m_UnitedPolygon.boundingRect().right();
    if (minX>maxX){
        maxX=m_UnitedPolygon.boundingRect().left();
        minX=m_UnitedPolygon.boundingRect().right();
    }

    if (x<minX || x>maxX){
        ERROR("The x coordinate is not within the "+std::to_string(minX) +" to "+std::to_string(maxX)+".");
        return false;
    }
    if (m_TestPoint.size()<3){
        m_TestPoint.reserve(3);
    }
    m_TestPoint[0]=x;
    return true;
}

bool LeakCheck::setY(double y){
    if (m_UnitedPolygon.isEmpty()){
        if (!unitePolygons()){
            return false;
        }
    }
    if (m_UnitedPolygon.isEmpty()){
        ERROR("The uniting of the polygons has failed.");
        return false;
    }
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
    if (m_UnitedPolygon.isEmpty()){
        if (!unitePolygons()){
            return false;
        }
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

bool LeakCheck::setReflectance(int ref){
    if (ref!=0 && ref!=1){
        ERROR("The reflectance needs to be either 0 or 1.");
        return false;
    }
    m_Reflectance=ref;
    return true;
}

//Private
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

bool LeakCheck::writePTS(){
    std::ofstream oFile;
    oFile.open("Test.pts");
    if (!oFile.is_open()){
        return false;
    }
    oFile<<m_TestPoint[0]<<" "<<m_TestPoint[1]<<" "<<m_TestPoint[2]<<" 0 0 1";
    oFile.close();

    return true;
}

bool LeakCheck::xformModifiers(){
    std::vector<std::string> arguments;
    arguments.push_back("-m");
    arguments.push_back("modified");
    for (int i=0;i<m_RadFiles.size();i++){
        arguments.push_back(m_RadFiles[i]);
    }
    std::string programName="xform";
    Process xform(programName,arguments);
    xform.setStandardOutputFile("Mod.rad");
    xform.start();

    //There should be a test in here that if it doesn't finish it returns false
    if (!xform.wait()){
        ERROR("The xform of the modifiers has failed.");
        return false;
    }

    return true;
}

bool LeakCheck::writeExtraRad(){
    std::ofstream oFile;
    oFile.open("Extra.rad");
    if (!oFile.is_open()){
        return false;
    }
    oFile<<"void glow sky_glow"<<std::endl;
    oFile<<"0"<<std::endl<<"0"<<std::endl<<"4 1 1 1 0"<<std::endl<<std::endl;
    oFile<<"sky_glow source sky"<<std::endl;
    oFile<<"0"<<std::endl<<"0"<<std::endl<<"4 0 0 1 180"<<std::endl<<std::endl;

    oFile<<"void plastic modified"<<std::endl;
    oFile<<"0"<<std::endl<<"0"<<std::endl;
    oFile<<"5 "<<m_Reflectance<<" "<<m_Reflectance<<" "<<m_Reflectance<<" 0 0"<<std::endl;
    oFile.close();

    return true;
}

bool LeakCheck::createOct(){
    std::vector<std::string> arguments;
    arguments.push_back("Extra.rad");
    arguments.push_back("Mod.rad");
    std::string programName="oconv";
    Process oconv(programName,arguments);

    oconv.setStandardOutputFile("Test.oct");
    oconv.start();
    if(!oconv.wait()){
        ERROR("There was a problem creating the octree.");
        return false;
    }
    return true;
}

bool LeakCheck::runCalc(){
    std::vector<std::string> arguments;
    arguments.push_back("-I");
    arguments.push_back("-h");
    arguments.push_back("-ab");
    if (m_Reflectance<1){
        arguments.push_back("0");
    }else{
        arguments.push_back("4");
    }
    arguments.push_back("-ad");
    arguments.push_back("5000");
    arguments.push_back("-as");
    arguments.push_back("3000");
    arguments.push_back("-av");
    arguments.push_back("0");
    arguments.push_back("0");
    arguments.push_back("0");
    arguments.push_back("-aw");
    arguments.push_back("0");
    arguments.push_back("-aa");
    arguments.push_back("0.15");
    arguments.push_back("Test.oct");
    std::string programName="rtrace";
    Process rtrace(programName,arguments);
    rtrace.setStandardInputFile("Test.pts");

    std::vector<std::string> arguments2;
    arguments2.clear();
    arguments2.push_back("-e");
    arguments2.push_back("$1=179*($1*0.265+$2*0.670+$3*0.065)");
    programName="rcalc";
    Process rcalc(programName,arguments2);
    rtrace.setStandardOutputProcess(&rcalc);
    rcalc.setStandardOutputFile("Final.res");
    rtrace.start();
    if(!rtrace.wait()){
        ERROR("The running of rtrace has failed.");
        return false;
    }
    rcalc.start();
    if(!rcalc.wait()){
        ERROR("The running of rcalc has failed.");
        return false;
    }
    return true;
}

}
