#include "stadiccontrol.h"
#include "windowgroup.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <iostream>
#include "logging.h"

namespace stadic {

Control::Control(QObject *parent) :
    QObject(parent)
{



}

//Setters
//******************
//Folder Information
//******************
void Control::setProjectName(QString name){
    m_ProjectName=name;
}
void Control::setProjectFolder(QString folder){
    m_ProjectFolder=folder;
}
void Control::setTmpFolder(QString folder){
    m_TmpFolder=folder;
}
void Control::setGeoFolder(QString folder){
    m_GeoFolder=folder;
}
void Control::setIESFolder(QString folder){
    m_IESFolder=folder;
}
void Control::setResultsFolder(QString folder){
    m_ResultsFolder=folder;
}
void Control::setDataFolder(QString folder){
    m_DataFolder=folder;
}

//******************
//Site Information
//******************
bool Control::setGroundReflect(double value){
    if (value>1 || value<0){
        WARNING(std::string("The ground reflectance must be between 0 and  1.\n\tA default value of 0.2 will be applied."));
        //std::cerr<<"WARNING: The ground reflectance must be between 0 and  1.\n\tA default value of 0.2 will be applied."<<std::endl;
        m_GroundReflect=0.2;
    }else{
        m_GroundReflect=value;
    }
    return true;
}
void Control::setWeaDataFile(QString file){
    m_WeaDataFile=file;
}
bool Control::setFirstDay(int value){
    if (value>7 || value<1){
        std::cerr<<"WARNING: The first day must be an integer between 1 and 7.\n\tA default value of 1 will be applied."<<std::endl;
        m_FirstDay=1;
    }else{
        m_FirstDay=value;
    }
    return true;
}

//******************
//Geometry Information
//******************
void Control::setMatFile(QString file){
    m_MatFile=file;
}
void Control::setGeoFile(QString file){
    m_GeoFile=file;
}
bool Control::setBuildingRotation(double value){
    if (value>360 || value<-360){
        std::cerr<<"ERROR: The building rotation must be between -360 and 360."<<std::endl;
        return false;
    }else{
        m_BuildingRotation=value;
    }
    return true;
}
void Control::setPTSFile(QString file){
    m_PTSFile=file;
}
bool Control::setImportUnits(QString units){
    if (units=="ft" || units=="in" || units=="mm" || units=="m"){
        m_ImportUnits=units;
    }else{
        std::cerr<<"ERROR: The import units must be one of the following:\n\t\"ft\", \"in\", \"mm\", \"m\""<<std::endl;
        return false;
    }
    return true;
}
bool Control::setIllumUnits(QString units){
    if (units=="lux" || units=="fc"){
        m_IllumUnits=units;
    }else{
        std::cerr<<"ERROR: The illuminance units must be either \"lux\" or \"fc\"."<<std::endl;
        return false;
    }
    return true;
}
bool Control::setDisplayUnits(QString units){
    if (units=="ft" || units=="in" || units=="mm" || units=="m"){
        m_DisplayUnits=units;
    }else{
        std::cerr<<"ERROR: The display units must be one of the following:\n\t\"ft\", \"in\", \"mm\", \"m\""<<std::endl;
        return false;
    }
    return true;
}
void Control::setOccSchedule(QString file){
    m_OccSchedule=file;
}
bool Control::setTargetIlluminance(double value){
    if (value<0){
        std::cerr<<"ERROR: The target illuminance must be greater than 0."<<std::endl;
        return false;
    }else{
        m_TargetIlluminance=value;
    }
    return true;
}

//******************
//Simulation Settings
//******************
bool Control::setSunDivisions(int value){
    if (value<1 || value>6){
        std::cerr<<"WARNING: The sun divisions must be between 1 and 6.\n\tA default value of 3 will be applied."<<std::endl;
        m_SunDivisions=3;
    }else{
        m_SunDivisions=value;
    }
    return true;
}
bool Control::setSkyDivisions(int value){
    if (value<1 || value>6){
        std::cerr<<"WARNING: The sky divisions must be between 1 and 6.\n\tA default value of 3 will be applied."<<std::endl;
        m_SkyDivisions=3;
    }else{
        m_SkyDivisions=value;
    }
    return true;
}
void Control::setDaylightSavingsTime(bool value){
    m_DaylightSavingsTime=value;
}
bool Control::setDefaultRadianceParameters(){
    std::cerr<<"\t\tab=4"<<std::endl;
    if (!setAB(4)){
        return false;
    }
    std::cerr<<"\t\tad=300"<<std::endl;
    if (!setAD(300)){
        return false;
    }
    std::cerr<<"\t\tas=20"<<std::endl;
    if (!setAS(20)){
        return false;
    }
    std::cerr<<"\t\tar=150"<<std::endl;
    if (!setAR(150)){
        return false;
    }
    std::cerr<<"\t\taa=0.1"<<std::endl;
    if (!setAA(0.1)){
        return false;
    }
    std::cerr<<"\t\tlr=6"<<std::endl;
    if (!setLR(6)){
        return false;
    }
    std::cerr<<"\t\tst=0.15"<<std::endl;
    if (!setST(0.15)){
        return false;
    }
    std::cerr<<"\t\tsj=1.0"<<std::endl;
    if (!setSJ(1.0)){
        return false;
    }
    std::cerr<<"\t\tlw=0.004"<<std::endl;
    if (!setLW(0.004)){
        return false;
    }
    std::cerr<<"\t\tdj=0.0000"<<std::endl;
    if (!setDJ(0.0000)){
        return false;
    }
    std::cerr<<"\t\tds=0.200"<<std::endl;
    if (!setDS(0.200)){
        return false;
    }
    std::cerr<<"\t\tdr=2"<<std::endl;
    if (!setDR(2)){
        return false;
    }
    std::cerr<<"\t\tdp=512"<<std::endl;
    if (!setDP(512)){
        return false;
    }
    return true;
}

bool Control::setAB(int value){
    m_AB=value;
    return true;
}
bool Control::setAD(int value){
    m_AD=value;
    return true;
}
bool Control::setAS(int value){
    m_AS=value;
    return true;
}
bool Control::setAR(int value){
    m_AR=value;
    return true;
}
bool Control::setAA(double value){
    m_AA=value;
    return true;
}
bool Control::setLR(int value){
    m_LR=value;
    return true;
}
bool Control::setST(double value){
    m_ST=value;
    return true;
}
bool Control::setSJ(double value){
    m_SJ=value;
    return true;
}
bool Control::setLW(double value){
    m_LW=value;
    return true;
}
bool Control::setDJ(double value){
    m_DJ=value;
    return true;
}
bool Control::setDS(double value){
    m_DS=value;
    return true;
}
bool Control::setDR(int value){
    m_DR=value;
    return true;
}
bool Control::setDP(double value){
    m_DP=value;
    return true;
}


//******************
//Metrics
//******************
bool Control::setDA(bool run, double illum){
    m_DA=run;
    if (illum>0){
        m_DAIllum=illum;
    }else{
        std::cerr<<"ERROR: The DA illuminance must be greater than 0."<<std::endl;
        return false;
    }
    return true;
}
bool Control::setcDA(bool run, double illum){
    m_cDA=run;
    if (illum>0){
        m_cDAIllum=illum;
    }else{
        std::cerr<<"ERROR: The cDA illuminance must be greater than 0."<<std::endl;
        return false;
    }
    return true;
}
bool Control::setsDA(bool run, double illum, double DAFrac, double startTime, double endTime){
    m_sDA=run;
    if (illum>0){
        m_sDAIllum=illum;
    }else{
        std::cerr<<"ERROR: The sDA illuminance must be greater than 0."<<std::endl;
        return false;
    }
    if (DAFrac>0 && DAFrac<1){
        m_sDAFrac=DAFrac;
    }else{
        std::cerr<<"ERROR: The sDA fraction must be between 0 and 1."<<std::endl;
        return false;
    }
    if (startTime>0 && startTime<endTime){
        m_sDAStart=startTime;
    }else{
        std::cerr<<"ERROR: The sDA start time has to be greater than 0 and less than the end time."<<std::endl;
        return false;
    }
    if (endTime<24 && endTime>startTime){
        m_sDAEnd=endTime;
    }else{
        std::cerr<<"ERROR: The sDA end time has to be greater than the start time and less than 24."<<std::endl;
        return false;
    }
    return true;
}
bool Control::setOccsDA(bool run, double illum, double DAFrac){
    m_OccsDA=run;
    if (illum>0){
        m_OccsDAIllum=illum;
    }else{
        std::cerr<<"ERROR: The occupancy schedule based sDA illuminance must be greater than 0."<<std::endl;
        return false;
    }
    if (DAFrac>0 && DAFrac<1){
        m_OccsDAFrac=DAFrac;
    }else{
        std::cerr<<"ERROR: The occupancy schedule based sDA fraction must be between 0 and 1."<<std::endl;
        return false;
    }
    return true;
}
void Control::setDF(bool run){
    m_DF=run;
}
bool Control::setUDI(bool run, double minIllum, double maxIllum){
    m_UDI=run;
    if (minIllum>=0 && minIllum<maxIllum){
        m_UDIMin=minIllum;
    }else{
        std::cerr<<"ERROR: The UDI minimum illuminance must be between 0 and the maximum illuminance."<<std::endl;
        return false;
    }
    m_UDIMax=maxIllum;
    return true;
}



//Getters
//******************
//Folder Information
//******************
QString Control::projectName(){
    return m_ProjectName;
}
QString Control::projectFolder(){
    return m_ProjectFolder;
}
QString Control::tmpFolder(){
    return m_TmpFolder;
}
QString Control::geoFolder(){
    return m_GeoFolder;
}
QString Control::iesFolder(){
    return m_IESFolder;
}
QString Control::resultsFolder(){
    return m_ResultsFolder;
}
QString Control::dataFolder(){
    return m_DataFolder;
}

//******************
//Site Information
//******************
double Control::groundReflect(){
    return m_GroundReflect;
}
QString Control::weaDataFile(){
    return m_WeaDataFile;
}
int Control::firstDay(){
    return m_FirstDay;
}

//******************
//Geometry Information
//******************
QString Control::matFile(){
    return m_MatFile;
}
QString Control::geoFile(){
    return m_GeoFile;
}
double Control::buildingRotation(){
    return m_BuildingRotation;
}
QString Control::ptsFile(){
    return m_PTSFile;
}
std::vector<WindowGroup*> Control::windowGroups(){
    return m_WindowGroups;
}
QString Control::importUnits(){
    return m_ImportUnits;
}
QString Control::illumUnits(){
    return m_IllumUnits;
}
QString Control::displayUnits(){
    return m_DisplayUnits;
}
QString Control::occSchedule(){
    return m_OccSchedule;
}
double Control::targetIlluminance(){
    return m_TargetIlluminance;
}

//******************
//Simulation Settings
//******************
int Control::sunDivisions(){
    return m_SunDivisions;
}
int Control::skyDivisions(){
    return m_SkyDivisions;
}

bool Control::daylightSavingsTime(){
    return m_DaylightSavingsTime;
}
int Control::ab(){
    return m_AB;
}
int Control::ad(){
    return m_AD;
}
int Control::as(){
    return m_AS;
}
int Control::ar(){
    return m_AR;
}
double Control::aa(){
    return m_AA;
}
int Control::lr(){
    return m_LR;
}
double Control::st(){
    return m_ST;
}
double Control::sj(){
    return m_SJ;
}
double Control::lw(){
    return m_LW;
}
double Control::dj(){
    return m_DJ;
}
double Control::ds(){
    return m_DS;
}
int Control::dr(){
    return m_DR;
}
double Control::dp(){
    return m_DP;
}

//******************
//Lighting Control
//******************
std::vector<ControlZone*> Control::controlZones(){
    return m_ControlZones;
}

//******************
//Metrics
//******************
bool Control::runDA(){
    return m_DA;
}
double Control::DAIllum(){
    return m_DAIllum;
}
bool Control::runsDA(){
    return m_sDA;
}
double Control::sDAIllum(){
    return m_sDAIllum;
}
double Control::sDAFrac(){
    return m_sDAFrac;
}
double Control::sDAStart(){
    return m_sDAStart;
}
double Control::sDAEnd(){
    return m_sDAEnd;
}
bool Control::runOccsDA(){
    return m_OccsDA;
}
double Control::occsDAIllum(){
    return m_OccsDAIllum;
}
double Control::occsDAFrac(){
    return m_OccsDAFrac;
}
bool Control::runcDA(){
    return m_cDA;
}
double Control::cDAIllum(){
    return m_cDAIllum;
}
bool Control::runDF(){
    return m_DF;
}
bool Control::runUDI(){
    return m_UDI;
}
double Control::UDIMin(){
    return m_UDIMin;
}
double Control::UDIMax(){
    return m_UDIMax;
}


//******************
//PARSER
//******************
bool Control::parseJson(QString file){
    QString data;
    QFile iFile;
    iFile.setFileName(file);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    data=iFile.readAll();
    iFile.close();
    QJsonDocument json= QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObj= json.object();

    //******************
    //Folder Information
    //******************
    QJsonValue val= jsonObj.value(QString("project_name"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"project_name\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setProjectName(val.toString());
        }else{
            std::cerr<<"ERROR: The \"project_name\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("project_folder"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"project_folder\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setProjectFolder(val.toString());
        }else{
            std::cerr<<"ERROR: The \"project_folder\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("tmp_folder"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"tmp_folder\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setTmpFolder(val.toString());
        }else{
            std::cerr<<"ERROR: The \"tmp_folder\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("geometry_folder"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"geometry_folder\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setGeoFolder(val.toString());
        }else{
            std::cerr<<"ERROR: The \"geometry_folder\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("ies_folder"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"ies_folder\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setIESFolder(val.toString());
        }else{
            std::cerr<<"ERROR: The \"ies_folder\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("results_folder"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"results_folder\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setResultsFolder(val.toString());
        }else{
            std::cerr<<"ERROR: The \"results_folder\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("data_folder"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"data_folder\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setDataFolder(val.toString());
        }else{
            std::cerr<<"ERROR: The \"data_folder\" is not a string."<<std::endl;
            return false;
        }
    }

    //******************
    //Site Information
    //******************
    val=jsonObj.value(QString("ground_reflectance"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"ground_reflectance\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isDouble()){
            if (!setGroundReflect(val.toDouble())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"ground_reflectance\" is not a double."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("wea_data_file"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"wea_data_file\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setWeaDataFile(val.toString());
        }else{
            std::cerr<<"ERROR: The \"wea_data_file\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("first_day"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"first_day\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        int num;
        num=val.toDouble();
        if ((val.toDouble()-num)<0.01 && (val.toDouble()-num>-0.01)){
            if (!setFirstDay(val.toDouble())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"first_day\" is not an integer."<<std::endl;
            return false;
        }
    }

    //******************
    //Geometry Information
    //******************
    val=jsonObj.value(QString("material_file"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"material_file\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setMatFile(val.toString());
        }else{
            std::cerr<<"ERROR: The \"material_file\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("geometry_file"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"geometry_file\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setGeoFile(val.toString());
        }else{
            std::cerr<<"ERROR: The \"geometry_file\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("building_rotation"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"building_rotation\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isDouble()){
            if (!setBuildingRotation(val.toDouble())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"building_rotation\" is not a double."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("analysis_points"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"analysis_points\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setPTSFile(val.toString());
        }else{
            std::cerr<<"ERROR: The \"analysis_points\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("window_groups"));
    QJsonArray array=val.toArray();
    if (val.isUndefined()){
        std::cerr<<"ERROR:  The key \"window_groups\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        for (int i=0;i<array.size();i++){
            WindowGroup *WG=new WindowGroup(this);
            if (array[i].isObject()){
                if(WG->parseJson(array[i].toObject())){
                    m_WindowGroups.push_back(WG);
                }else{
                    return false;
                }
            }else{
                std::cerr<<"ERROR: The \"window_groups\" is not an object."<<std::endl;
                return false;
            }
        }
    }


    val=jsonObj.value(QString("import_units"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"import_units\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            if (!setImportUnits(val.toString())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"import_units\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("illum_units"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"illum_units\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            if (!setIllumUnits(val.toString())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"illum_units\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("display_units"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"display_units\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            if (!setDisplayUnits(val.toString())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"display_units\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("occupancy_schedule"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"occupancy_schedule\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isString()){
            setOccSchedule(val.toString());
        }else{
            std::cerr<<"ERROR: The \"occupancy_schedule\" is not a string."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("target_illuminance"));
    if (val.isUndefined()){
        std::cerr<<"ERROR: The key \"target_illuminance\" does not appear in the STADIC Control File."<<std::endl;
        return false;
    }else{
        if (val.isDouble()){
            if (!setTargetIlluminance(val.toDouble())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"target_illuminance\" is not a double."<<std::endl;
            return false;
        }
    }

    //******************
    //Simulation Settings
    //******************
    val=jsonObj.value(QString("sun_divisions"));
    if (val.isUndefined()){
        std::cerr<<"WARNING: The key \"sun_divisions\" does not appear in the STADIC Control File.\n\tSun Divisions will be set to 3."<<std::endl;
        setSunDivisions(3);
    }else{
        int num;
        num=val.toDouble();
        if ((val.toDouble()-num)<0.01 && (val.toDouble()-num>-0.01)){
            if (!setSunDivisions(val.toDouble())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"sun_divisions\" is not an integer."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("sky_divisions"));
    if (val.isUndefined()){
        std::cerr<<"WARNING: The key \"sky_divisions\" does not appear in the STADIC Control File.\n\tSky Divisions will be set to 3."<<std::endl;
        setSkyDivisions(3);
    }else{
        int num;
        num=val.toDouble();
        if ((val.toDouble()-num)<0.01 && (val.toDouble()-num>-0.01)){
            if (!setSkyDivisions(val.toDouble())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"sky_divisions\" is not an integer."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("daylight_savings_time"));
    if (val.isUndefined()){
        std::cerr<<"WARNING: The key \"daylight_savings_time\" does not appear in the STADIC Control File.\n\tDaylight Savings Time will be enabled."<<std::endl;
        setDaylightSavingsTime(true);
    }else{
        if (val.isBool()){
               setDaylightSavingsTime(val.toBool());
        }else{
            std::cerr<<"ERROR: The \"daylight_savings_time\" is not a boolean."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("radiance_parameters"));
    if (val.isUndefined()){
        std::cerr<<"WARNING: The key \"radiance_parameters\" does not appear in the STADIC Control File.\n\tThe default values of the parameters will be as follows."<<std::endl;
        if (!setDefaultRadianceParameters()){
            return false;
        }
    }else{
        if (val.isObject()){
            QJsonObject tempObject=val.toObject();
            val=tempObject.value("ab");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"ab\" was not found. A default value of 4 will be applied."<<std::endl;
                if (!setAB(4)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAB(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"ab\" is not a number. A default value of 4 will be applied."<<std::endl;
                    if (!setAB(4)){
                        return false;
                    }
                }
            }

            val=tempObject.value("ad");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"ad\" was not found. A default value of 300 will be applied."<<std::endl;
                if (!setAD(300)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAD(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"ad\" is not a number. A default value of 300 will be applied."<<std::endl;
                    if (!setAD(300)){
                        return false;
                    }
                }
            }

            val=tempObject.value("as");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"as\" was not found. A default value of 20 will be applied."<<std::endl;
                if (!setAS(20)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAS(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"as\" is not a number. A default value of 20 will be applied."<<std::endl;
                    if (!setAS(20)){
                        return false;
                    }
                }
            }

            val=tempObject.value("ar");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"ar\" was not found. A default value of 150 will be applied."<<std::endl;
                if (!setAR(150)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAR(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"ar\" is not a number. A default value of 150 will be applied."<<std::endl;
                    if (!setAR(150)){
                        return false;
                    }
                }
            }

            val=tempObject.value("aa");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"aa\" was not found. A default value of 0.1 will be applied."<<std::endl;
                if (!setAA(0.1)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAA(val.toDouble())){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"aa\" is not a number. A default value of 0.1 will be applied."<<std::endl;
                    if (!setAA(0.1)){
                        return false;
                    }
                }
            }

            val=tempObject.value("lr");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"lr\" was not found. A default value of 6 will be applied."<<std::endl;
                if (!setLR(6)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setLR(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"lr\" is not a number. A default value of 6 will be applied."<<std::endl;
                    if (!setLR(6)){
                        return false;
                    }
                }
            }

            val=tempObject.value("st");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"st\" was not found. A default value of 0.15 will be applied."<<std::endl;
                if (!setST(0.15)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setST(val.toDouble())){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"st\" is not a number. A default value of 0.15 will be applied."<<std::endl;
                    if (!setST(0.15)){
                        return false;
                    }
                }
            }

            val=tempObject.value("sj");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"sj\" was not found. A default value of 1.0 will be applied."<<std::endl;
                if (!setSJ(1.0)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setSJ(val.toDouble())){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"sj\" is not a number. A default value of 1.0 will be applied."<<std::endl;
                    if (!setSJ(1.0)){
                        return false;
                    }
                }
            }

            val=tempObject.value("lw");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"lw\" was not found. A default value of 0.004 will be applied."<<std::endl;
                if (!setLW(0.004)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setLW(val.toDouble())){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"lw\" is not a number. A default value of 0.004 will be applied."<<std::endl;
                    if (!setLW(0.004)){
                        return false;
                    }
                }
            }

            val=tempObject.value("dj");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"dj\" was not found. A default value of 0.0 will be applied."<<std::endl;
                if (!setDJ(0.0000)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setDJ(val.toDouble())){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"dj\" is not a number. A default value of 0.0 will be applied."<<std::endl;
                    if (!setDJ(0.0000)){
                        return false;
                    }
                }
            }

            val=tempObject.value("ds");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"ds\" was not found. A default value of 0.200 will be applied."<<std::endl;
                if (!setDS(0.200)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setDS(val.toDouble())){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"ds\" is not a number. A default value of 0.200 will be applied."<<std::endl;
                    if (!setDS(0.200)){
                        return false;
                    }
                }
            }

            val=tempObject.value("dr");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"dr\" was not found. A default value of 2 will be applied."<<std::endl;
                if (!setDR(2)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setDR(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"dr\" is not a number. A default value of 2 will be applied."<<std::endl;
                    if (!setDR(2)){
                        return false;
                    }
                }
            }

            val=tempObject.value("dp");
            if (val.isUndefined()){
                std::cerr<<"WARNING: The parameter \"dp\" was not found. A default value of 512 will be applied."<<std::endl;
                if (!setDP(512)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setDP(val.toDouble())){
                        return false;
                    }
                }else{
                    std::cerr<<"WARNING: The parameter \"dp\" is not a number. A default value of 512 will be applied."<<std::endl;
                    if (!setDP(512)){
                        return false;
                    }
                }
            }
            if (!setDP(512)){
                return false;
            }
        }else{
            std::cerr<<"The key \"radiance_parameters\" is not an object.\n\tThe default values of the parameters will be as follows."<<std::endl;
            if (!setDefaultRadianceParameters()){
                return false;
            }
        }
    }

    //******************
    //Lighting Control
    //******************
    val=jsonObj.value(QString("control_zones"));
    array=val.toArray();
    if (val.isUndefined()){
        std::cerr<<"WARNING:  The key \"control_zones\" does not appear in the STADIC Control File."<<std::endl;
    }else{
        for (int i=0;i<array.size();i++){
            ControlZone *zone=new ControlZone(this);
            if (array[i].isObject()){
                if(zone->parseJson(array[i].toObject())){
                    m_ControlZones.push_back(zone);
                }else{
                    return false;
                }
            }else{
                std::cerr<<"ERROR: The \"control_zones\" is not an object."<<std::endl;
                return false;
            }
        }
    }


    //******************
    //Metrics
    //******************
    val=jsonObj.value(QString("sDA"));
    if (!val.isUndefined()){
        if (val.isObject()){
            double illum, frac, start, endTime;
            QJsonObject tempObject=val.toObject();
            val=tempObject.value(QString("illuminance"));
            if (val.isUndefined()){
                std::cerr<<"WARNING: The key \"illuminance\" is missing under sDA."<<std::endl;
                std::cerr<<"\tAn assumed value of 300 will be used."<<std::endl;
                illum=300;
            }else{
                if (val.isDouble()){
                    illum=val.toDouble();
                }else{
                    std::cerr<<"WARNING: The key \"illuminance\" does not contain a number."<<std::endl;
                    std::cerr<<"\tAn assumed value of 300 will be used."<<std::endl;
                    illum=300;
                }
            }
            val=tempObject.value(QString("DA_fraction"));
            if (val.isUndefined()){
                std::cerr<<"WARNING: The key \"DA_fraction\" is missing under sDA."<<std::endl;
                std::cerr<<"\tAn assumed value of 0.5 will be used."<<std::endl;
                frac=0.5;
            }else{
                if (val.isDouble()){
                    frac=val.toDouble();
                }else{
                    std::cerr<<"WARNING: The key \"DA_fraction\" does not contain a number."<<std::endl;
                    std::cerr<<"\tAn assumed value of 0.5 will be used."<<std::endl;
                    frac=0.5;
                }
            }
            val=tempObject.value(QString("start_time"));
            if (val.isUndefined()){
                std::cerr<<"WARNING: The key \"start_time\" is missing under sDA."<<std::endl;
                std::cerr<<"\tAn assumed value of 8 will be used."<<std::endl;
                start=8;
            }else{
                if (val.isDouble()){
                    start=val.toDouble();
                }else{
                    std::cerr<<"WARNING: The key \"start_time\" does not contain a number."<<std::endl;
                    std::cerr<<"\tAn assumed value of 8 will be used."<<std::endl;
                    start=8;
                }
            }
            val=tempObject.value(QString("end_time"));
            if (val.isUndefined()){
                std::cerr<<"WARNING: The key \"end_time\" is missing under sDA."<<std::endl;
                std::cerr<<"\tAn assumed value of 17 will be used."<<std::endl;
                endTime=17;
            }else{
                if (val.isDouble()){
                    endTime=val.toDouble();
                }else{
                    std::cerr<<"WARNING: The key \"end_time\" does not contain a number."<<std::endl;
                    std::cerr<<"\tAn assumed value of 17 will be used."<<std::endl;
                    endTime=17;
                }
            }
            if (!setsDA(true, illum, frac, start, endTime)){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"sDA\" is not an object."<<std::endl;
            return false;
        }
    }
    val=jsonObj.value(QString("occupied_sDA"));
    if (!val.isUndefined()){
        if (val.isObject()){
            double illum, frac;
            QJsonObject tempObject=val.toObject();
            val=tempObject.value(QString("illuminance"));
            if (val.isUndefined()){
                std::cerr<<"WARNING: The key \"illuminance\" is missing under occupied_sDA."<<std::endl;
                std::cerr<<"\tAn assumed value of 300 will be used."<<std::endl;
                illum=300;
            }else{
                if (val.isDouble()){
                    illum=val.toDouble();
                }else{
                    std::cerr<<"WARNING: The key \"illuminance\" does not contain a number."<<std::endl;
                    std::cerr<<"\tAn assumed value of 300 will be used."<<std::endl;
                    illum=300;
                }
            }
            val=tempObject.value(QString("DA_fraction"));
            if (val.isUndefined()){
                std::cerr<<"WARNING: The key \"DA_fraction\" is missing under occupied_sDA."<<std::endl;
                std::cerr<<"\tAn assumed value of 0.5 will be used."<<std::endl;
                frac=0.5;
            }else{
                if (val.isDouble()){
                    frac=val.toDouble();
                }else{
                    std::cerr<<"WARNING: The key \"DA_fraction\" does not contain a number."<<std::endl;
                    std::cerr<<"\tAn assumed value of 0.5 will be used."<<std::endl;
                    frac=0.5;
                }
            }
            if (!setOccsDA(true, illum, frac)){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The key \"occupied_sDA\" is not an object."<<std::endl;
            return false;
        }
    }
    val=jsonObj.value(QString("DA"));
    if (!val.isUndefined()){
        if (val.isDouble()){
            if (!setDA(true, val.toDouble())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"DA\" is not a double."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("cDA"));
    if (!val.isUndefined()){
        if (val.isDouble()){
            if (!setcDA(true, val.toDouble())){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The \"cDA\" is not a double."<<std::endl;
            return false;
        }
    }

    val=jsonObj.value(QString("DF"));
    if (!val.isUndefined()){
        if (val.isBool()){
               setDF(val.toBool());
        }else{
            std::cerr<<"ERROR: The \"DF\" is not a boolean."<<std::endl;
            return false;
        }
    }
    val=jsonObj.value(QString("UDI"));
    if (!val.isUndefined()){
        if (val.isObject()){
            double minimum, maximum;
            QJsonObject tempObject=val.toObject();
            val=tempObject.value(QString("minimum"));
            if (val.isUndefined()){
                std::cerr<<"WARNING: The key \"minimum\" is missing under UDI."<<std::endl;
                std::cerr<<"\tAn assumed value of 100 will be used."<<std::endl;
                minimum=100;
            }else{
                if (val.isDouble()){
                    minimum=val.toDouble();
                }else{
                    std::cerr<<"WARNING: The key \"minimum\" is not a number."<<std::endl;
                    std::cerr<<"\tAn assumed value of 100 will be used."<<std::endl;
                    minimum=100;
                }
            }
            val=tempObject.value(QString("maximum"));
            if (val.isUndefined()){
                std::cerr<<"WARNING: The key \"maximum\" is missing under UDI."<<std::endl;
                std::cerr<<"\tAn assumed value of 250 will be used."<<std::endl;
                maximum=250;
            }else{
                if (val.isDouble()){
                    maximum=val.toDouble();
                }else{
                    std::cerr<<"WARNING: The key \"maximum\" is not a number."<<std::endl;
                    std::cerr<<"\tAn assumed value of 250 will be used."<<std::endl;
                    maximum=250;
                }
            }
            if (!setUDI(true, minimum, maximum)){
                return false;
            }
        }else{
            std::cerr<<"ERROR: The key \"UDI\" is not an object."<<std::endl;
            return false;
        }
    }


    return true;
}

}
