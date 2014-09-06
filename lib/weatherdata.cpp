#include "weatherdata.h"
//#include <QTextStream>
//#include <QFile>
//#include <QStringList>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "logging.h"
#include "functions.h"

namespace stadic {

WeatherData::WeatherData()
{
}

//Setters
void WeatherData::setPlace(std::string place){
    m_Place=place;
}
void WeatherData::setLatitude(std::string lat){
    m_Latitude=lat;
}
void WeatherData::setLongitude(std::string lon){
    m_Longitude=lon;
}
void WeatherData::setTimeZone(std::string timeZone){
    m_TimeZone=timeZone;
}
void WeatherData::setElevation(std::string elev){
    m_Elevation=elev;
}

//Getters
std::vector<int> WeatherData::month() const {
    return m_Month;
}
std::vector<int> WeatherData::day() const {
    return m_Day;
}
std::vector<double> WeatherData::hour()const {
    return m_Hour;
}
std::vector<std::string> WeatherData::directNormal() const {
    return m_DirectNormal;
}
std::vector<std::string> WeatherData::directHorizontal() const{
    return m_DirectHorizontal;
}
std::string WeatherData::place() const {
    return m_Place;
}
std::string WeatherData::latitude() const {
    return m_Latitude;
}
std::string WeatherData::longitude() const {
    return m_Longitude;
}
std::string WeatherData::timeZone() const {
    return m_TimeZone;
}
std::string WeatherData::elevation() const {
    return m_Elevation;
}

//Utilities
bool WeatherData::parseWeather(std::string file){
    std::ifstream iFile;
    iFile.open(file);
    if (!iFile.is_open()){
        ERROR("The opening of the weather file "+file+" has failed.");
        return false;
    }
    std::string line;
    getline(iFile,line);
    iFile.close();
    if (line.find("LOCATION")!=line.npos){
        parseEPW(file);
    }else{
        parseTMY(file);
    }
    /*
    QFile iFile;
    iFile.setFileName(QString().fromStdString(file));
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
    */
    return true;
}
bool WeatherData::writeWea(std::string file){
    std::ofstream oFile;
    oFile.open(file);
    if(!oFile.is_open()){
        ERROR("The opening of the output weather file \""+file+"\" has failed.");
        return false;
    }
    oFile<<"place "<<place()<<std::endl;
    oFile<<"latitude "<<latitude()<<std::endl;
    oFile<<"longitude "<<longitude()<<std::endl;
    oFile<<"time_zone "<<timeZone()<<std::endl;
    oFile<<"site_elevation "<<elevation()<<std::endl;
    oFile<<"weather_data_file_units 1"<<std::endl;
    for (int i=0;i<month().size();i++){
        oFile<<month()[i]<<" "<<day()[i]<<" "<<hour()[i]<<" "<<directNormal()[i]<<" "<<directHorizontal()[i]<<std::endl;
    }
    oFile.close();

    /*
    QFile oFile;
    oFile.setFileName(QString().fromStdString(file));
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        return false;
    }
    QTextStream out(&oFile);
    out<<"place "<<QString().fromStdString(place())<<endl;
    out<<"latitude "<<QString().fromStdString(latitude())<<endl;
    out<<"longitude "<<QString().fromStdString(longitude())<<endl;
    out<<"time_zone "<<QString().fromStdString(timeZone())<<endl;
    out<<"site_elevation "<<QString().fromStdString(elevation())<<endl;
    out<<"weather_data_file_units 1"<<endl;
    for (int i=0;i<month().size();i++){
        out<<month()[i]<<" "<<day()[i]<<" "<<hour()[i]<<" "<<QString().fromStdString(directNormal()[i])<<" "<<QString().fromStdString(directHorizontal()[i])<<endl;
    }
    oFile.close();
    */
    return true;
}
bool WeatherData::parseEPW(std::string file){
    std::ifstream iFile;
    iFile.open(file);
    if (!iFile.is_open()){
        ERROR("The opening of the weather file "+file+" has failed.");
        return false;
    }
    std::string line;
    std::vector<std::string> vals;
    vals.clear();
    getline(iFile,line);
    vals=stadic::split(line,',');
    setPlace(vals[1]);
    setLatitude(vals[6]);
    setLongitude(vals[7]);
    setTimeZone(vals[8]);
    setElevation(vals[9]);
    while (!line.find("DATA")==line.npos){
        getline(iFile,line);
    }
    //This is where the number of periods per hour should be read in.
    vals=split(line,',');
    int intervals=atoi(vals[2].c_str());
    double delta=1.0/(double)intervals;
    int counter=0;
    while(getline(iFile,line)){
        vals.clear();
        vals=split(line,',');
        //For now, assume the date/time is legit
        int month=atoi(vals[1].c_str());
        int day=atoi(vals[2].c_str());
        double hour=atof(vals[3].c_str())-1.0+(counter+0.5)*delta;
        //Probably should check that these conversions go ok
        double DN = atof(vals[14].c_str());
        double DH=atof(vals[15].c_str());
        counter++;
        if (counter==intervals) {
            counter=0;
        }
        //Validate before keeping the data
        if (DN > 9999 || DH >=9999){
            //Does this need a warning?
            continue;
        }
        m_Month.push_back(month);
        m_Day.push_back(day);
        m_Hour.push_back(hour);
        m_DirectHorizontal.push_back(vals[15]);
        m_DirectNormal.push_back(vals[14]);
    }
    iFile.close();

    /*
    QFile iFile;
    iFile.setFileName(QString().fromStdString(file));
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data=iFile.readLine();
    QStringList vals=data.split(',');
    setPlace(vals[1].trimmed().toStdString());
    setLatitude(vals[6].trimmed().toStdString());
    setLongitude(vals[7].trimmed().toStdString());
    setTimeZone(vals[8].trimmed().toStdString());
    setElevation(vals[9].trimmed().toStdString());
    while(!data.contains("DATA")){
        data=iFile.readLine();
    }
    //This is where the number of periods per hour should be read in.
    vals=data.split(',');
    int intervals=vals[2].toInt();
    double delta = 1.0/(double)intervals;
    int counter=0;
    while (!iFile.atEnd()){
        data=iFile.readLine();
        vals.clear();
        vals=data.split(',');
        // For now, assume the date/time is legit
        int month = vals[1].toInt();
        int day = vals[2].toInt();
        double hour=vals[3].toDouble()-1.0+(counter+0.5)*delta;
        // Probably should check that these conversions go Ok
        double DN = vals[14].toDouble();
        double DH = vals[15].toDouble();
        counter++;
        if(counter == intervals) {
          counter = 0;
        }
        // Validate before keeping the data
        if(DN >= 9999 || DH >= 9999) {
          // Does this need a warning?
          continue;
        }
        m_Month.push_back(month);
        m_Day.push_back(day);
        m_Hour.push_back(hour);
        m_DirectHorizontal.push_back(vals[15].toStdString());
        m_DirectNormal.push_back(vals[14].toStdString());
    }
    iFile.close();
    */
    return true;
}
bool WeatherData::parseTMY(std::string file){
    std::ifstream iFile;
    iFile.open(file);
    if (!iFile.is_open()){
        ERROR("The opening of the weather file "+file+" has failed.");
        return false;
    }
    std::string line;
    std::vector<std::string> vals;
    vals.clear();
    getline(iFile,line);
    vals=stadic::split(line,',');
    setPlace(vals[1]);
    setLatitude(vals[4]);
    setLongitude(vals[5]);
    setTimeZone(vals[3]);
    setElevation(vals[6]);
    std::string tempString;
    std::vector<std::string> parseDate;
    getline(iFile,line);
    vals=split(line,',');
    //Read Date String
    tempString=vals[0];
    parseDate=split(tempString,'/');
    m_Month.push_back(atoi(parseDate[0].c_str()));
    m_Day.push_back(atoi(parseDate[1].c_str()));
    //Read Hour String
    tempString=vals.at(1);
    parseDate.clear();
    parseDate=split(tempString,':');
    double tempHour=atof(parseDate[0].c_str())+atof(parseDate[1].c_str())/60;
    //Determine time correction factor
    double correction=tempHour-0.5;
    m_Hour.push_back(tempHour-correction);
    m_DirectNormal.push_back(vals[7]);
    m_DirectHorizontal.push_back(vals[10]);
    while(getline(iFile,line)){
        vals.clear();
        vals=split(line,',');
        //Read Date String
        tempString=vals[0];
        parseDate.clear();
        parseDate=split(tempString,'/');
        m_Month.push_back(atoi(parseDate[0].c_str()));
        m_Day.push_back(atoi(parseDate[1].c_str()));
        //Read Hour String
        tempString=vals[1];
        parseDate.clear();
        parseDate=split(tempString,':');
        double tempHour=atof(parseDate[0].c_str())+atof(parseDate[1].c_str())/60;
        m_Hour.push_back(tempHour-correction);
        m_DirectNormal.push_back(vals[7]);
        m_DirectHorizontal.push_back(vals[10]);
    }
    iFile.close();
    /*
    QFile iFile;
    iFile.setFileName(QString().fromStdString(file));
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QString data=iFile.readLine();
    QStringList vals=data.split(',');
    setPlace(vals[1].toStdString());
    setLatitude(vals[4].toStdString());
    setLongitude(vals[5].toStdString());
    setTimeZone(vals[3].toStdString());
    setElevation(vals[6].toStdString());
    data=iFile.readLine();
    QString tempString;
    QStringList parseDate;

    data=iFile.readLine();
    vals=data.split(',');
    //Read Date String
    tempString=vals[0];
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
    m_DirectNormal.push_back(vals[7].toStdString());
    m_DirectHorizontal.push_back(vals[10].toStdString());
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
        m_DirectNormal.push_back(vals[7].toStdString());
        m_DirectHorizontal.push_back(vals[10].toStdString());
    }

    iFile.close();
    */
    return true;
}

}
