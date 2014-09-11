#include "radfiledata.h"
#include <QFile>
#include <QTextStream>
#include "logging.h"


namespace stadic {

RadFileData::RadFileData()
{
}

RadFileData::RadFileData(const shared_vector<RadPrimitive> &primitives) : m_Primitives(primitives)
{
}

//Setters
bool RadFileData::addRad(std::string file){
    QFile iFile;
    iFile.setFileName(QString().fromStdString(file));
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!iFile.exists()){
        ERROR(QString("The opening of the rad file '%1' failed.").arg(QString().fromStdString(file)));
        return false;
    }

    QTextStream data(&iFile);
    shared_vector<RadPrimitive> primitives;
    while (!data.atEnd()) {
        RadPrimitive *primitive = RadPrimitive::fromRad(data);
        if(primitive == nullptr) {
            break;
        }
        primitives.push_back(std::shared_ptr<RadPrimitive>(primitive));
    }
    iFile.close();
    if(primitives.size() == 0) {
        return false;
    }
    m_Primitives.insert(m_Primitives.end(),primitives.begin(),primitives.end());
    return true;
}

/*
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

QPair<RadFileData*, RadFileData*> RadFileData::split(const std::vector<QString> &vector)
{
    std::vector<RadPrimitive*> in, out;
    for(RadPrimitive *primitive : m_Primitives) {
        if(primitive->isMaterial()) {
            if(std::find(vector.begin(),vector.end(),primitive->name()) != vector.end()) {
                in.push_back(primitive);
            } else {
                out.push_back(primitive);
            }
        } else if(primitive->isGeometry()) {
            if(std::find(vector.begin(),vector.end(),primitive->modifier()) != vector.end()) {
                in.push_back(primitive);
            } else {
                out.push_back(primitive);
            }
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
*/

/*
static bool checkLayer(RadPrimitive *primitive, const QString &name)
{
    if(primitive->isMaterial() && primitive->name() == name) {
        return true;
    } else if(primitive->isGeometry() && primitive->modifier() == name) {
        return true;
    }
    return false;
}

static bool checkLayers(RadPrimitive *primitive, const std::vector<QString> &names)
{
    for (int i=0;i<names.size();i++){
        if(primitive->isMaterial() && primitive->name() == names[i]) {
            return true;
        } else if(primitive->isGeometry() && primitive->modifier() == names[i]) {
            return true;
        }
    }
    return false;
}
*/
/*
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

    //QPair<RadFileData*,RadFileData*> results = split(checkLayer,layer);

    // Write out the two files

    oFile1.close();
    oFile2.close();

    return false;
}
*/

bool RadFileData::blackOutLayer(std::string layer){
    for(int i=0;i<m_Primitives.size();i++) {
        if(m_Primitives[i]->modifier()==QString().fromStdString(layer)) {
            m_Primitives[i]->setModifier("black");
        }
    }
    return true;
}

bool RadFileData::writeRadFile(std::string file){
    QFile oFile;
    oFile.setFileName(QString().fromStdString(file));
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        ERROR("The opening of the rad file named " + file + " has failed.");
        return false;
    }
    QTextStream out(&oFile);
    shared_vector<RadPrimitive> primitives=materials();
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

std::vector<double> RadFileData::surfaceNormal(std::string layer){
    std::vector<double> normalVector;
    for(int i=0;i<m_Primitives.size();i++) {
        if(m_Primitives[i]->modifier()==QString().fromStdString(layer) && QString(m_Primitives[i]->type())=="Polygon") {
            std::vector<std::vector<double>> normalPoints;
            for (int j=0;j<m_Primitives[i]->arg3().size();j++){
                std::vector<double> temp;
                for (int p=0;p<3;p++){
                    temp.push_back(QString(m_Primitives[i]->arg3()[j]).toDouble());
                    j++;
                }
                normalPoints.push_back(temp);
            }
            double x=(normalPoints[2][1]-normalPoints[1][1])*(normalPoints[0][2]-normalPoints[1][2])-(normalPoints[2][2]-normalPoints[1][2])*(normalPoints[0][1]-normalPoints[1][2]);
            double y=(normalPoints[2][0]-normalPoints[1][0])*(normalPoints[0][2]-normalPoints[1][2])-(normalPoints[2][2]-normalPoints[1][2])*(normalPoints[0][0]-normalPoints[1][0]);
            double z=(normalPoints[2][0]-normalPoints[1][0])*(normalPoints[0][1]-normalPoints[1][1])-(normalPoints[2][1]-normalPoints[1][1])*(normalPoints[0][0]-normalPoints[1][0]);
            double length=sqrt(x*x+y*y+z*z);
            x=x/length;
            y=y/length;
            z=z/length;
            normalVector.push_back(x);
            normalVector.push_back(y);
            normalVector.push_back(z);
            return normalVector;
        }
    }
    return normalVector;
}

bool RadFileData::addPrimitive(RadPrimitive *primitive)
{
    m_Primitives.push_back(std::shared_ptr<RadPrimitive>(primitive));
    return true;
}

//Getters
shared_vector<RadPrimitive> RadFileData::geometry() const
{
    shared_vector<RadPrimitive> primitives;
    for(auto primitive : m_Primitives) {
        if(primitive->isGeometry()) {
            primitives.push_back(primitive);
        }
    }
    return primitives;
}

shared_vector<RadPrimitive> RadFileData::materials() const
{
    shared_vector<RadPrimitive> primitives;
    for(auto primitive : m_Primitives) {
        if(primitive->isMaterial()) {
            primitives.push_back(primitive);
        }
    }
    return primitives;
}

shared_vector<RadPrimitive> RadFileData::primitives() const
{
    return m_Primitives;
}

}
