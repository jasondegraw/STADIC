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

#include "daylight.h"
#include "logging.h"
#include "dayill.h"
#include "filepath.h"
#include "stadicprocess.h"
#include <fstream>
#include "materialprimitives.h"
#include "gridmaker.h"
#include "weatherdata.h"
#include "photosensor.h"
#include <iostream>

namespace stadic {
Daylight::Daylight(BuildingControl *model) :
    m_Model(model)
{
}

bool Daylight::simDaylight()
{
    std::vector<std::shared_ptr<Control>> spaces=m_Model->spaces();
    for (int i=0;i<spaces.size();i++){
        //Set up the directories if they do not already exist
        PathName resDir(spaces[i].get()->spaceDirectory()+spaces[i].get()->resultsDirectory());
        if (!resDir.exists()){
            if (!resDir.create()){
                STADIC_LOG(Severity::Fatal, "The creation of the results directory failed at "+resDir.toString());
                return false;
            }
        }
        PathName intermediateDir(spaces[i].get()->spaceDirectory()+spaces[i].get()->intermediateDataDirectory());
        if (!intermediateDir.exists()){
            if (!intermediateDir.create()){
                STADIC_LOG(Severity::Fatal, "The creation of the intermediate directory failed at "+intermediateDir.toString());
                return false;
            }
        }
        PathName inputDir(spaces[i].get()->spaceDirectory()+spaces[i].get()->inputDirectory());
        if (!inputDir.exists()){
            if (!inputDir.create()){
                STADIC_LOG(Severity::Fatal, "The creation of the input directory failed at "+inputDir.toString());
                return false;
            }
        }

        if (!uniqueGlazingMaterials(spaces[i].get())){
            return false;
        }
        //If the next line causes a crash in the program, it is most likely in setSimCase having to do with the second test.
        // This comment is all well and good... but the program should never crash
        if (!testSimCase(spaces[i].get())){
            return false;
        }

        bool BSDFs=false;
        for (int j=0;j<m_SimCase.size();j++){
            if (m_SimCase[j]>0){
                BSDFs=true;
            }
        }
        if (!writeSky(spaces[i].get())){
            return false;
        }
        if (!createBaseRadFiles(spaces[i].get())){
            return false;
        }
        //Configure the simulation for each window group
        for (int j=0;j<spaces[i].get()->windowGroups().size();j++){
            switch (m_SimCase[j]){
                case 1:
                    if (!simCase1(j,spaces[i].get())){
                        return false;
                    }
                    break;
                case 2:
                    if (!simCase2(j, spaces[i].get())){
                        return false;
                    }
                    break;
                case 3:
                    //Simulation case 3 will be for window groups that contain BSDFs even in the base case, but the glazing layers are not BSDFs
                    if(!simCase3(j,spaces[i].get())){
                        return false;
                    }
                    break;
                case 4:
                    //Simulation case 4 will be for window groups that have shade materials in addition to the glazing layer
                    if (!simCase4(j,spaces[i].get())){
                        return false;
                    }
                    break;
                case 5:
                    //Simulation case 5 will be for window groups that have added geometry, but it is a proxy geometry
                    if (!simCase5(j,spaces[i].get())){
                        return false;
                    }
                    break;
                case 6:
                    //Simulation case 6 will be for window groups that only have the glazing layer as a BSDF
                    if (!simCase6(j,spaces[i].get())){
                        return false;
                    }
                    break;

            }
        }
        if(!sumIlluminanceFiles(spaces[i].get())){
            return false;
        }
    }
    return true;
}

//Private
bool Daylight::simBSDF(int blindGroupNum, int setting, int bsdfNum, std::string bsdfRad,std::string remainingRad, std::vector<double> normal, std::string thickness, std::string bsdfXML, std::string bsdfLayer, Control *model){
    std::string mainFileName;
    if (setting==-1){
        mainFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(bsdfNum);
    }else{
        mainFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting)+"_bsdf"+std::to_string(bsdfNum);
    }

    //Create initial octrees
    std::vector<std::string> files;
    files.push_back(remainingRad);
    files.push_back(bsdfRad);
    files.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+"sky_white1.rad");
    std::string mainOct=mainFileName+".oct";
    if(!createOctree(files,mainOct)){
        return false;
    }

    //FivePhaseOut<<endl<<"xform -m black "<<RadFileName<<" > "<<string(project_directory)<<"tmp/black.rad"<<endl;
    //Black out the remainingRad file
    std::vector<std::string> arguments;
    arguments.push_back("-m");
    arguments.push_back("black");
    arguments.push_back(remainingRad);
    std::string xformProgram="xform";
    Process xform(xformProgram,arguments);
    std::string blackRad=mainFileName+"_allblack.rad";
    xform.setStandardOutputFile(blackRad);
    xform.start();
    if (!xform.wait()){
        STADIC_ERROR("The xform command failed to convert layers to black.");
        //DISPLAY ERRORS HERE
        return false;
    }


    //Create the octree for the blacked out rad file
    files[0]=blackRad;
    std::string blackOct=mainFileName+"_allblack.oct";
    if(!createOctree(files,blackOct)){
        return false;
    }

    //Create Suns
    int nSuns;
    if(model->sunDivisions()==1){
        nSuns=145;
    }else if (model->sunDivisions()==2){
        nSuns=577;
    }else if (model->sunDivisions()==3){
        nSuns=1297;
    }else if (model->sunDivisions()==4){
        nSuns=2305;
    }else if (model->sunDivisions()==5){
        nSuns=3601;
    }else if (model->sunDivisions()==6){
        nSuns=5185;
    }

    std::string tempFile=model->intermediateDataDirectory()+model->spaceName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad";
    if(!isFile(tempFile)){
        arguments.clear();
        arguments.push_back(std::to_string(nSuns));
        std::string cntProgram="cnt";
        Process cnt(cntProgram,arguments);

        std::vector<std::string> arguments2;
        arguments2.push_back("-e");
        arguments2.push_back("MF:"+std::to_string(model->sunDivisions()));
        arguments2.push_back("-f");
        arguments2.push_back("reinsrc.cal");
        arguments2.push_back("-e");
        arguments2.push_back("Rbin=recno");
        arguments2.push_back("-o");
        arguments2.push_back("\"solar source sun 0 0 4 ${ Dx } ${ Dy } ${ Dz } 0.533\"");
        std::string rcalcProgram="rcalc";
        Process rcalc(rcalcProgram,arguments2);

        cnt.setStandardOutputProcess(&rcalc);
        rcalc.setStandardOutputFile(model->intermediateDataDirectory()+model->spaceName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");
        cnt.start();
        rcalc.start();

        if(!rcalc.wait()){
            STADIC_ERROR("The running of rcalc for the suns has failed.");
            //I want to display the errors here if the standard error has any errors to show.
            return false;
        }
    }

    //Create suns octree
    std::string sunsOct;
    files.clear();
    files.push_back(blackRad);
    files.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");
    files.push_back(bsdfRad);
    files.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+"sky_white1.rad");
    sunsOct=mainFileName+"_suns.oct";
    if(!createOctree(files,sunsOct)){
        return false;
    }

    //Compute V matrix
    //rcontrib
    arguments.clear();
    arguments.push_back("-f");
    arguments.push_back("klems_int.cal");
    arguments.push_back("-b");
    arguments.push_back("kbin("+std::to_string((-1)*normal[0])+","+std::to_string((-1)*normal[1])+","+std::to_string((-1)*normal[2])+"0,0,1)");
    arguments.push_back("-bn");
    arguments.push_back("Nkbins");
    arguments.push_back("-m");
    arguments.push_back(bsdfLayer);
    arguments.push_back("-I+");
    if (model->getParamSet("vmx")){
        for (auto param : model->getParamSet("vmx").get()){
            arguments.push_back("-"+param.first);
            arguments.push_back(param.second);
        }
    }else{
        STADIC_LOG(Severity::Fatal, "The vmx parameter set is not found for " + model->spaceName());
    }
    arguments.push_back(mainOct);
    std::string rcontribProgram="rcontrib";

    Process rcontrib(rcontribProgram,arguments);

    std::string vmx=mainFileName+"_3PH.vmx";
    rcontrib.setStandardOutputFile(vmx);
    rcontrib.setStandardInputFile(model->inputDirectory()+model->ptsFile()[0]);

    rcontrib.start();
    if (!rcontrib.wait()){
        STADIC_ERROR("The rcontrib run for the 3-phase vmx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute D Matrix
    //genklemsamp | rcontrib
    arguments.clear();
    arguments.push_back("genklemsamp.pl");
    arguments.push_back("-vd");
    arguments.push_back(std::to_string(normal[0]));
    arguments.push_back(std::to_string(normal[1]));
    arguments.push_back(std::to_string(normal[2]));
    arguments.push_back("-vo");
    arguments.push_back(thickness);
    arguments.push_back(bsdfRad);
    std::string perlProgram="perl";
    Process perl(perlProgram,arguments);

    std::vector<std::string> arguments2;
    arguments2.push_back("-c");
    arguments2.push_back("1000");
    arguments2.push_back("-e");
    arguments2.push_back("MF:"+std::to_string(model->skyDivisions()));
    arguments2.push_back("-f");
    arguments2.push_back("reinhart.cal");
    arguments2.push_back("-b");
    arguments2.push_back("rbin");
    arguments2.push_back("-bn");
    arguments2.push_back("Nrbins");
    arguments2.push_back("-m");
    arguments2.push_back("sky_glow");
    arguments2.push_back("-faa");
    arguments2.push_back(mainOct);

    Process rcontrib2(rcontribProgram,arguments2);
    perl.setStandardOutputProcess(&rcontrib2);
    std::string dmx=mainFileName+"_3PH.dmx";
    rcontrib2.setStandardOutputFile(dmx);

    perl.start();
    rcontrib2.start();
    if (!rcontrib2.wait()){
        STADIC_ERROR("The rcontrib run for the 3-phase dmx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute S Matrix
    //gendaymtx
    arguments.clear();
    arguments.push_back("MF:"+std::to_string(model->skyDivisions()));
    if (m_Model->buildingRotation() && m_Model->buildingRotation().get()!=0){
        arguments.push_back("-r");
        arguments.push_back(std::to_string((-1)*m_Model->buildingRotation().get()));
    }
    arguments.push_back("-c");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back(m_Model->weaDataFile().get());
    std::string gendaymtxProgram="gendaymtx";
    Process gendaymtx(gendaymtxProgram,arguments);
    std::string smx=mainFileName+"_3PH.smx";
    gendaymtx.setStandardOutputFile(smx);
    gendaymtx.start();
    if (!gendaymtx.wait()){
        STADIC_ERROR("The gendaymtx run for the smx has failed with the following errors.");        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute Dd matrix
    //genklemsamp | rcontrib
    arguments.clear();
    arguments.push_back("genklemsamp.pl");
    arguments.push_back("-vd");
    arguments.push_back(std::to_string(normal[0]));
    arguments.push_back(std::to_string(normal[1]));
    arguments.push_back(std::to_string(normal[2]));
    arguments.push_back("-vo");
    arguments.push_back(thickness);
    arguments.push_back(bsdfRad);
    Process perl2(perlProgram,arguments);

    arguments2.clear();
    arguments2.push_back("-c");
    arguments2.push_back("1000");
    arguments2.push_back("-e");
    arguments2.push_back("MF:"+std::to_string(model->skyDivisions()));
    arguments2.push_back("-f");
    arguments2.push_back("reinhart.cal");
    arguments2.push_back("-b");
    arguments2.push_back("rbin");
    arguments2.push_back("-bn");
    arguments2.push_back("Nrbins");
    arguments2.push_back("-m");
    arguments2.push_back("sky_glow");
    arguments2.push_back("-faa");
    arguments2.push_back(blackOct);
    Process rcontrib3(rcontribProgram,arguments2);
    perl2.setStandardOutputProcess(&rcontrib3);
    std::string dirDMX=mainFileName+"_3DIR.dmx";
    rcontrib.setStandardOutputFile(dirDMX);
    perl2.start();
    rcontrib3.start();
    if (!rcontrib3.wait()){
        STADIC_ERROR("The rcontrib run for the 3-phase direct dmx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute Vd matrix
    //rcontrib
    arguments.clear();
    arguments.push_back("-f");
    arguments.push_back("klems_int.cal");
    arguments.push_back("-b");
    arguments.push_back("kbin("+std::to_string((-1)*normal[0])+","+std::to_string((-1)*normal[1])+","+std::to_string((-1)*normal[2])+",0,0,1)");
    arguments.push_back("-bn");
    arguments.push_back("Nkbins");
    arguments.push_back("-m");
    arguments.push_back(bsdfLayer);
    arguments.push_back("-I+");
    if (model->getParamSet("vmx")){
        for (auto param : model->getParamSet("vmx").get()){
            arguments.push_back("-"+param.first);
            arguments.push_back(param.second);
        }
    }else{
        STADIC_LOG(Severity::Fatal, "The vmx parameter set is not found for " + model->spaceName());
    }
    arguments.push_back(blackOct);
    Process rcontrib4(rcontribProgram,arguments);
    std::string dirVMX=mainFileName+"_3Dir.vmx";
    rcontrib4.setStandardOutputFile(dirVMX);
    rcontrib4.setStandardInputFile(model->inputDirectory()+model->ptsFile()[0]);

    rcontrib4.start();
    if (!rcontrib4.wait()){
        STADIC_ERROR("The rcontrib run for the 3-phase direct vmx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute Sd Matrix
    //gendaymtx
    arguments.clear();
    arguments.push_back("MF:"+std::to_string(model->skyDivisions()));
    if (m_Model->buildingRotation() && m_Model->buildingRotation().get()!=0){
        arguments.push_back("-r");
        arguments.push_back(std::to_string((-1)*m_Model->buildingRotation().get()));
    }
    arguments.push_back("-d");
    arguments.push_back(m_Model->weaDataFile().get());
    Process gendaymtx2(gendaymtxProgram,arguments);
    std::string dirSMX=mainFileName+"_3DIR.smx";
    gendaymtx2.setStandardOutputFile(dirSMX);
    gendaymtx2.start();
    if (!gendaymtx2.wait()){
        STADIC_ERROR("The gendaymtx run for the direct smx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute Ssun Matrix
    //gendaymtx
    arguments.clear();
    arguments.push_back("MF:"+std::to_string(model->sunDivisions()));
    if (m_Model->buildingRotation() && m_Model->buildingRotation().get()!=0){
        arguments.push_back("-r");
        arguments.push_back(std::to_string((-1)*m_Model->buildingRotation().get()));
    }
    arguments.push_back("-5");
    arguments.push_back("-d");
    arguments.push_back(m_Model->weaDataFile().get());
    Process gendaymtx3(gendaymtxProgram,arguments);
    std::string dir5PHsmx=mainFileName+"_5PH.smx";
    gendaymtx3.setStandardOutputFile(dir5PHsmx);
    gendaymtx3.start();
    if (!gendaymtx3.wait()){
        STADIC_ERROR("The gendaymtx run for the direct 5 phase smx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute DCsun matrix
    //rcontrib
    arguments.clear();
    arguments.push_back("-I");
    if (model->getParamSet("dmx")){
        for (auto param : model->getParamSet("dmx").get()){
            arguments.push_back("-"+param.first);
            arguments.push_back(param.second);
        }
    }else{
        STADIC_LOG(Severity::Fatal, "The dmx parameter set is not found for " + model->spaceName());
    }
    arguments.push_back("-faa");
    arguments.push_back("-e");
    arguments.push_back("MF:"+std::to_string(model->sunDivisions()));
    arguments.push_back("-f");
    arguments.push_back("klems_int.cal");
    arguments.push_back("-b");
    arguments.push_back("rbin");
    arguments.push_back("-bn");
    arguments.push_back("Nrbins");
    arguments.push_back("-m");
    arguments.push_back("solar");
    arguments.push_back(sunsOct);
    std::string dirDSMX=mainFileName+"_5PH.dsmx";
    Process rcontrib5(rcontribProgram,arguments);
    rcontrib5.setStandardOutputFile(dirDSMX);
    rcontrib5.setStandardInputFile(model->inputDirectory()+model->ptsFile()[0]);

    rcontrib5.start();
    if (!rcontrib5.wait()){
        STADIC_ERROR("The rcontrib run for the 5-phase direct smx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //3Phase
    //dctimestep | rcollate
    arguments.clear();
    arguments.push_back("-n");
    arguments.push_back("8760");
    arguments.push_back(vmx);
    arguments.push_back(bsdfXML);
    arguments.push_back(dmx);
    arguments.push_back(smx);
    std::string dctimestepProgram="dctimestep";
    Process dctimestep(dctimestepProgram,arguments);

    arguments2.clear();
    arguments2.push_back("-h");
    arguments2.push_back("-fa");
    arguments2.push_back("-oc");
    arguments2.push_back("3");
    std::string rcollateProgram="rcollate";
    Process rcollate(rcollateProgram,arguments2);
    dctimestep.setStandardOutputProcess(&rcollate);

    std::string threePhaseCollated=mainFileName+"_3ph.dat";
    rcollate.setStandardOutputFile(threePhaseCollated);

    dctimestep.start();
    rcollate.start();

    if(!rcollate.wait()){
        STADIC_ERROR("The running of rcollate for the 3-phase has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //3Phase Direct
    //dctimestep | rcollate
    arguments.clear();
    arguments.push_back("-n");
    arguments.push_back("8760");
    arguments.push_back(dirVMX);
    arguments.push_back(bsdfXML);
    arguments.push_back(dirDMX);
    arguments.push_back(dirSMX);
    Process dctimestep2(dctimestepProgram,arguments);

    arguments2.clear();
    arguments2.push_back("-h");
    arguments2.push_back("-fa");
    arguments2.push_back("-oc");
    arguments2.push_back("3");
    Process rcollate2(rcollateProgram,arguments2);
    dctimestep2.setStandardOutputProcess(&rcollate2);

    std::string threePhaseDirectCollated=mainFileName+"_3Dir.dat";
    rcollate2.setStandardOutputFile(threePhaseDirectCollated);

    dctimestep2.start();
    rcollate2.start();

    if(!rcollate2.wait()){
        STADIC_ERROR("The running of rcollate for the 3-phase direct has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //5Phase
    //dctimestep | rcollate
    arguments.clear();
    arguments.push_back("-n");
    arguments.push_back("8760");
    arguments.push_back(dirDSMX);
    arguments.push_back(dir5PHsmx);
    Process dctimestep3(dctimestepProgram,arguments);

    arguments2.clear();
    arguments2.push_back("-h");
    arguments2.push_back("-fa");
    arguments2.push_back("-oc");
    arguments2.push_back("3");
    Process rcollate3(rcollateProgram,arguments2);
    dctimestep3.setStandardOutputProcess(&rcollate3);

    std::string fivePhaseCollated=mainFileName+"_5PH.dat";
    rcollate3.setStandardOutputFile(fivePhaseCollated);

    dctimestep3.start();
    rcollate3.start();

    if(!rcollate3.wait()){
        STADIC_ERROR("The running of rcollate for the 3-phase has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //Process final data into ill file
    //FivePhaseOut<<"rlam "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_3ph.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_3DIR.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_ds5ph.dat | rcalc -e \"r=$1-$4+$7;g=$2-$5+$8;b=$3-$6+$9\" -e \"ill=179*(.265*r +.670*g + .65*b)\" -e \"$1=floor(ill+.5)\" | rcollate -h -fa -or "<<(double(60)/time_step)*8760<<" -t > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<".ill"<<endl;
    arguments.clear();
    arguments.push_back(threePhaseCollated);
    arguments.push_back(threePhaseDirectCollated);
    arguments.push_back(fivePhaseCollated);
    std::string rlamProgram="rlam";
    Process rlam(rlamProgram,arguments);

    arguments2.clear();
    arguments2.push_back("-e");
    arguments2.push_back("r=$1-$4+7;g=$2-$5+$8;b=$3-$6+$9");
    arguments2.push_back("-e");
    arguments2.push_back("ill=179*(.265*r+.670*g+.65*b)");
    arguments2.push_back("-e");
    arguments2.push_back("$1=floor(ill+.5)");
    std::string rcalcProgram="rcalc";
    Process rcalc(rcalcProgram,arguments2);
    rlam.setStandardOutputProcess(&rcalc);

    std::vector<std::string> arguments3;
    arguments3.push_back("-h");
    arguments3.push_back("-fa");
    arguments3.push_back("-or");
    arguments3.push_back("8760");
    arguments3.push_back("-t");
    Process rcollate4(rcollateProgram,arguments3);
    rcalc.setStandardOutputProcess(&rcollate4);
    rcollate4.setStandardOutputFile(mainFileName+".ill");

    rlam.start();
    rcalc.start();
    rcollate4.start();
    if(!rcollate4.wait()){
        STADIC_ERROR("The running of rcollate for final illuminance has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

  return true;
}

bool Daylight::simStandard(int blindGroupNum, int setting, Control *model){
    bool writeCL=true;
    std::ofstream outCL;
    outCL.open(model->spaceDirectory()+"commandLine.bat");
    //Get number of suns
    int nSuns;
    if(model->sunDivisions()==1){
        nSuns=145;
    }else if (model->sunDivisions()==2){
        nSuns=577;
    }else if (model->sunDivisions()==3){
        nSuns=1297;
    }else if (model->sunDivisions()==4){
        nSuns=2305;
    }else if (model->sunDivisions()==5){
        nSuns=3601;
    }else if (model->sunDivisions()==6){
        nSuns=5185;
    }
    //Generate Weather file if it hasn't been generated already.
    if (!m_WeaFileName){
        if (writeCL){
            std::cout<<"***\nParsing the weather file.\n***"<<std::endl;
        }
        WeatherData tmpWeather;
        if (m_Model->weaDataFile()){
            if (!tmpWeather.parseWeather(m_Model->weaDataFile().get())){
                return false;
            }
            std::string tmpWeaFileName;
            tmpWeaFileName=model->spaceDirectory()+model->inputDirectory()+tmpWeather.place()+".wea";
            std::vector<std::string> placeArgs;
            placeArgs=trimmedSplit(tmpWeaFileName, ' ');
            tmpWeaFileName.clear();
            for (int i=0;i<placeArgs.size();i++){
                tmpWeaFileName=tmpWeaFileName+placeArgs.at(i);
            }
            m_WeaFileName=tmpWeaFileName;
            if (!tmpWeather.writeWea(m_WeaFileName.get())){
                STADIC_LOG(stadic::Severity::Error, "The creation of the .wea file failed.");
                return false;
            }
        }else{
            STADIC_LOG(stadic::Severity::Error, "The weather file needed for running the simulation does not exist.");
            return false;
        }
    }
    std::vector<std::string> arguments;
    std::string skyDC;
    std::string skySMX;
    std::string sunDC;
    std::string sunSMX;
    std::string stdOct;
    std::string directSunDC;
    std::string sunPatchSMX;
    std::string sensorSkyDC;
    std::string sensorSunDC;
    std::string sensorDirSunDC;
    if ((setting==-1 && model->windowGroups()[blindGroupNum].runBase())||(setting>=0 && model->windowGroups()[blindGroupNum].runSetting()[setting])){
        //rcontrib for sky
        arguments.push_back("-I");
        if (model->getParamSet("default")){
            std::unordered_map<std::string, std::string> tempMap=model->getParamSet("default").get();
            for (std::unordered_map<std::string, std::string>::iterator it=tempMap.begin(); it!=tempMap.end();++it){
                if (it->first!="sj" && it->first!="lw"){
                    arguments.push_back("-"+it->first);
                    arguments.push_back(it->second);
                }else if (it->first=="lw"){
                    arguments.push_back("-lw");
                    if (toDouble(it->second)>0.0000001){
                        STADIC_LOG(Severity::Info, "The lw argument has been changed from "+it->second+" to .0000001 .");
                        arguments.push_back("0.0000001");
                    }else{
                        arguments.push_back(it->second);
                    }
                }
            }
        }else{
            STADIC_LOG(Severity::Fatal, "The default parameter set is not found for " + model->spaceName());
        }
        arguments.push_back("-e");
        arguments.push_back("MF:"+std::to_string(model->skyDivisions()));
        arguments.push_back("-f");
        arguments.push_back("reinhart.cal");
        arguments.push_back("-b");
        arguments.push_back("rbin");
        arguments.push_back("-bn");
        arguments.push_back("Nrbins");
        arguments.push_back("-m");
        arguments.push_back("sky_glow");
        arguments.push_back("-faa");
        std::string rcontribProgram="rcontrib";



        if (setting==-1){
            //This is the base case
            stdOct=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.oct";
            arguments.push_back(stdOct);
            skyDC=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_1k.dc";
        }else{
            stdOct=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_std.oct";
            arguments.push_back(stdOct);
            skyDC=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_1k_std.dc";
        }
        Process rcontrib(rcontribProgram,arguments);
        rcontrib.setStandardOutputFile(skyDC);

        //Test whether the points file exists.  If it doesn't, test whether the necessary arguments to create one exist.
        if (stadic::exists(model->spaceDirectory()+model->inputDirectory()+model->ptsFile()[0])){
            rcontrib.setStandardInputFile(model->spaceDirectory()+model->inputDirectory()+model->ptsFile()[0]);
        }else{
            if (model->xSpacing()&&model->ySpacing()&& model->offset() && model->zOffset()&& (model->modifiers()||model->identifiers())){
                STADIC_LOG(stadic::Severity::Info, "The points file "+model->ptsFile()[0] + " does not exist.  The creation of a new points file will be attempted.");
                GridMaker ptsCreator(model->spaceDirectory()+model->geoDirectory()+ model->geoFile());
                ptsCreator.setSpaceX(toDouble(model->xSpacing().get()));
                ptsCreator.setSpaceY(toDouble(model->ySpacing().get()));
                ptsCreator.setOffset(toDouble(model->offset().get()));
                ptsCreator.setOffsetZ(toDouble(model->zOffset().get()));
                if (model->modifiers()){
                    ptsCreator.setLayerNames(model->modifiers().get());
                }else{
                    ptsCreator.setIdentifiers(model->identifiers().get());
                }
                if (ptsCreator.makeGrid()){
                    if (!ptsCreator.writePTS(model->spaceDirectory()+model->inputDirectory()+model->spaceName()+"_AutoGen.pts")){
                        STADIC_LOG(stadic::Severity::Error, "The writing of the points file has failed.");
                        return false;
                    }
                    std::vector<std::string> ptsFiles;
                    ptsFiles.push_back(model->spaceName()+"_AutoGen.pts");
                    model->setPTSFile(ptsFiles);
                }else{
                    STADIC_LOG(stadic::Severity::Error, "The creation of the points file has failed.");
                    return false;
                }
            }else{
                STADIC_LOG(stadic::Severity::Info, "The points file "+model->ptsFile()[0] + " does not exist.  And no arguments exist for one to be generated.");
                return false;
            }
            rcontrib.setStandardInputFile(model->spaceDirectory()+model->inputDirectory()+model->ptsFile()[0]);
            STADIC_LOG(stadic::Severity::Info, "A new points file has been successfully generated.");
        }
        if (writeCL){
            outCL<<rcontrib.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning rcontrib for the sky.\n"<<rcontrib.commandLine()<<"\n***"<<std::endl;
        }
        rcontrib.start();
        if (!rcontrib.wait()){
            STADIC_ERROR("The rcontrib run for the sky has failed with the following errors.");
            //I want to display the errors here if the standard error has any errors to show.
            STADIC_LOG(stadic::Severity::Info, "The command line entry is as follows:\n\t"+rcontrib.commandLine());
            return false;
        }


        std::string tempFile=model->intermediateDataDirectory()+model->spaceName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad";
        if(!isFile(tempFile)){
            arguments.clear();
            arguments.push_back(std::to_string(nSuns));
            std::string cntProgram="cnt";
            Process cnt(cntProgram,arguments);

            std::vector<std::string> arguments2;
            arguments2.push_back("-e");
            arguments2.push_back("MF:"+std::to_string(model->sunDivisions()));
            arguments2.push_back("-f");
            arguments2.push_back("reinsrc.cal");
            arguments2.push_back("-e");
            arguments2.push_back("Rbin=recno");
            arguments2.push_back("-o");
            arguments2.push_back("\"solar source sun 0 0 4 ${ Dx } ${ Dy } ${ Dz } 0.533\"");
            std::string rcalcProgram="rcalc";
            Process rcalc(rcalcProgram,arguments2);
            cnt.setStandardOutputProcess(&rcalc);
            rcalc.setStandardOutputFile(model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");
            if (writeCL){
                outCL<<"## Pipe the next two lines together."<<std::endl;
                outCL<<cnt.commandLine()<<std::endl;
                outCL<<rcalc.commandLine()<<std::endl<<std::endl;;
            }
            if (writeCL){
                std::cout<<"***\nRunning cnt and rcalc for the suns.\n"<<cnt.commandLine()<<"\n is piped to\n"<<rcalc.commandLine()<<"\n***"<<std::endl;
            }
            cnt.start();
            rcalc.start();

            if(!rcalc.wait()){
                STADIC_LOG(Severity::Error, "The running of rcalc for the suns has failed.");
                //I want to display the errors here if the standard error has any errors to show.
                PathName badFile(tempFile);
                if (!badFile.remove()){
                    STADIC_LOG(Severity::Error, "The deletion of the file "+tempFile+" has failed.  Please manually delete this file.");
                }
                return false;
            }
        }
    }
    //Create suns octree
    std::vector<std::string> octFiles;
    std::string sunsOct;
    if (setting==-1){
        octFiles.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad");
        //Added the next line because oconv produced a fatal error with undefined modifier "solar" without it.
        octFiles.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_suns.rad");
        octFiles.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");
        sunsOct=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_sun_base.oct";

    }else{
        octFiles.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_std.rad");
        //Added the next line because oconv produced a fatal error with undefined modifier "solar" without it.
        octFiles.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_suns.rad");
        octFiles.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");
        sunsOct=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_sun_set"+std::to_string(setting+1)+"_std.oct";

    }
    if (writeCL){
            std::cout<<"***\nCreating suns octree.\n***"<<std::endl;
    }
    if(!createOctree(octFiles,sunsOct)){
        return false;
    }
    if (writeCL){
        outCL<<"## Create the suns octree here"<<std::endl<<std::endl;;
    }
    if ((setting==-1 && model->windowGroups()[blindGroupNum].runBase())||(setting>=0 && model->windowGroups()[blindGroupNum].runSetting()[setting])){
        //rcontrib for sun (sky patch version)
        arguments.clear();
        arguments.push_back("-I");
        if (model->getParamSet("default")){
            std::unordered_map<std::string, std::string> tempMap=model->getParamSet("default").get();
            for (std::unordered_map<std::string, std::string>::iterator it=tempMap.begin(); it!=tempMap.end();++it){
                if (it->first!="sj" && it->first!="ab" && it->first!="lw"){
                    arguments.push_back("-"+it->first);
                    arguments.push_back(it->second);
                }
                else if (it->first=="lw"){
                    arguments.push_back("-lw");
                    if (toDouble(it->second)>0.0000001){
                        STADIC_LOG(Severity::Info, "The lw argument has been changed from "+it->second+" to .0000001 .");
                        arguments.push_back("0.0000001");
                    }else{
                        arguments.push_back(it->second);
                    }
                }
            }
            arguments.push_back("-ab");
            arguments.push_back("1");
        }else{
            STADIC_LOG(Severity::Fatal, "The default parameter set is not found for " + model->spaceName());
        }
        arguments.push_back("-e");
        arguments.push_back("MF:"+std::to_string(model->skyDivisions()));
        arguments.push_back("-f");
        arguments.push_back("reinhart.cal");
        arguments.push_back("-b");
        arguments.push_back("rbin");
        arguments.push_back("-bn");
        arguments.push_back("Nrbins");
        arguments.push_back("-m");
        arguments.push_back("sky_glow");
        arguments.push_back("-faa");
        arguments.push_back(stdOct);
        if (setting==-1){
            //This is the base case
            sunDC=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_kd.dc";
        }else{
            //This is for the settings
            sunDC=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_1d_std.dc";
        }
        std::string rcontribProgram="rcontrib";
        Process rcontrib2(rcontribProgram,arguments);
        rcontrib2.setStandardOutputFile(sunDC);
        rcontrib2.setStandardInputFile(model->spaceDirectory()+model->inputDirectory()+model->ptsFile()[0]);
        if (writeCL){
            outCL<<rcontrib2.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning rcontrib for the sun.\n"<<rcontrib2.commandLine()<<"\n***"<<std::endl;
        }
        rcontrib2.start();
        if (!rcontrib2.wait()){
            STADIC_ERROR("The sun rcontrib run failed with the following errors.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }

        //rcontrib for direct sun
        arguments.clear();
        arguments.push_back("-I");
        if (model->getParamSet("default")){
            std::unordered_map<std::string, std::string> tempMap=model->getParamSet("default").get();
            for (std::unordered_map<std::string, std::string>::iterator it=tempMap.begin(); it!=tempMap.end();++it){
                if (it->first!="sj" && it->first!="ab"){
                    arguments.push_back("-"+it->first);
                    arguments.push_back(it->second);
                }
            }
            arguments.push_back("-ab");
            arguments.push_back("0");
        }else{
            STADIC_LOG(Severity::Fatal, "The default parameter set is not found for " + model->spaceName());
        }
        arguments.push_back("-e");
        arguments.push_back("MF:"+std::to_string(model->sunDivisions()));
        arguments.push_back("-f");
        arguments.push_back("reinhart.cal");
        arguments.push_back("-b");
        arguments.push_back("rbin");
        arguments.push_back("-bn");
        arguments.push_back("Nrbins");
        arguments.push_back("-m");
        arguments.push_back("solar");
        arguments.push_back("-faa");
        arguments.push_back(sunsOct);
        if (setting==-1){
            //This is the base case
            directSunDC=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_d.dc";
        }else{
            //This is for the settings
            directSunDC=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_d_std.dc";
        }
        Process rcontrib3(rcontribProgram,arguments);
        rcontrib3.setStandardOutputFile(directSunDC);
        rcontrib3.setStandardInputFile(model->spaceDirectory()+model->inputDirectory()+model->ptsFile()[0]);
        if (writeCL){
            outCL<<rcontrib3.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning rcontrib for the sun (direct).\n"<<rcontrib3.commandLine()<<"\n***"<<std::endl;
        }
        rcontrib3.start();
        if (!rcontrib3.wait()){
            STADIC_ERROR("The direct sun rcontrib run failed with the following errors.");
            //I want to display the errors here if the standard error has any errors to show.
            return false;
        }
    }
    if ((setting==-1 && model->windowGroups()[blindGroupNum].shadeControl()->needsSensor())){
        //rsensor and rcontrib for shade sensors for sky contribution
        arguments.clear();
        arguments.push_back("-h");
        arguments.push_back("-rd");
        arguments.push_back("10000");
        //set view point (location of photosensor)
        arguments.push_back("-vp");
        arguments.push_back(toString(model->windowGroups()[blindGroupNum].shadeControl()->xLoc()));
        arguments.push_back(toString(model->windowGroups()[blindGroupNum].shadeControl()->yLoc()));
        arguments.push_back(toString(model->windowGroups()[blindGroupNum].shadeControl()->zLoc()));
        //set view direction
        arguments.push_back("-vd");
        arguments.push_back(toString(model->windowGroups()[blindGroupNum].shadeControl()->xDir()));
        arguments.push_back(toString(model->windowGroups()[blindGroupNum].shadeControl()->yDir()));
        arguments.push_back(toString(model->windowGroups()[blindGroupNum].shadeControl()->zDir()));
        //set up direction for sensor (obtained from spin)
        //************************
        //************************
        //The next three lines will have to be calculated
        arguments.push_back("-vu");
        arguments.push_back("1");
        arguments.push_back("0");
        arguments.push_back("0");
        if (model->windowGroups()[blindGroupNum].shadeControl()->sensorType()!="sensitivity_file"){
            Photosensor sensor;
            if (!sensor.setType(model->windowGroups()[blindGroupNum].shadeControl()->sensorType())){
                return false;
            }
            std::string sensorFileName=model->spaceDirectory()+model->inputDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_shade.sen";
            if (!sensor.writeSensorFile(sensorFileName)){
                return false;
            }
            arguments.push_back(sensorFileName);
        }else{
            arguments.push_back(model->windowGroups()[blindGroupNum].shadeControl()->sensorFile());
        }
        std::string rsensorProgram="rsensor";
        Process rsensor(rsensorProgram, arguments);
        std::vector<std::string> arguments2;


        arguments2.clear();
        arguments2.push_back("-c");
        arguments2.push_back("10000");
        arguments2.push_back("-e");
        arguments2.push_back("MF:"+std::to_string(model->skyDivisions()));
        arguments2.push_back("-f");
        arguments2.push_back("reinhart.cal");
        arguments2.push_back("-b");
        arguments2.push_back("rbin");
        arguments2.push_back("-bn");
        arguments2.push_back("Nrbins");
        arguments2.push_back("-m");
        arguments2.push_back("sky_glow");
        //arguments.push_back("-faa");
        if (model->getParamSet("default")){
            std::unordered_map<std::string, std::string> tempMap=model->getParamSet("default").get();
            for (std::unordered_map<std::string, std::string>::iterator it=tempMap.begin(); it!=tempMap.end();++it){
                if (it->first!="sj" && it->first!="lw"){
                    arguments2.push_back("-"+it->first);
                    arguments2.push_back(it->second);
                }else if (it->first=="lw"){
                    arguments2.push_back("-lw");
                    if (toDouble(it->second)>0.00002){
                        STADIC_LOG(Severity::Info, "The lw argument has been changed from "+it->second+" to .00002 .");
                        arguments2.push_back("0.00002");
                    }else{
                        arguments2.push_back(it->second);
                    }
                }
            }
        }else{
            STADIC_LOG(Severity::Fatal, "The default parameter set is not found for " + model->spaceName());
        }
        arguments2.push_back(stdOct);
        std::string rcontribProgram="rcontrib";
        Process rcontribSkySen(rcontribProgram, arguments2);
        rsensor.setStandardOutputProcess(&rcontribSkySen);
        sensorSkyDC=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_shade_k.dc";
        rcontribSkySen.setStandardOutputFile(sensorSkyDC);
        if (writeCL){
            std::cout<<"***\nRunning rsensor and rcontrib for the sky (sensor).\n"<<rsensor.commandLine()<<"\n is piped to\n"<<rcontribSkySen.commandLine()<<"\n***"<<std::endl;
        }
        rsensor.start();
        rcontribSkySen.start();

        if (!rcontribSkySen.wait()){
            STADIC_LOG(Severity::Error, "The running of rcontrib for the shade sensor has failed for window group "+model->windowGroups()[blindGroupNum].name()+" within "+model->spaceName()+".");
            return false;
        }

        //rsensor and rcontrib for shade sensors for sun contribution
        Process rsensor2(rsensorProgram, arguments);
        arguments2.clear();
        arguments2.push_back("-e");
        arguments2.push_back("MF:"+std::to_string(model->skyDivisions()));
        arguments2.push_back("-f");
        arguments2.push_back("reinhart.cal");
        arguments2.push_back("-b");
        arguments2.push_back("rbin");
        arguments2.push_back("-bn");
        arguments2.push_back("Nrbins");
        arguments2.push_back("-m");
        arguments2.push_back("sky_glow");
        //arguments.push_back("-faa");
        if (model->getParamSet("default")){
            std::unordered_map<std::string, std::string> tempMap=model->getParamSet("default").get();
            for (std::unordered_map<std::string, std::string>::iterator it=tempMap.begin(); it!=tempMap.end();++it){
                if (it->first!="sj" && it->first!="lw" && it->first!="ab"){
                    arguments2.push_back("-"+it->first);
                    arguments2.push_back(it->second);
                }else if (it->first=="lw"){
                    arguments2.push_back("-lw");
                    arguments2.push_back("0.00005");
                    STADIC_LOG(Severity::Info, "The lw argument has been changed from "+it->second+" to .00002 .");
                }
                arguments2.push_back("-ab");
                arguments2.push_back("1");
            }
        }else{
            STADIC_LOG(Severity::Fatal, "The default parameter set is not found for " + model->spaceName());
        }
        arguments2.push_back(stdOct);
        Process rcontribSunSen(rcontribProgram, arguments2);
        rsensor2.setStandardOutputProcess(&rcontribSunSen);
        sensorSunDC=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_shade_kd.dc";
        rcontribSunSen.setStandardOutputFile(sensorSunDC);

        if (writeCL){
            std::cout<<"***\nRunning rsensor and rcontrib for the sun (sensor).\n"<<rsensor.commandLine()<<"\n is piped to\n"<<rcontribSunSen.commandLine()<<"\n***"<<std::endl;
        }
        rsensor2.start();
        rcontribSunSen.start();

        if (!rcontribSunSen.wait()){
            STADIC_LOG(Severity::Error, "The running of rcontrib for the shade sensor has failed for window group "+model->windowGroups()[blindGroupNum].name()+" within "+model->spaceName()+".");
            return false;
        }
        arguments2.clear();

        //rcontrib for direct sun for shades
        Process rsensor3 (rsensorProgram, arguments);
        arguments2.clear();
        arguments2.push_back("-I");
        if (model->getParamSet("default")){
            std::unordered_map<std::string, std::string> tempMap=model->getParamSet("default").get();
            for (std::unordered_map<std::string, std::string>::iterator it=tempMap.begin(); it!=tempMap.end();++it){
                if (it->first!="sj" && it->first!="ab"){
                    arguments2.push_back("-"+it->first);
                    arguments2.push_back(it->second);
                }
            }
            arguments2.push_back("-ab");
            arguments2.push_back("0");
        }else{
            STADIC_LOG(Severity::Fatal, "The default parameter set is not found for " + model->spaceName());
        }
        arguments2.push_back("-e");
        arguments2.push_back("MF:"+std::to_string(model->sunDivisions()));
        arguments2.push_back("-f");
        arguments2.push_back("reinhart.cal");
        arguments2.push_back("-b");
        arguments2.push_back("rbin");
        arguments2.push_back("-bn");
        arguments2.push_back("Nrbins");
        arguments2.push_back("-m");
        arguments2.push_back("solar");
        arguments2.push_back("-faa");
        arguments2.push_back(sunsOct);
        Process rcontribDirSunSen(rcontribProgram,arguments2);
        rsensor3.setStandardOutputProcess(&rcontribDirSunSen);
        sensorDirSunDC=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_shade_d.dc";
        rcontribDirSunSen.setStandardOutputFile(sensorDirSunDC);
        rcontribDirSunSen.setStandardInputFile(model->spaceDirectory()+model->inputDirectory()+model->ptsFile()[0]);
        if (writeCL){
            outCL<<rcontribDirSunSen.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning rcontrib for the sun (direct).\n"<<rcontribDirSunSen.commandLine()<<"\n***"<<std::endl;
        }
        rsensor3.start();
        rcontribDirSunSen.start();
        if (!rcontribDirSunSen.wait()){
            STADIC_LOG(Severity::Error, "The running of rcontrib for the shade sensor has failed for window group "+model->windowGroups()[blindGroupNum].name()+" within "+model->spaceName()+".");
            return false;
        }


    }


    if ((setting==-1 && model->windowGroups()[blindGroupNum].runBase())||(setting>=0 && model->windowGroups()[blindGroupNum].runSetting()[setting]) || (model->windowGroups()[blindGroupNum].shadeControl()->needsSensor()&&setting==-1)){
        //gendaymtx for sun
        arguments.clear();
        arguments.push_back("-m");
        arguments.push_back(std::to_string( model->sunDivisions()));
        if (m_Model->buildingRotation() && m_Model->buildingRotation().get()!=0){
            arguments.push_back("-r");
            arguments.push_back(std::to_string((-1)*m_Model->buildingRotation().get()));
        }
        arguments.push_back("-d");
        arguments.push_back("-5");
        arguments.push_back("0.533");
        /*eliminating the following line per Dr. Mistrick's request
        arguments.push_back("-ho");
        */
        arguments.push_back(m_WeaFileName.get());
        arguments.push_back(".");
        std::string gendaymtxProgram="gendaymtx";
        Process gendaymtx(gendaymtxProgram,arguments);

        if (setting==-1){
            sunSMX=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_d.smx";
        }else{
            sunSMX=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_d_std.smx";
        }
        gendaymtx.setStandardOutputFile(sunSMX);
        if (writeCL){
            outCL<<gendaymtx.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning gendaymtx for the sun.\n"<<gendaymtx.commandLine()<<"\n***"<<std::endl;
        }
        gendaymtx.start();
        if (!gendaymtx.wait()){
            STADIC_ERROR("The creation of the suns has failed. The command line is displayed below:\n\t"+gendaymtx.commandLine());
            return false;
        }


        //gendaymtx for sky
        arguments.clear();
        arguments.push_back("-m");
        arguments.push_back(std::to_string( model->skyDivisions()));
        if (m_Model->buildingRotation() && m_Model->buildingRotation().get()!=0){
            arguments.push_back("-r");
            arguments.push_back(std::to_string((-1)*m_Model->buildingRotation().get()));
        }
        arguments.push_back("-c");
        arguments.push_back("1");
        arguments.push_back("1");
        arguments.push_back("1");
        /*eliminating the following line per Dr. Mistrick's request
        arguments.push_back("-ho");
        */
        arguments.push_back(m_WeaFileName.get());
        arguments.push_back(".");
        Process gendaymtx2(gendaymtxProgram,arguments);

        if (setting==-1){
            skySMX=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_k.smx";
        }else{
            skySMX=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_k_std.smx";
        }
        gendaymtx2.setStandardOutputFile(skySMX);
        if (writeCL){
            outCL<<gendaymtx2.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning gendaymtx sky.\n"<<gendaymtx2.commandLine()<<"\n***"<<std::endl;
        }
        gendaymtx2.start();
        if (!gendaymtx2.wait()){
            STADIC_ERROR("The creation of the sky has failed with the following errors.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }

        //gendaymtx for sun in patches
        arguments.clear();
        arguments.push_back("-m");
        arguments.push_back(std::to_string( model->skyDivisions()));
        if (m_Model->buildingRotation() && m_Model->buildingRotation().get()!=0){
            arguments.push_back("-r");
            arguments.push_back(std::to_string((-1)*m_Model->buildingRotation().get()));
        }
        arguments.push_back("-d");
        /*eliminating the following line per Dr. Mistrick's request
        arguments.push_back("-ho");
        */
        arguments.push_back(m_WeaFileName.get());
        arguments.push_back(".");
        Process gendaymtx3(gendaymtxProgram,arguments);

        if (setting==-1){
            sunPatchSMX=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_kd.smx";
        }else{
            sunPatchSMX=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_kd_std.smx";
        }
        gendaymtx3.setStandardOutputFile(sunPatchSMX);
        if (writeCL){
            outCL<<gendaymtx3.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning gendaymtx for the sun patches.\n"<<gendaymtx3.commandLine()<<"\n***"<<std::endl;
        }
        gendaymtx3.start();
        if (!gendaymtx3.wait()){
            STADIC_ERROR("The creation of the sun patches has failed.  The command line is as follows:\n\t"+gendaymtx3.commandLine());
            return false;
        }
    }

    if ((setting==-1 && model->windowGroups()[blindGroupNum].shadeControl()->needsSensor())){
        //dctimestep | rcollate for the sensor for the sky
        arguments.clear();
        /* eliminating this per Dr. Mistrick's request
        arguments.push_back("-n");
        arguments.push_back("8760");
        */
        arguments.push_back(sensorSkyDC);
        arguments.push_back(skySMX);
        std::string dctimestepProgram="dctimestep";
        Process dctimestep(dctimestepProgram, arguments);

        std::vector<std::string> arguments2;
        arguments2.push_back("-ho");
        arguments2.push_back("-oc");
        arguments2.push_back("1");
        std::string rcollateProgram="rcollate";
        Process rcollate(rcollateProgram,arguments2);
        dctimestep.setStandardOutputProcess(&rcollate);

        std::string sensorSkyCollated=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_shade_sky.txt";
        rcollate.setStandardOutputFile(sensorSkyCollated);
        if (writeCL){
            std::cout<<"***\nRunning dctimestep and rcollate for the sky (sensor).\n"<<dctimestep.commandLine()<<"\n is piped to\n"<<rcollate.commandLine()<<"\n***"<<std::endl;
        }
        dctimestep.start();
        rcollate.start();
        if (!rcollate.wait()){
            STADIC_LOG(Severity::Error, "The running of rcollate for the sensor sky contribution has failed.");
            return false;
        }

        //dctimestep | rcollate for the sensor for the sun
        //Added this line from an email that didn't exist before
        arguments[0]=sensorDirSunDC;
        arguments[1]=sunSMX;
        Process dctimestep2(dctimestepProgram,arguments);
        std::string sensorSunCollated=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_shade_sun.txt";
        Process rcollate2(rcollateProgram,arguments2);
        dctimestep2.setStandardOutputProcess(&rcollate2);
        rcollate2.setStandardOutputFile(sensorSunCollated);
        if (writeCL){
            std::cout<<"***\nRunning dctimestep and rcollate for the sun (sensor).\n"<<dctimestep2.commandLine()<<"\n is piped to\n"<<rcollate2.commandLine()<<"\n***"<<std::endl;
        }
        dctimestep2.start();
        rcollate2.start();

        if(!rcollate2.wait()){
            STADIC_ERROR("The running of rcollate for the sensor sun contribution has failed.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }


        //dctimestep | rcollate for the sensor for the sun patch
        //Added this line from an email that didn't exist before
        arguments[0]=sensorSunDC;
        arguments[1]=sunPatchSMX;
        Process dctimestep3(dctimestepProgram,arguments);
        std::string sensorSunPatchCollated=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_shade_sun_patch.txt";
        Process rcollate3(rcollateProgram,arguments2);
        dctimestep3.setStandardOutputProcess(&rcollate3);
        rcollate3.setStandardOutputFile(sensorSunPatchCollated);
        if (writeCL){
            std::cout<<"***\nRunning dctimestep and rcollate for the sun patch (sensor).\n"<<dctimestep3.commandLine()<<"\n is piped to\n"<<rcollate3.commandLine()<<"\n***"<<std::endl;
        }
        dctimestep3.start();
        rcollate3.start();

        if(!rcollate3.wait()){
            STADIC_ERROR("The running of rcollate for the sensor sun patch contribution has failed.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }


        //rlam | rcalc bringing it all together for the sensors
        arguments.clear();
        arguments.push_back(sensorSkyCollated);
        arguments.push_back(sensorSunPatchCollated);
        arguments.push_back(sensorSunCollated);

        std::string rlamProgram="rlam";
        Process rlam(rlamProgram,arguments);

        arguments2.clear();
        arguments2.push_back("-e");
        arguments2.push_back("r=$1-$4+$7;g=$2-$5+$8;b=$3-$6+$9");
        arguments2.push_back("-e");
        arguments2.push_back("ill=179*(.265*r+.670*g+.065*b)");
        arguments2.push_back("-e");
        arguments2.push_back("$1=floor(ill+.5)");
        std::string rcalcProgram="rcalc";
        Process rcalc(rcalcProgram,arguments2);
        rlam.setStandardOutputProcess(&rcalc);

        std::string finalIll=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+ model->windowGroups()[blindGroupNum].name()+"_shade_sig.tmp";
        rcalc.setStandardOutputFile(finalIll);
        if (writeCL){
            std::cout<<"***\nRunning rlam and rcalc (sensor).\n"<<rlam.commandLine()<<"\n is piped to\n"<<rcalc.commandLine()<<"\n***"<<std::endl;
        }
        rlam.start();
        rcalc.start();

        if(!rcalc.wait()){
            STADIC_LOG(Severity::Error, "The running of rcalc for the shade signal file for "+model->spaceName()+" has failed.");
            return false;
        }
    }

    if ((setting==-1 && model->windowGroups()[blindGroupNum].runBase())||(setting>=0 && model->windowGroups()[blindGroupNum].runSetting()[setting])){
        //dctimestep | rcollate for the sky
        arguments.clear();
        /* eliminating this per Dr. Mistrick's request
        arguments.push_back("-n");
        arguments.push_back("8760");
        */
        //Added this line from an email that didn't exist before
        arguments.push_back(skyDC);
        arguments.push_back(skySMX);
        std::string dctimestepProgram="dctimestep";
        Process dctimestep(dctimestepProgram,arguments);

        std::vector<std::string> arguments2;
        arguments2.clear();
        arguments2.push_back("-ho");
        arguments2.push_back("-oc");
        arguments2.push_back("1");
        std::string rcollateProgram="rcollate";
        Process rcollate(rcollateProgram,arguments2);
        dctimestep.setStandardOutputProcess(&rcollate);

        std::string skyCollated;
        if (setting==-1){
            skyCollated=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_sky.txt";
        }else{
            skyCollated=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_sky_std.txt";
        }
        rcollate.setStandardOutputFile(skyCollated);
        if (writeCL){
            outCL<<"## Pipe the next two lines together"<<std::endl;
            outCL<<dctimestep.commandLine()<<std::endl;
            outCL<<rcollate.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning dctimestep and rcollate for the sky.\n"<<dctimestep.commandLine()<<"\n is piped to\n"<<rcollate.commandLine()<<"\n***"<<std::endl;
        }
        dctimestep.start();
        rcollate.start();

        if(!rcollate.wait()){
            STADIC_ERROR("The running of rcollate for the sky has failed.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }

        //dctimestep | rcollate for the sun
        //Added this line from an email that didn't exist before
        arguments[0]=directSunDC;
        STADIC_LOG(Severity::Info, "sunSMX file = "+sunSMX);
        arguments[1]=sunSMX;
        Process dctimestep2(dctimestepProgram,arguments);
        std::string sunCollated;
        if (setting==-1){
            sunCollated=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_sun.txt";
        }else{
            sunCollated=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_sun_std.txt";
        }
        Process rcollate2(rcollateProgram,arguments2);
        dctimestep2.setStandardOutputProcess(&rcollate2);
        rcollate2.setStandardOutputFile(sunCollated);
        if (writeCL){
            outCL<<"## Pipe the next two lines together"<<std::endl;
            outCL<<dctimestep2.commandLine()<<std::endl;
            outCL<<rcollate2.commandLine()<<std::endl<<std::endl;;
        }
        STADIC_LOG(Severity::Info, dctimestep2.commandLine());
        STADIC_LOG(Severity::Info, rcollate2.commandLine());
        if (writeCL){
            std::cout<<"***\nRunning dctimestep and rcollate for the sun.\n"<<dctimestep2.commandLine()<<"\n is piped to\n"<<rcollate2.commandLine()<<"\n***"<<std::endl;
        }
        dctimestep2.start();
        rcollate2.start();

        if(!rcollate2.wait()){
            STADIC_ERROR("The running of rcollate for the sun has failed.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }

        //dctimestep | rcollate for the sun patch
        //Added this line from an email that didn't exist before
        arguments[0]=sunDC;
        arguments[1]=sunPatchSMX;
        Process dctimestep3(dctimestepProgram,arguments);
        std::string sunPatchCollated;
        if (setting==-1){
            sunPatchCollated=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_sunPatch.txt";
        }else{
            sunPatchCollated=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_sunPatch_std.txt";
        }
        Process rcollate3(rcollateProgram,arguments2);
        dctimestep3.setStandardOutputProcess(&rcollate3);
        rcollate3.setStandardOutputFile(sunPatchCollated);
        if (writeCL){
            outCL<<"## Pipe the next two lines together"<<std::endl;
            outCL<<dctimestep3.commandLine()<<std::endl;
            outCL<<rcollate3.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning dctimestep and rcollate for the sun patches.\n"<<dctimestep3.commandLine()<<"\n is piped to\n"<<rcollate3.commandLine()<<"\n***"<<std::endl;
        }
        dctimestep3.start();
        rcollate3.start();

        if(!rcollate3.wait()){
            STADIC_ERROR("The running of rcollate for the sun patches has failed.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }

        //rlam | rcalc bringing it all together
        arguments.clear();
        arguments.push_back(skyCollated);
        arguments.push_back(sunPatchCollated);
        arguments.push_back(sunCollated);

        std::string rlamProgram="rlam";
        Process rlam(rlamProgram,arguments);

        arguments2.clear();
        arguments2.push_back("-e");
        arguments2.push_back("r=$1-$4+$7;g=$2-$5+$8;b=$3-$6+$9");
        arguments2.push_back("-e");
        arguments2.push_back("ill=179*(.265*r+.670*g+.065*b)");
        arguments2.push_back("-e");
        arguments2.push_back("$1=floor(ill+.5)");
        std::string rcalcProgram="rcalc";
        Process rcalc(rcalcProgram,arguments2);
        rlam.setStandardOutputProcess(&rcalc);

        std::string finalIll;
        if (setting==-1){
            finalIll=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_ill.tmp";
        }else{
            finalIll=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_ill_std.tmp";
        }

        rcalc.setStandardOutputFile(finalIll);
        if (writeCL){
            outCL<<"## Pipe the next two lines together"<<std::endl;
            outCL<<rlam.commandLine()<<std::endl;
            outCL<<rcalc.commandLine()<<std::endl<<std::endl;;
        }
        if (writeCL){
            std::cout<<"***\nRunning rlam and rcalc.\n"<<rlam.commandLine()<<"\n is piped to\n"<<rcalc.commandLine()<<"\n***"<<std::endl;
        }
        rlam.start();
        rcalc.start();

        if(!rcalc.wait()){
            return false;
        }

        //rcalc for the direct component illuminance file by itself
        arguments.clear();
        arguments.push_back("-e");
        arguments.push_back("ill=179*(.265*$1+.670*$2+.065*$3)");
        arguments.push_back("-e");
        arguments.push_back("$1=floor(ill+.5)");
        Process rcalc2(rcalcProgram, arguments);
        rcalc2.setStandardInputFile(sunCollated);
        std::string directIll;
        if (setting==-1){
            directIll=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_direct_ill.tmp";
        }else{
            directIll=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_direct_ill_std.tmp";
        }
        rcalc2.setStandardOutputFile(directIll);
        if (writeCL){
            outCL<<rcalc.commandLine()<<std::endl<<std::endl;
        }
        if (writeCL){
            std::cout<<"***\nRunning rcalc fr the direct component.\n"<<rcalc2.commandLine()<<"\n***"<<std::endl;
        }
        rcalc2.start();

        if (!rcalc2.wait()){
            return false;
        }
    }
    outCL.close();
    return true;
}

bool Daylight::simCase1(int blindGroupNum, Control *model){
    // Passing an integer blind group number is very, very dangerous
    //Simulation Case 1 will be for window groups that do not contain BSDFs
    //First simulate the base condition
    // This is not making a copy *and* it is a memory leak.
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());    //This used to be (m_RadFiles[i],this), but the program failed to build
    baseRad->addRad(model->spaceDirectory()+model->geoDirectory()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);
    //Test for primitive continuity once that is working
    /*
    if(!baseRad->isConsistent()){
        STADIC_LOG(stadic::Severity::Error, "The base rad file for window group "+toString(blindGroupNum)+" is not continuous through the primitive tree.");
        return false;
    }
    */
    std::vector<std::string> files;
    files.push_back(wgBaseFile);
    files.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+"sky_white1.rad");
    std::string outFileName;
    outFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.oct";
    if (!createOctree(files, outFileName)){
        return false;
    }
    //Call Standard Radiance run
    if (!simStandard(blindGroupNum,-1,model)){
        return false;
    }

    //Loop through the shade settings
    if (model->windowGroups()[blindGroupNum].shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum].shadeSettingGeometry().size();i++){
            // Memory leak
            RadFileData *wgRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            wgRad->addRad(model->spaceDirectory()+model->geoDirectory()+model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            std::string wgSetFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.rad";
            wgRad->writeRadFile(wgSetFile);
            files.clear();
            files.push_back(wgSetFile);
            files.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+"sky_white1.rad");
            outFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.oct";
            if (!createOctree(files, outFileName)){
                return false;
            }
            //call Standard Radiance run
            if (!simStandard(blindGroupNum,i,model)){
                return false;
            }
        }
    }
    return true;
}

bool Daylight::simCase2(int blindGroupNum, Control *model){
    //Simulation case 2 will be for window groups that contain BSDFs, but not in the base case
    //First simulate the base condition
    // Memory leak
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());    //This used to be (m_RadFiles[i],this), but the program failed to build
    baseRad->addRad(model->spaceDirectory()+model->geoDirectory()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);
    std::vector<std::string> files;
    files.push_back(wgBaseFile);
    files.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+"sky_white1.rad");
    std::string outFileName;
    outFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.oct";
    if (!createOctree(files, outFileName)){
        return false;
    }

    //Call Standard Radiance run
    if (!simStandard(blindGroupNum,-1,model)){
        return false;
    }

    //Loop through the shade settings
    if (model->windowGroups()[blindGroupNum].shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum].shadeSettingGeometry().size();i++){
            // Memory leak
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            settingRad->addRad(model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size()>0){
                //Create a file of the glazing layers with all BSDFs blacked out and simulate it
                // Memory leak
                RadFileData *settingStdRad=new RadFileData(settingRad->primitives());
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
//                    if (!settingStdRad->blackOutLayer(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j])){
//                        return false;
//                    }
                }
                std::string wgSettingFileStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.rad";
                files[0]=wgSettingFileStd;
                outFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.oct";
                if (!createOctree(files, outFileName)){
                    return false;
                }

                //Call Standard Radiance run
                if (!simStandard(blindGroupNum,i,model)){
                    return false;
                }

                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
                    std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]);
                    std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive> > splitGeo = settingRad->split(layers);
                    if (splitGeo.first.size() == 0 || splitGeo.second.size() == 0){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                    std::string wgSettingFileBSDF=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+".rad";
                    RadFileData first(splitGeo.first);
                    first.writeRadFile(wgSettingFileBSDF);
                    std::vector<double> normal=first.surfaceNormal(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]);
                    std::string thickness;
                    std::string bsdfXML;
                    for (int k=0;k<first.primitives().size();k++){
                        if (first.primitives()[k]->name()==model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]){
                            thickness=first.primitives()[k]->getArg1(0);
                            bsdfXML=first.primitives()[k]->getArg1(1);
                        }
                    }
                    std::string wgSettingFileBSDFStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+"_std.rad";
                    RadFileData second(splitGeo.second);
                    second.writeRadFile(wgSettingFileBSDFStd);
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j],model)){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                }
            }else{
                // Memory leak
                RadFileData *wgRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
                wgRad->addRad(model->spaceDirectory()+model->geoDirectory()+model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
                std::string wgSetFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+".rad";
                wgRad->writeRadFile(wgSetFile);
                files.clear();
                files.push_back(wgSetFile);
                files.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+"sky_white1.rad");
                outFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+".oct";
                if (!createOctree(files, outFileName)){
                    return false;
                }

                //call Standard Radiance run
                if (!simStandard(blindGroupNum,i,model)){
                    return false;
                }
            }
            delete settingRad;
        }
    }
    return true;
}

bool Daylight::simCase3(int blindGroupNum, Control *model){
    //	Simulation case 3 will be for window groups that contain BSDFs even in the base case, but the glazing layers are not BSDFs
    //First simulate the base condition
    //Standard radiance run with all bsdfs blacked out
    // Memory leak
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
    baseRad->addRad(model->spaceDirectory()+model->geoDirectory()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);

    // Memory leak
    RadFileData *baseStdRad=new RadFileData(baseRad->primitives());
    for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfBaseLayers().size();j++){
//        if (!baseStdRad->blackOutLayer(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j])){
//            return false;
//        }
    }
    //Create base standard octree
    std::string wgBaseFileStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_std.rad";
    std::vector<std::string> files;
    files.push_back(wgBaseFileStd);
    files.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+"sky_white1.rad");
    std::string outFileName;
    outFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_std.oct";
    if (!createOctree(files, outFileName)){
        return false;
    }
    //Call Standard Radiance run
    if (!simStandard(blindGroupNum,-1,model)){
        return false;
    }
    //BSDF run for each of the BSDFs
    if (model->windowGroups()[blindGroupNum].bsdfBaseLayers().size()>0){
        //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
        for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfBaseLayers().size();j++){
            std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
            layers.push_back(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]);
            std::pair<shared_vector<RadPrimitive>,shared_vector<RadPrimitive> > splitGeo=baseRad->split(layers);
            if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                STADIC_ERROR("The program quit...");
                return false;
            }
            std::string wgBaseFileBSDF=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+".rad";
            RadFileData first(splitGeo.first);
            first.writeRadFile(wgBaseFileBSDF);
            std::vector<double> normal=first.surfaceNormal(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]);
            std::string thickness;
            std::string bsdfXML;
            for (int k=0;k<first.primitives().size();k++){
                if (first.primitives()[k]->name()==model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]){
                    thickness=first.primitives()[k]->getArg1(0);
                    bsdfXML=first.primitives()[k]->getArg1(1);
                }
            }
            std::string wgBaseFileBSDFStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+"_std.rad";
            RadFileData second(splitGeo.second);
            second.writeRadFile(wgBaseFileBSDFStd);
            if (!simBSDF(blindGroupNum,-1,j,wgBaseFileBSDF,wgBaseFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j],model)){
                STADIC_ERROR("The program quit...");
                return false;
            }
        }
    }

    //Loop through the shade settings
    if (model->windowGroups()[blindGroupNum].shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum].shadeSettingGeometry().size();i++){
            // Memory leak
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            settingRad->addRad(model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size()>0){
                //Create a file of the glazing layers with all BSDFs blacked out and simulate it
                // Memory leak
                RadFileData *settingStdRad=new RadFileData(settingRad->primitives());
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
//                    if (!settingStdRad->blackOutLayer(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j])){
//                        return false;
//                    }
                }
                std::string wgSettingFileStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.rad";
                files[0]=wgSettingFileStd;
                outFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.oct";
                if (!createOctree(files, outFileName)){
                    return false;
                }

                //Call Standard Radiance run
                if (!simStandard(blindGroupNum,i,model)){
                    return false;
                }

                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
                    std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]);
                    std::pair<shared_vector<RadPrimitive>,shared_vector<RadPrimitive> > splitGeo=settingRad->split(layers);
                    if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                    std::string wgSettingFileBSDF=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+".rad";
                    RadFileData first(splitGeo.first);
                    first.writeRadFile(wgSettingFileBSDF);
                    std::vector<double> normal=first.surfaceNormal(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]);
                    std::string thickness;
                    std::string bsdfXML;
                    for (int k=0;k<first.primitives().size();k++){
                        if (first.primitives()[k]->name()==model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]){
                            thickness=first.primitives()[k]->getArg1(0);
                            bsdfXML=first.primitives()[k]->getArg1(1);
                        }
                    }
                    std::string wgSettingFileBSDFStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+"_std.rad";
                    RadFileData second(splitGeo.second);
                    second.writeRadFile(wgSettingFileBSDFStd);
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j],model)){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                }
            }else{
                // Memory leak
                RadFileData *wgRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
                wgRad->addRad(model->spaceDirectory()+model->geoDirectory()+model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
                std::string wgSetFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+".rad";
                wgRad->writeRadFile(wgSetFile);
                files.clear();
                files.push_back(wgSetFile);
                files.push_back(model->spaceDirectory()+model->intermediateDataDirectory()+"sky_white1.rad");
                outFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+".oct";
                if (!createOctree(files, outFileName)){
                    return false;
                }

                //call Standard Radiance run
                if (!simStandard(blindGroupNum,i,model)){
                    return false;
                }
            }
            delete settingRad;
        }
    }
    return true;
}

bool Daylight::simCase4(int blindGroupNum, Control *model){
    //	Simulation case 4 will be for window groups that have shade materials in addition to the glazing layer which is a BSDF
    // Memory leak
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
    baseRad->addRad(model->spaceDirectory()+model->geoDirectory()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);

    //BSDF run for each of the BSDFs
    if (model->windowGroups()[blindGroupNum].bsdfBaseLayers().size()>0){
        //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
        for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfBaseLayers().size();j++){
            std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
            if (std::find(layers.begin(),layers.end(),model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j])==layers.end()){
                layers.push_back(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]);
            }
            std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive> > splitGeo=baseRad->split(layers);
            if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                STADIC_ERROR("The program quit...");
                return false;
            }
            std::string wgBaseFileBSDF=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+".rad";
            RadFileData first(splitGeo.first);
            first.writeRadFile(wgBaseFileBSDF);
            std::vector<double> normal=first.surfaceNormal(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]);
            std::string thickness;
            std::string bsdfXML;
            for (int k=0;k<first.primitives().size();k++){
                if (first.primitives()[k]->name()==model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]){
                    thickness=first.primitives()[k]->getArg1(0);
                    bsdfXML=first.primitives()[k]->getArg1(1);
                }
            }
            std::string wgBaseFileBSDFStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+"_std.rad";
            RadFileData second(splitGeo.second);
            second.writeRadFile(wgBaseFileBSDFStd);
            if (!simBSDF(blindGroupNum,-1,j,wgBaseFileBSDF,wgBaseFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j],model)){
                STADIC_ERROR("The program quit...");
                return false;
            }
        }
    }

    //Loop through the shade settings
    if (model->windowGroups()[blindGroupNum].shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum].shadeSettingGeometry().size();i++){
            // Memory leak
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            settingRad->addRad(model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size()>0){
                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
                    std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]);
                    std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive> > splitGeo=settingRad->split(layers);
                    if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                    std::string wgSettingFileBSDF=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+".rad";
                    RadFileData first(splitGeo.first);
                    first.writeRadFile(wgSettingFileBSDF);
                    std::vector<double> normal=first.surfaceNormal(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]);
                    std::string thickness;
                    std::string bsdfXML;
                    for (int k=0;k<first.primitives().size();k++){
                        if (first.primitives()[k]->name()==model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]){
                            thickness=first.primitives()[k]->getArg1(0);
                            bsdfXML=first.primitives()[k]->getArg1(1);
                        }
                    }
                    std::string wgSettingFileBSDFStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+"_std.rad";
                    RadFileData second(splitGeo.second);
                    second.writeRadFile(wgSettingFileBSDFStd);
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j],model)){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                }
            }else{
                STADIC_ERROR("Blind Group "+std::to_string(blindGroupNum)+" setting "+std::to_string(i)+ " does not contain a bsdf layer.");
                return false;
            }
            delete settingRad;
        }
    }
    return true;
}

bool Daylight::simCase5(int blindGroupNum, Control *model){
    //	Simulation case 5 will be for window groups that have added geometry, but it is a proxy geometry

    return true;
}

bool Daylight::simCase6(int blindGroupNum, Control *model){
    //	Simulation case 6 will be for window groups that only have the glazing layer as a BSDF
    // Memory leak
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
    baseRad->addRad(model->spaceDirectory()+model->geoDirectory()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);

    //BSDF run for each of the BSDFs
    if (model->windowGroups()[blindGroupNum].bsdfBaseLayers().size()>0){
        //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
        for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfBaseLayers().size();j++){
            std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
            if (std::find(layers.begin(),layers.end(),model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j])==layers.end()){
                layers.push_back(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]);
            }
            std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive> > splitGeo=baseRad->split(layers);
            if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                STADIC_ERROR("The program quit...");
                return false;
            }
            std::string wgBaseFileBSDF=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+".rad";
            RadFileData first(splitGeo.first);
            first.writeRadFile(wgBaseFileBSDF);
            std::vector<double> normal=first.surfaceNormal(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]);
            std::string thickness;
            std::string bsdfXML;
            for (int k=0;k<first.primitives().size();k++){
                if (first.primitives()[k]->name()==model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]){
                    thickness=first.primitives()[k]->getArg1(0);
                    bsdfXML=first.primitives()[k]->getArg1(1);
                }
            }
            std::string wgBaseFileBSDFStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+"_std.rad";
            RadFileData second(splitGeo.second);
            second.writeRadFile(wgBaseFileBSDFStd);
            if (!simBSDF(blindGroupNum,-1,j,wgBaseFileBSDF,wgBaseFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j],model)){
                STADIC_ERROR("The program quit...");
                return false;
            }
        }
    }
    //For the settings only run the last part of the calculation
    if (model->windowGroups()[blindGroupNum].shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum].shadeSettingGeometry().size();i++){
            // Memory leak
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            settingRad->addRad(model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size()>0){
                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
                    std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]);
                    std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive> > splitGeo=settingRad->split(layers);
                    if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                    std::string wgSettingFileBSDF=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+".rad";
                    RadFileData first(splitGeo.first);
                    first.writeRadFile(wgSettingFileBSDF);
                    //std::vector<double> normal=splitGeo.first->surfaceNormal(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]);
                    std::string thickness;
                    std::string bsdfXML;
                    for (int k=0;k<first.primitives().size();k++){
                        if (first.primitives()[k]->name()==model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]){
                            thickness=first.primitives()[k]->getArg1(0);
                            bsdfXML=first.primitives()[k]->getArg1(1);
                        }
                    }
                    std::string wgSettingFileBSDFStd=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+"_std.rad";
                    RadFileData second;
                    second.writeRadFile(wgSettingFileBSDFStd);
                    /*  This would be correct, but for time savings we don't have to run the entire calculation so the next steps are taken.
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j],model)){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                    */
                    //Create the blacked out rad file
                    std::string mainFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i)+"_bsdf"+std::to_string(j);
                    std::string baseFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j);
                    std::vector<std::string> arguments;
                    arguments.push_back("-m");
                    arguments.push_back("black");
                    arguments.push_back(wgSettingFileBSDFStd);
                    std::string xformProgram="xform";
                    Process xform(xformProgram,arguments);
                    std::string blackRad=mainFileName+"_allblack.rad";
                    xform.setStandardOutputFile(blackRad);
                    xform.start();
                    if (!xform.wait()){
                        STADIC_ERROR("The xform command failed to convert layers to black.");
                        //I want to display the errors here if the standard error has any errors to show.
                        return false;
                    }
                    std::vector<std::string> files;
                    files.push_back(blackRad);
                    std::string blackOct=mainFileName+"_allblack.oct";
                    if(!createOctree(files,blackOct)){
                        return false;
                    }
                    std::string sunsOct=baseFileName+"_suns.oct";
                    //rcontrib
                    arguments.clear();
                    arguments.push_back("-I");
                    arguments.push_back("-ab");
                    //This value should probably be based off of a value in the control file
                    arguments.push_back("1");
                    arguments.push_back("-ad");
                    //This value should probably be based off of a value in the control file
                    arguments.push_back("65000");
                    arguments.push_back("-lw");
                    //This value should probably be based off of a value in the control file
                    arguments.push_back("2e-5");
                    arguments.push_back("-dc");
                    arguments.push_back("1");
                    arguments.push_back("-dt");
                    arguments.push_back("0");
                    arguments.push_back("-dj");
                    arguments.push_back("0");
                    arguments.push_back("-st");
                    arguments.push_back("1");
                    arguments.push_back("-ss");
                    arguments.push_back("0");
                    arguments.push_back("-faa");
                    arguments.push_back("-e");
                    arguments.push_back("MF:"+std::to_string(model->sunDivisions()));
                    arguments.push_back("-f");
                    arguments.push_back("klems_int.cal");
                    arguments.push_back("-b");
                    arguments.push_back("rbin");
                    arguments.push_back("-bn");
                    arguments.push_back("Nrbins");
                    arguments.push_back("-m");
                    arguments.push_back("solar");
                    arguments.push_back(sunsOct);
                    std::string rcontribProgram="rcontrib";
                    Process rcontrib(rcontribProgram,arguments);
                    std::string dirDSMX=mainFileName+"_5PH.dsmx";
                    rcontrib.setStandardOutputFile(dirDSMX);
                    rcontrib.setStandardInputFile(model->inputDirectory()+model->ptsFile()[0]);

                    rcontrib.start();
                    if (!rcontrib.wait()){
                        STADIC_ERROR("The rcontrib run for the 5-phase direct smx has failed with the following errors.");
                        //I want to display the errors here if the standard error has any errors to show.
                        return false;
                    }

                    //Create file names from base output

                    std::string vmx=baseFileName+"_3PH.vmx";
                    std::string dmx=baseFileName+"_3PH.dmx";
                    std::string smx=baseFileName+"_3PH.smx";
                    std::string dirDMX=baseFileName+"_3DIR.dmx";
                    std::string dirVMX=baseFileName+"_3Dir.vmx";
                    std::string dirSMX=baseFileName+"_3DIR.smx";
                    std::string dir5PHsmx=baseFileName+"_5PH.smx";
                    //3Phase
                    //dctimestep | rcollate
                    arguments.clear();
                    arguments.push_back("-n");
                    arguments.push_back("8760");
                    arguments.push_back(vmx);
                    arguments.push_back(bsdfXML);
                    arguments.push_back(dmx);
                    arguments.push_back(smx);
                    std::string dctimestepProgram="dctimestep.exe";
                    Process dctimestep(dctimestepProgram,arguments);

                    std::vector<std::string> arguments2;
                    arguments2.push_back("-h");
                    arguments2.push_back("-fa");
                    arguments2.push_back("-oc");
                    arguments2.push_back("3");
                    std::string rcollateProgram="rcollate.exe";
                    Process rcollate(rcollateProgram,arguments2);
                    dctimestep.setStandardOutputProcess(&rcollate);

                    std::string threePhaseCollated=mainFileName+"_3ph.dat";
                    rcollate.setStandardOutputFile(threePhaseCollated);

                    dctimestep.start();
                    rcollate.start();

                    if(!rcollate.wait()){
                        STADIC_ERROR("The running of rcollate for the 3-phase has failed.");
                        //I want to display the errors here if the standard error has any errors to show.

                        return false;
                    }

                    //3Phase Direct
                    //dctimestep | rcollate
                    arguments.clear();
                    arguments.push_back("-n");
                    arguments.push_back("8760");
                    arguments.push_back(dirVMX);
                    arguments.push_back(bsdfXML);
                    arguments.push_back(dirDMX);
                    arguments.push_back(dirSMX);
                    Process dctimestep2(dctimestepProgram,arguments);

                    arguments2.clear();
                    arguments2.push_back("-h");
                    arguments2.push_back("-fa");
                    arguments2.push_back("-oc");
                    arguments2.push_back("3");
                    Process rcollate2(rcollateProgram,arguments2);
                    dctimestep2.setStandardOutputProcess(&rcollate2);

                    std::string threePhaseDirectCollated=mainFileName+"_3Dir.dat";
                    rcollate2.setStandardOutputFile(threePhaseDirectCollated);

                    dctimestep2.start();
                    rcollate2.start();

                    if(!rcollate2.wait()){
                        STADIC_ERROR("The running of rcollate for the 3-phase direct has failed.");
                        //I want to display the errors here if the standard error has any errors to show.

                        return false;
                    }

                    //5Phase
                    //dctimestep | rcollate
                    arguments.clear();
                    arguments.push_back("-n");
                    arguments.push_back("8760");
                    arguments.push_back(dirDSMX);
                    arguments.push_back(dir5PHsmx);
                    Process dctimestep3(dctimestepProgram,arguments);


                    arguments2.clear();
                    arguments2.push_back("-h");
                    arguments2.push_back("-fa");
                    arguments2.push_back("-oc");
                    arguments2.push_back("3");
                    Process rcollate3(rcollateProgram,arguments2);
                    dctimestep3.setStandardOutputProcess(&rcollate3);
                    std::string fivePhaseCollated=mainFileName+"_5PH.dat";
                    rcollate3.setStandardOutputFile(fivePhaseCollated);

                    dctimestep3.start();
                    rcollate3.start();

                    if(!rcollate3.wait()){
                        STADIC_ERROR("The running of rcollate for the 3-phase has failed.");
                        //I want to display the errors here if the standard error has any errors to show.

                        return false;
                    }

                    //Process final data into ill file

                    arguments.clear();
                    arguments.push_back(threePhaseCollated);
                    arguments.push_back(threePhaseDirectCollated);
                    arguments.push_back(fivePhaseCollated);
                    std::string rlamProgram="rlam";
                    Process rlam(rlamProgram,arguments);

                    arguments2.clear();
                    arguments2.push_back("-e");
                    arguments2.push_back("r=$1-$4+7;g=$2-$5+$8;b=$3-$6+$9");
                    arguments2.push_back("-e");
                    arguments2.push_back("ill=179*(.265*r+.670*g+.65*b)");
                    arguments2.push_back("-e");
                    arguments2.push_back("$1=floor(ill+.5)");
                    std::string rcalcProgram="rcalc";
                    Process rcalc(rcalcProgram,arguments2);
                    rlam.setStandardOutputProcess(&rcalc);


                    std::vector<std::string> arguments3;
                    arguments3.clear();
                    arguments3.push_back("-h");
                    arguments3.push_back("-fa");
                    arguments3.push_back("-or");
                    arguments3.push_back("8760");
                    arguments3.push_back("-t");
                    Process rcollate4(rcollateProgram,arguments3);
                    rcalc.setStandardOutputProcess(&rcollate4);
                    rcollate4.setStandardOutputFile(mainFileName+".ill");

                    rlam.start();
                    rcalc.start();
                    rcollate4.start();
                    if(!rcollate4.wait()){
                        STADIC_ERROR("The running of rcollate for final illuminance has failed.");
                        //I want to display the errors here if the standard error has any errors to show.

                        return false;
                    }
                }
            }else{
                STADIC_ERROR("Blind Group "+std::to_string(blindGroupNum)+" setting "+std::to_string(i)+ " does not contain a bsdf layer.");
                return false;
            }
            delete settingRad;
        }
    }
    return true;
}

bool Daylight::uniqueGlazingMaterials(Control *model){
    for (int i=0;i<model->windowGroups().size();i++){
        for (int j=0;j<model->windowGroups()[i].glazingLayers().size();j++){
            for (int n=i;n<model->windowGroups().size();n++){
                for (int m=j+1;m<model->windowGroups()[n].glazingLayers().size();m++){
                    if (model->windowGroups()[i].glazingLayers()[j]==model->windowGroups()[n].glazingLayers()[m]){
                        STADIC_ERROR("The calculation cannot be performed because "+model->windowGroups()[i].name()
                            +"\n\tcontains the same glazing layer as "+model->windowGroups()[n].name()+".");
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool Daylight::testSimCase(Control *model){
    //	Simulation cases are based on whether the control file contains a BSDF for a particular window group
    //	The following loop will change the simulation case for each window group.
    //  Simulation Case 1 will be for window groups that do not contain BSDFs
    //	Simulation case 2 will be for window groups that contain BSDFs, but not in the base case and the glazing layers are not BSDFs
    //	Simulation case 3 will be for window groups that contain BSDFs even in the base case, but the glazing layers are not BSDFs
    //	Simulation case 4 will be for window groups that have shade materials in addition to the glazing layer which is a BSDF
    //	Simulation case 5 will be for window groups that have added geometry, but it is a proxy geometry
    //	Simulation case 6 will be for window groups that only have the glazing layer as a BSDF

    for (int i=0;i<model->windowGroups().size();i++){
        if (model->windowGroups()[i].isBSDF()){
            if (!setSimCase(i,2)){
                return false;
            }
            if (model->windowGroups()[i].bsdfBaseLayers().size()!=0){					//If there are no BSDFs then this will remain material case 2
                bool GlazingBSDF=false;
                for (int j=0;j<model->windowGroups()[i].bsdfBaseLayers().size();j++){
                    for (int k=0;k<model->windowGroups()[i].glazingLayers().size();k++){
                        if (model->windowGroups()[i].glazingLayers()[k]==model->windowGroups()[i].bsdfBaseLayers()[j]){
                            GlazingBSDF=true;
                        }
                    }
                }
                if (model->windowGroups()[i].bsdfSettingLayers().size()!=0){
                    if (model->windowGroups()[i].shadeSettingGeometry().size()!=0){
                        for (int x=0;x<model->windowGroups()[i].shadeSettingGeometry().size();x++){
                            for (int j=0;j<model->windowGroups()[i].bsdfSettingLayers()[x].size();j++){
                                for (int k=0;k<model->windowGroups()[i].glazingLayers().size();k++){
                                    if (model->windowGroups()[i].glazingLayers()[k]==model->windowGroups()[i].bsdfSettingLayers()[x][j]){
                                        GlazingBSDF=true;
                                    }
                                }
                            }
                        }
                    }
                }
                if (GlazingBSDF==false){					//If the glazing layer is not a BSDF then this will be material case 3
                    if (!setSimCase(i,3)){
                        return false;
                    }
                }else if (model->windowGroups()[i].shadeSettingGeometry().size()==0){
                    if (!setSimCase(i,6)){
                        return false;
                    }
                }else{
                    bool OnlyProxy=true;
                    bool OnlyGlazing=true;
                    for (int j=0;j<model->windowGroups()[i].shadeSettingGeometry().size();j++){

                        std::string tempString;
                        std::ifstream iFile;
                        iFile.open(model->spaceDirectory()+model->windowGroups()[i].shadeSettingGeometry()[j]);
                        if (!iFile.is_open()){
                            STADIC_ERROR("The opening of the geometry file " +model->spaceDirectory()+model->windowGroups()[i].shadeSettingGeometry()[j]+" has failed.");
                            return false;
                        }
                        std::getline(iFile,tempString);
                        iFile.close();
                        if (tempString.find("proxy")!=tempString.npos){      //If all files contain "# proxy" on the first line then it is material case 5
                            OnlyProxy=false;
                        }else if (tempString.find("exchange")!=tempString.npos){		//If all files contain "# exchange" on the first line then it is material case 6
                            OnlyGlazing=false;
                        }
                        if (OnlyProxy==true){
                            if (!setSimCase(i,5)){					//If there is added geometry, but only within the thickness
                                return false;
                            }
                        }else if (OnlyGlazing==true){
                            if (!setSimCase(i,6)){					//If only the glazing layers are BSDFs
                                return false;
                            }
                        }else{
                            if (!setSimCase(i,4)){					//If there is added geometry outside of the thickness
                                return false;
                            }
                        }
                    }
                }
            }
        }else{
            if (!setSimCase(i,1)){
                return false;
            }
        }
    }
    return true;
}

bool Daylight::setSimCase(int setting, int simCase){
    if (m_SimCase.size()==0){
        m_SimCase.resize(1);
    }
    if (setting>(m_SimCase.size()-1)){
        m_SimCase.resize(setting+1);
    }else if (setting<0){
        STADIC_ERROR("The setting of the simulation case failed.");
        return false;
    }
    int j=m_SimCase.size()-1;
    m_SimCase.at(setting)=simCase;
    return true;
}

bool Daylight::writeSky(Control *model){
    std::ofstream oFile;
    std::string tmpFile=model->spaceDirectory()+model->intermediateDataDirectory()+"sky_white1.rad";
    oFile.open(tmpFile);
    if (!oFile.is_open()){
        STADIC_ERROR("The opening of the file "+tmpFile +" has failed.");
        return false;
    }

    oFile<<"void glow sky_glow"<<std::endl<<"0 0 4 1 1 1 0"<<std::endl<<std::endl;
    oFile<<"sky_glow source sky"<<std::endl<<"0 0 4 0 0 1 180"<<std::endl<<std::endl;
    oFile<<"sky_glow source ground1"<<std::endl<<"0 0 4 0 0 -1 180"<<std::endl;
    oFile.close();

    tmpFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_suns.rad";
    oFile.open(tmpFile);
    if (!oFile.is_open()){
        STADIC_ERROR("The opning of the file "+tmpFile +" has failed.");
        return false;
    }
    oFile<<"void light solar 0 0 3 1.00e+06 1.00e+06 1.00e+06";
    oFile.close();
    return true;
}

bool Daylight::createBaseRadFiles(Control *model){
    RadFileData radModel;
    //Add the main material file to the primitive list
    radModel.addRad(model->spaceDirectory()+model->geoDirectory()+model->matFile());
    RadPrimitive *black = new PlasticMaterial(0,0,0,0,0);
    black->setModifier(RadPrimitive::sharedVoid());
    black->setName("black");
    radModel.addPrimitive(black);
    //Add the main geometry file to the primitive list
    radModel.addRad(model->spaceDirectory()+model->geoDirectory()+model->geoFile());

    //Create main rad files for each of the window groups
        //The window group rad file will contain the base rad files and each of the other
        //base rad files except their own.  The glazing layers for the other groups will
        //be blacked out.
    //tempFile=model.spaceDirectory()+model.intermediateDataDirectory()+model.spaceName()+"_Main.rad";
    //radModel.writeRadFile(tempFile);
    for (int i=0;i<model->windowGroups().size();i++){
        // Memory leak
        RadFileData *wgRadModel = new RadFileData(radModel.primitives()); // Careful! Stack allocation! What? This is heap allocation
        //wgRadModel.addRad(tempFile);
        for (int j=0;j<model->windowGroups().size();j++){
            if (i!=j){
                if(!wgRadModel->addRad(model->spaceDirectory()+model->geoDirectory()+model->windowGroups()[j].baseGeometry())){
                    return false;
                }
                for (int k=0;k<model->windowGroups()[j].glazingLayers().size();k++){
                    //if(!wgRadModel->blackOutLayer(model->windowGroups()[j].glazingLayers()[k])){
                    //    return false;
                    //}
                }
            }
        }
        /*  This section not needed due to next line
        QString wgFile;
        wgFile=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_Main.rad";
        if (!wgRadModel->writeRadFile(wgFile)){
            return false;
        }
        */
        m_RadFiles.push_back(wgRadModel);
    }
    return true;
}

bool Daylight::createOctree(std::vector<std::string> files, std::string octreeName){
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

bool Daylight::sumIlluminanceFiles(Control *model){
    std::string FinalIllFileName;
    std::string tempFileName;
    std::string finalSensorFileName;
    std::string tempSensorFileName;
    for (int i=0;i<model->windowGroups().size();i++){
        //Base Illuminance files
        FinalIllFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base.ill";
        tempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_ill.tmp";
        DaylightIlluminanceData baseIll("lux");

        if(isFile(tempFileName)){
            if (!baseIll.parse(tempFileName,m_Model->weaDataFile().get())){
                return false;
            }
            if (model->windowGroups()[i].bsdfBaseLayers().size()>0){
                for (int j=0;j<model->windowGroups()[i].bsdfBaseLayers().size();j++){
                    tempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_bsdf"+std::to_string(j)+".ill";
                    if(isFile(tempFileName)){
                        if (!baseIll.addIllFile(tempFileName)){
                            return false;
                        }
                    }
                }
            }
            if (model->illumUnits()=="lux"){
                if (!baseIll.writeIllFileLux(FinalIllFileName)){
                    return false;
                }
            }else if (model->illumUnits()=="fc"){
                if (!baseIll.writeIllFileFC(FinalIllFileName)){
                    return false;
                }
            }else{
                STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                return false;
            }
        }else{
            tempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_bsdf0.ill";
            if(isFile(tempFileName)){
                if (!baseIll.parse(tempFileName, m_Model->weaDataFile().get())){
                    return false;
                }
            }else{
                STADIC_ERROR("The illuminance file "+tempFileName+" does not exist.");
                return false;
            }
            if (model->windowGroups()[i].bsdfBaseLayers().size()>1){
                for (int j=1;j<model->windowGroups()[i].bsdfBaseLayers().size();j++){
                    tempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_bsdf"+std::to_string(j)+".ill";
                    if (!baseIll.addIllFile(tempFileName)){
                        return false;
                    }
                }
            }
            if (model->illumUnits()=="lux"){
                if (!baseIll.writeIllFileLux(FinalIllFileName)){
                    return false;
                }
            }else if (model->illumUnits()=="fc"){
                if (!baseIll.writeIllFileFC(FinalIllFileName)){
                    return false;
                }
            }else{
                STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                return false;
            }
        }
        //base signal files
        if (model->windowGroups()[i].shadeControl()->needsSensor()){
            finalSensorFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+ model->windowGroups()[i].name()+"_shade.sig";
            tempSensorFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+ model->windowGroups()[i].name()+"_shade_sig.tmp";
            DaylightIlluminanceData baseSig("lux");
            if(isFile(tempSensorFileName)){
                if (!baseSig.parse(tempSensorFileName,m_Model->weaDataFile().get())){
                    return false;
                }
                if (model->windowGroups()[i].bsdfBaseLayers().size()>0){
                    for (int j=0;j<model->windowGroups()[i].bsdfBaseLayers().size();j++){
                        tempSensorFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_shade_bsdf"+std::to_string(j)+".sig";
                        if(isFile(tempSensorFileName)){
                            if (!baseSig.addIllFile(tempSensorFileName)){
                                return false;
                            }
                        }
                    }
                }
                if (model->illumUnits()=="lux"){
                    if (!baseSig.writeIllFileLux(finalSensorFileName)){
                        return false;
                    }
                }else if (model->illumUnits()=="fc"){
                    if (!baseSig.writeIllFileFC(finalSensorFileName)){
                        return false;
                    }
                }else{
                    STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                    return false;
                }
            }else{
                tempSensorFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_shade_bsdf0.sig";
                if(isFile(tempSensorFileName)){
                    if (!baseSig.parse(tempSensorFileName, m_Model->weaDataFile().get())){
                        return false;
                    }
                }else{
                    STADIC_ERROR("The illuminance file "+tempSensorFileName+" does not exist.");
                    return false;
                }
                if (model->windowGroups()[i].bsdfBaseLayers().size()>1){
                    for (int j=1;j<model->windowGroups()[i].bsdfBaseLayers().size();j++){
                        tempSensorFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_shade_bsdf"+std::to_string(j)+".sig";
                        if (!baseSig.addIllFile(tempSensorFileName)){
                            return false;
                        }
                    }
                }
                if (model->illumUnits()=="lux"){
                    if (!baseSig.writeIllFileLux(finalSensorFileName)){
                        return false;
                    }
                }else if (model->illumUnits()=="fc"){
                    if (!baseSig.writeIllFileFC(finalSensorFileName)){
                        return false;
                    }
                }else{
                    STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                    return false;
                }
            }
        }
        //Shade Setting Illuminance files
        for (int j=0;j<model->windowGroups()[i].shadeSettingGeometry().size();j++){
            DaylightIlluminanceData settingIll("lux");
            tempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string((j+1))+"_ill_std.tmp";
            FinalIllFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string((j+1))+".ill";
            if(isFile(tempFileName)){
                if (!settingIll.parse(tempFileName,m_Model->weaDataFile().get())){
                    return false;
                }
                if (model->windowGroups()[i].bsdfSettingLayers().size()>=j+1){
                    if (model->windowGroups()[i].bsdfSettingLayers()[j].size()>0){
                        for (int k=0;k<model->windowGroups()[i].bsdfSettingLayers()[j].size();k++){
                            tempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string(j)+"_bsdf"+std::to_string(k)+".ill";
                            settingIll.addIllFile(tempFileName);
                        }
                    }
                }
                if (model->illumUnits()=="lux"){
                    if (!settingIll.writeIllFileLux(FinalIllFileName)){
                        return false;
                    }
                }else if (model->illumUnits()=="fc"){
                    if (!settingIll.writeIllFileFC(FinalIllFileName)){
                        return false;
                    }
                }else{
                    STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                    return false;
                }
            }else{
                tempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string(j)+"_bsdf0.ill";
                if(isFile(tempFileName)){
                    if (!settingIll.parse(tempFileName,m_Model->weaDataFile().get())){
                        return false;
                    }
                    if (model->windowGroups()[i].bsdfSettingLayers()[j].size()!=1){
                        for (int k=1;k<model->windowGroups()[i].bsdfSettingLayers()[j].size();k++){
                            tempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string(j)+"_bsdf"+std::to_string(k)+".ill";
                            settingIll.addIllFile(tempFileName);
                        }
                    }
                    if (model->illumUnits()=="lux"){
                        if (!settingIll.writeIllFileLux(FinalIllFileName)){
                            return false;
                        }
                    }else if (model->illumUnits()=="fc"){
                        if (!settingIll.writeIllFileFC(FinalIllFileName)){
                            return false;
                        }
                    }else{
                        STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                        return false;
                    }
                }else{
                    STADIC_ERROR("The illuminance file "+tempFileName+" does not exist.");
                    return false;
                }
            }
        }
    }

    //Direct illuminance file generation (for sDA and ASE)
    std::string directFinalIllFileName;
    std::string directTempFileName;
    for (int i=0;i<model->windowGroups().size();i++){
        //Base Illuminance files
        directFinalIllFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_direct.ill";
        directTempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_direct_ill.tmp";
        DaylightIlluminanceData baseIllDirect("lux");
        if(isFile(directTempFileName)){
            if (!baseIllDirect.parse(directTempFileName,m_Model->weaDataFile().get())){
                return false;
            }
            if (model->windowGroups()[i].bsdfBaseLayers().size()>0){
                for (int j=0;j<model->windowGroups()[i].bsdfBaseLayers().size();j++){
                    directTempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_bsdf"+std::to_string(j)+"_direct.ill";
                    if(isFile(directTempFileName)){
                        if (!baseIllDirect.addIllFile(directTempFileName)){
                            return false;
                        }
                    }
                }
            }
            if (model->illumUnits()=="lux"){
                if (!baseIllDirect.writeIllFileLux(directFinalIllFileName)){
                    return false;
                }
            }else if (model->illumUnits()=="fc"){
                if (!baseIllDirect.writeIllFileFC(directFinalIllFileName)){
                    return false;
                }
            }else{
                STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                return false;
            }
        }else{
            directTempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_bsdf0_direct.ill";
            if(isFile(directTempFileName)){
                if (!baseIllDirect.parse(directTempFileName, m_Model->weaDataFile().get())){
                    return false;
                }
            }else{
                STADIC_ERROR("The illuminance file "+directTempFileName+" does not exist.");
                return false;
            }
            if (model->windowGroups()[i].bsdfBaseLayers().size()>1){
                for (int j=1;j<model->windowGroups()[i].bsdfBaseLayers().size();j++){
                    directTempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base_bsdf"+std::to_string(j)+"_direct.ill";
                    if (!baseIllDirect.addIllFile(directTempFileName)){
                        return false;
                    }
                }
            }
            if (model->illumUnits()=="lux"){
                if (!baseIllDirect.writeIllFileLux(directFinalIllFileName)){
                    return false;
                }
            }else if (model->illumUnits()=="fc"){
                if (!baseIllDirect.writeIllFileFC(directFinalIllFileName)){
                    return false;
                }
            }else{
                STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                return false;
            }
        }
        //Shade Setting Illuminance files
        for (int j=0;j<model->windowGroups()[i].shadeSettingGeometry().size();j++){
            DaylightIlluminanceData settingIllDirect("lux");
            directTempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string((j+1))+"_direct_ill_std.tmp";
            directFinalIllFileName=model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string((j+1))+"_direct.ill";
            if(isFile(tempFileName)){
                if (!settingIllDirect.parse(directTempFileName,m_Model->weaDataFile().get())){
                    return false;
                }
                if (model->windowGroups()[i].bsdfSettingLayers().size()>=j+1){
                    if (model->windowGroups()[i].bsdfSettingLayers()[j].size()>0){
                        for (int k=0;k<model->windowGroups()[i].bsdfSettingLayers()[j].size();k++){
                            directTempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string(j)+"_bsdf"+std::to_string(k)+"_direct.ill";
                            settingIllDirect.addIllFile(directTempFileName);
                        }
                    }
                }
                if (model->illumUnits()=="lux"){
                    if (!settingIllDirect.writeIllFileLux(directFinalIllFileName)){
                        return false;
                    }
                }else if (model->illumUnits()=="fc"){
                    if (!settingIllDirect.writeIllFileFC(directFinalIllFileName)){
                        return false;
                    }
                }else{
                    STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                    return false;
                }
            }else{
                directTempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string(j)+"_bsdf0_direct.ill";
                if(isFile(directTempFileName)){
                    if (!settingIllDirect.parse(directTempFileName,m_Model->weaDataFile().get())){
                        return false;
                    }
                    if (model->windowGroups()[i].bsdfSettingLayers()[j].size()!=1){
                        for (int k=1;k<model->windowGroups()[i].bsdfSettingLayers()[j].size();k++){
                            directTempFileName=model->spaceDirectory()+model->intermediateDataDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string(j)+"_bsdf"+std::to_string(k)+"_direct.ill";
                            settingIllDirect.addIllFile(directTempFileName);
                        }
                    }
                    if (model->illumUnits()=="lux"){
                        if (!settingIllDirect.writeIllFileLux(directFinalIllFileName)){
                            return false;
                        }
                    }else if (model->illumUnits()=="fc"){
                        if (!settingIllDirect.writeIllFileFC(directFinalIllFileName)){
                            return false;
                        }
                    }else{
                        STADIC_LOG(Severity::Error, "The illuminance units must be \"fc\" or \"lux\".");
                        return false;
                    }
                }else{
                    STADIC_ERROR("The illuminance file "+directTempFileName+" does not exist.");
                    return false;
                }
            }
        }
    }

    return true;
}

}
        /*
        }else if (NewMaterialCase[i]==5){			//The glazing layer is a BSDF and there is only proxy geometry added
            #pragma region "MaterialCase 5"
            #pragma region "Base Case"
            File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";
            cout<<"File1: "<<File1<<endl;
            File2=string(project_directory)+string(BlindGroupGeometryInRadiance[0][i]);
            cout<<"File2: "<<File2<<endl;
            File3=string(project_directory)+"tmp/"+ string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.tmp";
            AddTwoRadFiles(File1, File2, File3);
            cout<<"I have added the two files together."<<endl;

            File1=string(project_directory)+"tmp/"+ string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.rad";
            File2=string(project_directory)+"tmp/"+ string(project_name)+"_"+string(BlindGroupName[i])+"_base_glass.rad";
            RemoveLayerFunc(File3, File1, GlazingMaterials[i][0], File2);
            cout<<"I have removed the glazing layer."<<endl;

            AddToBat=WriteBSDFBat(i, -1, File1, 0, "null", File2, HeaderFileName);
            cout<<"I have written to the batch file."<<endl;
            BatchFileOut<<"call "<<AddToBat<<endl;
            #pragma endregion

            #pragma region "Shade Settings"
            for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
                File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";
                File2=string(project_directory)+string(BlindGroupGeometryInRadiance[n+1][i]);
                ss<<string(project_directory)<<"tmp/"<< string(project_name)<<"_"<< string(BlindGroupName[i]) <<"_setting_"<<n+1<<".tmp";
                ss>>File3;
                ss.clear();
                AddTwoRadFiles(File1, File2, File3);

                File1="";
                ss<<string(project_directory)<<"tmp/"<< string(project_name)<<"_"<< string(BlindGroupName[i]) <<"_setting_"<<n+1<<".rad";
                ss>>File1;
                ss.clear();

                RadIn.open(File1);
                RadIn.close();

                ss<<string(project_directory)<<"tmp/"<< string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_glass.rad";
                ss>>File2;
                ss.clear();
                RemoveLayerFunc(File3, File1, GlazingMaterials[i][0], File2);

                RadIn.open(File1);
                if (RadIn.fail()){
                    cout<<"The opening of the rad file for group "<<i<<" setting "<<n+1<<" has failed."<<endl;
                    cout<<"The file name is "<<File1<<"."<<endl;
                    cout<<"The program will now close."<<endl;
                    exit(1);
                }
                MatFound=false;
                while (!RadIn.eof() && MatFound==false){
                    RadIn>>PassString;
                    if (PassString=="void"){
                        RadIn>>PassString;
                        RadIn>>PassString;
                        cout<<"A material has been found named:  "<<PassString<<endl;
                        if (PassString==BSDFExchangeMaterials[i][n+1][0]){
                            cout<<"This is the correct material."<<endl;
                            MatFound=true;
                            RadIn>>PassString;
                            RadIn>>PassString;
                            RadIn>>PassString;			//Read the xml file
                        }
                    }
                }
                RadIn.close();


                ss<<string(project_directory)+"tmp/"+string(project_name)+"_"+ string(BlindGroupName[i]) +"_setting_"<<n+1<<"_5Phase.bat";
                ss>>File3;
                ss.clear();

                FivePhaseOut.open(File3);
                FivePhaseOut<<"@echo off"<<endl;
                FivePhaseOut<<"REM Sky Divisions"<<endl;
                FivePhaseOut<<"REM Sun Divisions"<<endl;
                FivePhaseOut<<"REM MF1=145 ; MF2=577 ; MF3=1297 ; MF4=2305 ; MF5=3601 ; MF6=5185"<<endl;
                FivePhaseOut<<"set MFsun="<<SunDivisions<<endl;
                FivePhaseOut<<"set NSuns=";
                switch (SunDivisions){
                    case 1:
                        FivePhaseOut<<"145"<<endl<<endl;
                        break;
                    case 2:
                        FivePhaseOut<<"577"<<endl<<endl;
                        break;
                    case 3:
                        FivePhaseOut<<"1297"<<endl<<endl;
                        break;
                    case 4:
                        FivePhaseOut<<"2305"<<endl<<endl;
                        break;
                    case 5:
                        FivePhaseOut<<"3601"<<endl<<endl;
                        break;
                    case 6:
                        FivePhaseOut<<"5185"<<endl<<endl;
                        break;
                    default:
                        cout<<"The number of sun divisions is not recognized.  The program will now quit."<<endl;
                        exit(1);
                }
                FivePhaseOut<<"xform -m black "<<File1<<" > "<<string(project_directory)<<"tmp/black.rad"<<endl;
                FivePhaseOut<<"oconv "<<string(project_directory)<<"tmp/black.rad "<< string(project_directory)<<"tmp/"<<string(project_name)<<"_suns.rad "<<File2<<" "<<string(project_directory)<<"tmp/suns_m1.rad > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF"<<1<<"_suns.oct"<<endl<<endl;

                FivePhaseOut<<"REM DCsun matrix"<<endl;
                FivePhaseOut<<"echo rcontrib ^< "<<string(sensor_file)<<" -I -ab 1 -ad 65000 -lw 1.52e-5 -dc 1 -dt 0 -dj 0 -st 1 -ss 0 -faa -e MF:%MFsun% -f reinhart.cal -b rbin -bn Nrbins -m solar "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF"<<1<<"_suns.oct ^> "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_5PH.dsmx > DCsun.bat"<<endl;
                FivePhaseOut<<"echo exit >> DCsun.bat"<<endl;
                FivePhaseOut<<"start \"DCsun matrix\" /wait DCsun.bat"<<endl<<endl;

                FivePhaseOut<<"del Dcsun.bat"<<endl<<endl;

                FivePhaseOut<<"echo putting it all together..."<<endl;
                FivePhaseOut<<"dctimestep -n "<<(double(60)/time_step)*8760<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3PH.vmx "<<PassString<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3PH.dmx "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1.smx | rcollate -h -fa -oc 3 > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_3ph.dat"<<endl;
                FivePhaseOut<<"dctimestep -n "<<(double(60)/time_step)*8760<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3DIR.vmx "<<PassString<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3DIR.dmx "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3DIR.smx | rcollate -h -fa -oc 3 > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_3DIR.dat"<<endl;
                FivePhaseOut<<"dctimestep -n "<<(double(60)/time_step)*8760<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_5PH.dsmx "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_5PH.smx | rcollate -h -fa -oc 3 > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_ds5ph.dat"<<endl;
                FivePhaseOut<<"rlam "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_3ph.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_3DIR.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_ds5ph.dat | rcalc -e \"r=$1-$4+$7;g=$2-$5+$8;b=$3-$6+$9\" -e \"ill=179*(.265*r +.670*g + .65*b)\" -e \"$1=floor(ill+.5)\" | rcollate -h -fa -or "<<(double(60)/time_step)*8760<<" -t > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF1.ill"<<endl;
                FivePhaseOut.close();

                BatchFileOut<<"call "<<File3<<endl;
            }
            #pragma endregion
            #pragma endregion
*/
