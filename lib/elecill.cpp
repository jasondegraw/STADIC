#include "elecill.h"
#include <QFile>
#include <QStringList>
#include "logging.h"
#include "functions.h"

namespace stadic {


SpatialIlluminance::SpatialIlluminance()
{
    m_Illuminance = 0;
    m_x = m_y = m_z = "0.0";
}

SpatialIlluminance::SpatialIlluminance(std::string x, std::string y, std::string z, double illuminance) : SpatialIlluminance()
{
    m_Illuminance = illuminance;
    bool ok;
    stadic::toDouble(x, &ok);
    if(ok) {
        m_x = x;
    } else {
        STADIC_ERROR("Bad spatial illuminance x value ("+x+")");
    }
    stadic::toDouble(y, &ok);
    if(ok) {
        m_y = y;
    } else {
        STADIC_ERROR("Bad spatial illuminance y value ("+y+")");
    }
    stadic::toDouble(z, &ok);
    if(ok) {
        m_z = z;
    } else {
        STADIC_ERROR("Bad spatial illuminance z value ("+z+")");
    }
}

//Getters
double SpatialIlluminance::lux(){
    return m_Illuminance;
}
double SpatialIlluminance::fc(){
    return m_Illuminance/10.764;
}

std::string SpatialIlluminance::x(){
    return m_x;
}
std::string SpatialIlluminance::y(){
    return m_y;
}
std::string SpatialIlluminance::z(){
    return m_z;
}


ElectricIlluminanceData::ElectricIlluminanceData()
{
}

//Getters
std::vector<SpatialIlluminance> ElectricIlluminanceData::illuminance(){
    return m_Illuminance;
}

bool ElectricIlluminanceData::parseIlluminance(std::string fileName){
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
        if(vals.size() != 3) {
            continue;
        }
        m_Illuminance.push_back(SpatialIlluminance(vals.at(0).toStdString(), vals.at(1).toStdString(), vals.at(2).toStdString(),
            vals.at(3).toDouble()));
    }
    iFile.close();
    return true;
}

}
