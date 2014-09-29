#include "leakcheck.h"
#include "logging.h"
#include <iostream>
#include <fstream>
#include "objects.h"
#include "gridmaker.h"
#include "functions.h"

namespace stadic{

LeakCheck::LeakCheck(QObject *parent) :
    QObject(parent)
{
}
bool LeakCheck::isEnclosed(){
    if (!makeGrid()){
        return false;
    }
    if (!writeExtraRad()){
        return false;
    }

    if (!xformModifiers()){
        return false;
    }

    if (!createOct()){
        return false;
    }

    if (!runCalc()){
        return false;
    }
    std::ifstream iFile;
    iFile.open("Final.res");
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the results file failed.");
        return false;
    }
    std::string val;
    bool containsLeak=false;
    bool likelyEnclosed=false;
    for (int i=0;i<m_Points.size();i++){
        for (int j=0;j<m_Points[i].size();j++){
            iFile>>val;
            if (toDouble(val)>0 &&toDouble(val)<0.5){
                likelyEnclosed=true;
            }else if (toDouble(val)>=0.5){
                containsLeak=true;
            }
        }
    }
    iFile.close();

    if (containsLeak){
        STADIC_ERROR("The provided model either contains a leak or the provided point is outside the space.");
        return false;
    }else if (likelyEnclosed){
        STADIC_WARNING("The illuminance value is greater than 0 at the analysis point, but less than 0.5.\n\tIt will be assumed that there is no light leak.");
    }else{
        std::cout<<"The model is fully enclosed."<<std::endl;
    }
    return true;
}
//Setters
bool LeakCheck::setRadFile(std::vector<std::string> files){
    for (int i=0;i<files.size();i++){
        FilePath radFile(files[i]);
        if (!radFile.exists()){
            STADIC_ERROR("The rad file named "+files[i]+" does not exist.");
            return false;
        }
        m_RadFiles.push_back(files[i]);
    }
    return true;
}

void LeakCheck::setFloorLayers(std::vector<std::string> layers){
    m_FloorLayers=layers;
}
bool LeakCheck::setUnits(int unit){
    if (unit>=0 &&unit<=3){
        m_Units=unit;
        return true;
    }
    return false;
}

bool LeakCheck::setReflectance(int ref){
    if (ref!=0 && ref!=1){
        STADIC_ERROR("The reflectance needs to be either 0 or 1.");
        return false;
    }
    m_Reflectance=ref;
    return true;
}

//Private
bool LeakCheck::makeGrid(){
    GridMaker grid(m_RadFiles);
    grid.setLayerNames(m_FloorLayers);
    switch (m_Units){
        case 0:
            //Inches
            grid.setOffset(24);
            grid.setSpaceX(24);
            grid.setSpaceY(24);
            grid.setOffsetZ(30);
            break;
        case 1:
            //Feet
            grid.setOffset(2);
            grid.setSpaceX(2);
            grid.setSpaceY(2);
            grid.setOffsetZ(2.5);
            break;
        case 2:
            //Millimeters
            grid.setOffset(609.6);
            grid.setSpaceX(609.6);
            grid.setSpaceY(609.6);
            grid.setOffsetZ(762);
            break;
        case 3:
            //Millimeters
            grid.setOffset(.6096);
            grid.setSpaceX(.6096);
            grid.setSpaceY(.6096);
            grid.setOffsetZ(.762);
            break;
    }
    if (!grid.makeGrid()){
        return false;
    }
    if (!grid.writePTS("Test.pts")){
        return false;
    }
    m_Points=grid.points();
    if (m_Points.empty()){
        STADIC_ERROR("The points vector is empty.");
        return false;
    }
    return true;
}

bool LeakCheck::xformModifiers(){
    std::vector<std::string> arguments;
    arguments.push_back("-m");
    arguments.push_back("modified");
    for (int i=0;i<m_RadFiles.size();i++){
        arguments.push_back(m_RadFiles[i]);
    }
    std::string programName="xform";
    Process xform(programName,arguments);
    xform.setStandardOutputFile("Mod.rad");
    xform.start();

    //There should be a test in here that if it doesn't finish it returns false
    if (!xform.wait()){
        STADIC_ERROR("The xform of the modifiers has failed.");
        return false;
    }

    return true;
}

bool LeakCheck::writeExtraRad(){
    std::ofstream oFile;
    oFile.open("Extra.rad");
    if (!oFile.is_open()){
        return false;
    }
    oFile<<"void glow sky_glow"<<std::endl;
    oFile<<"0"<<std::endl<<"0"<<std::endl<<"4 1 1 1 0"<<std::endl<<std::endl;
    oFile<<"sky_glow source sky"<<std::endl;
    oFile<<"0"<<std::endl<<"0"<<std::endl<<"4 0 0 1 180"<<std::endl<<std::endl;

    oFile<<"void plastic modified"<<std::endl;
    oFile<<"0"<<std::endl<<"0"<<std::endl;
    oFile<<"5 "<<m_Reflectance<<" "<<m_Reflectance<<" "<<m_Reflectance<<" 0 0"<<std::endl;
    oFile.close();

    return true;
}

bool LeakCheck::createOct(){
    std::vector<std::string> arguments;
    arguments.push_back("Extra.rad");
    arguments.push_back("Mod.rad");
    std::string programName="oconv";
    Process oconv(programName,arguments);

    oconv.setStandardOutputFile("Test.oct");
    oconv.start();
    if(!oconv.wait()){
        STADIC_ERROR("There was a problem creating the octree.");
        return false;
    }
    return true;
}

bool LeakCheck::runCalc(){
    std::vector<std::string> arguments;
    arguments.push_back("-I");
    arguments.push_back("-h");
    arguments.push_back("-ab");
    if (m_Reflectance<1){
        arguments.push_back("0");
    }else{
        arguments.push_back("4");
    }
    arguments.push_back("-ad");
    arguments.push_back("5000");
    arguments.push_back("-as");
    arguments.push_back("3000");
    arguments.push_back("-av");
    arguments.push_back("0");
    arguments.push_back("0");
    arguments.push_back("0");
    arguments.push_back("-aw");
    arguments.push_back("0");
    arguments.push_back("-aa");
    arguments.push_back("0.15");
    arguments.push_back("Test.oct");
    std::string programName="rtrace";
    Process rtrace(programName,arguments);
    rtrace.setStandardInputFile("Test.pts");

    std::vector<std::string> arguments2;
    arguments2.clear();
    arguments2.push_back("-e");
    arguments2.push_back("$1=179*($1*0.265+$2*0.670+$3*0.065)");
    programName="rcalc";
    Process rcalc(programName,arguments2);
    rtrace.setStandardOutputProcess(&rcalc);
    rcalc.setStandardOutputFile("Final.res");
    rtrace.start();
    if(!rtrace.wait()){
        STADIC_ERROR("The running of rtrace has failed.");
        return false;
    }
    rcalc.start();
    if(!rcalc.wait()){
        STADIC_ERROR("The running of rcalc has failed.");
        return false;
    }
    return true;
}

}
