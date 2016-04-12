/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission of the
 *    respective copyright holder or contributor.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *****************************************************************************/

/*The direct illuminance calculations in this code were adapted from gendaymtx.
 * Greg Ward and Ian Ashdown
 */

#include "weatherdata.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "logging.h"
#include "functions.h"
#include "math.h"

const double PI=3.1415926535897932;

namespace stadic {

WeatherData::WeatherData()
{
}

//Setters
void WeatherData::setPlace(std::string place)
{
    m_place=place;
}

void WeatherData::setLatitude(std::string lat)
{
    m_latitude=lat;
}

void WeatherData::setLongitude(std::string lon)
{
    m_longitude=lon;
}

void WeatherData::setTimeZone(std::string timeZone)
{
    m_timeZone=timeZone;
}

void WeatherData::setElevation(std::string elev)
{
    m_elevation=elev;
}

//Getters
std::vector<int> WeatherData::month() const {
    return m_month;
}

std::vector<int> WeatherData::day() const {
    return m_day;
}

std::vector<double> WeatherData::hour()const {
    return m_hour;
}

std::vector<std::string> WeatherData::directNormal() const {
    return m_directNormal;
}

std::vector<std::string> WeatherData::diffuseHorizontal() const{
    return m_diffuseHorizontal;
}

std::vector<double> WeatherData::directIlluminance() const{
    return m_directIlluminance;
}
std::vector<double> WeatherData::dewPointC() const{
    return m_dewPointC;
}


std::vector<int> WeatherData::julianDate() const{
    return m_julianDate;
}


std::string WeatherData::place() const
{
    return m_place;
}

std::string WeatherData::latitude() const
{
    return m_latitude;
}

std::string WeatherData::longitude() const
{
    return m_longitude;
}

std::string WeatherData::timeZone() const
{
    return m_timeZone;
}
double WeatherData::timeZoneDeg() const
{
    return toDouble(m_timeZone)*15;
}

std::string WeatherData::elevation() const 
{
    return m_elevation;
}

std::vector<double> WeatherData::getProfileAngles(double elevationAzimuth){
    std::vector<double> profileAngles;
    for (int i=0;i<m_month.size();i++){
        if (m_solarAlt[i]>0){
            profileAngles.push_back(180-atan2(tan(m_solarAlt[i]),cos(m_solarAz[i]-elevationAzimuth*PI/180.))*180/PI);
            // xxx = 100+int(elevationAzimuth);
            //profileAngles.push_back(xxx);

        }else{
            profileAngles.push_back(-1);
        }
    }
    return profileAngles;
}

//Utilities
bool WeatherData::parseWeather(std::string file)
{
    std::ifstream iFile;
    iFile.open(file);
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the weather file "+file+" has failed.");
        return false;
    }
    std::string line;
    std::getline(iFile,line);
    iFile.close();
    if (line.find("LOCATION")!=line.npos){
        parseEPW(file);
    }else{
        parseTMY(file);
    }
    if (m_directNormal.size()==8760){
        for (int i=0;i<365;i++){
            for (int j=0;j<24;j++){
                m_julianDate.push_back(i+1);
            }
        }
    }
    if (!calcDirectIll()){
        return false;
    }
    return true;
}

bool WeatherData::writeWea(std::string file)
{
    std::ofstream oFile;
    oFile.open(file);
    if(!oFile.is_open()){
        STADIC_ERROR("The opening of the output weather file \""+file+"\" has failed.");
        return false;
    }
    oFile<<"place "<<m_place<<std::endl;
    oFile<<"latitude "<<m_latitude<<std::endl;
    oFile<<"longitude "<<m_longitude<<std::endl;
    oFile<<"time_zone "<<timeZoneDeg()<<std::endl;
    oFile<<"site_elevation "<<m_elevation<<std::endl;
    oFile<<"weather_data_file_units 1";
    for (int i=0;i<month().size();i++){
        oFile<<std::endl<<m_month[i]<<" "<<m_day[i]<<" "<<m_hour[i]<<" "<<m_directNormal[i]<<" "<<m_diffuseHorizontal[i];
    }
    oFile.close();
    return true;
}

bool WeatherData::parseEPW(std::string file)
{
    std::ifstream iFile;
    iFile.open(file);
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the weather file "+file+" has failed.");
        return false;
    }
    std::string line;
    std::vector<std::string> vals;
    vals.clear();
    std::getline(iFile,line);
    vals=stadic::trimmedSplit(line,',');
    if(vals.size() < 10) {
        STADIC_ERROR("Weather file " + file + " first line is missing information.");
        return false;
    }
    setPlace(vals[1]+"_"+vals[3]);
    setLatitude(vals[6]);
    setLongitude(toString(-1*toDouble(vals[7])));
    setTimeZone(toString(-1*toDouble(vals[8])));
    setElevation(vals[9]);
    for(int i = 1; i<8; i++){
        std::getline(iFile, line);
    }
    //This is where the number of periods per hour should be read in.
    vals=trimmedSplit(line,',');
    if(vals.size() < 7) {
        STADIC_ERROR("Weather file " + file + " DATA PERIODS line is missing information.");
        return false;
    }
    int intervals=atoi(vals[2].c_str());
    double delta=1.0/(double)intervals;
    int counter=0;
    while(std::getline(iFile, line)){
        vals.clear();
        vals=trimmedSplit(line,',');
        if(vals.size() < 35) {
            STADIC_ERROR("Weather file " + file + " contains incomplete data lines.");
            if(vals.size() < 16) {
                continue;
            }
        }
        //For now, assume the date/time is legit
        int month=atoi(vals[1].c_str());
        int day=atoi(vals[2].c_str());
        double hour=atof(vals[3].c_str())-1.0+(counter+0.5)*delta;
        //Probably should check that these conversions go ok
        double DN = atof(vals[14].c_str());
        double DH = atof(vals[15].c_str());
        counter++;
        if (counter==intervals) {
            counter=0;
        }
        //Validate before keeping the data
        if (DN > 9999 || DH >=9999){
            //Does this need a warning?
            continue;
        }
        m_month.push_back(month);
        m_day.push_back(day);
        m_hour.push_back(hour);
        m_diffuseHorizontal.push_back(vals[15]);
        m_directNormal.push_back(vals[14]);
        m_dewPointC.push_back(toDouble(vals[6]));
    }
    return true;
}
bool WeatherData::parseTMY(std::string file){
    std::ifstream iFile;
    iFile.open(file);
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the weather file "+file+" has failed.");
        return false;
    }
    std::string line;
    std::vector<std::string> vals;
    vals.clear();
    getline(iFile,line);
    vals=stadic::split(line,',');
    std::vector<std::string> vals2;
    vals2=stadic::split(vals[1],'\"');
    setPlace(vals2[1]);
    setLatitude(vals[4]);
    setLongitude(toString(-1*toDouble(vals[5])));
    setTimeZone(toString(-1*toDouble(vals[3])));
    setElevation(vals[6]);
    std::string tempString;
    std::vector<std::string> parseDate;
    getline(iFile,line);            //Read in the explanation line.
    getline(iFile,line);
    vals=split(line,',');
    //Read Date String
    tempString=vals[0];
    parseDate=split(tempString,'/');
    m_month.push_back(atoi(parseDate[0].c_str()));
    m_day.push_back(atoi(parseDate[1].c_str()));
    //Read Hour String
    tempString=vals.at(1);
    parseDate.clear();
    parseDate=split(tempString,':');
    double tempHour=toDouble(parseDate[0])+toDouble(parseDate[1])/60.0;
    //Determine time correction factor
    double correction=tempHour-0.5;
    m_hour.push_back(tempHour-correction);
    m_directNormal.push_back(vals[7]);
    m_diffuseHorizontal.push_back(vals[10]);
    m_dewPointC.push_back(toDouble(vals[34]));
    while(getline(iFile,line)){
        vals.clear();
        vals=split(line,',');
        //Read Date String
        tempString=vals[0];
        parseDate.clear();
        parseDate=split(tempString,'/');
        m_month.push_back(atoi(parseDate[0].c_str()));
        m_day.push_back(atoi(parseDate[1].c_str()));
        //Read Hour String
        tempString=vals[1];
        parseDate.clear();
        parseDate=split(tempString,':');
        double tempHour=toDouble(parseDate[0])+toDouble(parseDate[1])/60.0;
        m_hour.push_back(tempHour-correction);
        m_directNormal.push_back(vals[7]);
        m_diffuseHorizontal.push_back(vals[10]);
        m_dewPointC.push_back(toDouble(vals[34]));
        line.clear();
    }
    iFile.close();
    /*
    QFile iFile;
    iFile.setFileName(QString::fromStdString(file));
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
bool WeatherData::calcDirectIll(){
    if (m_julianDate.empty()){
        STADIC_WARNING("There are not 8760 intervals in the weather file.");
        return false;
    }
    setSolarPositions();
    calcEpsilon();
    calcDelta();
    calcAPWC();
    m_directIlluminance.clear();
    //std::ofstream oFile;
    //oFile.open("c:/001/bracketvalues.txt");
    for (int i=0;i<m_julianDate.size();i++){
        double tempVal;
        std::vector<double> tempVec;
        tempVec=directLumEff(m_epsilon[i]);
        if (tempVec[0]==0){
            STADIC_ERROR("There was a problem with the direct luminous efficiency values.");
            return false;
        }
        tempVal=toDouble(m_directNormal[i])*(tempVec[0]+tempVec[1]*m_APWC[i]+tempVec[2]*exp(5.73*m_solarZenAng[i]-5)+tempVec[3]*m_delta[i])/179.0;
        //oFile<<"a="<<tempVec[0]<<" b="<<tempVec[1]<<" apwc="<<m_APWC[i]<<" c="<<tempVec[2]<<" solarZen="<<m_SolarZenAng[i]<<" d="<<tempVec[3]<<" delta="<<m_Delta[i]<<" resultant=";
        if (tempVal<0){
            tempVal=0;
        }
        if (m_solarZenAng[i]>=(PI/2.0)){
            tempVal=0;
        }
        //oFile<<tempVal<<std::endl;
        m_directIlluminance.push_back(tempVal);
    }
    //oFile.close();
    return true;
}
void WeatherData::setSolarPositions(){
    //std::ofstream oFile;
    //oFile.open("c:/001/hours.txt");
    for (int i=0;i<m_julianDate.size();i++){
        m_solarDec.push_back(solarDec(m_julianDate[i]));
        m_solarTimeAdj.push_back(solarTimeAdj(m_julianDate[i]));
        m_solarAlt.push_back(solarAlt(m_solarDec[i],m_hour[i]+m_solarTimeAdj[i]));
        //oFile<<"julianDate="<<m_JulianDate[i]<<" time="<<m_Hour[i]+m_SolarTimeAdj[i]<<std::endl;
        m_solarAz.push_back(solarAz(m_solarDec[i],m_hour[i]+m_solarTimeAdj[i])+PI);
        m_solarZenAng.push_back(solarZen(m_solarAlt[i]));
    }
    //oFile.close();
}

double WeatherData::solarDec(int julianDate){
    return 0.4093*sin((2*PI/368)*(julianDate-81));
}

double WeatherData::solarTimeAdj(int julianDate){
    double sta=0.170*sin((4*PI/373)*(julianDate-80))-0.129*sin((2*PI/355)*(julianDate-8))+12 *(degToRad(timeZoneDeg())-degToRad(toDouble(longitude())))/PI;
    //std::clog<<sta<<std::endl;
    return sta;
}

double WeatherData::solarAlt(double solarDeclination, double time){
    return asin(sin(degToRad(toDouble(latitude())))*sin(solarDeclination)-cos(degToRad(toDouble(latitude())))*cos(solarDeclination)*cos(PI*time/12));
}

double WeatherData::solarAz(double solarDeclination, double time){
    return -atan2(cos(solarDeclination)*sin(time*(PI/12)),-cos(degToRad(toDouble(latitude())))*sin(solarDeclination)-sin(degToRad(toDouble(latitude())))*cos(solarDeclination)*cos(time*(PI/12)));
}

double WeatherData::solarZen(double solarAltAng){
    if (solarAltAng<=0){
        return degToRad(90);
    }else if (solarAltAng>= degToRad(87)){
        return degToRad(3);
    }
    return PI/2-solarAltAng;
}

double WeatherData::degToRad(double val){
    return val*PI/180.0;
}

void WeatherData::calcEpsilon(){
    //This is the sky clearness
    m_epsilon.clear();
    for (int i=0;i<m_julianDate.size();i++){
        double epsilon=((toDouble(m_diffuseHorizontal[i])+toDouble(m_directNormal[i]))/toDouble(m_diffuseHorizontal[i])+1.041*pow(m_solarZenAng[i],3))/1+1.041*pow(m_solarZenAng[i],3);
        if (epsilon>11.9){
            m_epsilon.push_back(11.9);
        }else{
            m_epsilon.push_back(epsilon);
        }
    }
}

void WeatherData::calcDelta(){
    //This is the sky brightness
    //std::ofstream oFile;
    //oFile.open("c:/001/AirMass.txt");
    m_delta.clear();
    double dayAngle;
    double eccentricity;
    for (int i=0;i<m_julianDate.size();i++){
        dayAngle=(m_julianDate[i]-1.0)*(2.0*PI/365);
        eccentricity=1.00011+0.034221*cos(dayAngle)+0.00128*sin(dayAngle)+0.000719*cos(2.0*dayAngle)+0.000077*sin(2.0*dayAngle);
        double delta=(1.0/(cos(m_solarZenAng[i])+0.15*pow(93.885-(180.0/PI)*m_solarZenAng[i],-1.253)));
        //oFile<<"Hour="<<m_Hour[i]<<" AirMass="<<delta<<std::endl;
        delta=toDouble(m_diffuseHorizontal[i])*delta/(1367*eccentricity);
        if (delta<0.01){
            m_delta.push_back(0.01);
        }else{
            m_delta.push_back(delta);
        }
    }
    //oFile.close();
}

void WeatherData::calcAPWC(){
    m_APWC.clear();
    for (int i=0;i<m_julianDate.size();i++){
        m_APWC.push_back(exp(0.07*11-0.075));
        //m_APWC.push_back(exp(0.07*m_DewPointC[i]-0.075));
    }
}

int WeatherData::skyBin(double epsilon){
    if (epsilon<1){
        STADIC_ERROR("The value of epsilon was less than zero and therefore cannot be computed");
        return 0;
    }else if (epsilon<1.065){
        return 1;
    }else if (epsilon<1.230){
        return 2;
    }else if (epsilon<1.5){
        return 3;
    }else if (epsilon<1.95){
        return 4;
    }else if (epsilon<2.8){
        return 5;
    }else if (epsilon<4.5){
        return 6;
    }else if (epsilon<6.2){
        return 7;
    }else{
        return 8;
    }
}
std::vector<double> WeatherData::directLumEff(double epsilon){
    std::vector<double> tempVec;
    int bin=skyBin(epsilon);
    if (bin==0){
        tempVec.push_back(0);
        tempVec.push_back(0);
        tempVec.push_back(0);
        tempVec.push_back(0);
    }else if (bin==1){
        tempVec.push_back(57.20);
        tempVec.push_back(-4.55);
        tempVec.push_back(-2.98);
        tempVec.push_back(117.12);
    }else if (bin==2){
        tempVec.push_back(98.99);
        tempVec.push_back(-3.46);
        tempVec.push_back(-1.21);
        tempVec.push_back(12.38);
    }else if (bin==3){
        tempVec.push_back(109.83);
        tempVec.push_back(-4.90);
        tempVec.push_back(-1.71);
        tempVec.push_back(-8.81);
    }else if (bin==4){
        tempVec.push_back(110.34);
        tempVec.push_back(-5.84);
        tempVec.push_back(-1.99);
        tempVec.push_back(-4.56);
    }else if (bin==5){
        tempVec.push_back(106.36);
        tempVec.push_back(-3.97);
        tempVec.push_back(-1.75);
        tempVec.push_back(-6.16);
    }else if (bin ==6){
        tempVec.push_back(107.19);
        tempVec.push_back(-1.25);
        tempVec.push_back(-1.51);
        tempVec.push_back(-26.73);
    }else if (bin==7){
        tempVec.push_back(105.75);
        tempVec.push_back(0.77);
        tempVec.push_back(-1.26);
        tempVec.push_back(-34.44);
    }else if (bin==8){
        tempVec.push_back(101.18);
        tempVec.push_back(1.58);
        tempVec.push_back(-1.10);
        tempVec.push_back(-8.29);
    }
    return tempVec;
}

}
