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
    QString val;
    m_Output2>>val;
    if (val.toDouble()>0){
        return false;
    }

    return true;
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
                layerExists=true;
            }
        }
        if (!layerExists){
            ERROR("The layer "+layers[i]+" does not exist in the model.");
            return false;
        }
        m_FloorLayers.append(layers[i]);
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
    if (ref!=0 || ref!=1){
        WARNING("The reflectance needs to be either 0 or 1.  It will take on a value of 1.");
        ref=1;
    }
    m_Reflectance=ref;
    return true;
}

bool LeakCheck::setWorkingDirectory(QString wDir){
    QDir tempDir(wDir);
    if (!tempDir.exists()){
        ERROR("The working directory that was set does not exist.");
        return false;
    }
    if (!m_Dir.setCurrent(wDir)){
        ERROR("There was an error when setting the workig directory.");
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

    return true;
}

bool LeakCheck::xformModifiers(){
    QProcess *xform=new QProcess(this);
    QString programName="xform.exe";
    xform->setProgram(programName);
    QStringList arguments;
    arguments.append("-m");
    arguments.append("modified");
    for (int i=0;i<m_RadFiles.size();i++){
        arguments.append(m_RadFiles[i]);
    }
    xform->setArguments(arguments);
    xform->setStandardOutputFile(m_Dir.path()+"Mod.rad");
    xform->setWorkingDirectory(m_Dir.path());
    xform->start();

    //There should be a test in here that if it doesn't finish it returns false
    xform->waitForFinished();

    return true;
}

bool LeakCheck::writeExtraRad(){

    QFile oFile;
    oFile.setFileName(m_Dir.path()+"Extra.rad");
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
    QString programName="oconv.exe";
    m_Process->setProgram(programName);
    QStringList arguments;
    arguments.append(m_Dir.path()+"Extra.rad");
    arguments.append(m_Dir.path()+"Mod.rad");
    m_Process->setStandardOutputFile(m_Dir.path()+"Test.oct");
    m_Process->setArguments(arguments);
    m_Process->setWorkingDirectory(m_Dir.path());
    connect(m_Process,SIGNAL(readyReadStandardError()),this, SLOT(captureErros()));

    m_Process->start();
    m_Process->waitForFinished();

    QString line;
    while (!m_ErrorLog.atEnd()){        //If this contains an error maybe it should return false after writing out the errors.
        line=m_ErrorLog.readLine();
        ERROR(line);
    }

    return true;
}

bool LeakCheck::runCalc(){
    m_Process=new QProcess(this);
    m_Process->setProgram(QString("rtrace.exe"));
    QStringList arguments;
    arguments.append("-I");
    arguments.append("-ab");
    if (m_Reflectance<1){
        arguments.append("0");
    }else{
        arguments.append("4");
    }
    arguments.append("-ad");
    arguments.append("2000");
    arguments.append("-as");
    arguments.append("1000");
    m_Process->setWorkingDirectory(m_Dir.path());
    m_Process->setArguments(arguments);
    connect(m_Process,SIGNAL(readyReadStandardError()),this,SLOT(captureErros()));


    m_Process2=new QProcess(this);
    m_Process2->setProgram(QString("rcalc.exe"));
    m_Process->setStandardOutputProcess(m_Process2);
    QStringList arguments2;
    arguments2.append("-e");
    arguments2.append("\"$1=179*($1*0.265+$2*0.670+$3*0.065)\"");
    m_Process2->setArguments(arguments2);
    m_Process2->setWorkingDirectory(m_Dir.path());
    connect(m_Process2,SIGNAL(readyReadStandardError()),this,SLOT(captureErrors2()));
    connect(m_Process2,SIGNAL(readyReadStandardOutput()),this,SLOT(captureOutput2()));

    m_Process->start();
    m_Process2->start();

    m_Process2->waitForFinished(-1);
    QString line;
    while (!m_ErrorLog.atEnd()){        //If this contains an error maybe it should return false after writing out the errors.
        line=m_ErrorLog.readLine();
        ERROR(line);
    }

    while(!m_ErrorLog2.atEnd()){        //If this contains an error maybe it should return false after writing out the errors.
        line=m_ErrorLog2.readLine();
        ERROR(line);
    }

    return true;
}


//Slots
void LeakCheck::captureErros(){
    m_ErrorLog<<m_Process->readAllStandardError();
}

void LeakCheck::captureErrors2(){
    m_ErrorLog2<<m_Process2->readAllStandardError();
}

void LeakCheck::captureOutput2(){
    m_Output2<<m_Process2->readAllStandardOutput();
}

}
