#ifndef STADICCONTROL_H
#define STADICCONTROL_H

#include <QObject>
#include <QString>
#include <vector>
#include "windowgroup.h"
#include "controlzone.h"

namespace stadic {

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = 0);
    bool parseJson(QString file);

    //Setters
    //******************
    //Folder Information
    //******************
    void setProjectName(QString name);
    void setProjectFolder(QString folder);
    void setTmpFolder(QString folder);
    void setGeoFolder(QString folder);
    void setIESFolder(QString folder);
    void setResultsFolder(QString folder);
    void setDataFolder(QString folder);

    //******************
    //Site Information
    //******************
    bool setGroundReflect(double value);
    void setWeaDataFile(QString file);
    bool setFirstDay(int value);


    //******************
    //Geometry Information
    //******************
    void setMatFile(QString file);
    void setGeoFile(QString file);
    bool setBuildingRotation(double value);
    void setPTSFile(QString file);
    bool setImportUnits(QString units);
    bool setIllumUnits(QString units);
    bool setDisplayUnits(QString units);
    void setOccSchedule(QString file);
    bool setTargetIlluminance(double value);

    //******************
    //Simulation Settings
    //******************
    bool setSunDivisions(int value);
    bool setSkyDivisions(int value);
    void setDaylightSavingsTime(bool value);
    bool setDefaultRadianceParameters();
    bool setAB(int value);
    bool setAD(int value);
    bool setAS(int value);
    bool setAR(int value);
    bool setAA(double value);
    bool setLR(int value);
    bool setST(double value);
    bool setSJ(double value);
    bool setLW(double value);
    bool setDJ(double value);
    bool setDS(double value);
    bool setDR(int value);
    bool setDP(double value);

    //******************
    //Metrics
    //******************
    bool setDA(bool run, double illum);
    bool setcDA(bool run, double illum);
    bool setsDA(bool run, double illum, double DAFrac, double startTime, double endTime);
    bool setOccsDA(bool run, double illum, double DAFrac);
    void setDF(bool run);
    bool setUDI(bool run, double minIllum, double maxIllum);


    //Getters
    //******************
    //Folder Information
    //******************
    QString projectName();
    QString projectFolder();
    QString tmpFolder();
    QString geoFolder();
    QString iesFolder();
    QString resultsFolder();
    QString dataFolder();

    //******************
    //Site Information
    //******************
    double groundReflect();
    QString weaDataFile();
    int firstDay();

    //******************
    //Geometry Information
    //******************
    QString matFile();
    QString geoFile();
    double buildingRotation();
    QString ptsFile();
    std::vector<WindowGroup*> windowGroups();
    QString importUnits();
    QString illumUnits();
    QString displayUnits();
    QString occSchedule();
    double targetIlluminance();

    //******************
    //Lighting Control
    //******************
    std::vector<ControlZone*> controlZones();

    //******************
    //Simulation Settings
    //******************
    int sunDivisions();
    int skyDivisions();
    bool daylightSavingsTime();
    int ab();
    int ad();
    int as();
    int ar();
    double aa();
    int lr();
    double st();
    double sj();
    double lw();
    double dj();
    double ds();
    int dr();
    double dp();

    //******************
    //Metrics
    //******************
    bool runDA();
    double DAIllum();
    bool runsDA();
    double sDAIllum();
    double sDAFrac();
    double sDAStart();
    double sDAEnd();
    bool runOccsDA();
    double occsDAIllum();
    double occsDAFrac();
    bool runcDA();
    double cDAIllum();
    bool runDF();
    bool runUDI();
    double UDIMin();
    double UDIMax();

private:
    //******************
    //Folder Information
    //******************
    QString m_ProjectName;                              //  Variable holding the project name
    QString m_ProjectFolder;                            //  Variable holding the project folder
    QString m_TmpFolder;                                //  Variable holding the tmp folder
    QString m_GeoFolder;                                //  Variable holding the geometry folder
    QString m_IESFolder;                                //  Variable holding the luminaire folder
    QString m_ResultsFolder;                            //  Variable holding the results folder
    QString m_DataFolder;                               //  Variable holding the data folder

    //******************
    //Site Information
    //******************
    double m_GroundReflect;                             //  Variable holding the ground reflectance
    QString m_WeaDataFile;                              //  Variable holding the weather data file
    int m_FirstDay;                                     //  Variable holding the start day of the year

    //******************
    //Geometry Information
    //******************
    QString m_MatFile;                                  //  Variable holding the main material file
    QString m_GeoFile;                                  //  Variable holding the main geometry file
    double m_BuildingRotation;                          //  Variable holding the building rotation which is assumed to be positive=counter-clockwise
    QString m_PTSFile;                                  //  Variable holding the analysis grid file
    QString m_ImportUnits;                              //  Variable holding the geometry file import units
    QString m_IllumUnits;                               //  Variable holding the illuminance units
    QString m_DisplayUnits;                             //  Variable holding the distance units for display
    QString m_OccSchedule;                              //  Variable holding the occupancy schedule file
    double m_TargetIlluminance;                         //  Variable holding the target illuminance
    std::vector<WindowGroup*> m_WindowGroups;

    //******************
    //Simulation Settings
    //******************
    int m_SunDivisions;                                 //  Variable holding the integer for the number of sun divisions
    int m_SkyDivisions;                                 //  Variable holding the integer for the number of sky divisions
    bool m_DaylightSavingsTime;                         //  Variable holding whether daylight savings time is enabled
    int m_AB;                                           //  Variable holding the number of ambient bounces
    int m_AD;                                           //  Variable holding the number of ambient divisions
    int m_AS;                                           //  Variable holding the number of ambient super-samples
    int m_AR;                                           //  Variable holding the ambient resolutions
    double m_AA;                                        //  Variable holding the ambient accuracy
    int m_LR;                                           //  Variable holding the maximum number of reflections
    double m_ST;                                        //  Variable holding the specular sampling threshold
    double m_SJ;                                        //  Variable holding the specular sampling jitter
    double m_LW;                                        //  Variable holding the minimum weight fraction to continue tracing a ray
    double m_DJ;                                        //  Variable holding the direct jitter fraction
    double m_DS;                                        //  Variable holding the direct sampling ratio
    int m_DR;                                           //  Variable holding the number of relays for secondary sources
    double m_DP;                                        //  Variable holding the secondary source presampling density

    //******************
    //Lighting Control
    //******************
    std::vector<ControlZone*> m_ControlZones;

    //******************
    //Metrics
    //******************
    bool m_DA;                                          //  Variable holding whether the DA analysis should be completed
    double m_DAIllum;                                   //  Variable holding the illuminance for the DA analysis
    bool m_sDA;                                         //  Variable holding whether the sDA analysis should be completed
    double m_sDAIllum;                                  //  Variable holding the illuminance for the sDA analysis
    double m_sDAFrac;                                   //  Variable holding the DA fraction for the sDA analysis
    double m_sDAStart;                                  //  Variable holding the start time for the sDA analysis
    double m_sDAEnd;                                    //  Variable holding the end time for the sDA analysis
    bool m_OccsDA;                                      //  Variable holding whether the occupancy schedule based sDA should be completed
    double m_OccsDAIllum;                               //  Variable holding the illuminance for the occupancy schedule based sDA analysis
    double m_OccsDAFrac;                                //  Variable holding the DA fraction for the occupancy schedule based sDA analysis
    bool m_cDA;                                         //  Variable holding whether the cDA analysis should be completed
    double m_cDAIllum;                                  //  Variable holding the illuminance for the cDA analysis
    bool m_DF;                                          //  Variable holding whether the DF analysis should be completed
    bool m_UDI;                                         //  Variable holding whether the UDI analysis shoud be completed
    double m_UDIMin;                                    //  Variable holding the minimum illuminance for UDI
    double m_UDIMax;                                    //  Variable holding the maximum illuminance for UDI


signals:
    
public slots:
    
};

}

#endif // STADICCONTROL_H
