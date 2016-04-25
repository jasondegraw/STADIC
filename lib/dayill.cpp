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

#include "dayill.h"
#include "logging.h"
#include <fstream>
#include <cmath>
#include "functions.h"
#include "weatherdata.h"

namespace stadic {

TemporalIlluminance::TemporalIlluminance()
{
    m_Month = 1;
    m_Day = 1;
    m_Hour = 1.0/60.0;
}

TemporalIlluminance::TemporalIlluminance(int month, int day, double hour, const std::vector<double> &illuminance)
    : m_Month(month), m_Day(day), m_Hour(hour)
{
    m_Illuminance = illuminance;
}

bool TemporalIlluminance::add(std::vector<double> addIll){
    if (addIll.size()!=m_Illuminance.size()){
        STADIC_ERROR("The adding of the two illuminance vectors cannot be completed because they are not the same size.");
        return false;
    }
    for (int i=0;i<m_Illuminance.size();i++){
        m_Illuminance[i]=m_Illuminance[i]+addIll[i];
    }

    return true;
}

//Getters
std::vector<double> TemporalIlluminance::lux(){
    return m_Illuminance;
}
std::vector<double> TemporalIlluminance::fc(){
    std::vector<double> tempVec;
    for (int i=0;i<m_Illuminance.size();i++){
        tempVec.push_back(std::floor(m_Illuminance[i]/1.0764+0.5)/10);
    }
    return tempVec;
}

int TemporalIlluminance::month(){
    return m_Month;
}
int TemporalIlluminance::day(){
    return m_Day;
}
double TemporalIlluminance::hour(){
    return m_Hour;
}
bool TemporalIlluminance::allZeros(){
    for (int i=0;i<m_Illuminance.size();i++){
        if (m_Illuminance[i]>0){
            return false;
        }
    }
    return true;
}
double TemporalIlluminance::fractionAboveTarget(double target){
    double tempFrac=0;
    for (int i=0;i<m_Illuminance.size();i++){
        if (m_Illuminance[i]>target){
            tempFrac++;
        }
    }
    return tempFrac/m_Illuminance.size();
}

DaylightIlluminanceData::DaylightIlluminanceData(std::string units)
{
    m_Units=units;
}


bool DaylightIlluminanceData::parse(std::string fileName, std::string weaFile){
    std::ifstream iFile;
    iFile.open(fileName);
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the illuminance file "+fileName+" failed.");
        return false;
    }

    WeatherData weaData;
    if(!weaData.parseWeather(weaFile)){
        return false;
    }

    std::string line;
    int countHours=0;
    bool firstPoint=true;
    std::vector<std::vector<double>> illData;
    while (std::getline(iFile, line)){
        countHours++;
        if (firstPoint){
            std::vector<double> tmpIll;
            tmpIll.push_back(toDouble(line));
            if (m_Units=="fc"){
                for (int i=0;i<tmpIll.size();i++){
                    tmpIll[i]=tmpIll[i]*10.764;
                }
            }
            illData.push_back(tmpIll);
        }else{
            illData[countHours-1].push_back(toDouble(line));
        }
        if (countHours==weaData.hour().size()){
            countHours=0;
            firstPoint=false;
        }
    }
    iFile.close();
    for (int i=0;i<weaData.hour().size();i++){
        TemporalIlluminance datapoint(weaData.month()[i],weaData.day()[i],weaData.hour()[i],illData[i]);
        m_data.push_back(datapoint);
    }

    return true;
}


bool DaylightIlluminanceData::parseTimeBased(std::string fileName){
    std::ifstream iFile;
    iFile.open(fileName);
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the illuminance file "+fileName+" could not be opened.");
        return false;
    }
    std::string line;
    while (std::getline(iFile,line)){
        int month,day;
        double hour;
        std::vector<std::string> vals;
        vals=split(line,' ');
        if(vals.size() < 4) {
            //ERROR!
        }
        month = atoi(vals[0].c_str());
        if (month<1 || month>12){
            STADIC_ERROR("One of the month values is not acceptable.");
            return false;
        }
        day=atoi(vals[1].c_str());
        if (day<1 || day>31){
            STADIC_ERROR("One of the day values is not acceptable.");
            return false;
        }
        hour=atof(vals[2].c_str());
        if (hour<0 || hour>24){
            STADIC_ERROR("One of the hour values is not acceptable.");
            return false;
        }
        std::vector<double> ill;

        for (int i=3;i<vals.size();i++){
            ill.push_back(atof(vals[i].c_str()));
        }
        if (m_Units=="fc"){
            for (int i=0;i<ill.size();i++){
                ill[i]=ill[i]*10.764;
            }
        }
        TemporalIlluminance datapoint(month,day,hour,ill);
        m_data.push_back(datapoint);
    }
    iFile.close();
    return true;
}

bool DaylightIlluminanceData::addIllFile(std::string fileName){
    std::ifstream iFile;
    iFile.open(fileName);
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the illuminance file "+fileName+" failed.");
        return false;
    }
    std::string line;
    int i;
    while (std::getline(iFile,line)){
        i=0;
        std::vector<std::string> vals;

        vals=split(line,' ');
        std::vector<double> ill;

        for (int j=0;j<vals.size();j++){
            ill.push_back(atof(vals[j].c_str()));
        }
        if (m_Units=="fc"){
            for (int i=0;i<ill.size();i++){
                ill[i]=ill[i]*10.764;
            }
        }
        m_data[i].add(ill);
        i++;
    }
    iFile.close();
    return true;
}

bool DaylightIlluminanceData::addTimeBasedIll(std::string fileName){
    std::ifstream iFile;
    iFile.open(fileName);
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the illuminance file "+fileName+" failed.");
        return false;
    }
    std::string line;
    int i;
    while (std::getline(iFile,line)){
        i=0;
        std::vector<std::string> vals;

        vals=split(line,' ');
        std::vector<double> ill;

        for (int j=3;j<vals.size();j++){
            ill.push_back(atof(vals[j].c_str()));
        }
        if (m_Units=="fc"){
            for (int i=0;i<ill.size();i++){
                ill[i]=ill[i]*10.764;
            }
        }
        m_data[i].add(ill);
        i++;
    }
    iFile.close();
    return true;
}

bool DaylightIlluminanceData::writeIllFileLux(std::string fileName){
    std::ofstream oFile;
    oFile.open(fileName);
    if (!oFile.is_open()){
        return false;
    }
    for (int i=0;i<m_data.size();i++){
        oFile<<m_data[i].month()<<" "<<m_data[i].day()<<" "<<m_data[i].hour();
        for (int j=0;j<m_data[i].lux().size();j++){
            oFile<<" "<<m_data[i].lux()[j];
        }
        oFile<<std::endl;
    }
    oFile.close();
    return true;
}
bool DaylightIlluminanceData::writeIllFileFC(std::string fileName){
    std::ofstream oFile;
    oFile.open(fileName);
    if (!oFile.is_open()){
        return false;
    }
    for (int i=0;i<m_data.size();i++){
        oFile<<m_data[i].month()<<" "<<m_data[i].day()<<" "<<m_data[i].hour();
        for (int j=0;j<m_data[i].fc().size();j++){
            oFile<<" "<<m_data[i].fc()[j];
        }
        oFile<<std::endl;
    }
    oFile.close();
    return true;
}
void DaylightIlluminanceData::addDataPoint(TemporalIlluminance dataPoint){
    m_data.push_back(dataPoint);
}

/*
int DaylightIlluminanceData::hoursGreaterThan(double value,int point)
{
    int count = 0;
    for(unsigned i=0; i<m_data.size();i++) {
        if(m_data[i].illuminance()[point] > value) {
            count++;
        }
    }
    return count;
}
*/

std::vector<TemporalIlluminance> DaylightIlluminanceData::illuminance() const
{
    return m_data;
}

}
