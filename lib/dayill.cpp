#include "dayill.h"
#include "logging.h"
#include <QFile>
#include <QStringList>

DayIll::DayIll(QObject *parent) :
    QObject(parent)
{
}


//Setters
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

//Getters
std::vector<double> DayIll::illuminance(){
    return m_Illuminance;
}
std::vector<QString> DayIll::month(){
    return m_Month;
}
std::vector<QString> DayIll::day(){
    return m_Day;
}
std::vector<QString> DayIll::hour(){
    return m_Hour;
}




bool DayIll::parseIll(QString fileName){
    QFile iFile;
    iFile.setFileName(fileName);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!iFile.isOpen()){
        ERROR("The opening of the illuminance file "+fileName+" could not be opened.");
        return false;
    }
    QString line;
    while (!iFile.atEnd()){
        line=iFile.readLine();
        QStringList vals;
        vals=line.split(" ");
        if (setMonth(vals.at(0))){
            return false;
        }
        if (setDay(vals.at(1))){
            return false;
        }
        if (setHour(vals.at(2))){
            return false;
        }
        for (int i=3;i<vals.size()-3;i++){
            setIlluminance(vals.at(i).toDouble());
        }
    }
    iFile.close();
}
