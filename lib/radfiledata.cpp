#include "radfiledata.h"
#include <QFile>
#include <QTextStream>
#include "logging.h"

namespace stadic {

RadFileData::RadFileData(QObject *parent) :
    QObject(parent)
{
}

RadFileData::RadFileData(const std::vector<RadPrimitive *> &primitives, QObject *parent) : QObject(parent)
{
    m_Primitives = primitives;
}

//Setters
bool RadFileData::addRad(QString file){
    QFile iFile;
    iFile.setFileName(file);
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!iFile.exists()){
        ERROR(QString("The opening of the rad file '%1' failed.").arg(file));
        return false;
    }

    QTextStream data(&iFile);
    std::vector<RadPrimitive*> primitives;
    while (!data.atEnd()) {
        RadPrimitive *primitive = RadPrimitive::fromRad(&iFile,this->parent());
        if(!primitive) {
            iFile.close();
            return false;
        }
        primitives.push_back(primitive);
    }
    m_Primitives.insert(m_Primitives.end(),primitives.begin(),primitives.end());
    iFile.close();
    return true;
}

QPair<RadFileData*,RadFileData*> RadFileData::split(bool (*f)(RadPrimitive*,const QString&), const QString &label)
{
    std::vector<RadPrimitive*> in, out;
    for(RadPrimitive *primitive : m_Primitives) {
        if(f(primitive,label)) {
            in.push_back(primitive);
        } else {
            out.push_back(primitive);
        }
    }
    // Need to account for o size vectors!
    return QPair<RadFileData*,RadFileData*>(new RadFileData(in,this->parent()),
                                            new RadFileData(out,this->parent()));
}

static bool checkLayer(RadPrimitive *primitive, const QString &name)
{
    if(primitive->isMaterial() && primitive->name() == name) {
        return true;
    } else if(primitive->isGeometry() && primitive->modifier() == name) {
        return true;
    }
    return false;
}

bool RadFileData::removeLayer(const QString &layer, const QString &removing, const QString &rest)
{
    QFile oFile1;
    oFile1.setFileName(removing);
    oFile1.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile1.exists()){
        ERROR("The opening of the rad file named " + removing +" has failed.");
        return false;
    }

    QFile oFile2;
    oFile2.setFileName(rest);
    oFile2.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile2.exists()){
        ERROR("The opening of the rad file named " + rest +" has failed.");
        return false;
    }

    QPair<RadFileData*,RadFileData*> results = split(checkLayer,layer);

    // Write out the two files

    oFile1.close();
    oFile2.close();

    return false;
}
bool RadFileData::blackOutLayer(QString layer){

}
bool RadFileData::writeRadFile(QString file){

}


//Getters
std::vector<RadPrimitive *> RadFileData::geometry()
{
    std::vector<RadPrimitive*> primitives;
    for(RadPrimitive *primitive : m_Primitives) {
        if(primitive->isGeometry()) {
            primitives.push_back(primitive);
        }
    }
    return primitives;
}
std::vector<RadPrimitive *> RadFileData::materials()
{
    std::vector<RadPrimitive*> primitives;
    for(RadPrimitive *primitive : m_Primitives) {
        if(primitive->isMaterial()) {
            primitives.push_back(primitive);
        }
    }
    return primitives;
}

}
