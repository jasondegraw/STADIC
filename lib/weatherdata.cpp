#include "weatherdata.h"
#include <QTextStream>
#include <QFile>
#include <QStringList>
#include <iostream>

namespace stadic {

WeatherData::WeatherData(QObject *parent) :
    QObject(parent)
{
}

//Setters
void WeatherData::setMonth(QString month){
    m_Month.push_back(month);
}
void WeatherData::setDay(QString day){
    m_Day.push_back(day);
}
void WeatherData::setHour(double hour){
    m_Hour.push_back(hour);
}
bool WeatherData::setDN(QString dn){
    if (dn=="9999"){
        std::cerr<<"ERROR: The imported file is missing Direct Normal data."<<std::endl;
        return false;
    }
    m_DirectNormal.push_back(dn);
    return true;
}
bool WeatherData::setDH(QString dh){
    if (dh=="9999"){
        std::cerr<<"ERROR: The imported file is missing Direct Horizontal data."<<std::endl;
        return false;
    }
    m_DirectHorizontal.push_back(dh);
    return true;
}
void WeatherData::setPlace(QString place){
    m_Place=place;
}
void WeatherData::setLatitude(QString lat){
    m_Latitude=lat;
}
void WeatherData::setLongitude(QString lon){
    m_Longitude=lon;
}
void WeatherData::setTimeZone(QString timeZone){
    m_TimeZone=timeZone;
}
void WeatherData::setElevation(QString elev){
    m_Elevation=elev;
}

//Getters
std::vector<QString> WeatherData::month() const {
    return m_Month;
}
std::vector<QString> WeatherData::day() const {
    return m_Day;
}
std::vector<double> WeatherData::hour()const {
    return m_Hour;
}
std::vector<QString> WeatherData::directNormal() const {
    return m_DirectNormal;
}
std::vector<QString> WeatherData::directHorizontal() const{
    return m_DirectHorizontal;
}
QString WeatherData::place() const {
    return m_Place;
}
QString WeatherData::latitude() const {
    return m_Latitude;
}
QString WeatherData::longitude() const {
    return m_Longitude;
}
QString WeatherData::timeZone() const {
    return m_TimeZone;
}
QString WeatherData::elevation() const {
    return m_Elevation;
}

//Utilities
bool WeatherData::parseWeather(QString file){
    QFile iFile;
    iFile.setFileName(file);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream data(&iFile);
    QString string;
    string=data.readLine();
    iFile.close();
    if (string.contains("LOCATION")){
        parseEPW(file);
    }else{
        parseTMY(file);
    }

    return true;
}
bool WeatherData::writeWea(QString file){
    QFile oFile;
    oFile.setFileName(file);
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        return false;
    }
    QTextStream out(&oFile);
    out<<"place "<<place()<<endl;
    out<<"latitude "<<latitude()<<endl;
    out<<"longitude "<<longitude()<<endl;
    out<<"time_zone "<<timeZone()<<endl;
    out<<"site_elevation "<<elevation()<<endl;
    out<<"weather_data_file_units 1"<<endl;
    for (int i=0;i<month().size();i++){
        out<<month().at(i)<<" "<<day().at(i)<<" "<<hour().at(i)<<" "<<directNormal().at(i)<<" "<<directHorizontal().at(i)<<endl;
    }
    oFile.close();
    return true;
}
bool WeatherData::parseEPW(QString file){
    QFile iFile;
    iFile.setFileName(file);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data=iFile.readLine();
    QStringList vals=data.split(',');
    setPlace(vals[1].trimmed());
    setLatitude(vals[6].trimmed());
    setLongitude(vals[7].trimmed());
    setTimeZone(vals[8].trimmed());
    setElevation(vals[9].trimmed());
    while(!data.contains("DATA")){
        data=iFile.readLine();
    }
    //This is where the number of periods per hour should be read in.
    vals=data.split(',');
    int intervals=vals.at(2).toInt();
    double correction;
    data=iFile.readLine();
    vals=data.split(',');
    setMonth(vals.at(1));
    setDay(vals.at(2));
    double tempHour=vals.at(3).toDouble()+vals.at(4).toDouble()/60;
    correction=tempHour-(intervals/2.0);
    setHour(tempHour-correction);
    setDN(vals.at(14));
    setDH(vals.at(15));
    while (!iFile.atEnd()){
        data=iFile.readLine();
        vals.clear();
        vals=data.split(',');
        setMonth(vals.at(1));
        setDay(vals.at(2));
        tempHour=vals.at(3).toDouble()+vals.at(4).toDouble()/60;
        setHour(tempHour-correction);
        setDN(vals.at(14));
        setDH(vals.at(15));
    }
    iFile.close();
    return true;
}
bool WeatherData::parseTMY(QString file){
    QFile iFile;
    iFile.setFileName(file);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data=iFile.readLine();
    QStringList vals=data.split(',');
    setPlace(vals.at(1));
    setLatitude(vals.at(4));
    setLongitude(vals.at(5));
    setTimeZone(vals.at(3));
    setElevation(vals.at(6));
    data=iFile.readLine();
    QString tempString;
    QStringList parseDate;
    data=iFile.readLine();
    vals=data.split(',');
    //Read Date String
    tempString=vals.at(0);
    parseDate=tempString.split('/');
    setMonth(parseDate.at(0));
    setDay(parseDate.at(1));
    //Read Hour String
    tempString=vals.at(1);
    parseDate.clear();
    parseDate=tempString.split(':');
    double tempHour=parseDate.at(0).toDouble()+parseDate.at(1).toDouble()/60;
    //Determine time correction factor
    double correction=tempHour-0.5;
    setHour(tempHour-correction);
    setDN(vals.at(7));
    setDH(vals.at(10));
    while(!iFile.atEnd()){
        data=iFile.readLine();
        vals.clear();
        vals=data.split(',');
        //Read Date String
        tempString=vals.at(0);
        parseDate=tempString.split('/');
        setMonth(parseDate.at(0));
        setDay(parseDate.at(1));
        //Read Hour String
        tempString=vals.at(1);
        parseDate.clear();
        parseDate=tempString.split(':');
        double tempHour=parseDate.at(0).toDouble()+parseDate.at(1).toDouble()/60;
        setHour(tempHour-correction);
        setDN(vals.at(7));
        setDH(vals.at(10));
    }

    iFile.close();
    return true;
}

}
