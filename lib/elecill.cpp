#include "elecill.h"
#include <QFile>
#include <QStringList>
#include "logging.h"

ElecIll::ElecIll(QObject *parent) :
    QObject(parent)
{
}

//Setters
void ElecIll::setIlluminance(double value){
    m_Illuminace.push_back(value);
}
void ElecIll::setX(QString x){
    m_X.push_back(x);
}
void ElecIll::setY(QString y){
    m_Y.push_back(y);
}
void ElecIll::setZ(QString z){
    m_Z.push_back(z);
}

//Getters
std::vector<double> ElecIll::illuminance(){
    return m_Illuminace;
}
std::vector<QString> ElecIll::x(){
    return m_X;
}
std::vector<QString> ElecIll::y(){
    return m_Y;
}
std::vector<QString> ElecIll::z(){
    return m_Z;
}


bool ElecIll::parseIll(QString fileName){
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
        setX(vals.at(0));
        setY(vals.at(1));
        setZ(vals.at(2));
        setIlluminance(vals.at(3).toDouble());
    }
    iFile.close();
}
