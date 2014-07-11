#include "makewea.h"
#include <QTextStream>
#include <QFile>
#include <QStringList>

MakeWea::MakeWea(QObject *parent) :
    QObject(parent)
{
}

//Setters
/*
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
*/
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
std::vector<QString> MakeWea::directNormal(){
    return m_DirectNormal;
}
std::vector<QString> MakeWea::directHorizontal(){
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
    int intervals=vals[2].toInt();
    double delta = 60.0/(double)intervals;
    int counter=1;
    while (!iFile.atEnd()){
        data=iFile.readLine();
        vals.clear();
        vals=data.split(',');
        // For now, assume the date/time is legit
        int month = vals[1].toInt();
        int day = vals[2].toInt();
        int hour=vals[3].toDouble()-1.0+counter*delta;
        // Probably should check that these conversions go Ok
        double DN = vals[14].toDouble();
        double DH = vals[15].toDouble();
        counter++;
        if(counter == intervals) {
          counter = 1;
        }
        // Validate before keeping the data
        if(DN >= 9999 || DH >= 9999) {
          // Does this need a warning?
          continue;
        }
        m_Month.push_back(month);
        m_Day.push_back(day);
        m_Hour.push_back(hour);
        m_DirectHorizontal.push_back(vals[15]);
        m_DirectNormal.push_back(vals[14]);
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
    m_Month.push_back(parseDate.at(0).toInt());
    m_Day.push_back(parseDate.at(1).toInt());
    //Read Hour String
    tempString=vals.at(1);
    parseDate.clear();
    parseDate=tempString.split(':');
    double tempHour=parseDate.at(0).toDouble()+parseDate.at(1).toDouble()/60;
    //Determine time correction factor
    double correction=tempHour-0.5;
    m_Hour.push_back(tempHour-correction);
    m_DirectNormal.push_back(vals.at(7));
    m_DirectHorizontal.push_back(vals.at(10));
    while(!iFile.atEnd()){
        data=iFile.readLine();
        vals.clear();
        vals=data.split(',');
        //Read Date String
        tempString=vals.at(0);
        parseDate=tempString.split('/');
        m_Month.push_back(parseDate.at(0).toInt());
        m_Day.push_back(parseDate.at(1).toInt());
        //Read Hour String
        tempString=vals.at(1);
        parseDate.clear();
        parseDate=tempString.split(':');
        double tempHour=parseDate.at(0).toDouble()+parseDate.at(1).toDouble()/60;
        m_Hour.push_back(tempHour-correction);
        m_DirectNormal.push_back(vals.at(7));
        m_DirectHorizontal.push_back(vals.at(10));
    }

    iFile.close();
    return true;
}
