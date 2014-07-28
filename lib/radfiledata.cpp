#include "radfiledata.h"
#include <QFile>
#include <QTextStream>
#include "logging.h"

namespace stadic {

RadFileData::RadFileData(QObject *parent) :
    QObject(parent)
{
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

bool RadFileData::removeLayer(QString layer, QString outFile){
    /*
     What is this doing?
    QFile oFile;
    oFile.setFileName(outFile);
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        ERROR("The opening of the rad file named " + outFile +" has failed.");
        return false;
    }

    QTextStream out(&oFile);
    for (int i=0;i<m_RadMat.size();i++){
        if (m_RadMat.at(i)->name()==layer){
            out<<m_RadMat.at(i)->modifier()<<" "<<m_RadMat.at(i)->type()<<" "<<m_RadMat.at(i)->name()<<endl;
            out<<m_RadMat.at(i)->arg1().size();
            if (m_RadMat.at(i)->arg1().size()>0){
                for (int j=0;j<m_RadMat.at(i)->arg1().size();j++){
                    out<<" "<<m_RadMat.at(i)->arg1().at(j);
                }
            }
            out<<endl;
            out<<m_RadMat.at(i)->arg2().size();
            if (m_RadMat.at(i)->arg2().size()>0){
                for (int j=0;j<m_RadMat.at(i)->arg2().size();j++){
                    out<<" "<<m_RadMat.at(i)->arg2().at(j);
                }
            }
            out<<endl;
            out<<m_RadMat.at(i)->arg3().size();
            if (m_RadMat.at(i)->arg3().size()>0){
                for (int j=0;j<m_RadMat.at(i)->arg3().size();j++){
                    out<<" "<<m_RadMat.at(i)->arg3().at(j);
                }
            }
            out<<endl<<endl;
        }
    }

    for (int i=0;i<m_RadGeo.size();i++){
        if (m_RadGeo.at(i)->modifier()==layer){
            out<<m_RadGeo.at(i)->modifier()<<" "<<m_RadGeo.at(i)->type()<<" "<<m_RadGeo.at(i)->name()<<endl;
            out<<m_RadGeo.at(i)->arg1().size();
            if (m_RadGeo.at(i)->arg1().size()>0){
                for (int j=0;j<m_RadGeo.at(i)->arg1().size();j++){
                    out<<" "<<m_RadGeo.at(i)->arg1().at(j);
                }
            }
            out<<endl;
            out<<m_RadGeo.at(i)->arg2().size();
            if (m_RadGeo.at(i)->arg2().size()>0){
                for (int j=0;j<m_RadGeo.at(i)->arg2().size();j++){
                    out<<" "<<m_RadGeo.at(i)->arg2().at(j);
                }
            }
            out<<endl;
            out<<m_RadGeo.at(i)->arg3().size();
            if (m_RadGeo.at(i)->arg3().size()>0){
                for (int j=0;j<m_RadGeo.at(i)->arg3().size();j++){
                    out<<" "<<m_RadGeo.at(i)->arg3().at(j);
                }
            }
            out<<endl<<endl;
        }
    }
    //Remove material and geometry from respective vectors


    oFile.close();
    */
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
