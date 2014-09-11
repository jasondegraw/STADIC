#include "leakcheck.h"
#include <QFile>
#include "logging.h"
#include <iostream>

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

    QFile iFile;
    iFile.setFileName("Final.res");
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!iFile.isOpen()){
        ERROR("The opening of the results file failed.");
        return false;
    }
    if (iFile.atEnd()){
        ERROR("The results file is empty.");
        return false;
    }
    QString val1=iFile.readLine();
    iFile.close();

    if (val1.toDouble()>0&&val1.toDouble()<0.5){
        WARNING("The illuminance value is greater than 0 at the analysis point, but less than 0.5.\n\tIt will be assumed that there is no light leak.");
    }else if (val1.toDouble()>=0.5){
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
        QFile radFile(QString().fromStdString(files[i]));
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

bool LeakCheck::setFloorLayers(std::vector<std::string> layers){
    if (m_RadGeo.primitives().empty()){
        ERROR("No radiance geometry or materials have been specified.");
        return false;
    }
    for (int i=0;i<layers.size();i++){
        bool layerExists=false;
        for (int j=0;j<m_RadGeo.primitives().size();j++){
            if (QString().fromStdString(layers[i])==m_RadGeo.primitives()[j]->modifier()){
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
        ERROR("The x coordinate is not within the "+QString().sprintf("%g",minX) +" to "+QString().sprintf("%g",maxX)+".");
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

bool LeakCheck::setWorkingDirectory(std::string wDir){
    QDir tempDir(QString().fromStdString(wDir));
    if (!tempDir.exists()){
        ERROR("The working directory that was set does not exist.");
        return false;
    }
    if (!m_Dir.setCurrent(QString().fromStdString(wDir))){
        ERROR("There was an error when setting the working directory.");
        return false;
    }
    return true;
}

//Private
bool LeakCheck::unitePolygons(){
    //unite polygons that are the right layer name
    bool firstPolygon=true;
    for (int i=0;i<m_RadGeo.geometry().size();i++){
        bool properName=false;
        for (int j=0;j<m_FloorLayers.size();j++){
            if (m_RadGeo.geometry()[i]->modifier()==QString().fromStdString(m_FloorLayers[j])){
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
    QFile oFile;
    oFile.setFileName("Test.pts");
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        return false;
    }
    QTextStream out(&oFile);
    out<<m_TestPoint[0]<<" "<<m_TestPoint[1]<<" "<<m_TestPoint[2]<<" 0 0 1";
    oFile.close();

    return true;
}

bool LeakCheck::xformModifiers(){
    QProcess xform;
    QString programName="xform";
    xform.setProgram(programName);
    QStringList arguments;
    arguments.append("-m");
    arguments.append("modified");
    for (int i=0;i<m_RadFiles.size();i++){
        arguments.append(QString().fromStdString(m_RadFiles[i]));
    }
    xform.setArguments(arguments);
    xform.setStandardOutputFile("Mod.rad");
    xform.setWorkingDirectory(m_Dir.path());
    xform.start();

    //There should be a test in here that if it doesn't finish it returns false
    xform.waitForFinished();

    return true;
}

bool LeakCheck::writeExtraRad(){

    QFile oFile;
    oFile.setFileName("Extra.rad");
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        return false;
    }
    QTextStream out(&oFile);
    out<<"void glow sky_glow"<<endl;
    out<<"0"<<endl<<"0"<<endl<<"4 1 1 1 0"<<endl<<endl;
    out<<"sky_glow source sky"<<endl;
    out<<"0"<<endl<<"0"<<endl<<"4 0 0 1 180"<<endl<<endl;

    out<<"void plastic modified"<<endl;
    out<<"0"<<endl<<"0"<<endl;
    out<<"5 "<<m_Reflectance<<" "<<m_Reflectance<<" "<<m_Reflectance<<" 0 0"<<endl;
    oFile.close();

    return true;
}

bool LeakCheck::createOct(){
    m_Process=new QProcess(this);
    QString programName="oconv";
    m_Process->setProgram(programName);
    QStringList arguments;
    arguments.append("Extra.rad");
    arguments.append("Mod.rad");
    m_Process->setStandardOutputFile("Test.oct");
    m_Process->setArguments(arguments);
    m_Process->setWorkingDirectory(m_Dir.path());
    m_Process->start();
    m_Process->waitForFinished();
    return true;
}

bool LeakCheck::runCalc(){
    m_Process=new QProcess(this);
    m_Process->setProgram(QString("rtrace"));
    QStringList arguments;
    arguments.append("-I");
    arguments.append("-h");
    arguments.append("-ab");
    if (m_Reflectance<1){
        arguments.append("0");
    }else{
        arguments.append("4");
    }
    arguments.append("-ad");
    arguments.append("5000");
    arguments.append("-as");
    arguments.append("3000");
    arguments.append("-av");
    arguments.append("0");
    arguments.append("0");
    arguments.append("0");
    arguments.append("-aw");
    arguments.append("0");
    arguments.append("-aa");
    arguments.append("0.15");
    arguments.append("Test.oct");
    m_Process->setWorkingDirectory(m_Dir.path());
    m_Process->setArguments(arguments);
    m_Process->setStandardInputFile("Test.pts");

    m_Process2=new QProcess(this);
    m_Process2->setProgram(QString("rcalc"));
    m_Process->setStandardOutputProcess(m_Process2);
    QStringList arguments2;
    arguments2.clear();
    arguments2.append("-e");
    arguments2.append("$1=179*($1*0.265+$2*0.670+$3*0.065)");
    m_Process2->setArguments(arguments2);
    m_Process2->setWorkingDirectory(m_Dir.path());
    m_Process2->setStandardOutputFile("Final.res");
    m_Process->start();
    m_Process->waitForFinished(-1);
    m_Process2->start();
    m_Process2->waitForStarted();
    m_Process2->waitForFinished();
    return true;
}

}
