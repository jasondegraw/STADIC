#include "makewea.h"
#include <QTextStream>
#include <QFile>
#include <QStringList>

MakeWea::MakeWea(QObject *parent) :
    QObject(parent)
{
}

//Setters
void MakeWea::setMonth(int month){
    m_Month.push_back(month);
}
void MakeWea::setDay(int day){
    m_Day.push_back(day);
}
void MakeWea::setHour(double hour){
    m_Hour.push_back(hour);
}
void MakeWea::setDN(double dn){
    m_DirectNormal.push_back(dn);
}
void MakeWea::setDH(double dh){
    m_DirectHorizontal.push_back(dh);
}
void MakeWea::setPlace(QString place){
    m_Place=place;
}
void MakeWea::setLatitude(double lat){
    m_Latitude=lat;
}
void MakeWea::setLongitude(double lon){
    m_Longitude=lon;
}
void MakeWea::setTimeZone(double timeZone){
    m_TimeZone=timeZone;
}
void MakeWea::setElevation(double elev){
    m_Elevation=elev;
}

//Getters
std::vector<int> MakeWea::month(){
    return m_Month;
}
std::vector<int> MakeWea::day(){
    return m_Day;
}
std::vector<double> MakeWea::hour(){
    return m_Hour;
}
std::vector<double> MakeWea::directNormal(){
    return m_DirectNormal;
}
std::vector<double> MakeWea::directHorizontal(){
    return m_DirectHorizontal;
}
QString MakeWea::place(){
    return m_Place;
}
double MakeWea::latitude(){
    return m_Latitude;
}
double MakeWea::longitude(){
    return m_Longitude;
}
double MakeWea::timeZone(){
    return m_TimeZone;
}
double MakeWea::elevation(){
    return m_Elevation;
}

//Utilities
bool MakeWea::parseWeather(QString file){
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
bool MakeWea::writeWea(QString file){
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
bool MakeWea::parseEPW(QString file){
    QFile iFile;
    iFile.setFileName(file);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data=iFile.readLine();
    QStringList vals=data.split(',');
    setPlace(vals.at(1));
    setLatitude(vals.at(6).toDouble());
    setLongitude(vals.at(7).toDouble());
    setTimeZone(vals.at(8).toDouble());
    setElevation(vals.at(9).toDouble());
    while(!data.contains("DATA")){
        data=iFile.readLine();
    }
    //This is where the number of periods per hour should be read in.
    vals=data.split(',');
    int intervals=vals.at(2).toInt();
    double correction;
    data=iFile.readLine();
    vals=data.split(',');
    setMonth(vals.at(1).toInt());
    setDay(vals.at(2).toInt());
    double tempHour=vals.at(3).toDouble()+vals.at(4).toDouble()/60;
    correction=tempHour-(intervals/2.0);
    setHour(tempHour-correction);
    setDN(vals.at(14).toDouble());
    setDH(vals.at(15).toDouble());
    while (!iFile.atEnd()){
        data=iFile.readLine();
        vals.clear();
        vals=data.split(',');
        setMonth(vals.at(1).toInt());
        setDay(vals.at(2).toInt());
        tempHour=vals.at(3).toDouble()+vals.at(4).toDouble()/60;
        setHour(tempHour-correction);
        setDN(vals.at(14).toDouble());
        setDH(vals.at(15).toDouble());
    }
    iFile.close();
    return true;
}
bool MakeWea::parseTMY(QString file){
    QFile iFile;
    iFile.setFileName(file);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data=iFile.readLine();
    QStringList vals=data.split(',');
    setPlace(vals.at(1));
    setLatitude(vals.at(4).toDouble());
    setLongitude(vals.at(5).toDouble());
    setTimeZone(vals.at(3).toDouble());
    setElevation(vals.at(6).toDouble());
    data=iFile.readLine();
    QString tempString;
    QStringList parseDate;
    data=iFile.readLine();
    vals=data.split(',');
    //Read Date String
    tempString=vals.at(0);
    parseDate=tempString.split('/');
    setMonth(parseDate.at(0).toInt());
    setDay(parseDate.at(1).toInt());
    //Read Hour String
    tempString=vals.at(1);
    parseDate.clear();
    parseDate=tempString.split(':');
    double tempHour=parseDate.at(0).toDouble()+parseDate.at(1).toDouble()/60;
    //Determine time correction factor
    double correction=tempHour-0.5;
    setHour(tempHour-correction);
    setDN(vals.at(7).toDouble());
    setDH(vals.at(10).toDouble());
    while(!iFile.atEnd()){
        data=iFile.readLine();
        vals.clear();
        vals=data.split(',');
        //Read Date String
        tempString=vals.at(0);
        parseDate=tempString.split('/');
        setMonth(parseDate.at(0).toInt());
        setDay(parseDate.at(1).toInt());
        //Read Hour String
        tempString=vals.at(1);
        parseDate.clear();
        parseDate=tempString.split(':');
        double tempHour=parseDate.at(0).toDouble()+parseDate.at(1).toDouble()/60;
        setHour(tempHour-correction);
        setDN(vals.at(7).toDouble());
        setDH(vals.at(10).toDouble());
    }

    iFile.close();
    return true;
}
