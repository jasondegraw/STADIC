#include "dayill.h"
#include "logging.h"
#include <fstream>
#include "functions.h"
#include <QFile>
#include <QStringList>
#include <QTextStream>

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
        ERROR("The adding of the two illuminance vectors cannot be completed because they are not the same size.");
        return false;
    }
    for (int i=0;i<m_Illuminance.size();i++){
        m_Illuminance[i]=m_Illuminance[i]+addIll[i];
    }

    return true;
}

//Getters
std::vector<double> TemporalIlluminance::illuminance(){
    return m_Illuminance;
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

DaylightIlluminanceData::DaylightIlluminanceData(QObject *parent) :
    QObject(parent)
{
}


//Setters
/*
void DayIll::setIlluminance(double value){
    if (value>0){
        m_Illuminance.push_back(value);
    }else{
        WARNING("The illuminance data contains value less than 0 which will be set to 0.");
        m_Illuminance.push_back(0);
    }
}
bool DayIll::setMonth(QString month){
    if (month.toInt()>0 && month.toInt()<13){
        m_Month.push_back(month);
    }else{
        ERROR("The illuminance data contains month values that are not between 1 and 12.");
        return false;
    }
    return true;
}
bool DayIll::setDay(QString day){
    if (day.toInt()>0 && day.toInt()<32){
        m_Month.push_back(day);
    }else{
        ERROR("The illuminance data contains day values that are not between 1 and 31.");
        return false;
    }
    return true;
}
bool DayIll::setHour(QString hour){
    if (hour.toDouble()>0 &&hour.toDouble()<24){
        m_Hour.push_back(hour);
    }else{
        ERROR("The illuminance data contains hour values that are not between 0 and 24,\n\texcluding 0 and 24.");
        return false;
    }
    return true;
}
*/

bool DaylightIlluminanceData::parse(std::string fileName, std::string weaFile){
    std::ifstream iFile;
    iFile.open(fileName);
    if (!iFile.is_open()){
        ERROR("The opening of the illuminance file "+fileName+" failed.");
        return false;
    }

    std::ifstream weaInFile;
    weaInFile.open(weaFile);
    if (!weaInFile.is_open()){
        ERROR("The opening of the wea file " +weaFile+" failed.");
        return false;
    }

    std::string line;
    std::string weaLine;
    while (std::getline(iFile,line)){
        int month,day;
        double hour;
        std::getline(weaInFile,weaLine);
        std::vector<std::string> vals;
        vals=split(weaLine,' ');
        if(vals.size() < 5) {
            ERROR("The number of items on each line of the wea file is less than 5.");
            return false;
        }
        month =atoi(vals[0].c_str());
        if (month<1 || month>12){
            ERROR("One of the month values is not acceptable.");
            return false;
        }
        day=atoi(vals[1].c_str());
        if (day<1 || day>31){
            ERROR("One of the day values is not acceptable.");
            return false;
        }
        hour=atof(vals[2].c_str());
        if (hour<0 || hour>24){
            ERROR("One of the hour values is not acceptable.");
            return false;
        }
        vals=split(line,' ');
        std::vector<double> ill;

        for (int i=0;i<vals.size();i++){
            ill.push_back(atof(vals[i].c_str()));
        }

        TemporalIlluminance datapoint(month,day,hour,ill);
        m_data.push_back(datapoint);
    }
    iFile.close();
    weaInFile.close();
    return true;
}


bool DaylightIlluminanceData::parseTimeBased(std::string fileName){
    std::ifstream iFile;
    iFile.open(fileName);
    if (!iFile.is_open()){
        ERROR("The opening of the illuminance file "+fileName+" could not be opened.");
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
            ERROR("One of the month values is not acceptable.");
            return false;
        }
        day=atoi(vals[1].c_str());
        if (day<1 || day>31){
            ERROR("One of the day values is not acceptable.");
            return false;
        }
        hour=atof(vals[2].c_str());
        if (hour<0 || hour>24){
            ERROR("One of the hour values is not acceptable.");
            return false;
        }
        std::vector<double> ill;

        for (int i=3;i<vals.size()-3;i++){
            ill.push_back(atof(vals[i].c_str()));
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
        ERROR("The opening of the illuminance file "+fileName+" failed.");
        return false;
    }
    std::string line;
    int i;
    while (std::getline(iFile,line)){
        i=0;
        std::vector<std::string> vals;

        vals=split(line,' ');
        std::vector<double> ill;

        for (int i=0;i<vals.size();i++){
            ill.push_back(atof(vals[i].c_str()));
        }
        m_data[i].add(ill);
        i++;
    }
    iFile.close();
    return true;
}

bool DaylightIlluminanceData::addTimeBasedIll(std::string fileName){

    return true;
}

bool DaylightIlluminanceData::writeIllFile(std::string fileName){
    std::ofstream oFile;
    oFile.open(fileName);
    if (!oFile.is_open()){
        return false;
    }
    for (int i=0;i<m_data.size();i++){
        oFile<<m_data[i].month()<<" "<<m_data[i].day()<<" "<<m_data[i].hour();
        for (int j=0;j<m_data[i].illuminance().size();j++){
            oFile<<" "<<m_data[i].illuminance()[j];
        }
        oFile<<std::endl;
    }
    oFile.close();
    return true;
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

}
