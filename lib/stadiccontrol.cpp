#include "stadiccontrol.h"
#include "windowgroup.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QString>
#include <iostream>
#include <fstream>
#include <string>
#include "functions.h"
#include <boost/property_tree/json_parser.hpp>
//#include <boost/foreach.hpp>

namespace stadic {

Control::Control()
{
}

//Setters
//******************
//Folder Information
//******************
void Control::setProjectName(std::string name){
    m_ProjectName=name;
}
void Control::setProjectFolder(std::string folder){
    m_ProjectFolder=folder;
}
void Control::setTmpFolder(std::string folder){
    m_TmpFolder=folder;
}
void Control::setGeoFolder(std::string folder){
    m_GeoFolder=folder;
}
void Control::setIESFolder(std::string folder){
    m_IESFolder=folder;
}
void Control::setResultsFolder(std::string folder){
    m_ResultsFolder=folder;
}
void Control::setDataFolder(std::string folder){
    m_DataFolder=folder;
}

//******************
//Site Information
//******************
bool Control::setGroundReflect(double value){
    if (value>1 || value<0){
        STADIC_WARNING("The ground reflectance must be between 0 and  1.\n\tA default value of 0.2 will be applied.");
        //std::cerr<<"WARNING: The ground reflectance must be between 0 and  1.\n\tA default value of 0.2 will be applied."<<std::endl;
        m_GroundReflect=0.2;
    }else{
        m_GroundReflect=value;
    }
    return true;
}
void Control::setWeaDataFile(std::string file){
    m_WeaDataFile=file;
}
bool Control::setFirstDay(int value){
    if (value>7 || value<1){
        STADIC_WARNING("The first day must be an integer between 1 and 7.\n\tA default value of 1 will be applied.");
        m_FirstDay=1;
    }else{
        m_FirstDay=value;
    }
    return true;
}

//******************
//Geometry Information
//******************
void Control::setMatFile(std::string file){
    m_MatFile=file;
}
void Control::setGeoFile(std::string file){
    m_GeoFile=file;
}
bool Control::setBuildingRotation(double value){
    if (value>360 || value<-360){
        STADIC_ERROR("The building rotation must be between -360 and 360.");
        return false;
    }else{
        m_BuildingRotation=value;
    }
    return true;
}
void Control::setPTSFile(std::string file){
    m_PTSFile=file;
}
bool Control::setImportUnits(std::string units){
    if (units=="ft" || units=="in" || units=="mm" || units=="m"){
        m_ImportUnits=units;
    }else{
        STADIC_ERROR("The import units must be one of the following:\n\t\"ft\", \"in\", \"mm\", \"m\"");
        return false;
    }
    return true;
}
void Control::setIllumUnits(std::string units){
    if (units=="lux" || units=="fc"){
        m_IllumUnits=units;
    }else{
        STADIC_WARNING("The illuminance units must be either \"lux\" or \"fc\".  The default illuminance units will be set to lux.");
        m_IllumUnits="lux";
    }
}
bool Control::setDisplayUnits(std::string units){
    if (units=="ft" || units=="in" || units=="mm" || units=="m"){
        m_DisplayUnits=units;
    }else{
        STADIC_ERROR("The display units must be one of the following:\n\t\"ft\", \"in\", \"mm\", \"m\"");
        return false;
    }
    return true;
}
void Control::setOccSchedule(std::string file){
    m_OccSchedule=file;
}
bool Control::setTargetIlluminance(double value){
    if (value<0){
        STADIC_ERROR("The target illuminance must be greater than 0.");
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
        STADIC_WARNING("The sun divisions must be between 1 and 6.\n\tA default value of 3 will be applied.");
        m_SunDivisions=3;
    }else{
        m_SunDivisions=value;
    }
    return true;
}

bool Control::setSkyDivisions(int value)
{
    if (value<1 || value>6){
        STADIC_WARNING("The sky divisions must be between 1 and 6.\n\tA default value of 3 will be applied.");
        m_SkyDivisions=3;
    }else{
        m_SkyDivisions=value;
    }
    return true;
}

void Control::setDaylightSavingsTime(bool value)
{
    m_DaylightSavingsTime=value;
}

bool Control::setDefaultRadianceParameters()
{
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
        STADIC_ERROR("The DA illuminance must be greater than 0.");
        return false;
    }
    return true;
}
bool Control::setcDA(bool run, double illum){
    m_cDA=run;
    if (illum>0){
        m_cDAIllum=illum;
    }else{
        STADIC_ERROR("The cDA illuminance must be greater than 0.");
        return false;
    }
    return true;
}
bool Control::setsDA(bool run, double illum, double DAFrac, double startTime, double endTime){
    m_sDA=run;
    if (illum>0){
        m_sDAIllum=illum;
    }else{
        STADIC_ERROR("The sDA illuminance must be greater than 0.");
        return false;
    }
    if (DAFrac>0 && DAFrac<1){
        m_sDAFrac=DAFrac;
    }else{
        STADIC_ERROR("The sDA fraction must be between 0 and 1.");
        return false;
    }
    if (startTime>0 && startTime<endTime){
        m_sDAStart=startTime;
    }else{
        STADIC_ERROR("The sDA start time has to be greater than 0 and less than the end time.");
        return false;
    }
    if (endTime<24 && endTime>startTime){
        m_sDAEnd=endTime;
    }else{
        STADIC_ERROR("The sDA end time has to be greater than the start time and less than 24.");
        return false;
    }
    return true;
}
bool Control::setOccsDA(bool run, double illum, double DAFrac){
    m_OccsDA=run;
    if (illum>0){
        m_OccsDAIllum=illum;
    }else{
        STADIC_ERROR("The occupancy schedule based sDA illuminance must be greater than 0.");
        return false;
    }
    if (DAFrac>0 && DAFrac<1){
        m_OccsDAFrac=DAFrac;
    }else{
        STADIC_ERROR("The occupancy schedule based sDA fraction must be between 0 and 1.");
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
        STADIC_ERROR("The UDI minimum illuminance must be between 0 and the maximum illuminance.");
        return false;
    }
    m_UDIMax=maxIllum;
    return true;
}



//Getters
//******************
//Folder Information
//******************
std::string Control::projectName(){
    return m_ProjectName;
}
std::string Control::projectFolder(){
    return m_ProjectFolder;
}
std::string Control::tmpFolder(){
    return m_TmpFolder;
}
std::string Control::geoFolder(){
    return m_GeoFolder;
}
std::string Control::iesFolder(){
    return m_IESFolder;
}
std::string Control::resultsFolder(){
    return m_ResultsFolder;
}
std::string Control::dataFolder(){
    return m_DataFolder;
}

//******************
//Site Information
//******************
double Control::groundReflect(){
    return m_GroundReflect;
}
std::string Control::weaDataFile(){
    return m_WeaDataFile;
}
int Control::firstDay(){
    return m_FirstDay;
}

//******************
//Geometry Information
//******************
std::string Control::matFile(){
    return m_MatFile;
}
std::string Control::geoFile(){
    return m_GeoFile;
}
double Control::buildingRotation(){
    return m_BuildingRotation;
}
std::string Control::ptsFile(){
    return m_PTSFile;
}
std::vector<WindowGroup> Control::windowGroups(){
    return m_WindowGroups;
}
std::string Control::importUnits(){
    return m_ImportUnits;
}
std::string Control::illumUnits(){
    return m_IllumUnits;
}
std::string Control::displayUnits(){
    return m_DisplayUnits;
}
std::string Control::occSchedule(){
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
std::vector<ControlZone> Control::controlZones(){
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
/*
This is my proposed change - there'd only be one function per type instead of two
boost::optional<boost::property_tree::ptree> Control::getTreeWarn(boost::property_tree::ptree json, std::string key, std::string errorMessage, Severity severity){
    boost::property_tree::ptree treeVal;
    try {
        treeVal = json.get_child(key);
    } catch(const boost::property_tree::ptree_bad_path &) {
        STADIC_LOG(severity, errorMessage);
        return boost::none;
    }
    return boost::optional<boost::property_tree::ptree>(treeVal);
}
*/

bool Control::parseJson(std::string file){
    QString data;
    QFile iFile;
    iFile.setFileName(QString::fromStdString(file));
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!iFile.isOpen()) {
        return false;
    }
    data=iFile.readAll();
    iFile.close();
    QJsonDocument json= QJsonDocument::fromJson(data.toUtf8());
    QJsonObject jsonObj= json.object();

    //******************
    //Folder Information
    //******************
    QJsonValue val= jsonObj.value("project_name");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"project_name\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setProjectName(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"project_name\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("project_folder");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"project_folder\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setProjectFolder(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"project_folder\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("tmp_folder");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"tmp_folder\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setTmpFolder(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"tmp_folder\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("geometry_folder");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"geometry_folder\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setGeoFolder(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"geometry_folder\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("ies_folder");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"ies_folder\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setIESFolder(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"ies_folder\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("results_folder");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"results_folder\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setResultsFolder(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"results_folder\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("data_folder");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"data_folder\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setDataFolder(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"data_folder\" is not a string.");
            return false;
        }
    }

    //******************
    //Site Information
    //******************
    val=jsonObj.value("ground_reflectance");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"ground_reflectance\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isDouble()){
            if (!setGroundReflect(val.toDouble())){
                return false;
            }
        }else{
            STADIC_ERROR("The \"ground_reflectance\" is not a double.");
            return false;
        }
    }

    val=jsonObj.value("wea_data_file");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"wea_data_file\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setWeaDataFile(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"wea_data_file\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("first_day");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"first_day\" does not appear in the STADIC Control File.");
        return false;
    }else{
        int num;
        num=val.toDouble();
        if ((val.toDouble()-num)<0.01 && (val.toDouble()-num>-0.01)){
            if (!setFirstDay(val.toDouble())){
                return false;
            }
        }else{
            STADIC_ERROR("The \"first_day\" is not an integer.");
            return false;
        }
    }

    //******************
    //Geometry Information
    //******************
    val=jsonObj.value("material_file");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"material_file\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setMatFile(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"material_file\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("geometry_file");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"geometry_file\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setGeoFile(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"geometry_file\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("building_rotation");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"building_rotation\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isDouble()){
            if (!setBuildingRotation(val.toDouble())){
                return false;
            }
        }else{
            STADIC_ERROR("The \"building_rotation\" is not a double.");
            return false;
        }
    }

    val=jsonObj.value("analysis_points");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"analysis_points\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setPTSFile(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"analysis_points\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value("window_groups");
    QJsonArray array=val.toArray();
    if (val.isUndefined()){
        STADIC_ERROR(" The key \"window_groups\" does not appear in the STADIC Control File.");
        return false;
    }else{
        for (int i=0;i<array.size();i++){
            WindowGroup WG;
            if (array[i].isObject()){
                if(WG.parseJson(array[i].toObject())){
                    m_WindowGroups.push_back(WG);
                }else{
                    return false;
                }
            }else{
                STADIC_ERROR("The \"window_groups\" is not an object.");
                return false;
            }
        }
    }


    val=jsonObj.value("import_units");
    if (val.isUndefined()){
        STADIC_ERROR("The key \"import_units\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            if(!setImportUnits(val.toString().toStdString())){
                return false;
            }
        }else{
            STADIC_ERROR("The \"import_units\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value(QString("illum_units"));
    if (val.isUndefined()){
        STADIC_ERROR("The key \"illum_units\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setIllumUnits(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"illum_units\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value(QString("display_units"));
    if (val.isUndefined()){
        STADIC_ERROR("The key \"display_units\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            if (!setDisplayUnits(val.toString().toStdString())){
                return false;
            }
        }else{
            STADIC_ERROR("The \"display_units\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value(QString("occupancy_schedule"));
    if (val.isUndefined()){
        STADIC_ERROR("The key \"occupancy_schedule\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isString()){
            setOccSchedule(val.toString().toStdString());
        }else{
            STADIC_ERROR("The \"occupancy_schedule\" is not a string.");
            return false;
        }
    }

    val=jsonObj.value(QString("target_illuminance"));
    if (val.isUndefined()){
        STADIC_ERROR("The key \"target_illuminance\" does not appear in the STADIC Control File.");
        return false;
    }else{
        if (val.isDouble()){
            if (!setTargetIlluminance(val.toDouble())){
                return false;
            }
        }else{
            STADIC_ERROR("The \"target_illuminance\" is not a double.");
            return false;
        }
    }

    //******************
    //Simulation Settings
    //******************
    val=jsonObj.value(QString("sun_divisions"));
    if (val.isUndefined()){
        STADIC_WARNING("The key \"sun_divisions\" does not appear in the STADIC Control File.\n\tSun Divisions will be set to 3.");
        setSunDivisions(3);
    }else{
        int num;
        num=val.toDouble();
        if ((val.toDouble()-num)<0.01 && (val.toDouble()-num>-0.01)){
            if (!setSunDivisions(val.toDouble())){
                return false;
            }
        }else{
            STADIC_ERROR("The \"sun_divisions\" is not an integer.");
            return false;
        }
    }

    val=jsonObj.value(QString("sky_divisions"));
    if (val.isUndefined()){
        STADIC_WARNING("The key \"sky_divisions\" does not appear in the STADIC Control File.\n\tSky Divisions will be set to 3.");
        setSkyDivisions(3);
    }else{
        int num;
        num=val.toDouble();
        if ((val.toDouble()-num)<0.01 && (val.toDouble()-num>-0.01)){
            if (!setSkyDivisions(val.toDouble())){
                return false;
            }
        }else{
            STADIC_ERROR("The \"sky_divisions\" is not an integer.");
            return false;
        }
    }

    val=jsonObj.value(QString("daylight_savings_time"));
    if (val.isUndefined()){
        STADIC_WARNING("The key \"daylight_savings_time\" does not appear in the STADIC Control File.\n\tDaylight Savings Time will be enabled.");
        setDaylightSavingsTime(true);
    }else{
        if (val.isBool()){
               setDaylightSavingsTime(val.toBool());
        }else{
            STADIC_ERROR("The \"daylight_savings_time\" is not a boolean.");
            return false;
        }
    }

    val=jsonObj.value(QString("radiance_parameters"));
    if (val.isUndefined()){
        STADIC_WARNING("The key \"radiance_parameters\" does not appear in the STADIC Control File.\n\tThe default values of the parameters will be as follows.");
        if (!setDefaultRadianceParameters()){
            return false;
        }
    }else{
        if (val.isObject()){
            QJsonObject tempObject=val.toObject();
            val=tempObject.value("ab");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"ab\" was not found. A default value of 4 will be applied.");
                if (!setAB(4)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAB(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"ab\" is not a number. A default value of 4 will be applied.");
                    if (!setAB(4)){
                        return false;
                    }
                }
            }

            val=tempObject.value("ad");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"ad\" was not found. A default value of 300 will be applied.");
                if (!setAD(300)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAD(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"ad\" is not a number. A default value of 300 will be applied.");
                    if (!setAD(300)){
                        return false;
                    }
                }
            }

            val=tempObject.value("as");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"as\" was not found. A default value of 20 will be applied.");
                if (!setAS(20)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAS(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"as\" is not a number. A default value of 20 will be applied.");
                    if (!setAS(20)){
                        return false;
                    }
                }
            }

            val=tempObject.value("ar");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"ar\" was not found. A default value of 150 will be applied.");
                if (!setAR(150)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAR(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"ar\" is not a number. A default value of 150 will be applied.");
                    if (!setAR(150)){
                        return false;
                    }
                }
            }

            val=tempObject.value("aa");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"aa\" was not found. A default value of 0.1 will be applied.");
                if (!setAA(0.1)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setAA(val.toDouble())){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"aa\" is not a number. A default value of 0.1 will be applied.");
                    if (!setAA(0.1)){
                        return false;
                    }
                }
            }

            val=tempObject.value("lr");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"lr\" was not found. A default value of 6 will be applied.");
                if (!setLR(6)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setLR(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"lr\" is not a number. A default value of 6 will be applied.");
                    if (!setLR(6)){
                        return false;
                    }
                }
            }

            val=tempObject.value("st");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"st\" was not found. A default value of 0.15 will be applied.");
                if (!setST(0.15)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setST(val.toDouble())){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"st\" is not a number. A default value of 0.15 will be applied.");
                    if (!setST(0.15)){
                        return false;
                    }
                }
            }

            val=tempObject.value("sj");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"sj\" was not found. A default value of 1.0 will be applied.");
                if (!setSJ(1.0)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setSJ(val.toDouble())){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"sj\" is not a number. A default value of 1.0 will be applied.");
                    if (!setSJ(1.0)){
                        return false;
                    }
                }
            }

            val=tempObject.value("lw");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"lw\" was not found. A default value of 0.004 will be applied.");
                if (!setLW(0.004)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setLW(val.toDouble())){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"lw\" is not a number. A default value of 0.004 will be applied.");
                    if (!setLW(0.004)){
                        return false;
                    }
                }
            }

            val=tempObject.value("dj");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"dj\" was not found. A default value of 0.0 will be applied.");
                if (!setDJ(0.0000)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setDJ(val.toDouble())){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"dj\" is not a number. A default value of 0.0 will be applied.");
                    if (!setDJ(0.0000)){
                        return false;
                    }
                }
            }

            val=tempObject.value("ds");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"ds\" was not found. A default value of 0.200 will be applied.");
                if (!setDS(0.200)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setDS(val.toDouble())){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"ds\" is not a number. A default value of 0.200 will be applied.");
                    if (!setDS(0.200)){
                        return false;
                    }
                }
            }

            val=tempObject.value("dr");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"dr\" was not found. A default value of 2 will be applied.");
                if (!setDR(2)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setDR(int(val.toDouble()))){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"dr\" is not a number. A default value of 2 will be applied.");
                    if (!setDR(2)){
                        return false;
                    }
                }
            }

            val=tempObject.value("dp");
            if (val.isUndefined()){
                STADIC_WARNING("The parameter \"dp\" was not found. A default value of 512 will be applied.");
                if (!setDP(512)){
                    return false;
                }
            }else{
                if (val.isDouble()){
                    if (!setDP(val.toDouble())){
                        return false;
                    }
                }else{
                    STADIC_WARNING("The parameter \"dp\" is not a number. A default value of 512 will be applied.");
                    if (!setDP(512)){
                        return false;
                    }
                }
            }
            if (!setDP(512)){
                return false;
            }
        }else{
            STADIC_WARNING("The key \"radiance_parameters\" is not an object.\n\tThe default values of the parameters will be as follows.");
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
        STADIC_WARNING(" The key \"control_zones\" does not appear in the STADIC Control File.");
    }else{
        for (int i=0;i<array.size();i++){
            ControlZone zone;
            if (array[i].isObject()){
                if(zone.parseJson(array[i].toObject())){
                    m_ControlZones.push_back(zone);
                }else{
                    return false;
                }
            }else{
                STADIC_ERROR("The \"control_zones\" is not an object.");
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
                STADIC_WARNING("The key \"illuminance\" is missing under sDA.\n\tAn assumed value of 300 will be used.");
                illum=300;
            }else{
                if (val.isDouble()){
                    illum=val.toDouble();
                }else{
                    STADIC_WARNING("The key \"illuminance\" does not contain a number.\n\tAn assumed value of 300 will be used.");
                    illum=300;
                }
            }
            val=tempObject.value(QString("DA_fraction"));
            if (val.isUndefined()){
                STADIC_WARNING("The key \"DA_fraction\" is missing under sDA.\n\tAn assumed value of 0.5 will be used.");
                frac=0.5;
            }else{
                if (val.isDouble()){
                    frac=val.toDouble();
                }else{
                    STADIC_WARNING("The key \"DA_fraction\" does not contain a number.\n\tAn assumed value of 0.5 will be used.");
                    frac=0.5;
                }
            }
            val=tempObject.value(QString("start_time"));
            if (val.isUndefined()){
                STADIC_WARNING("The key \"start_time\" is missing under sDA.\n\tAn assumed value of 8 will be used.");
                start=8;
            }else{
                if (val.isDouble()){
                    start=val.toDouble();
                }else{
                    STADIC_WARNING("The key \"start_time\" does not contain a number.\n\tAn assumed value of 8 will be used.");
                    start=8;
                }
            }
            val=tempObject.value(QString("end_time"));
            if (val.isUndefined()){
                STADIC_WARNING("The key \"end_time\" is missing under sDA.\n\tAn assumed value of 17 will be used.");
                endTime=17;
            }else{
                if (val.isDouble()){
                    endTime=val.toDouble();
                }else{
                    STADIC_WARNING("The key \"end_time\" does not contain a number.\n\tAn assumed value of 17 will be used.");
                    endTime=17;
                }
            }
            if (!setsDA(true, illum, frac, start, endTime)){
                return false;
            }
        }else{
            STADIC_ERROR("The \"sDA\" is not an object.");
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
                STADIC_WARNING("The key \"illuminance\" is missing under occupied_sDA.\n\tAn assumed value of 300 will be used.");
                illum=300;
            }else{
                if (val.isDouble()){
                    illum=val.toDouble();
                }else{
                    STADIC_WARNING("The key \"illuminance\" does not contain a number.\n\tAn assumed value of 300 will be used.");
                    illum=300;
                }
            }
            val=tempObject.value(QString("DA_fraction"));
            if (val.isUndefined()){
                STADIC_WARNING("The key \"DA_fraction\" is missing under occupied_sDA.\n\tAn assumed value of 0.5 will be used.");
                frac=0.5;
            }else{
                if (val.isDouble()){
                    frac=val.toDouble();
                }else{
                    STADIC_WARNING("The key \"DA_fraction\" does not contain a number.\n\tAn assumed value of 0.5 will be used.");
                    frac=0.5;
                }
            }
            if (!setOccsDA(true, illum, frac)){
                return false;
            }
        }else{
            STADIC_ERROR("The key \"occupied_sDA\" is not an object.");
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
            STADIC_ERROR("The \"DA\" is not a double.");
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
            STADIC_ERROR("The \"cDA\" is not a double.");
            return false;
        }
    }

    val=jsonObj.value(QString("DF"));
    if (!val.isUndefined()){
        if (val.isBool()){
               setDF(val.toBool());
        }else{
            STADIC_ERROR("The \"DF\" is not a boolean.");
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
                STADIC_WARNING("The key \"minimum\" is missing under UDI.\n\tAn assumed value of 100 will be used.");
                minimum=100;
            }else{
                if (val.isDouble()){
                    minimum=val.toDouble();
                }else{
                    STADIC_WARNING("The key \"minimum\" is not a number.\n\tAn assumed value of 100 will be used.");
                    minimum=100;
                }
            }
            val=tempObject.value(QString("maximum"));
            if (val.isUndefined()){
                STADIC_WARNING("The key \"maximum\" is missing under UDI.\n\tAn assumed value of 250 will be used.");
                maximum=250;
            }else{
                if (val.isDouble()){
                    maximum=val.toDouble();
                }else{
                    STADIC_WARNING("The key \"maximum\" is not a number.\n\tAn assumed value of 250 will be used.");
                    maximum=250;
                }
            }
            if (!setUDI(true, minimum, maximum)){
                return false;
            }
        }else{
            STADIC_ERROR("The key \"UDI\" is not an object.");
            return false;
        }
    }


    return true;
}

bool Control::parseJson2(std::string file){
    std::fstream iFile;
    iFile.open(file);
    if (!iFile.is_open()){
        return false;
    }
    iFile.close();

    boost::property_tree::ptree json;
    boost::property_tree::read_json(file, json);
    if (json.empty()){
        STADIC_LOG(Severity::Fatal, "The json file is empty");
        return false;
    }
    //get_value_or(/*default*/);
    boost::optional<std::string> sVal;
    boost::optional<double> dVal;
    boost::optional<int> iVal;
    boost::optional<bool> bVal;
    boost::optional<boost::property_tree::ptree> treeVal;

    //******************
    //Folder Information
    //******************
    sVal=getString(json, "project_name", "The key \"project_name\" does not appear in the STADIC Control File.", "The key \"project_name\" does not contain a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setProjectName(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "project_folder", "The key \"project_folder\" does not appear in the STADIC Control File.", "The key \"project_folder\" does not contain a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setProjectFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "tmp_folder", "The key \"tmp_folder\" does not appear in the STADIC Control File.", "The \"tmp_folder\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setTmpFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "geometry_folder", "The key \"geometry_folder\" does not appear in the STADIC Control File.", "The \"geometry_folder\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setGeoFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "ies_folder", "The key \"ies_folder\" does not appear in the STADIC Control File.", "The \"ies_folder\" is not a string.", Severity::Warning);
    if (sVal){
        setIESFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "results_folder", "The key \"results_folder\" does not appear in the STADIC Control File.", "The \"results_folder\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setResultsFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "data_folder", "The key \"data_folder\" does not appear in the STADIC Control File.", "The \"data_folder\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setDataFolder(sVal.get());
        sVal.reset();
    }

    //******************
    //Site Information
    //******************
    dVal=getDouble(json, "ground_reflectance", "The key \"ground_reflectance\" does not appear in the STADIC Control File.", "The \"ground_reflectance\" is not a double.", Severity::Warning);
    if (!dVal){
        STADIC_LOG(Severity::Info, "A default value of 0.2 will be applied for the ground reflectance.");
        setGroundReflect(0.2);
    }else{
        setGroundReflect(dVal.get());
        dVal.reset();
    }

    sVal=getString(json, "wea_data_file", "The key \"wea_data_file\" does not appear in the STADIC Control File.", "The \"wea_data_file\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setWeaDataFile(sVal.get());
        sVal.reset();
    }

    iVal=getInt(json, "first_day", "The key \"first_day\" does not appear in the STADIC Control File.", "The \"first_day\" is not an integer.", Severity::Warning);
    if (!iVal){
        STADIC_LOG(Severity::Info, "The default first day will be set to 1 (Sunday).");
        setFirstDay(1);
    }else{
        setFirstDay(iVal.get());
        iVal.reset();
    }


    //******************
    //Geometry Information
    //******************
    sVal=getString(json, "material_file", "The key \"material_file\" does not appear in the STADIC Control File.", "The \"material_file\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setMatFile(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "geometry_file", "The key \"geometry_file\" does not appear in the STADIC Control File.", "The \"geometry_file\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setGeoFile(sVal.get());
    }

    dVal=getDouble(json, "building_rotation", "The key \"building_rotation\" does not appear in the STADIC Control File.", "The \"building_rotation\" is not a double.", Severity::Warning);
    if (!dVal){
        STADIC_LOG(Severity::Info, "The default building rotation will be set to 0");
        setBuildingRotation(0);
    }else{
        setBuildingRotation(dVal.get());
        dVal.reset();
    }

    sVal=getString(json, "analysis_points", "The key \"analysis_points\" does not appear in the STADIC Control File.", "The \"analysis_points\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setPTSFile(sVal.get());
        sVal.reset();
    }

    treeVal=getTree(json, "window_groups", "The key \"window_groups\" does not appear in the STADIC Control File.", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        for(boost::property_tree::ptree::value_type &v : treeVal.get()){
            WindowGroup WG;
            if (WG.parseJson(v.second)){
                m_WindowGroups.push_back(WG);
            }else{
                return false;
            }
        }
        treeVal.reset();
    }

    sVal=getString(json, "import_units", "The key \"import_units\" does not appear in the STADIC Control File.", "The \"import_units\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        if (!setImportUnits(sVal.get())){
            return false;
        }
        sVal.reset();
    }

    sVal=getString(json, "illum_units", "The key \"illum_units\" does not appear in the STADIC Control File.", "The \"illum_units\" is not a string.", Severity::Warning);
    if (!sVal){
        STADIC_LOG(Severity::Info, "The default illuminance units will be set to lux.");
        setIllumUnits("lux");
    }else{
        setIllumUnits(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "display_units", "The key \"display_units\" does not appear in the STADIC Control File.", "The \"display_units\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        if (!setDisplayUnits(sVal.get())){
            return false;
        }
        sVal.reset();
    }

    sVal=getString(json, "occupancy_schedule", "The key \"occupancy_schedule\" does not appear in the STADIC Control File.", "The \"occupancy_schedule\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setOccSchedule(sVal.get());
        sVal.reset();
    }

    dVal=getDouble(json, "target_illuminance", "The key \"target_illuminance\" does not appear in the STADIC Control File.", "The \"target_illuminance\" is not a double.", Severity::Error);
    if (!dVal){
        return false;
    }else{
        if (!setTargetIlluminance(dVal.get())){
            return false;
        }
        dVal.reset();
    }
 

    //******************
    //Simulation Settings
    //******************
    iVal=getInt(json, "sun_divisions", "The key \"sun_divisions\" does not appear in the STADIC Control File.", "The \"sun_divisions\" is not an integer.", Severity::Warning);
    if (!iVal){
        STADIC_LOG(Severity::Info, "Sun Divisions will be set to 3.");
        setSunDivisions(3);
    }else{
        if (!setSunDivisions(iVal.get())){
            STADIC_LOG(Severity::Info, "Sun Divisions will be set to 3.");
            setSunDivisions(3);
        }
        iVal.reset();
    }

    iVal=getInt(json, "sky_divisions", "The key \"sky_divisions\" does not appear in the STADIC Control File.", "The \"sky_divisions\" is not an integer.", Severity::Warning);
    if (!iVal){
        STADIC_LOG(Severity::Info, "Sky Divisions will be set to 3.");
        setSkyDivisions(3);
    }else{
        if (!setSkyDivisions(iVal.get())){
            STADIC_LOG(Severity::Info, "Sky Divisions will be set to 3.");
            setSkyDivisions(3);
        }
        iVal.reset();
    }

    bVal=getBool(json, "daylight_savings_time", "The key \"daylight_savings_time\" does not appear in the STADIC Control File.", "The \"daylight_savings_time\" is not a boolean.", Severity::Warning);
    if (!bVal){
        STADIC_LOG(Severity::Info, "Daylight Savings Time will be enabled.");
        setDaylightSavingsTime(true);
    }else{
        setDaylightSavingsTime(bVal.get());
        bVal.reset();
    }

    treeVal=getTree(json, "radiance_parameters", "The key \"radiance_parameters\" does not appear in the STADIC Control File.", Severity::Info);
    if (!treeVal){
        STADIC_LOG(Severity::Info, "The default radiance parameters will be set.");
        if (!setDefaultRadianceParameters()){
            return false;
        }
    }else{
        iVal=getInt(treeVal.get(), "ab", "The parameter \"ab\" was not found under radiance_parameters.", "The parameter \"ab\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 4 will be applied for \"ab\".");
            setAB(4);
        }else{
            if (!setAB(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 4 will be applied for \"ab\".");
                setAB(4);
            }
            iVal.reset();
        }

        iVal=getInt(treeVal.get(), "ad", "The parameter \"ad\" was not found under radiance_parameters.", "The parameter \"ad\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 300 will be applied for \"ad\".");
            setAD(300);
        }else{
            if (!setAD(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 300 will be applied for \"ad\".");
                setAD(300);
            }
            iVal.reset();
        }

        iVal=getInt(treeVal.get(), "as", "The parameter \"as\" was not found under radiance_parameters.", "The parameter \"as\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 20 will be applied for \"as\".");
            setAS(20);
        }else{
            if (!setAS(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 20 will be applied for \"as\".");
                setAS(20);
            }
            iVal.reset();
        }

        iVal=getInt(treeVal.get(), "ar", "The parameter \"ar\" was not found under radiance_parameters.",  "The parameter \"ar\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 150 will be applied for \"ar\".");
            setAR(150);
        }else{
            if (!setAR(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 150 will be applied for \"ar\".");
                setAR(150);
            }
            iVal.reset();
        }

        dVal=getDouble(treeVal.get(), "aa", "The parameter \"aa\" was not found under radiance_parameters.", "The parameter \"aa\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.1 will be applied for \"aa\".");
            setAA(0.1);
        }else{
            if (!setAA(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.1 will be applied for \"aa\".");
                setAA(0.1);
            }
            dVal.reset();
        }
        iVal=getInt(treeVal.get(), "lr", "The parameter \"lr\" was not found under radiance_parameters.", "The parameter \"lr\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 6 will be applied for \"lr\".");
            setLR(6);
        }else{
            if (!setLR(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 6 will be applied for \"lr\".");
                setLR(6);
            }
            iVal.reset();
        }

        dVal=getDouble(treeVal.get(), "st", "The parameter \"st\" was not found under radiance_parameters.", "The parameter \"st\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.15 will be applied for \"st\".");
            setST(0.15);
        }else{
            if (!setST(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.15 will be applied for \"st\".");
                setST(0.15);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "sj", "The parameter \"sj\" was not found under radiance_parameters.", "The parameter \"sj\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 1.0 will be applied for \"sj\".");
            setSJ(1.0);
        }else{
            if (!setSJ(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 1.0 will be applied for \"sj\".");
                setSJ(1.0);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "lw", "The parameter \"lw\" was not found under radiance_parameters.", "The parameter \"lw\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.004 will be applied for \"lw\".");
            setLW(0.004);
        }else{
            if (!setLW(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.004 will be applied for \"lw\".");
                setLW(0.004);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "dj", "The parameter \"dj\" was not found under radiance_parameters.", "The parameter \"dj\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.0 will be applied for \"dj\".");
            setDJ(0.0);
        }else{
            if (!setDJ(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.0 will be applied for \"dj\".");
                setDJ(0.0);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "ds", "The parameter \"ds\" was not found under radiance_parameters.", "The parameter \"ds\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.200 will be applied for \"ds\".");
            setDS(0.200);
        }else{
            if (!setDS(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.200 will be applied for \"ds\".");
                setDS(0.200);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "dr", "The parameter \"dr\" was not found under radiance_parameters.", "The parameter \"dr\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 2 will be applied for \"dr\".");
            setDR(2);
        }else{
            if (!setDR(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 2 will be applied for \"dr\".");
                setDR(2);
            }
            dVal.reset();
        }

        iVal=getInt(treeVal.get(), "dp", "The parameter \"dp\" was not found under radiance_parameters.", "The parameter \"dp\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 512 will be applied for \"dp\".");
            setDP(512);
        }else{
            if (!setDP(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 512 will be applied for \"dp\".");
                setDP(512);
            }
            iVal.reset();
        }
    }
    treeVal.reset();



    //******************
    //Lighting Control
    //******************
    treeVal=getTree(json, "control_zones", "The key \"control_zones\" does not appear in the STADIC Control File.", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        for(boost::property_tree::ptree::value_type &v : treeVal.get()){
            ControlZone zone;
            if (zone.parseJson(v.second)){
                m_ControlZones.push_back(zone);
            }else{
                return false;
            }
        }
        treeVal.reset();
    }

    //******************
    //Metrics
    //******************
    treeVal=getTree(json, "sDA", "The key \"sDA\" does not appear in the STADIC Control File.", Severity::Info);
    if (treeVal){
        double illum, frac, startTime, endTime;
        dVal=getDouble(treeVal.get(), "illuminance", "The key \"illuminance\" is missing under sDA.", "The key \"illuminance\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 300 will be used for sDA illuminance.");
            illum=300;
        }else{
            illum=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "DA_fraction", "The key \"DA_fraction\" is missing under sDA.", "The key \"DA_fraction\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 0.50 will be used for sDA fraction.");
            frac=0.50;
        }else{
            frac=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "start_time", "The key \"start_time\" is missing under sDA.", "The key \"start_time\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 8 will be used for sDA start time.");
            startTime=8;
        }else{
            startTime=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "end_time", "The key \"end_time\" is missing under sDA.", "The key \"end_time\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 17 will be used for sDA end time.");
            endTime=17;
        }else{
            endTime=dVal.get();
        }
        if (!setsDA(true, illum, frac, startTime, endTime)){
            return false;
        }
    }
    treeVal.reset();

    treeVal=getTree(json, "occupied_sDA", "The key \"occupied_sDA\" does not appear in the STADIC Control File.", Severity::Info);
    if (treeVal){
        double illum, frac;
        dVal=getDouble(treeVal.get(), "illuminance", "The key \"illuminance\" is missing under occupied_sDA.", "The key \"illuminance\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 300 will be used for occupied_sDA illuminance.");
            illum=300;
        }else{
            illum=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "DA_fraction", "The key \"DA_fraction\" is missing under occupied_sDA.", "The key \"DA_fraction\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 0.50 will be used for occupied_sDA fraction.");
            frac=0.50;
        }else{
            frac=dVal.get();
        }
        dVal.reset();
        if (!setOccsDA(true, illum, frac)){
            return false;
        }
    }
    treeVal.reset();

    dVal=getDouble(json, "DA", "The key \"DA\" does not appear in the STADIC Control File.", "The key \"DA\" is not a double.", Severity::Info);
    if (dVal){
        if (!setDA(true, dVal.get())){
            return false;
        }
    }
    dVal.reset();

    dVal=getDouble(json, "cDA", "The key \"cDA\" does not appear in the STADIC Control File.", "The key \"cDA\" is not a double.", Severity::Info);
    if (dVal){
        if (!setcDA(true, dVal.get())){
            return false;
        }
    }
    dVal.reset();

    bVal=getBool(json, "DF", "The key \"DF\" does not appear in the STADIC Control File.", "The key \"DF\" is not a boolean.", Severity::Info);
    if (bVal){
        setDF(bVal.get());
    }
    bVal.reset();

    treeVal=getTree(json, "UDI", "The key \"UDI\" does not appear in the STADIC Control File.", Severity::Info);
    if (treeVal){
        double minimum, maximum;
        dVal=getDouble(treeVal.get(), "minimum", "The key \"minimum\" is missing under UDI.", "The key \"minimum\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 100 will be used for UDI minimum.");
            minimum=100;
        }else{
            minimum=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "maximum", "The key \"maximum\" is missing under UDI.", "The key \"maximum\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 2500 will be used for UDI maximum.");
            maximum=2500;
        }else{
            maximum=dVal.get();
        }
        dVal.reset();
        if (!setUDI(true, minimum, maximum)){
                return false;
        }
    }
    treeVal.reset();
    return true;
}


}
