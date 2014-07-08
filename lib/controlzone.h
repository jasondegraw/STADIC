#ifndef CONTROLZONE_H
#define CONTROLZONE_H

#include <QObject>
#include <QString>
#include <vector>
#include <QJsonObject>


class ControlZone : public QObject
{
    Q_OBJECT
public:
    explicit ControlZone(QObject *parent = 0);
    bool parseJson(const QJsonObject &object);
    

    //Setters
    bool setOptimumMethod(QString method);
    bool setCPMethod(QString method);
    void setNumCPs(int value);
    void setTargetPercentage(double value);
    void setExcludedPoints(QString file);
    bool setCriticalPoints(int point);
    void setMaximumBFSignal(double value);
    void setMinimumBFSignal(double value);
    void setOffSignal(double value);
    void setOnSignal(double value);
    void setSetpointSignal(double value);
    void setSignal(double value);
    void setDimmingLevel(double value);
    void setSignalToBFFile(QString file);
    bool setSensorType(QString type);
    void setSensorFile(QString file);
    bool setSensorLocation(double x, double y, double z, double xd, double yd, double zd, double spin);
    bool setAlgorithm(QString algorithm);
    void setIESFile(QString file);
    bool setLLF(double value);
    bool setLampLumens(int value);
    bool setBallastType(QString type);
    bool setBFMin(double value);
    bool setBFMax(double value);
    bool setWattsMax(double value);
    bool setWattsMin(double value);
    bool setBallastFactor(double value);
    bool setWatts(double value);
    bool setLuminaireLayout(double x, double y, double z, double rotation, double tilt, double spin);


    //Getters
    QString optimumMethod();
    QString cpMethod();
    int numCPs();
    double targetPercentage();
    QString excludedPoints();
    std::vector<int> criticalPoints();
    double maximumBFSignal();
    double minimumBFSignal();
    double offSignal();
    double setpointSignal();
    double onSignal();
    double signal();
    double dimmingLevel();
    QString signalToBFFile();
    QString sensorType();
    QString sensorFile();
    std::vector<double> sensorLocation();
    QString algorithm();
    QString iesFile();
    double llf();
    int lampLumens();
    QString ballastType();
    double bfMin();
    double bfMax();
    double wattsMax();
    double wattsMin();
    double ballastFactor();
    double watts();
    std::vector<std::vector<double> > luminaireLayout();


private:
    QString m_OptimumMethod;                                    //  Variable holding the optimum control method
    QString m_ControlMethod;                                    //  Variable holding the actual control method
    QString m_CPMethod;                                         //  Variable holding the critical point method
    int m_NumCPs;                                               //  Variable holding the number of critical points to consider for the "auto" feature
    double m_TargetPercentage;                                  //  Variable holding the target percentage of illuminance from the controlled zone as a means of excluding analysis points
    QString m_ExcludedPoints;                                   //  Variable holding the excluded points file name
    std::vector<int> m_CriticalPoints;                          //  Variable holding the critical points for the "user" feature
    double m_MaximumBFSignal;                                   //  Variable holding the signal at the maximum ballast factor
    double m_MinimumBFSignal;                                   //  Variable holding the signal at the minimum ballast factor
    double m_OffSignal;                                         //  Variable holding the signal at which to turn the lights off
    double m_SetpointSignal;                                    //  Variable holding the signal at the set point
    double m_OnSignal;                                          //  Variable holding the signal at which to turn the lights on
    double m_Signal;                                            //  Variable holding the signal for the "user 1" algorithm
    double m_DimmingLevel;                                      //  Variable holding the dimming level for the "user 1" algorithm
    QString m_SignalToBFFile;                                   //  Variable holding the signal to ballast factor file for the "user 1" algorithm
    QString m_SensorType;                                       //  Variable holding the sensor type
    QString m_SensorFile;                                       //  Variable holding the sensor file
    std::vector<double> m_SensorLocation;                       //  Variable holding the sensor location
    QString m_Algorithm;                                        //  Variable holding the control algorithm
    QString m_IESFile;                                          //  Variable holding the IES file
    double m_LLF;                                               //  Variable holding the Light Loss Factor
    int m_LampLumens;                                           //  Variable holding the Lamp Lumens
    QString m_BallastType;                                      //  Variable holding the Ballast Type
    double m_BFMin;                                             //  Variable holding the Minimum Ballast Factor
    double m_BFMax;                                             //  Variable holding the Maximum Ballast Factor
    double m_WattsMax;                                          //  Variable holding the Maximum Watts
    double m_WattsMin;                                          //  Variable holding the Minimum Watts
    double m_BallastFactor;                                     //  Variable holding the Ballast Factor
    double m_Watts;                                             //  Variable holding the Watts
    std::vector<std::vector<double> > m_LuminaireLayout;         //  Variable holding the luminaire positions

signals:
    
public slots:
    
};

#endif // CONTROLZONE_H
