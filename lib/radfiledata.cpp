#include "radfiledata.h"
#include <QFile>
#include <QTextStream>
#include "logging.h"
#include <fstream>
#include <sstream>
#include <iostream>


namespace stadic {

RadFileData::RadFileData()
{
}

RadFileData::RadFileData(const shared_vector<RadPrimitive> &primitives) : m_Primitives(primitives)
{
}

//Setters
bool RadFileData::addRad(std::string file){
    std::ifstream iFile;
    iFile.open(file);
    if (!iFile.is_open()){
        ERROR(QString("The opening of the rad file '%1' failed.").arg(QString().fromStdString(file)));
        return false;
    }
    std::stringstream data;
    data<<iFile.rdbuf();
    shared_vector<RadPrimitive> primitives;
    while (!data.eof()) {
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
*/

QPair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive> >  RadFileData::split(const std::vector<std::string> &vector)
{
	shared_vector<RadPrimitive> in, out;
    for(std::shared_ptr<RadPrimitive> primitive : m_Primitives) {
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
    // In this new version, the caller is responsible for checking that the vectors actually contain something
	return QPair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive>>(in, out);
}

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
        if(m_Primitives[i]->modifier()==layer) {
            m_Primitives[i]->setModifier("black");
        }
    }
    return true;
}

bool RadFileData::writeRadFile(std::string file){
    std::ofstream oFile;
    oFile.open(file);
    if (!oFile.is_open()){
        ERROR("The opening of the rad file named " + file + " has failed.");
        return false;
    }
    shared_vector<RadPrimitive> primitives=materials();
    for (int i=0;i<primitives.size();i++){
        oFile<<std::endl<<primitives[i]->modifier()<<" "<<primitives[i]->type()<<" "<<primitives[i]->name()<<std::endl;
        oFile<<primitives[i]->arg1().size();
        if (primitives[i]->arg1().size()>0){
            for (int j=0;j<primitives[i]->arg1().size();j++){
                oFile<<" "<<primitives[i]->arg1()[j];
            }
        }
        oFile<<std::endl;
    }

    primitives=geometry();
    for (int i=0;i<primitives.size();i++){
        oFile<<std::endl<<primitives[i]->modifier()<<" "<<primitives[i]->type()<<" "<<primitives[i]->name()<<std::endl;
        oFile<<primitives[i]->arg1().size();
        if (primitives[i]->arg1().size()>0){
            for (int j=0;j<primitives[i]->arg1().size();j++){
                oFile<<" "<<primitives[i]->arg1()[j];
            }
        }
        oFile<<std::endl;
    }
    oFile.close();

    return true;
}

std::vector<double> RadFileData::surfaceNormal(std::string layer){
    std::vector<double> normalVector;
    for(int i=0;i<m_Primitives.size();i++) {
        if(m_Primitives[i]->modifier()==layer && QString(m_Primitives[i]->type())=="Polygon") {
            std::vector<std::vector<double>> normalPoints;
            for (int j=0;j<m_Primitives[i]->arg3().size();j++){
                std::vector<double> temp;
                for (int p=0;p<3;p++){
                    temp.push_back(atof(m_Primitives[i]->arg3()[j].c_str()));
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
