#include "daylight.h"
#include "logging.h"
#include "dayill.h"
#include "objects.h"
#include <fstream>
#include "materialprimitives.h"

namespace stadic {
Daylight::Daylight(Control *model) :
    m_Model(model)
{
}

bool Daylight::simDaylight(){
    if (!uniqueGlazingMaterials(m_Model)){
        return false;
    }

    if (!testSimCase(m_Model)){
        return false;
    }

    bool BSDFs=false;
    for (int i=0;i<m_SimCase.size();i++){
        if (m_SimCase[i]>0){
            BSDFs=true;
        }
    }
    if (!writeSky(m_Model)){
        return false;
    }
    if (!createBaseRadFiles(m_Model)){
        return false;
    }
    //Configure the simulation for each window group
    for (int i=0;i<m_Model->windowGroups().size();i++){
        switch (m_SimCase[i]){
            case 1:
                if (!simCase1(i,m_Model)){
                    return false;
                }
                break;
            case 2:
                if (!simCase2(i, m_Model)){
                    return false;
                }
                break;
            case 3:
                //Simulation case 3 will be for window groups that contain BSDFs even in the base case, but the glazing layers are not BSDFs
                if(!simCase3(i,m_Model)){
                    return false;
                }
                break;
            case 4:
                //Simulation case 4 will be for window groups that have shade materials in addition to the glazing layer
                if (!simCase4(i,m_Model)){
                    return false;
                }
                break;
            case 5:
                //Simulation case 5 will be for window groups that have added geometry, but it is a proxy geometry
                if (!simCase5(i,m_Model)){
                    return false;
                }
                break;
            case 6:
                //Simulation case 6 will be for window groups that only have the glazing layer as a BSDF
                if (!simCase6(i,m_Model)){
                    return false;
                }
                break;

        }
    }
    if(!sumIlluminanceFiles(m_Model)){
        return false;
    }

    return true;
}

//Private
bool Daylight::simBSDF(int blindGroupNum, int setting, int bsdfNum, std::string bsdfRad,std::string remainingRad, std::vector<double> normal, std::string thickness, std::string bsdfXML, std::string bsdfLayer, Control *model){
    std::string mainFileName;
    if (setting==-1){
        mainFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(bsdfNum);
    }else{
        mainFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting)+"_bsdf"+std::to_string(bsdfNum);
    }

    //Create initial octrees
    std::vector<std::string> files;
    files.push_back(remainingRad);
    files.push_back(bsdfRad);
    files.push_back(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
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

    FilePath tempFile=model->tmpFolder()+model->projectName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad";
    if (!tempFile.exists()){
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
        rcalc.setStandardOutputFile(model->tmpFolder()+model->projectName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");
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
    files.push_back(model->projectFolder()+model->tmpFolder()+model->projectName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");
    files.push_back(bsdfRad);
    files.push_back(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
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
    arguments.push_back("-ab");
    //This value should probably be based off of a value in the control file
    arguments.push_back("10");
    arguments.push_back("-ad");
    //This value should probably be based off of a value in the control file
    arguments.push_back("5000");
    arguments.push_back("-lw");
    //This value should probably be based off of a value in the control file
    arguments.push_back("2e-5");
    arguments.push_back(mainOct);
    std::string rcontribProgram="rcontrib";

    Process rcontrib(rcontribProgram,arguments);

    std::string vmx=mainFileName+"_3PH.vmx";
    rcontrib.setStandardOutputFile(vmx);
    rcontrib.setStandardInputFile(model->dataFolder()+model->ptsFile());

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
    if (model->buildingRotation()!=0){
        arguments.push_back("-r");
        arguments.push_back(std::to_string((-1)*model->buildingRotation()));
    }
    arguments.push_back("-c");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back(model->weaDataFile());
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
    arguments.push_back("-ab");
    //This value should probably be based off of a value in the control file
    arguments.push_back("10");
    arguments.push_back("-ad");
    //This value should probably be based off of a value in the control file
    arguments.push_back("5000");
    arguments.push_back("-lw");
    //This value should probably be based off of a value in the control file
    arguments.push_back("2e-5");
    arguments.push_back(blackOct);
    Process rcontrib4(rcontribProgram,arguments);
    std::string dirVMX=mainFileName+"_3Dir.vmx";
    rcontrib4.setStandardOutputFile(dirVMX);
    rcontrib4.setStandardInputFile(model->dataFolder()+model->ptsFile());

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
    if (model->buildingRotation()!=0){
        arguments.push_back("-r");
        arguments.push_back(std::to_string((-1)*model->buildingRotation()));
    }
    arguments.push_back("-d");
    arguments.push_back(model->weaDataFile());
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
    if (model->buildingRotation()!=0){
        arguments.push_back("-r");
        arguments.push_back(std::to_string((-1)*model->buildingRotation()));
    }
    arguments.push_back("-5");
    arguments.push_back("-d");
    arguments.push_back(model->weaDataFile());
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
    std::string dirDSMX=mainFileName+"_5PH.dsmx";
    Process rcontrib5(rcontribProgram,arguments);
    rcontrib5.setStandardOutputFile(dirDSMX);
    rcontrib5.setStandardInputFile(model->dataFolder()+model->ptsFile());

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
    //rcontrib for sky
    std::vector<std::string> arguments;
    arguments.push_back("-I+");
    arguments.push_back("-ab");
    //This value should probably be based off of a value in the control file
    arguments.push_back("10");
    arguments.push_back("-ad");
    //This value should probably be based off of a value in the control file
    arguments.push_back("5000");
    arguments.push_back("-lw");
    //This value should probably be based off of a value in the control file
    arguments.push_back("2e-5");
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
    Process rcontrib(rcontribProgram,arguments);

    std::string skyDC;
    if (setting==-1){
        //This is the base case
        arguments.push_back(model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.oct");
        skyDC=model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_1k.dc";
    }else{
        arguments.push_back(model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_std.oct");
        skyDC=model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_1k_std.dc";
    }
    rcontrib.setStandardOutputFile(skyDC);
    rcontrib.setStandardInputFile(model->dataFolder()+model->ptsFile());

    rcontrib.start();
    if (!rcontrib.wait()){
        STADIC_ERROR("The rcontrib run for the sky has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

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
    FilePath tempFile=model->tmpFolder()+model->projectName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad";
    if (!tempFile.exists()){
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
        rcalc.setStandardOutputFile(model->tmpFolder()+model->projectName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");

        cnt.start();
        rcalc.start();

        if(!rcalc.wait()){
            STADIC_ERROR("The running of rcalc for the suns has failed.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }
    }

    //Create suns octree
    std::vector<std::string> octFiles;
    std::string sunsOct;
    if (setting==-1){
        octFiles.push_back(model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad");
        octFiles.push_back(model->projectFolder()+model->tmpFolder()+model->projectName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");
        sunsOct=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_sun_base.oct";

    }else{
        octFiles.push_back(model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"set"+std::to_string(setting+1)+"_ste.rad");
        octFiles.push_back(model->projectFolder()+model->tmpFolder()+model->projectName()+"_suns_m"+std::to_string(model->sunDivisions())+".rad");
        sunsOct=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_sun_std"+std::to_string(setting+1)+"std.oct";

    }
    if(!createOctree(octFiles,sunsOct)){
        return false;
    }

    //rcontrib for sun
    arguments.clear();
    arguments.push_back("-I+");
    arguments.push_back("-ab");
    //This value should probably be based off of a value in the control file
    arguments.push_back("0");
    arguments.push_back("-ad");
    //This value should probably be based off of a value in the control file
    arguments.push_back("5000");
    arguments.push_back("-lw");
    //This value should probably be based off of a value in the control file
    arguments.push_back("2e-5");
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
    std::string sunDC;
    arguments.push_back(sunsOct);
    if (setting==-1){
        //This is the base case
        sunDC=model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_1d.dc";
    }else{
        //This is for the settings
        sunDC=model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_1d_std.dc";
    }
    Process rcontrib2(rcontribProgram,arguments);
    rcontrib2.setStandardOutputFile(sunDC);
    rcontrib2.setStandardInputFile(model->dataFolder()+model->ptsFile());

    rcontrib2.start();
    if (!rcontrib2.wait()){
        STADIC_ERROR("The sun rcontrib run failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //gendaymtx for sun
    arguments.clear();
    arguments.push_back("-m");
    arguments.push_back(std::to_string( model->sunDivisions()));
    arguments.push_back("-5");
    arguments.push_back("-d");
    arguments.push_back("-h");
    arguments.push_back(model->weaDataFile());
    std::string gendaymtxProgram="gendaymtx";
    Process gendaymtx(gendaymtxProgram,arguments);

    std::string sunSMX;
    if (setting==-1){
        sunSMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_d.smx";
    }else{
        sunSMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_d_std.smx";
    }
    gendaymtx.setStandardOutputFile(sunSMX);
    gendaymtx.start();
    if (!gendaymtx.wait()){
        STADIC_ERROR("The creation of the suns has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //gendaymtx for sky
    arguments.clear();
    arguments.push_back("-m");
    arguments.push_back(std::to_string( model->skyDivisions()));
    arguments.push_back("-c");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back("-h");
    arguments.push_back(model->weaDataFile());
    Process gendaymtx2(gendaymtxProgram,arguments);

    std::string skySMX;
    if (setting==-1){
        skySMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_k.smx";
    }else{
        skySMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_k_std.smx";
    }
    gendaymtx2.setStandardOutputFile(skySMX);
    gendaymtx2.start();
    if (!gendaymtx2.wait()){
        STADIC_ERROR("The creation of the sky has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //gendaymtx fun sun in patches
    arguments.clear();
    arguments.push_back("-m");
    arguments.push_back(std::to_string( model->skyDivisions()));
    arguments.push_back("d");
    arguments.push_back("-h");
    arguments.push_back(model->weaDataFile());
    Process gendaymtx3(gendaymtxProgram,arguments);

    std::string sunPatchSMX;
    if (setting==-1){
        sunPatchSMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_kd.smx";
    }else{
        sunPatchSMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_kd_std.smx";
    }
    gendaymtx3.setStandardOutputFile(sunPatchSMX);
    gendaymtx3.start();
    if (!gendaymtx3.wait()){
        STADIC_ERROR("The creation of the sun patches has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //dctimestep | rcollate for the sky
    arguments.clear();
    arguments.push_back("-n");
    arguments.push_back("8760");
    arguments.push_back(skySMX);
    std::string dctimestepProgram="dctimestep";
    Process dctimestep(dctimestepProgram,arguments);

    std::vector<std::string> arguments2;
    arguments2.push_back("-ho");
    arguments2.push_back("-oc");
    arguments2.push_back("1");
    std::string rcollateProgram="rcollate";
    Process rcollate(rcollateProgram,arguments2);
    dctimestep.setStandardOutputProcess(&rcollate);

    std::string skyCollated;
    if (setting==-1){
        skyCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_sky.txt";
    }else{
        skyCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_sky_std.txt";
    }
    rcollate.setStandardOutputFile(skyCollated);

    dctimestep.start();
    rcollate.start();

    if(!rcollate.wait()){
        STADIC_ERROR("The running of rcollate for the sky has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //dctimestep | rcollate for the sun
    arguments[2]=sunSMX;
    Process dctimestep2(dctimestepProgram,arguments);
    std::string sunCollated;
    if (setting==-1){
        sunCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_sun.txt";
    }else{
        sunCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_sun_std.txt";
    }
    Process rcollate2(rcollateProgram,arguments2);
    dctimestep2.setStandardOutputProcess(&rcollate2);
    rcollate2.setStandardOutputFile(sunCollated);

    dctimestep2.start();
    rcollate2.start();

    if(!rcollate2.wait()){
        STADIC_ERROR("The running of rcollate for the sun has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }


    //dctimestep | rcollate for the sun patch
    arguments[2]=sunPatchSMX;
    Process dctimestep3(dctimestepProgram,arguments);
    std::string sunPatchCollated;
    if (setting==-1){
        sunPatchCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_sunPatch.txt";
    }else{
        sunPatchCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_sunPatch_std.txt";
    }
    Process rcollate3(rcollateProgram,arguments2);
    dctimestep3.setStandardOutputProcess(&rcollate3);
    rcollate3.setStandardOutputFile(sunPatchCollated);
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
    arguments.push_back(sunCollated);
    arguments.push_back(sunPatchCollated);
    std::string rlamProgram="rlam";
    Process rlam(rlamProgram,arguments);

    arguments2.clear();
    arguments2.push_back("-e");
    arguments2.push_back("r=$1-$4+$7;g=$2-@5+8;b=$3-$6+$9");
    arguments2.push_back("-e");
    arguments2.push_back("ill=179*(.265*r+.670*g+.065*b)");
    arguments2.push_back("-e");
    arguments2.push_back("$1=floor(ill+.5)");
    std::string rcalcProgram="rcalc";
    Process rcalc(rcalcProgram,arguments2);
    rlam.setStandardOutputProcess(&rcalc);

    std::string finalIll;
    if (setting==-1){
        finalIll=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_ill.tmp";
    }else{
        finalIll=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(setting+1)+"_ill_std.tmp";
    }

    rcalc.setStandardOutputFile(finalIll);

    rlam.start();
    rcalc.start();

    if(!rcalc.wait()){
        return false;
    }

    return true;
}

bool Daylight::simCase1(int blindGroupNum, Control *model){
    //Simulation Case 1 will be for window groups that do not contain BSDFs
    //First simulate the base condition
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());    //This used to be (m_RadFiles[i],this), but the program failed to build
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);
    std::vector<std::string> files;
    files.push_back(wgBaseFile);
    files.push_back(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
    std::string outFileName;
    outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.oct";
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
            RadFileData *wgRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            wgRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            std::string wgSetFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.rad";
            wgRad->writeRadFile(wgSetFile);
            files.clear();
            files.push_back(wgSetFile);
            files.push_back(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
            outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.oct";
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
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());    //This used to be (m_RadFiles[i],this), but the program failed to build
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);
    std::vector<std::string> files;
    files.push_back(wgBaseFile);
    files.push_back(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
    std::string outFileName;
    outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.oct";
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
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            settingRad->addRad(model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size()>0){
                //Create a file of the glazing layers with all BSDFs blacked out and simulate it
                RadFileData *settingStdRad=new RadFileData(settingRad->primitives());
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
                    if (!settingStdRad->blackOutLayer(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j])){
                        return false;
                    }
                }
                std::string wgSettingFileStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.rad";
                files[0]=wgSettingFileStd;
                outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.oct";
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
                    std::string wgSettingFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+".rad";
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
                    std::string wgSettingFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+"_std.rad";
                    RadFileData second(splitGeo.second);
                    second.writeRadFile(wgSettingFileBSDFStd);
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j],model)){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                }
            }else{
                RadFileData *wgRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
                wgRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
                std::string wgSetFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+".rad";
                wgRad->writeRadFile(wgSetFile);
                files.clear();
                files.push_back(wgSetFile);
                files.push_back(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
                outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+".oct";
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
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);

    RadFileData *baseStdRad=new RadFileData(baseRad->primitives());
    for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfBaseLayers().size();j++){
        if (!baseStdRad->blackOutLayer(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j])){
            return false;
        }
    }
    //Create base standard octree
    std::string wgBaseFileStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_std.rad";
    std::vector<std::string> files;
    files.push_back(wgBaseFileStd);
    files.push_back(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
    std::string outFileName;
    outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_std.oct";
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
            std::pair<shared_vector<RadPrimitive>,shared_vector<RadPrimitive>> splitGeo=baseRad->split(layers);
            if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                STADIC_ERROR("The program quit...");
                return false;
            }
            std::string wgBaseFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+".rad";
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
            std::string wgBaseFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+"_std.rad";
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
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            settingRad->addRad(model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size()>0){
                //Create a file of the glazing layers with all BSDFs blacked out and simulate it
                RadFileData *settingStdRad=new RadFileData(settingRad->primitives());
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
                    if (!settingStdRad->blackOutLayer(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j])){
                        return false;
                    }
                }
                std::string wgSettingFileStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.rad";
                files[0]=wgSettingFileStd;
                outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_std.oct";
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
                    std::pair<shared_vector<RadPrimitive>,shared_vector<RadPrimitive>> splitGeo=settingRad->split(layers);
                    if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                    std::string wgSettingFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+".rad";
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
                    std::string wgSettingFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+"_std.rad";
                    RadFileData second(splitGeo.second);
                    second.writeRadFile(wgSettingFileBSDFStd);
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j],model)){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                }
            }else{
                RadFileData *wgRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
                wgRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
                std::string wgSetFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+".rad";
                wgRad->writeRadFile(wgSetFile);
                files.clear();
                files.push_back(wgSetFile);
                files.push_back(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
                outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+".oct";
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
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);

    //BSDF run for each of the BSDFs
    if (model->windowGroups()[blindGroupNum].bsdfBaseLayers().size()>0){
        //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
        for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfBaseLayers().size();j++){
            std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
            if (std::find(layers.begin(),layers.end(),model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j])==layers.end()){
                layers.push_back(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]);
            }
            std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive>> splitGeo=baseRad->split(layers);
            if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                STADIC_ERROR("The program quit...");
                return false;
            }
            std::string wgBaseFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+".rad";
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
            std::string wgBaseFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+"_std.rad";
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
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            settingRad->addRad(model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size()>0){
                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
                    std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]);
                    std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive>> splitGeo=settingRad->split(layers);
                    if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                    std::string wgSettingFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+".rad";
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
                    std::string wgSettingFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+"_std.rad";
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
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum].baseGeometry());
    std::string wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);

    //BSDF run for each of the BSDFs
    if (model->windowGroups()[blindGroupNum].bsdfBaseLayers().size()>0){
        //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
        for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfBaseLayers().size();j++){
            std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
            if (std::find(layers.begin(),layers.end(),model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j])==layers.end()){
                layers.push_back(model->windowGroups()[blindGroupNum].bsdfBaseLayers()[j]);
            }
            std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive>> splitGeo=baseRad->split(layers);
            if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                STADIC_ERROR("The program quit...");
                return false;
            }
            std::string wgBaseFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+".rad";
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
            std::string wgBaseFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j+1)+"_std.rad";
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
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives());
            settingRad->addRad(model->windowGroups()[blindGroupNum].shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size()>0){
                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i].size();j++){
                    std::vector<std::string> layers=model->windowGroups()[blindGroupNum].glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum].bsdfSettingLayers()[i][j]);
                    std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive>> splitGeo=settingRad->split(layers);
                    if (splitGeo.first.size()==0|| splitGeo.second.size()==0){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                    std::string wgSettingFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+".rad";
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
                    std::string wgSettingFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i+1)+"_bsdf"+std::to_string(j+1)+"_std.rad";
                    RadFileData second;
                    second.writeRadFile(wgSettingFileBSDFStd);
                    /*  This would be correct, but for time savings we don't have to run the entire calculation so the next steps are taken.
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j],model)){
                        STADIC_ERROR("The program quit...");
                        return false;
                    }
                    */
                    //Create the blacked out rad file
                    std::string mainFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_set"+std::to_string(i)+"_bsdf"+std::to_string(j);
                    std::string baseFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum].name()+"_base_bsdf"+std::to_string(j);
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
                    rcontrib.setStandardInputFile(model->dataFolder()+model->ptsFile());

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
                        iFile.open(model->projectFolder()+model->windowGroups()[i].shadeSettingGeometry()[j]);
                        if (!iFile.is_open()){
                            STADIC_ERROR("The opening of the geometry file " +model->projectFolder()+model->windowGroups()[i].shadeSettingGeometry()[j]+" has failed.");
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
    if (setting>(m_SimCase.size()-1)){
        m_SimCase.reserve(setting+1);
    }else if (setting<0){
        STADIC_ERROR("The setting of the simulation case failed.");
        return false;
    }
    m_SimCase.at(setting)=simCase;
    return true;
}

bool Daylight::writeSky(Control *model){
    std::ofstream oFile;
    std::string tmpFile=model->projectFolder()+model->tmpFolder()+"sky_white1.rad";
    oFile.open(tmpFile);
    if (!oFile.is_open()){
        STADIC_ERROR("The opning of the file "+tmpFile +" has failed.");
        return false;
    }

    oFile<<"void glow sky_glow"<<std::endl<<"0 0 4 1 1 1 0"<<std::endl<<std::endl;
    oFile<<"sky_glow source sky"<<std::endl<<"0 0 4 0 0 1 180"<<std::endl<<std::endl;
    oFile<<"sky_glow source ground1"<<std::endl<<"0 0 4 0 0 -1 180"<<std::endl;
    oFile.close();

    tmpFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_suns.rad";
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
    radModel.addRad(model->projectFolder()+model->geoFolder()+model->matFile());
    RadPrimitive *black = new PlasticMaterial();
    radModel.addPrimitive(black);
    //Add the main geometry file to the primitive list
    radModel.addRad(model->projectFolder()+model->geoFolder()+model->geoFile());

    //Create main rad files for each of the window groups
        //The window group rad file will contain the base rad files and each of the other
        //base rad files except their own.  The glazing layers for the other groups will
        //be blacked out.
    //tempFile=model.projectFolder()+model.tmpFolder()+model.projectName()+"_Main.rad";
    //radModel.writeRadFile(tempFile);
    for (int i=0;i<model->windowGroups().size();i++){
        RadFileData *wgRadModel = new RadFileData(radModel.primitives()); // Careful! Stack allocation!
        //wgRadModel.addRad(tempFile);
        for (int j=0;j<model->windowGroups().size();j++){
            if (i!=j){
                if(!wgRadModel->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[j].baseGeometry())){
                    return false;
                }
                for (int k=0;k<model->windowGroups()[j].glazingLayers().size();k++){
                    if(!wgRadModel->blackOutLayer(model->windowGroups()[j].glazingLayers()[k])){
                        return false;
                    }
                }
            }
        }
        /*  This section not needed due to next line
        QString wgFile;
        wgFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_Main.rad";
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
    for (int i=0;i<model->windowGroups().size();i++){
        //Base Illuminance files
        FinalIllFileName=model->projectFolder()+model->resultsFolder()+model->projectName()+model->windowGroups()[i].name()+"_base.ill";
        tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_base_ill.tmp";
        DaylightIlluminanceData baseIll;
        FilePath checkFile(tempFileName);
        if (checkFile.isFile()){
            if (!baseIll.parse(tempFileName,model->weaDataFile())){
                return false;
            }
            if (model->windowGroups()[i].bsdfBaseLayers().size()>0){
                for (int j=0;j<model->windowGroups()[i].bsdfBaseLayers().size();j++){
                    tempFileName==model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_base_bsdf"+std::to_string(j)+".ill";
                    FilePath checkFile2(tempFileName);
                    if (checkFile2.isFile()){
                        if (!baseIll.addIllFile(tempFileName)){
                            return false;
                        }
                    }
                }
            }
            baseIll.writeIllFileLux(FinalIllFileName);
        }else{
            tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_base_bsdf0.ill";
            FilePath checkFile2(tempFileName);
            if (checkFile2.isFile()){
                if (!baseIll.parse(tempFileName, model->weaDataFile())){
                    return false;
                }
            }else{
                STADIC_ERROR("The illuminance file "+tempFileName+" does not exist.");
                return false;
            }
            if (model->windowGroups()[i].bsdfBaseLayers().size()>1){
                for (int j=1;j<model->windowGroups()[i].bsdfBaseLayers().size();j++){
                    tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_base_bsdf"+std::to_string(j)+".ill";
                    if (!baseIll.addIllFile(tempFileName)){
                        return false;
                    }
                }
            }
            baseIll.writeIllFileLux(FinalIllFileName);
        }
        for (int j=0;j<model->windowGroups()[i].shadeSettingGeometry().size();j++){
            DaylightIlluminanceData settingIll;
            tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string((j+1))+"_ill_std.tmp";
            FinalIllFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string((j+1))+".ill";
            FilePath checkFile2(tempFileName);
            if (checkFile2.isFile()){
                if (!settingIll.parse(tempFileName,model->weaDataFile())){
                    return false;
                }
                if (model->windowGroups()[i].bsdfSettingLayers()[j].size()!=0){
                    for (int k=0;k<model->windowGroups()[i].bsdfSettingLayers()[j].size();k++){
                        tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string(j)+"_bsdf"+std::to_string(k)+".ill";
                        settingIll.addIllFile(tempFileName);
                    }
                }
                settingIll.writeIllFileLux(FinalIllFileName);
            }else{
                tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string(j)+"_bsdf0.ill";
                FilePath checkFile3(tempFileName);
                if (checkFile3.isFile()){
                    if (!settingIll.parse(tempFileName,model->weaDataFile())){
                        return false;
                    }
                    if (model->windowGroups()[i].bsdfSettingLayers()[j].size()!=1){
                        for (int k=1;k<model->windowGroups()[i].bsdfSettingLayers()[j].size();k++){
                            tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i].name()+"_set"+std::to_string(j)+"_bsdf"+std::to_string(k)+".ill";
                            settingIll.addIllFile(tempFileName);
                        }
                    }
                    settingIll.writeIllFileLux(FinalIllFileName);
                }else{
                    STADIC_ERROR("The illuminance file "+tempFileName+" does not exist.");
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
