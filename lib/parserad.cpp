#include "parserad.h"
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
        ERROR("The opening of the rad file failed.");
        return false;
    }

    QTextStream data(&iFile);
    while (!data.atEnd()){
        QString string;
        QString string2;
        data>>string;
        if (string.contains('#')){
            QString tempString;
            tempString=data.readLine();
        }else{
            if (string.contains("void")){
                RadGeometry *rad=new RadGeometry(this);
                rad->setModifier("void");
                data>>string;   //reads type
                rad->setType(string);
                data>>string;   //reads name
                rad->setName(string);
                data>>string;   //Reads number of arguments from first line
                if (string.toInt()>0){
                    std::vector<QString> args;
                    for (int i=0;i<string.toInt();i++){
                        data>>string2;
                        args.push_back(string2);
                    }
                    rad->setArg1(args);
                }
                data>>string;   //Reads number of arguments from second line
                if (string.toInt()>0){
                    std::vector<QString> args;
                    for (int i=0;i<string.toInt();i++){
                        data>>string2;
                        args.push_back(string2);
                    }
                    rad->setArg2(args);
                }
                data>>string;   //Reads number of arguments from third line
                if (string.toInt()>0){
                    std::vector<QString> args;
                    for (int i=0;i<string.toInt();i++){
                        data>>string2;
                        args.push_back(string2);
                    }
                    rad->setArg3(args);
                }
                m_RadMat.push_back(rad);
            }else{
                data>>string2;
                if (string2=="polygon"){
                    RadGeometry *rad=new RadGeometry(this);
                    rad->setModifier(string);
                    rad->setType(string2);
                    data>>string;
                    rad->setName(string);
                    data>>string;               //Read number of arguments from first line
                    if (string.toInt()>0){
                        std::vector<QString> args;
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                            args.push_back(string2);
                        }
                        rad->setArg1(args);
                    }
                    data>>string;               //Read number of arguments from second line
                    if (string.toInt()>0){
                        std::vector<QString> args;
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                            args.push_back(string2);
                        }
                        rad->setArg1(args);
                    }
                    data>>string;               //Read number of arguments from third line
                    if (string.toInt()>0){
                        std::vector<QString> args;
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                            args.push_back(string2);
                        }
                        rad->setArg3(args);
                    }
                    m_RadGeo.push_back(rad);
                }else{                          //If it isn't a polygon, the info still needs to be read in
                    WARNING("The rad file contains a modifer that is not \"null\", but is not a polygon.\nThis piece of input will be ignored.");
                    data>>string;   //reads name
                    data>>string;   //Reads number of arguments from first line
                    if (string.toInt()>0){
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                        }
                    }
                    data>>string;   //Reads number of arguments from second line
                    if (string.toInt()>0){
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                        }
                    }
                    data>>string;   //Reads number of arguments from third line
                    if (string.toInt()>0){
                        for (int i=0;i<string.toInt();i++){
                            data>>string2;
                        }
                    }
                }
            }
        }
    }
    iFile.close();
}
bool RadFileData::removeLayer(QString layer, QString outFile){
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
}
bool RadFileData::blackOutLayer(QString layer){

}
bool RadFileData::writeRadFile(QString file){

}


//Getters
std::vector<RadGeometry *> RadFileData::geometry(){
    return m_RadGeo;
}
std::vector<RadGeometry *> RadFileData::materials(){
    return m_RadMat;
}

}
