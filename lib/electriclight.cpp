/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission of the
 *    respective copyright holder or contributor.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *****************************************************************************/

#include "electriclight.h"
#include "logging.h"
#include <iostream>
#include <fstream>
#include "filepath.h"
#include "stadicprocess.h"
#include "gridmaker.h"
#include "functions.h"
#include "elecill.h"

namespace stadic{

ElectricLight::ElectricLight(BuildingControl *model) :
    m_Model(model)
{
}

bool ElectricLight::simElectricLight()
{
    std::vector<std::shared_ptr<Control>> spaces=m_Model->spaces();
    for (int i=0;i<spaces.size();i++){
        std::vector<std::string> illFileNames;
        std::vector<std::string> radFileNames;
        std::string iesDirectory;
        iesDirectory=spaces[i].get()->spaceDirectory()+spaces[i].get()->iesDirectory();
        std::vector<ControlZone> zones=spaces[i].get()->controlZones();
        std::string iesFileName;
        std::string radUnits;
        radUnits=spaces[i].get()->importUnits();
        if (radUnits=="mm"){
            radUnits="-dm/1000";
        }else if (radUnits=="m"){
            radUnits="-dm";
        }else if (radUnits=="in"){
            radUnits="-di";
        }else if (radUnits=="ft"){
            radUnits="-df";
        }else{
            STADIC_LOG(Severity::Error, "The import units are not of an allowable entry.");
            return false;
        }
        for (int j=0;j<zones.size();j++){
            radFileNames.push_back(iesDirectory+spaces[i].get()->spaceName()+"_"+zones[i].name()+".rad");
            illFileNames.push_back(spaces[i].get()->spaceDirectory()+spaces[i].get()->resultsDirectory()+spaces[i].get()->spaceName()+"_"+zones[i].name()+".ill");
            iesFileName=iesDirectory+zones[j].iesFile();
            if (!lum2Rad(zones[i], radFileNames[j], iesFileName, radUnits, iesDirectory)){
                return false;
            }
            //Test whether the points file exists.  If it doesn't, test whether the necessary arguments to create one exist.
            if (!stadic::exists(spaces[i].get()->spaceDirectory()+spaces[i].get()->inputDirectory()+spaces[i].get()->ptsFile()[0])){
                if (spaces[i].get()->xSpacing()&&spaces[i].get()->ySpacing()&& spaces[i].get()->offset() && spaces[i].get()->zOffset()&& (spaces[i].get()->modifiers()||spaces[i].get()->identifiers())){
                    STADIC_LOG(stadic::Severity::Info, "The points file "+spaces[i].get()->ptsFile()[0] + " does not exist.  The creation of a new points file will be attempted.");
                    GridMaker ptsCreator(spaces[i].get()->spaceDirectory()+spaces[i].get()->geoDirectory()+ spaces[i].get()->geoFile());
                    ptsCreator.setSpaceX(toDouble(spaces[i].get()->xSpacing().get()));
                    ptsCreator.setSpaceY(toDouble(spaces[i].get()->ySpacing().get()));
                    ptsCreator.setOffset(toDouble(spaces[i].get()->offset().get()));
                    ptsCreator.setOffsetZ(toDouble(spaces[i].get()->zOffset().get()));
                    if (spaces[i].get()->modifiers()){
                        ptsCreator.setLayerNames(spaces[i].get()->modifiers().get());
                    }else{
                        ptsCreator.setIdentifiers(spaces[i].get()->identifiers().get());
                    }
                    if (ptsCreator.makeGrid()){
                        if (!ptsCreator.writePTS(spaces[i].get()->spaceDirectory()+spaces[i].get()->inputDirectory()+spaces[i].get()->spaceName()+"_AutoGen.pts")){
                            STADIC_LOG(stadic::Severity::Error, "The writing of the points file has failed.");
                            return false;
                        }
                        std::vector<std::string> ptsFiles;
                        ptsFiles.clear();
                        ptsFiles.push_back(spaces[i].get()->spaceName()+"_AutoGen.pts");
                        spaces[i].get()->setPTSFile(ptsFiles);
                    }else{
                        STADIC_LOG(stadic::Severity::Error, "The creation of the points file has failed.");
                        return false;
                    }
                }else{
                    STADIC_LOG(stadic::Severity::Info, "The points file "+spaces[i].get()->ptsFile()[0] + " does not exist.  And no arguments exist for one to be generated.");
                    return false;
                }
                STADIC_LOG(stadic::Severity::Info, "A new points file has been successfully generated.");
            }
            std::vector<std::string> radFiles;
            radFiles.push_back(spaces[i].get()->spaceDirectory()+spaces[i].get()->geoDirectory()+spaces[i].get()->matFile());
            radFiles.push_back(spaces[i].get()->spaceDirectory()+spaces[i].get()->geoDirectory()+spaces[i].get()->geoFile());
            radFiles.push_back(radFileNames[j]);
            if (!simZone(radFiles, spaces[i].get()->ptsFile()[0], illFileNames[j], i)){
                return false;
            }
        }
    }


    return true;
}


//Setters

//Private
bool ElectricLight::lum2Rad(ControlZone zone, std::string zoneRadFile, std::string iesFile, std::string units, std::string iesDirectory){
    //Get the initial lumens from the ies photometry file.
    int initialLumens;
    initialLumens=getLampLumens(iesFile);
    if (initialLumens==0){
        return false;
    }
    if (zone.lampLumens()>0 && initialLumens==-1){
        STADIC_LOG(Severity::Info, "The ies file for "+zone.name()+" contains absolute photometry.  The lamp lumens in the control file will be ignored.");
    }
    //Write the rad file format from the ies photometry file using ies2rad.
    std::vector<std::string> arguments;
    std::string ies2radProgram;
    ies2radProgram="ies2rad";
    arguments.clear();
    //Populate the arguments for ies2rad at this point then start the program.
    arguments.push_back(units);
    arguments.push_back("-l");
    arguments.push_back(iesDirectory);
    arguments.push_back("-c");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back("-t");
    arguments.push_back("default");
    arguments.push_back("-m");
    if (initialLumens<0){       //This is typical if the fixture is an LED module that cannot be replaced for higher/lower output versions.
        arguments.push_back(toString(zone.llf()));
    }else{  //This is if the fixture has lamps that can be replaced.
        arguments.push_back(toString(zone.llf()*zone.lampLumens()/double(initialLumens)));
    }
    arguments.push_back("-o");
    arguments.push_back(zone.name()+"_temp");
    arguments.push_back(iesFile);

    Process ies2rad(ies2radProgram, arguments);
    ies2rad.setStandardOutputFile(zone.name()+".rad");
    ies2rad.start();
    if (!ies2rad.wait()){
        STADIC_LOG(Severity::Error, "The running of ies2rad has failed for creating the electric lighting rad file named "+zone.name()+".ies.");
        return false;
    }

    if (!fixDatFile(iesDirectory+zone.name()+"_temp.rad", iesDirectory+zone.name()+".rad", zone.name()+"_temp.dat", iesDirectory+zone.name()+"_temp.dat")){
        STADIC_LOG(Severity::Error, "The correcting of the .dat file reference has falied.");
        return false;
    }


    //Write the xform commands to move the luminaires from the origin.
    for (int i=0;i<zone.luminaireLayout().size();i++){
        std::string xformProgram;
        xformProgram="xform";
        arguments.clear();
        arguments.push_back("-c");
        arguments.push_back("-rz");
        arguments.push_back(toString(zone.luminaireLayout()[i][5]));
        arguments.push_back("-ry");
        arguments.push_back(toString(zone.luminaireLayout()[i][4]));
        arguments.push_back("-rz");
        arguments.push_back(toString(zone.luminaireLayout()[i][3]));
        arguments.push_back("-t");
        arguments.push_back(toString(zone.luminaireLayout()[i][0]));
        arguments.push_back(toString(zone.luminaireLayout()[i][1]));
        arguments.push_back(toString(zone.luminaireLayout()[i][2]));
        arguments.push_back(iesDirectory + zone.name()+".rad");

        Process xform(xformProgram, arguments);
        if (i==0){
            xform.setStandardOutputFile(zoneRadFile);
        }else{
            xform.setStandardOutputFile(zoneRadFile, Process::AppendOutput);
        }
        xform.start();
        if (!xform.wait()){
            STADIC_LOG(stadic::Severity::Error, "The running of xform has failed for creating the electric lighting zone rad file named "+zoneRadFile+".");
            return false;
        }
    }

    return true;
}
bool ElectricLight::simZone(std::vector<std::string> radFiles, std::string ptsFile, std::string zoneIllFile, int spaceIndex){
    //Simulate the electric lighting zones
    std::vector<std::string> arguments;
    std::string rtraceProgram;
    rtraceProgram="rtrace";
    arguments.clear();
    arguments.push_back("-h");
    arguments.push_back("-I");
    //Populate the arguments for rtrace at this point then start the program.
    if (m_Model->spaces()[spaceIndex]->getParamSet("default")){
        std::unordered_map<std::string, std::string> tempMap=m_Model->spaces()[spaceIndex]->getParamSet("default").get();
        for (std::unordered_map<std::string, std::string>::iterator it=tempMap.begin(); it!=tempMap.end();++it){
            if (it->first!="sj" && it->first!="lw" && it->first!="ad"){
                //Don't pass the -ad value to rtrace for the electric light.
                arguments.push_back("-"+it->first);
                arguments.push_back(it->second);
            }else if (it->first=="lw"){
                arguments.push_back("-lw");
                /*
                if (toDouble(it->second)>0.00002){
                    STADIC_LOG(Severity::Info, "The lw argument has been changed from "+it->second+" to .00002 .");
                    arguments.push_back("0.00002");
                }else{
                    arguments.push_back(it->second);
                }
                */
                arguments.push_back(it->second);
            }
        }
    }else{
        STADIC_LOG(Severity::Fatal, "The default parameter set is not found.");
    }
    //Create the octree
    std::string octName;
    octName=m_Model->spaces()[spaceIndex]->spaceDirectory()+m_Model->spaces()[spaceIndex]->geoDirectory()+m_Model->spaces()[spaceIndex]->spaceName()+".oct";
    if (!createOctree(radFiles, octName)){
        STADIC_LOG(Severity::Fatal, "The creation of the octree failed.");
    }
    arguments.push_back(octName);

    Process rtrace(rtraceProgram, arguments);
    rtrace.setStandardInputFile(m_Model->spaces()[spaceIndex]->spaceDirectory()+m_Model->spaces()[spaceIndex]->inputDirectory()+ptsFile);
    rtrace.setStandardOutputFile(zoneIllFile+".tmp");
    rtrace.start();
    if (!rtrace.wait()){
        STADIC_LOG(stadic::Severity::Error, "The running of rtrace has failed for creating the electric lighting illuminance file "+zoneIllFile+".tmp.");
        return false;
    }

    //Read in the temporary illuminance file and generate the final illuminance file once rcalc has been run.
    arguments.clear();
    std::string rcalcProgram;
    rcalcProgram="rcalc";
    arguments.push_back("-e");
    if (m_Model->illumUnits().get()=="lux"){
        arguments.push_back("$1=47.4*$1+120*$2+11.6*$3");
    }else{
        arguments.push_back("$1=4.405*$1+11.152*$2+1.078*$3");
    }

    Process rcalc(rcalcProgram, arguments);
    rcalc.setStandardInputFile(zoneIllFile+".tmp");
    rcalc.setStandardOutputFile(zoneIllFile+".tmp2");
    rcalc.start();
    if (!rcalc.wait()){
        STADIC_LOG(Severity::Error, "The running of rcalc has failed for creating the electric lighting illuminance file " + zoneIllFile+".tmp2.");
        return false;
    }

    ElectricIlluminanceData finalIll;
    if (!finalIll.parseNonSpatialIlluminance(zoneIllFile+".tmp2", m_Model->spaces()[spaceIndex]->spaceDirectory()+m_Model->spaces()[spaceIndex]->inputDirectory()+ptsFile, zoneIllFile, m_Model->illumUnits().get())){
        return false;
    }

    return true;
}
int ElectricLight::getLampLumens(std::string iesFile){
    std::ifstream iesIn;
    iesIn.open(iesFile);
    if (!iesIn.is_open()){
        STADIC_LOG(Severity::Fatal, "The opening of the .ies photometry file "+iesFile + " has failed.");
        return 0;
    }
    std::string tempString;
    do{
        iesIn>>tempString;
    }while (tempString!="TILT=NONE" && tempString!="TILT=INCLUDE");

    if (tempString=="TILT=INCLUDE"){
        STADIC_LOG(Severity::Fatal, "The .ies photometry file "+iesFile + " contains \"TILT=INCLUDE\", which cannot be used.");
        return 0;
    }
    int tempInt;
    iesIn>>tempInt;
    iesIn>>tempInt; //Pull in the initial lamp lumens from the ies file.
    return tempInt;
}
bool ElectricLight::createOctree(std::vector<std::string> files, std::string octreeName){
    std::string oconvProgram="oconv";
    Process oconv(oconvProgram,files);
    oconv.setStandardOutputFile(octreeName);

    oconv.start();

    if (!oconv.wait()){
        STADIC_ERROR("The creation of the octree has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }
    return true;
}

bool ElectricLight::fixDatFile(std::string radFileName,  std::string newRadFileName, std::string zoneName, std::string newDatFileName){
    std::ifstream iFile;
    iFile.open(radFileName);
    if (!iFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the .rad file named "+radFileName+ " failed.");
        return false;
    }

    std::ofstream oFile;
    oFile.open(newRadFileName);
    if (!oFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the .rad file named "+newRadFileName+" failed.");
        return false;
    }

    std::string line;

    while (std::getline(iFile, line)){
        std::vector<std::string> vals;
        vals=split(line, ' ');
        for (int i=0;i<vals.size();i++){
            if(vals[i]==(zoneName)) {
                vals[i]=newDatFileName;
            }
            if (i==(vals.size()-1)){
                oFile<<vals[i]<<std::endl;
            }else{
                oFile<<vals[i]<<" ";
            }
        }
    }
    iFile.close();
    oFile.close();
    return true;
}


}
