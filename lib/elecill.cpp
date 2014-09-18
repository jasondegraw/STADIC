#include "elecill.h"
#include <QFile>
#include <QStringList>
#include "logging.h"

namespace stadic {

ElectricIlluminance::ElectricIlluminance()
{
}

//Setters
/*
void ElectricIlluminance::setIlluminance(double value){
    m_Illuminace.push_back(value);
}
void ElectricIlluminance::setX(QString x){
    m_X.push_back(x);
}
void ElectricIlluminance::setY(QString y){
    m_Y.push_back(y);
}
void ElectricIlluminance::setZ(QString z){
    m_Z.push_back(z);
}
*/

//Getters
std::vector<double> ElectricIlluminance::illuminance(){
    return m_Illuminance;
}
std::vector<std::string> ElectricIlluminance::x(){
    return m_X;
}
std::vector<std::string> ElectricIlluminance::y(){
    return m_Y;
}
std::vector<std::string> ElectricIlluminance::z(){
    return m_Z;
}


bool ElectricIlluminance::parseIlluminance(std::string fileName){
    QFile iFile;
    iFile.setFileName(QString::fromStdString(fileName));
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!iFile.isOpen()){
        STADIC_ERROR("The opening of the illuminance file "+fileName+" could not be opened.");
        return false;
    }
    QString line;
    while (!iFile.atEnd()){
        line=iFile.readLine();
        QStringList vals;
        vals=line.split(" ");
        m_X.push_back(vals.at(0).toStdString());
        m_Y.push_back(vals.at(1).toStdString());
        m_Z.push_back(vals.at(2).toStdString());
        m_Illuminance.push_back(vals.at(3).toDouble());
    }
    iFile.close();
    return true;
}

}
