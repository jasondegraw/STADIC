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
        RadPrimitive *primitive = RadPrimitive::fromRad(data,this->parent());
        if(primitive == nullptr) {
            break;
        }
        primitives.push_back(primitive);
    }
    iFile.close();
    if(primitives.size() == 0) {
        return false;
    }
    m_Primitives.insert(m_Primitives.end(),primitives.begin(),primitives.end());
    return true;
}

QPair<RadFileData*,RadFileData*> RadFileData::split(bool (*f)(RadPrimitive*))
{
    std::vector<RadPrimitive*> in, out;
    for(RadPrimitive *primitive : m_Primitives) {
        if(f(primitive)) {
            in.push_back(primitive);
        } else {
            out.push_back(primitive);
        }
    }
    // Account for 0 size vectors
    RadFileData *first = nullptr;
    RadFileData *second = nullptr;
    if (in.size() > 0) {
        first = new RadFileData(in, this->parent());
    }
    if (out.size() > 0) {
        second = new RadFileData(out, this->parent());
    }
    return QPair<RadFileData*,RadFileData*>(first,second);
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
    for(int i=0;i<m_Primitives.size();i++) {
        if(m_Primitives[i]->modifier()==layer) {
            m_Primitives[i]->setModifier("black");
        }
    }
    return true;
}
bool RadFileData::writeRadFile(QString file){
    QFile oFile;
    oFile.setFileName(file);
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        ERROR("The opening of the rad file named "+file + " has failed.");
        return false;
    }
    QTextStream out(&oFile);
    std::vector<RadPrimitive*> primitives;
    primitives=materials();
    for (int i=0;i<primitives.size();i++){
        out<<endl<<primitives[i]->modifier()<<" "<<primitives[i]->type()<<" "<<primitives[i]->name()<<endl;
        out<<primitives[i]->arg1().size();
        if (primitives[i]->arg1().size()>0){
            for (int j=0;j<primitives[i]->arg1().size();j++){
                out<<" "<<primitives[i]->arg1()[j];
            }
        }
        out<<endl;
    }

    primitives=geometry();
    for (int i=0;i<primitives.size();i++){
        out<<endl<<primitives[i]->modifier()<<" "<<primitives[i]->type()<<" "<<primitives[i]->name()<<endl;
        out<<primitives[i]->arg1().size();
        if (primitives[i]->arg1().size()>0){
            for (int j=0;j<primitives[i]->arg1().size();j++){
                out<<" "<<primitives[i]->arg1()[j];
            }
        }
        out<<endl;
    }
    oFile.close();
    return true;
}

bool RadFileData::addPrimitive(RadPrimitive *primitive)
{
    m_Primitives.push_back(primitive);
    return true;
}

//Getters
std::vector<RadPrimitive *> RadFileData::geometry() const
{
    std::vector<RadPrimitive*> primitives;
    for(RadPrimitive *primitive : m_Primitives) {
        if(primitive->isGeometry()) {
            primitives.push_back(primitive);
        }
    }
    return primitives;
}
std::vector<RadPrimitive *> RadFileData::materials() const
{
    std::vector<RadPrimitive*> primitives;
    for(RadPrimitive *primitive : m_Primitives) {
        if(primitive->isMaterial()) {
            primitives.push_back(primitive);
        }
    }
    return primitives;
}

std::vector<RadPrimitive *> RadFileData::primitives() const
{
    return m_Primitives;
}

}
