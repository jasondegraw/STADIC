#include "daylight.h"
#include "logging.h"
#include "dayill.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include "materialprimitives.h"

namespace stadic {
Daylight::Daylight(Control *model, QObject *parent) :
    m_Model(model), QObject(parent)
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
bool Daylight::simBSDF(int blindGroupNum, int setting, int bsdfNum, QString bsdfRad,QString remainingRad, std::vector<double> normal, QString thickness, QString bsdfXML, QString bsdfLayer, Control *model){
    QString mainFileName;
    if (setting==-1){
        mainFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_bsdf"+bsdfNum;
    }else{
        mainFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+setting+"_bsdf"+bsdfNum;
    }

    //Create initial octrees
    QStringList files;
    files.append(remainingRad);
    files.append(bsdfRad);
    files.append(QString(model->projectFolder()+model->tmpFolder()+"sky_white1.rad"));
    QString mainOct=mainFileName+".oct";
    if(!createOctree(files,mainOct)){
        return false;
    }

    //FivePhaseOut<<endl<<"xform -m black "<<RadFileName<<" > "<<string(project_directory)<<"tmp/black.rad"<<endl;
    //Black out the remainingRad file
    QProcess *xform=new QProcess(this);
    QString xformProgram="xform.exe";
    xform->setProgram(xformProgram);
    QStringList arguments;
    arguments.push_back("-m");
    arguments.push_back("black");
    arguments.push_back(remainingRad);
    QString blackRad=mainFileName+"_allblack.rad";
    xform->setStandardOutputFile(blackRad);
    xform->setWorkingDirectory(model->projectFolder());
    xform->setArguments(arguments);
    xform->start();
    if (!xform->waitForFinished(-1)){
        ERROR("The xform command failed to convert layers to black.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Create the octree for the blacked out rad file
    files.replace(0,blackRad);
    QString blackOct=mainFileName+"_allblack.oct";
    if(!createOctree(files,blackOct)){
        return false;
    }

    //FivePhaseOut<<"cnt %Nsuns% | rcalc -e MF:%MFsun% -f reinsrc.cal -e Rbin=recno -o \"solar source sun 0 0 4 ${ Dx } ${ Dy } ${ Dz } 0.533\" > "<<string(project_directory)<<"tmp/suns_m1.rad"<<endl;
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
    QFile tempFile;
    tempFile.setFileName(model->tmpFolder()+model->projectName()+"_suns_m"+model->sunDivisions()+".rad");
    if (!tempFile.exists()){
        QProcess *cnt=new QProcess(this);
        QString cntProgram="cnt.exe";
        cnt->setProgram(cntProgram);
        arguments.clear();
        arguments.append(QString().sprintf("%g",nSuns));
        cnt->setArguments(arguments);

        QProcess *rcalc=new QProcess(this);
        cnt->setStandardOutputProcess(rcalc);
        QString rcalcProgram="rcalc.exe";
        rcalc->setProgram(rcalcProgram);
        QStringList arguments2;
        arguments2.append("-e");
        arguments2.append("MF:"+QString().sprintf("%g",model->sunDivisions()));
        arguments2.append("-f");
        arguments2.append("reinsrc.cal");
        arguments2.append("-e");
        arguments2.append("Rbin=recno");
        arguments2.append("-o");
        arguments2.append("\"solar source sun 0 0 4 ${ Dx } ${ Dy } ${ Dz } 0.533\"");
        rcalc->setWorkingDirectory(model->projectFolder());
        rcalc->setStandardOutputFile(model->tmpFolder()+model->projectName()+"_suns_m"+model->sunDivisions()+".rad");
        rcalc->setArguments(arguments2);

        cnt->start();
        rcalc->start();

        if(!rcalc->waitForFinished(-1)){
            ERROR("The running of rcalc for the suns has failed.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }
    }

    //Create suns octree
    QString sunsOct;
    files.clear();
    files.append(blackRad);
    files.append(QString(model->projectFolder()+model->tmpFolder()+model->projectName()+"_suns_m"+model->sunDivisions()+".rad"));
    files.append(bsdfRad);
    files.append(QString(model->projectFolder()+model->tmpFolder()+"sky_white1.rad"));
    sunsOct=mainFileName+"_suns.oct";
    if(!createOctree(files,sunsOct)){
        return false;
    }

    //Compute V matrix
    //rcontrib
    QProcess *rcontrib=new QProcess(this);
    QString rcontribProgram="rcontrib.exe";
    rcontrib->setProgram(rcontribProgram);
    arguments.clear();
    arguments.push_back("-f");
    arguments.push_back("klems_int.cal");
    arguments.push_back("-b");
    arguments.push_back(QString().sprintf("kbin(%g,%g,%g,0,0,1)",(-1)*normal[0],(-1)*normal[1],(-1)*normal[2]));
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
    QString vmx=mainFileName+"_3PH.vmx";
    rcontrib->setStandardOutputFile(vmx);
    rcontrib->setWorkingDirectory(model->projectFolder());
    rcontrib->setStandardInputFile(model->dataFolder()+model->ptsFile());
    rcontrib->setArguments(arguments);

    rcontrib->start();
    if (!rcontrib->waitForFinished(-1)){
        ERROR("The rcontrib run for the 3-phase vmx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute D Matrix
    //genklemsamp | rcontrib
    QProcess *perl=new QProcess(this);
    QString perlProgram="perl.exe";
    perl->setProgram(perlProgram);
    arguments.clear();
    arguments.push_back("genklemsamp.pl");
    arguments.push_back("-vd");
    arguments.push_back(QString().sprintf("%g",normal[0]));
    arguments.push_back(QString().sprintf("%g",normal[1]));
    arguments.push_back(QString().sprintf("%g",normal[2]));
    arguments.push_back("-vo");
    arguments.push_back(thickness);
    arguments.push_back(bsdfRad);
    perl->setWorkingDirectory(model->projectFolder());
    perl->setArguments(arguments);
    perl->setStandardOutputProcess(rcontrib);

    QStringList arguments2;
    arguments2.push_back("-c");
    arguments2.push_back("1000");
    arguments2.push_back("-e");
    arguments2.push_back("MF:"+QString().sprintf("%g",model->skyDivisions()));
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
    rcontrib->setArguments(arguments2);
    rcontrib->setWorkingDirectory(model->projectFolder());
    QString dmx=mainFileName+"_3PH.dmx";
    rcontrib->setStandardOutputFile(dmx);

    perl->start();
    rcontrib->start();
    if (!rcontrib->waitForFinished(-1)){
        ERROR("The rcontrib run for the 3-phase dmx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute S Matrix
    //gendaymtx
    QProcess *gendaymtx=new QProcess(this);
    QString gendaymtxProgram="gendaymtx.exe";
    gendaymtx->setProgram(gendaymtxProgram);
    arguments.clear();
    arguments.push_back("MF:"+QString().sprintf("%g",model->skyDivisions()));
    if (model->buildingRotation()!=0){
        arguments.push_back("-r");
        arguments.push_back(QString().sprintf("%g",(-1)*model->buildingRotation()));
    }
    arguments.push_back("-c");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back("1");
    arguments.push_back(model->weaDataFile());
    QString smx=mainFileName+"_3PH.smx";
    gendaymtx->setArguments(arguments);
    gendaymtx->setStandardOutputFile(smx);
    gendaymtx->setWorkingDirectory(model->projectFolder());
    gendaymtx->start();
    if (!gendaymtx->waitForFinished(-1)){
        ERROR("The gendaymtx run for the smx has failed with the following errors.");        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute Dd matrix
    //genklemsamp | rcontrib
    arguments.clear();
    arguments.push_back("genklemsamp.pl");
    arguments.push_back("-vd");
    arguments.push_back(QString().sprintf("%g",normal[0]));
    arguments.push_back(QString().sprintf("%g",normal[1]));
    arguments.push_back(QString().sprintf("%g",normal[2]));
    arguments.push_back("-vo");
    arguments.push_back(thickness);
    arguments.push_back(bsdfRad);
    perl->setWorkingDirectory(model->projectFolder());
    perl->setArguments(arguments);
    perl->setStandardOutputProcess(rcontrib);

    arguments2.clear();
    arguments2.push_back("-c");
    arguments2.push_back("1000");
    arguments2.push_back("-e");
    arguments2.push_back("MF:"+QString().sprintf("%g",model->skyDivisions()));
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
    rcontrib->setArguments(arguments2);
    rcontrib->setWorkingDirectory(model->projectFolder());
    QString dirDMX=mainFileName+"_3DIR.dmx";
    rcontrib->setStandardOutputFile(dirDMX);
    perl->start();
    rcontrib->start();
    if (!rcontrib->waitForFinished(-1)){
        ERROR("The rcontrib run for the 3-phase direct dmx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute Vd matrix
    //rcontrib
    arguments.clear();
    arguments.push_back("-f");
    arguments.push_back("klems_int.cal");
    arguments.push_back("-b");
    arguments.push_back(QString().sprintf("kbin(%g,%g,%g,0,0,1)",(-1)*normal[0],(-1)*normal[1],(-1)*normal[2]));
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
    QString dirVMX=mainFileName+"_3Dir.vmx";
    rcontrib->setStandardOutputFile(dirVMX);
    rcontrib->setWorkingDirectory(model->projectFolder());
    rcontrib->setStandardInputFile(model->dataFolder()+model->ptsFile());
    rcontrib->setArguments(arguments);

    rcontrib->start();
    if (!rcontrib->waitForFinished(-1)){
        ERROR("The rcontrib run for the 3-phase direct vmx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute Sd Matrix
    //gendaymtx
    arguments.clear();
    arguments.push_back("MF:"+QString().sprintf("%g",model->skyDivisions()));
    if (model->buildingRotation()!=0){
        arguments.push_back("-r");
        arguments.push_back(QString().sprintf("%g",(-1)*model->buildingRotation()));
    }
    arguments.push_back("-d");
    arguments.push_back(model->weaDataFile());
    QString dirSMX=mainFileName+"_3DIR.smx";
    gendaymtx->setArguments(arguments);
    gendaymtx->setStandardOutputFile(dirSMX);
    gendaymtx->setWorkingDirectory(model->projectFolder());
    gendaymtx->start();
    if (!gendaymtx->waitForFinished(-1)){
        ERROR("The gendaymtx run for the direct smx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //Compute Ssun Matrix
    //gendaymtx
    arguments.clear();
    arguments.push_back("MF:"+QString().sprintf("%g",model->sunDivisions()));
    if (model->buildingRotation()!=0){
        arguments.push_back("-r");
        arguments.push_back(QString().sprintf("%g",(-1)*model->buildingRotation()));
    }
    arguments.push_back("-5");
    arguments.push_back("-d");
    arguments.push_back(model->weaDataFile());
    QString dir5PHsmx=mainFileName+"_5PH.smx";
    gendaymtx->setArguments(arguments);
    gendaymtx->setStandardOutputFile(dir5PHsmx);
    gendaymtx->setWorkingDirectory(model->projectFolder());
    gendaymtx->start();
    if (!gendaymtx->waitForFinished(-1)){
        ERROR("The gendaymtx run for the direct 5 phase smx has failed with the following errors.");
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
    arguments.push_back("MF:"+QString().sprintf("%g",model->sunDivisions()));
    arguments.push_back("-f");
    arguments.push_back("klems_int.cal");
    arguments.push_back("-b");
    arguments.push_back("rbin");
    arguments.push_back("-bn");
    arguments.push_back("Nrbins");
    arguments.push_back("-m");
    arguments.push_back("solar");
    arguments.push_back(sunsOct);
    QString dirDSMX=mainFileName+"_5PH.dsmx";
    rcontrib->setStandardOutputFile(dirDSMX);
    rcontrib->setWorkingDirectory(model->projectFolder());
    rcontrib->setStandardInputFile(model->dataFolder()+model->ptsFile());
    rcontrib->setArguments(arguments);

    rcontrib->start();
    if (!rcontrib->waitForFinished(-1)){
        ERROR("The rcontrib run for the 5-phase direct smx has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.
        return false;
    }

    //3Phase
    //dctimestep | rcollate
    QProcess *dctimestep=new QProcess(this);
    QString dctimestepProgram="dctimestep.exe";
    dctimestep->setProgram(dctimestepProgram);
    arguments.clear();
    arguments.append("-n");
    arguments.append("8760");
    arguments.append(vmx);
    arguments.append(bsdfXML);
    arguments.append(dmx);
    arguments.append(smx);
    dctimestep->setWorkingDirectory(model->projectFolder());

    QProcess *rcollate=new QProcess(this);
    QString rcollateProgram="rcollate.exe";
    rcollate->setProgram(rcollateProgram);
    dctimestep->setStandardOutputProcess(rcollate);
    arguments2.clear();
    arguments2.append("-h");
    arguments2.append("-fa");
    arguments2.append("-oc");
    arguments2.append("3");
    rcollate->setArguments(arguments2);
    QString threePhaseCollated=mainFileName+"_3ph.dat";
    rcollate->setStandardOutputFile(threePhaseCollated);
    rcollate->setWorkingDirectory(model->projectFolder());

    dctimestep->start();
    rcollate->start();

    if(!rcollate->waitForFinished(-1)){
        ERROR("The running of rcollate for the 3-phase has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //3Phase Direct
    //dctimestep | rcollate
    arguments.clear();
    arguments.append("-n");
    arguments.append("8760");
    arguments.append(dirVMX);
    arguments.append(bsdfXML);
    arguments.append(dirDMX);
    arguments.append(dirSMX);
    dctimestep->setWorkingDirectory(model->projectFolder());

    dctimestep->setStandardOutputProcess(rcollate);
    arguments2.clear();
    arguments2.append("-h");
    arguments2.append("-fa");
    arguments2.append("-oc");
    arguments2.append("3");
    rcollate->setArguments(arguments2);
    QString threePhaseDirectCollated=mainFileName+"_3Dir.dat";
    rcollate->setStandardOutputFile(threePhaseDirectCollated);
    rcollate->setWorkingDirectory(model->projectFolder());

    dctimestep->start();
    rcollate->start();

    if(!rcollate->waitForFinished(-1)){
        ERROR("The running of rcollate for the 3-phase direct has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //5Phase
    //dctimestep | rcollate
    arguments.clear();
    arguments.append("-n");
    arguments.append("8760");
    arguments.append(dirDSMX);
    arguments.append(dir5PHsmx);
    dctimestep->setWorkingDirectory(model->projectFolder());

    dctimestep->setStandardOutputProcess(rcollate);
    arguments2.clear();
    arguments2.append("-h");
    arguments2.append("-fa");
    arguments2.append("-oc");
    arguments2.append("3");
    rcollate->setArguments(arguments2);
    QString fivePhaseCollated=mainFileName+"_5PH.dat";
    rcollate->setStandardOutputFile(fivePhaseCollated);
    rcollate->setWorkingDirectory(model->projectFolder());

    dctimestep->start();
    rcollate->start();

    if(!rcollate->waitForFinished(-1)){
        ERROR("The running of rcollate for the 3-phase has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //Process final data into ill file
    //FivePhaseOut<<"rlam "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_3ph.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_3DIR.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_ds5ph.dat | rcalc -e \"r=$1-$4+$7;g=$2-$5+$8;b=$3-$6+$9\" -e \"ill=179*(.265*r +.670*g + .65*b)\" -e \"$1=floor(ill+.5)\" | rcollate -h -fa -or "<<(double(60)/time_step)*8760<<" -t > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<".ill"<<endl;
    QProcess *rlam=new QProcess(this);
    QString rlamProgram="rlam.exe";
    rlam->setProgram(rlamProgram);
    rlam->setWorkingDirectory(model->projectFolder());
    arguments.clear();
    arguments.append(threePhaseCollated);
    arguments.append(threePhaseDirectCollated);
    arguments.append(fivePhaseCollated);
    rlam->setArguments(arguments);

    QProcess *rcalc=new QProcess(this);
    QString rcalcProgram="rcalc.exe";
    rcalc->setProgram(rcalcProgram);
    rcalc->setWorkingDirectory(model->projectFolder());
    rlam->setStandardOutputProcess(rcalc);
    arguments2.clear();
    arguments2.append("-e");
    arguments2.append("r=$1-$4+7;g=$2-$5+$8;b=$3-$6+$9");
    arguments2.append("-e");
    arguments2.append("ill=179*(.265*r+.670*g+.65*b)");
    arguments2.append("-e");
    arguments2.append("$1=floor(ill+.5)");
    rcalc->setArguments(arguments2);

    QStringList arguments3;
    arguments3.append("-h");
    arguments3.append("-fa");
    arguments3.append("-or");
    arguments3.append("8760");
    arguments3.append("-t");
    rcollate->setArguments(arguments3);
    rcalc->setStandardOutputProcess(rcollate);
    rcollate->setStandardOutputFile(mainFileName+".ill");

    rlam->start();
    rcalc->start();
    rcollate->start();
    if(!rcollate->waitForFinished(-1)){
        ERROR("The running of rcollate for final illuminance has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

  return true;
}

bool Daylight::simStandard(int blindGroupNum, int setting, Control *model){
    //rcontrib for sky
    QProcess *rcontrib=new QProcess(this);
    QString rcontribProgram="rcontrib.exe";
    rcontrib->setProgram(rcontribProgram);
    QStringList arguments;
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
    arguments.push_back("MF:"+QString().sprintf("%g",model->skyDivisions()));
    arguments.push_back("-f");
    arguments.push_back("reinhart.cal");
    arguments.push_back("-b");
    arguments.push_back("rbin");
    arguments.push_back("-bn");
    arguments.push_back("Nrbins");
    arguments.push_back("-m");
    arguments.push_back("sky_glow");
    arguments.push_back("-faa");
    QString skyDC;
    if (setting==-1){
        //This is the base case
        arguments.push_back(model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base.oct");
        skyDC=model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_1k.dc";
    }else{
        arguments.push_back(model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1))+"_std.oct");
        skyDC=model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1))+"_1k_std.dc";
    }
    rcontrib->setStandardOutputFile(skyDC);
    rcontrib->setWorkingDirectory(model->projectFolder());
    rcontrib->setStandardInputFile(model->dataFolder()+model->ptsFile());
    rcontrib->setArguments(arguments);

    rcontrib->start();
    if (!rcontrib->waitForFinished(-1)){
        ERROR("The rcontrib run for the sky has failed with the following errors.");
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
    QFile tempFile;
    tempFile.setFileName(model->tmpFolder()+model->projectName()+"_suns_m"+model->sunDivisions()+".rad");
    if (!tempFile.exists()){
        QProcess *cnt=new QProcess(this);
        QString cntProgram="cnt.exe";
        cnt->setProgram(cntProgram);
        arguments.clear();
        arguments.append(QString().sprintf("%g",nSuns));
        cnt->setArguments(arguments);

        QProcess *rcalc=new QProcess(this);
        cnt->setStandardOutputProcess(rcalc);
        QString rcalcProgram="rcalc.exe";
        rcalc->setProgram(rcalcProgram);
        QStringList arguments2;
        arguments2.append("-e");
        arguments2.append("MF:"+QString().sprintf("%g",model->sunDivisions()));
        arguments2.append("-f");
        arguments2.append("reinsrc.cal");
        arguments2.append("-e");
        arguments2.append("Rbin=recno");
        arguments2.append("-o");
        arguments2.append("\"solar source sun 0 0 4 ${ Dx } ${ Dy } ${ Dz } 0.533\"");
        rcalc->setWorkingDirectory(model->projectFolder());
        rcalc->setStandardOutputFile(model->tmpFolder()+model->projectName()+"_suns_m"+model->sunDivisions()+".rad");
        rcalc->setArguments(arguments2);

        cnt->start();
        rcalc->start();

        if(!rcalc->waitForFinished(-1)){
            ERROR("The running of rcalc for the suns has failed.");
            //I want to display the errors here if the standard error has any errors to show.

            return false;
        }
    }

    //Create suns octree
    QStringList octFiles;
    QString sunsOct;
    if (setting==-1){
        octFiles.append(QString(model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base.rad"));
        octFiles.append(QString(model->projectFolder()+model->tmpFolder()+model->projectName()+"_suns_m"+model->sunDivisions()+".rad"));
        sunsOct=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_sun_base.oct";

    }else{
        octFiles.append(QString(model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"set"+QString().sprintf("%g",(setting+1))+"_ste.rad"));
        octFiles.append(QString(model->projectFolder()+model->tmpFolder()+model->projectName()+"_suns_m"+model->sunDivisions()+".rad"));
        sunsOct=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_sun_std"+QString().sprintf("%g",(setting+1))+"std.oct";

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
    arguments.push_back("MF:"+QString().sprintf("%g",model->sunDivisions()));
    arguments.push_back("-f");
    arguments.push_back("reinhart.cal");
    arguments.push_back("-b");
    arguments.push_back("rbin");
    arguments.push_back("-bn");
    arguments.push_back("Nrbins");
    arguments.push_back("-m");
    arguments.push_back("solar");
    arguments.push_back("-faa");
    QString sunDC;
    arguments.push_back(sunsOct);
    if (setting==-1){
        //This is the base case
        sunDC=model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_1d.dc";
    }else{
        //This is for the settings
        sunDC=model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1)+"_1d_std.dc");
    }
    rcontrib->setStandardOutputFile(sunDC);
    rcontrib->setWorkingDirectory(model->projectFolder());
    rcontrib->setStandardInputFile(model->dataFolder()+model->ptsFile());
    rcontrib->setArguments(arguments);

    rcontrib->start();
    if (!rcontrib->waitForFinished(-1)){
        ERROR("The sun rcontrib run failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //gendaymtx for sun
    QProcess *gendaymtx=new QProcess(this);
    QString gendaymtxProgram="gendaymtx.exe";
    gendaymtx->setProgram(gendaymtxProgram);
    arguments.clear();
    arguments.append("-m");
    arguments.append(QString().sprintf("%g", model->sunDivisions()));
    arguments.append("-5");
    arguments.append("-d");
    arguments.append("-h");
    arguments.append(model->weaDataFile());
    gendaymtx->setArguments(arguments);
    QString sunSMX;
    if (setting==-1){
        sunSMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_d.smx";
    }else{
        sunSMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1)+"_d_std.smx");
    }
    gendaymtx->setStandardOutputFile(sunSMX);
    gendaymtx->setWorkingDirectory(model->projectFolder());
    gendaymtx->start();
    if (!gendaymtx->waitForFinished(-1)){
        ERROR("The creation of the suns has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //gendaymtx for sky
    arguments.clear();
    arguments.append("-m");
    arguments.append(QString().sprintf("%g", model->skyDivisions()));
    arguments.append("-c");
    arguments.append("1");
    arguments.append("1");
    arguments.append("1");
    arguments.append("-h");
    arguments.append(model->weaDataFile());
    gendaymtx->setArguments(arguments);
    QString skySMX;
    if (setting==-1){
        skySMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_k.smx";
    }else{
        skySMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1)+"_k_std.smx");
    }
    gendaymtx->setStandardOutputFile(skySMX);
    gendaymtx->start();
    if (!gendaymtx->waitForFinished(-1)){
        ERROR("The creation of the sky has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //gendaymtx fun sun in patches
    arguments.clear();
    arguments.append("-m");
    arguments.append(QString().sprintf("%g", model->skyDivisions()));
    arguments.append("d");
    arguments.append("-h");
    arguments.append(model->weaDataFile());
    gendaymtx->setArguments(arguments);
    QString sunPatchSMX;
    if (setting==-1){
        sunPatchSMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_kd.smx";
    }else{
        sunPatchSMX=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1))+"_kd_std.smx";
    }
    gendaymtx->setStandardOutputFile(sunPatchSMX);
    gendaymtx->start();
    if (!gendaymtx->waitForFinished(-1)){
        ERROR("The creation of the sun patches has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //dctimestep | rcollate for the sky
    QProcess *dctimestep=new QProcess(this);
    QString dctimestepProgram="dctimestep.exe";
    dctimestep->setProgram(dctimestepProgram);
    arguments.clear();
    arguments.append("-n");
    arguments.append("8760");
    arguments.append(skySMX);
    dctimestep->setWorkingDirectory(model->projectFolder());
    dctimestep->setArguments(arguments);

    QProcess *rcollate=new QProcess(this);
    QString rcollateProgram="rcollate.exe";
    rcollate->setProgram(rcollateProgram);
    dctimestep->setStandardOutputProcess(rcollate);
    QStringList arguments2;
    arguments2.append("-ho");
    arguments2.append("-oc");
    arguments2.append("1");
    rcollate->setArguments(arguments2);
    QString skyCollated;
    if (setting==-1){
        skyCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_sky.txt";
    }else{
        skyCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1))+"_sky_std.txt";
    }
    rcollate->setStandardOutputFile(skyCollated);
    rcollate->setWorkingDirectory(model->projectFolder());

    dctimestep->start();
    rcollate->start();

    if(!rcollate->waitForFinished(-1)){
        ERROR("The running of rcollate for the sky has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //dctimestep | rcollate for the sun
    arguments.replace(2,sunSMX);
    dctimestep->setArguments(arguments);
    QString sunCollated;
    if (setting==-1){
        sunCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_sun.txt";
    }else{
        sunCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1))+"_sun_std.txt";
    }
    rcollate->setStandardOutputFile(sunCollated);
    dctimestep->start();
    rcollate->start();

    if(!rcollate->waitForFinished(-1)){
        ERROR("The running of rcollate for the sun has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }


    //dctimestep | rcollate for the sun patch
    arguments.replace(2,sunPatchSMX);
    dctimestep->setArguments(arguments);
    QString sunPatchCollated;
    if (setting==-1){
        sunPatchCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_sunPatch.txt";
    }else{
        sunPatchCollated=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1))+"_sunPatch_std.txt";
    }
    rcollate->setStandardOutputFile(sunPatchCollated);
    dctimestep->start();
    rcollate->start();

    if(!rcollate->waitForFinished(-1)){
        ERROR("The running of rcollate for the sun patches has failed.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }

    //rlam | rcalc bringing it all together
    QProcess *rlam=new QProcess(this);
    QString rlamProgram="rlam.exe";
    rlam->setProgram(rlamProgram);

    arguments.clear();
    arguments.append(skyCollated);
    arguments.append(sunCollated);
    arguments.append(sunPatchCollated);
    rlam->setArguments(arguments);

    QProcess *rcalc=new QProcess(this);
    QString rcalcProgram="rcalc.exe";
    rcalc->setProgram(rcalcProgram);
    rlam->setStandardOutputProcess(rcalc);
    arguments2.clear();
    arguments2.append("-e");
    arguments2.append("r=$1-$4+$7;g=$2-@5+8;b=$3-$6+$9");
    arguments2.append("-e");
    arguments2.append("ill=179*(.265*r+.670*g+.065*b)");
    arguments2.append("-e");
    arguments2.append("$1=floor(ill+.5)");
    rcalc->setArguments(arguments2);
    QString finalIll;
    if (setting==-1){
        finalIll=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_ill.tmp";
    }else{
        finalIll=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",(setting+1))+"_ill_std.tmp";
    }

    rcalc->setStandardOutputFile(finalIll);

    rlam->start();
    rcalc->start();

    if(!rcalc->waitForFinished(-1)){
        return false;
    }

    return true;
}

bool Daylight::simCase1(int blindGroupNum, Control *model){
    //Simulation Case 1 will be for window groups that do not contain BSDFs
    //First simulate the base condition
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]);    //This used to be (m_RadFiles[i],this), but the program failed to build
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum]->baseGeometry());
    QString wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);
    QStringList files;
    files.append(wgBaseFile);
    files.append(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
    QString outFileName;
    outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base.oct";
    if (!createOctree(files, outFileName)){
        return false;
    }
    //Call Standard Radiance run
    if (!simStandard(blindGroupNum,-1,model)){
        return false;
    }

    //Loop through the shade settings
    if (model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size();i++){
            RadFileData *wgRad=new RadFileData(m_RadFiles[blindGroupNum]);
            wgRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum]->shadeSettingGeometry()[i]);
            QString wgSetFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g%",i+1)+"_std.rad";
            wgRad->writeRadFile(wgSetFile);
            files.clear();
            files.append(wgSetFile);
            files.append(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
            outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g%",i+1)+"_std.oct";
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
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives(), this);    //This used to be (m_RadFiles[i],this), but the program failed to build
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum]->baseGeometry());
    QString wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);
    QStringList files;
    files.append(wgBaseFile);
    files.append(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
    QString outFileName;
    outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base.oct";
    if (!createOctree(files, outFileName)){
        return false;
    }

    //Call Standard Radiance run
    if (!simStandard(blindGroupNum,-1,model)){
        return false;
    }

    //Loop through the shade settings
    if (model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size();i++){
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives(),this);
            settingRad->addRad(model->windowGroups()[blindGroupNum]->shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size()>0){
                //Create a file of the glazing layers with all BSDFs blacked out and simulate it
                RadFileData *settingStdRad=new RadFileData(settingRad->primitives(),this);
                for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size();j++){
                    if (!settingStdRad->blackOutLayer(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j])){
                        return false;
                    }
                }
                QString wgSettingFileStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_std.rad";
                files.replace(0,wgSettingFileStd);
                outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_std.oct";
                if (!createOctree(files, outFileName)){
                    return false;
                }

                //Call Standard Radiance run
                if (!simStandard(blindGroupNum,i,model)){
                    return false;
                }

                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size();j++){
                    std::vector<QString> layers=model->windowGroups()[blindGroupNum]->glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]);
                    QPair<stadic::RadFileData*, stadic::RadFileData*> splitGeo=settingRad->split(layers);
                    if (splitGeo.first==nullptr|| splitGeo.second==nullptr){
                        ERROR("The program quit...");
                        return false;
                    }
                    QString wgSettingFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_bsdf"+QString().sprintf("%g",j+1)+".rad";
                    splitGeo.first->writeRadFile(wgSettingFileBSDF);
                    std::vector<double> normal=splitGeo.first->surfaceNormal(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]);
                    QString thickness;
                    QString bsdfXML;
                    for (int k=0;k<splitGeo.first->primitives().size();k++){
                        if (splitGeo.first->primitives()[k]->name()==model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]){
                            thickness=splitGeo.first->primitives()[k]->getArg1(0);
                            bsdfXML=splitGeo.first->primitives()[k]->getArg1(1);
                        }
                    }
                    QString wgSettingFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_bsdf"+QString().sprintf("%g",j+1)+"_std.rad";
                    splitGeo.second->writeRadFile(wgSettingFileBSDFStd);
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j],model)){
                        ERROR("The program quit...");
                        return false;
                    }
                }
            }else{
                RadFileData *wgRad=new RadFileData(m_RadFiles[blindGroupNum]);
                wgRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum]->shadeSettingGeometry()[i]);
                QString wgSetFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g%",i+1)+".rad";
                wgRad->writeRadFile(wgSetFile);
                files.clear();
                files.append(wgSetFile);
                files.append(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
                outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g%",i+1)+".oct";
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
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives(), this);
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum]->baseGeometry());
    QString wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);

    RadFileData *baseStdRad=new RadFileData(baseRad->primitives(),this);
    for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfBaseLayers().size();j++){
        if (!baseStdRad->blackOutLayer(model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j])){
            return false;
        }
    }
    //Create base standard octree
    QString wgBaseFileStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_std.rad";
    QStringList files;
    files.append(wgBaseFileStd);
    files.append(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
    QString outFileName;
    outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_std.oct";
    if (!createOctree(files, outFileName)){
        return false;
    }
    //Call Standard Radiance run
    if (!simStandard(blindGroupNum,-1,model)){
        return false;
    }
    //BSDF run for each of the BSDFs
    if (model->windowGroups()[blindGroupNum]->bsdfBaseLayers().size()>0){
        //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
        for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfBaseLayers().size();j++){
            std::vector<QString> layers=model->windowGroups()[blindGroupNum]->glazingLayers();
            layers.push_back(model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j]);
            QPair<stadic::RadFileData*, stadic::RadFileData*> splitGeo=baseRad->split(layers);
            if (splitGeo.first==nullptr|| splitGeo.second==nullptr){
                ERROR("The program quit...");
                return false;
            }
            QString wgBaseFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_bsdf"+QString().sprintf("%g",j+1)+".rad";
            splitGeo.first->writeRadFile(wgBaseFileBSDF);
            std::vector<double> normal=splitGeo.first->surfaceNormal(model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j]);
            QString thickness;
            QString bsdfXML;
            for (int k=0;k<splitGeo.first->primitives().size();k++){
                if (splitGeo.first->primitives()[k]->name()==model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j]){
                    thickness=splitGeo.first->primitives()[k]->getArg1(0);
                    bsdfXML=splitGeo.first->primitives()[k]->getArg1(1);
                }
            }
            QString wgBaseFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_bsdf"+QString().sprintf("%g",j+1)+"_std.rad";
            splitGeo.second->writeRadFile(wgBaseFileBSDFStd);
            if (!simBSDF(blindGroupNum,-1,j,wgBaseFileBSDF,wgBaseFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j],model)){
                ERROR("The program quit...");
                return false;
            }
        }
    }

    //Loop through the shade settings
    if (model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size();i++){
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives(),this);
            settingRad->addRad(model->windowGroups()[blindGroupNum]->shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size()>0){
                //Create a file of the glazing layers with all BSDFs blacked out and simulate it
                RadFileData *settingStdRad=new RadFileData(settingRad->primitives(),this);
                for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size();j++){
                    if (!settingStdRad->blackOutLayer(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j])){
                        return false;
                    }
                }
                QString wgSettingFileStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_std.rad";
                files.replace(0,wgSettingFileStd);
                outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_std.oct";
                if (!createOctree(files, outFileName)){
                    return false;
                }

                //Call Standard Radiance run
                if (!simStandard(blindGroupNum,i,model)){
                    return false;
                }

                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size();j++){
                    std::vector<QString> layers=model->windowGroups()[blindGroupNum]->glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]);
                    QPair<stadic::RadFileData*, stadic::RadFileData*> splitGeo=settingRad->split(layers);
                    if (splitGeo.first==nullptr|| splitGeo.second==nullptr){
                        ERROR("The program quit...");
                        return false;
                    }
                    QString wgSettingFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_bsdf"+QString().sprintf("%g",j+1)+".rad";
                    splitGeo.first->writeRadFile(wgSettingFileBSDF);
                    std::vector<double> normal=splitGeo.first->surfaceNormal(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]);
                    QString thickness;
                    QString bsdfXML;
                    for (int k=0;k<splitGeo.first->primitives().size();k++){
                        if (splitGeo.first->primitives()[k]->name()==model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]){
                            thickness=splitGeo.first->primitives()[k]->getArg1(0);
                            bsdfXML=splitGeo.first->primitives()[k]->getArg1(1);
                        }
                    }
                    QString wgSettingFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_bsdf"+QString().sprintf("%g",j+1)+"_std.rad";
                    splitGeo.second->writeRadFile(wgSettingFileBSDFStd);
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j],model)){
                        ERROR("The program quit...");
                        return false;
                    }
                }
            }else{
                RadFileData *wgRad=new RadFileData(m_RadFiles[blindGroupNum]);
                wgRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum]->shadeSettingGeometry()[i]);
                QString wgSetFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g%",i+1)+".rad";
                wgRad->writeRadFile(wgSetFile);
                files.clear();
                files.append(wgSetFile);
                files.append(model->projectFolder()+model->tmpFolder()+"sky_white1.rad");
                outFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g%",i+1)+".oct";
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
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives(), this);
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum]->baseGeometry());
    QString wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);

    //BSDF run for each of the BSDFs
    if (model->windowGroups()[blindGroupNum]->bsdfBaseLayers().size()>0){
        //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
        for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfBaseLayers().size();j++){
            std::vector<QString> layers=model->windowGroups()[blindGroupNum]->glazingLayers();
            if (std::find(layers.begin(),layers.end(),model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j])==layers.end()){
                layers.push_back(model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j]);
            }
            QPair<stadic::RadFileData*, stadic::RadFileData*> splitGeo=baseRad->split(layers);
            if (splitGeo.first==nullptr|| splitGeo.second==nullptr){
                ERROR("The program quit...");
                return false;
            }
            QString wgBaseFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_bsdf"+QString().sprintf("%g",j+1)+".rad";
            splitGeo.first->writeRadFile(wgBaseFileBSDF);
            std::vector<double> normal=splitGeo.first->surfaceNormal(model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j]);
            QString thickness;
            QString bsdfXML;
            for (int k=0;k<splitGeo.first->primitives().size();k++){
                if (splitGeo.first->primitives()[k]->name()==model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j]){
                    thickness=splitGeo.first->primitives()[k]->getArg1(0);
                    bsdfXML=splitGeo.first->primitives()[k]->getArg1(1);
                }
            }
            QString wgBaseFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_bsdf"+QString().sprintf("%g",j+1)+"_std.rad";
            splitGeo.second->writeRadFile(wgBaseFileBSDFStd);
            if (!simBSDF(blindGroupNum,-1,j,wgBaseFileBSDF,wgBaseFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j],model)){
                ERROR("The program quit...");
                return false;
            }
        }
    }

    //Loop through the shade settings
    if (model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size();i++){
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives(),this);
            settingRad->addRad(model->windowGroups()[blindGroupNum]->shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size()>0){
                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size();j++){
                    std::vector<QString> layers=model->windowGroups()[blindGroupNum]->glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]);
                    QPair<stadic::RadFileData*, stadic::RadFileData*> splitGeo=settingRad->split(layers);
                    if (splitGeo.first==nullptr|| splitGeo.second==nullptr){
                        ERROR("The program quit...");
                        return false;
                    }
                    QString wgSettingFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_bsdf"+QString().sprintf("%g",j+1)+".rad";
                    splitGeo.first->writeRadFile(wgSettingFileBSDF);
                    std::vector<double> normal=splitGeo.first->surfaceNormal(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]);
                    QString thickness;
                    QString bsdfXML;
                    for (int k=0;k<splitGeo.first->primitives().size();k++){
                        if (splitGeo.first->primitives()[k]->name()==model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]){
                            thickness=splitGeo.first->primitives()[k]->getArg1(0);
                            bsdfXML=splitGeo.first->primitives()[k]->getArg1(1);
                        }
                    }
                    QString wgSettingFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_bsdf"+QString().sprintf("%g",j+1)+"_std.rad";
                    splitGeo.second->writeRadFile(wgSettingFileBSDFStd);
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j],model)){
                        ERROR("The program quit...");
                        return false;
                    }
                }
            }else{
                ERROR("Blind Group "+QString().sprintf("%g",blindGroupNum)+" setting "+QString().sprintf("%g",i)+ " does not contain a bsdf layer.");
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
    RadFileData *baseRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives(), this);
    baseRad->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[blindGroupNum]->baseGeometry());
    QString wgBaseFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base.rad";
    baseRad->writeRadFile(wgBaseFile);

    //BSDF run for each of the BSDFs
    if (model->windowGroups()[blindGroupNum]->bsdfBaseLayers().size()>0){
        //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
        for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfBaseLayers().size();j++){
            std::vector<QString> layers=model->windowGroups()[blindGroupNum]->glazingLayers();
            if (std::find(layers.begin(),layers.end(),model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j])==layers.end()){
                layers.push_back(model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j]);
            }
            QPair<stadic::RadFileData*, stadic::RadFileData*> splitGeo=baseRad->split(layers);
            if (splitGeo.first==nullptr|| splitGeo.second==nullptr){
                ERROR("The program quit...");
                return false;
            }
            QString wgBaseFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_bsdf"+QString().sprintf("%g",j+1)+".rad";
            splitGeo.first->writeRadFile(wgBaseFileBSDF);
            std::vector<double> normal=splitGeo.first->surfaceNormal(model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j]);
            QString thickness;
            QString bsdfXML;
            for (int k=0;k<splitGeo.first->primitives().size();k++){
                if (splitGeo.first->primitives()[k]->name()==model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j]){
                    thickness=splitGeo.first->primitives()[k]->getArg1(0);
                    bsdfXML=splitGeo.first->primitives()[k]->getArg1(1);
                }
            }
            QString wgBaseFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_bsdf"+QString().sprintf("%g",j+1)+"_std.rad";
            splitGeo.second->writeRadFile(wgBaseFileBSDFStd);
            if (!simBSDF(blindGroupNum,-1,j,wgBaseFileBSDF,wgBaseFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum]->bsdfBaseLayers()[j],model)){
                ERROR("The program quit...");
                return false;
            }
        }
    }
    //For the settings only run the last part of the calculation
    if (model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size()>0){
        for (unsigned int i=0;i<model->windowGroups()[blindGroupNum]->shadeSettingGeometry().size();i++){
            RadFileData *settingRad=new RadFileData(m_RadFiles[blindGroupNum]->primitives(),this);
            settingRad->addRad(model->windowGroups()[blindGroupNum]->shadeSettingGeometry()[i]);
            if (model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size()>0){
                //Loop through each of the BSDFs and remove it along with the glazing layers and simulate them with simBSDF
                for (int j=0;j<model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i].size();j++){
                    std::vector<QString> layers=model->windowGroups()[blindGroupNum]->glazingLayers();
                    layers.push_back(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]);
                    QPair<stadic::RadFileData*, stadic::RadFileData*> splitGeo=settingRad->split(layers);
                    if (splitGeo.first==nullptr|| splitGeo.second==nullptr){
                        ERROR("The program quit...");
                        return false;
                    }
                    QString wgSettingFileBSDF=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_bsdf"+QString().sprintf("%g",j+1)+".rad";
                    splitGeo.first->writeRadFile(wgSettingFileBSDF);
                    //std::vector<double> normal=splitGeo.first->surfaceNormal(model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]);
                    QString thickness;
                    QString bsdfXML;
                    for (int k=0;k<splitGeo.first->primitives().size();k++){
                        if (splitGeo.first->primitives()[k]->name()==model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j]){
                            thickness=splitGeo.first->primitives()[k]->getArg1(0);
                            bsdfXML=splitGeo.first->primitives()[k]->getArg1(1);
                        }
                    }
                    QString wgSettingFileBSDFStd=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+QString().sprintf("%g",i+1)+"_bsdf"+QString().sprintf("%g",j+1)+"_std.rad";
                    splitGeo.second->writeRadFile(wgSettingFileBSDFStd);
                    /*  This would be correct, but for time savings we don't have to run the entire calculation so the next steps are taken.
                    if (!simBSDF(blindGroupNum,i,j,wgSettingFileBSDF,wgSettingFileBSDFStd,normal,thickness,bsdfXML,model->windowGroups()[blindGroupNum]->bsdfSettingLayers()[i][j],model)){
                        ERROR("The program quit...");
                        return false;
                    }
                    */
                    //Create the blacked out rad file
                    QString mainFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_set"+i+"_bsdf"+j;
                    QString baseFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[blindGroupNum]->objectName()+"_base_bsdf"+j;
                    QProcess *xform=new QProcess(this);
                    QString xformProgram="xform.exe";
                    xform->setProgram(xformProgram);
                    QStringList arguments;
                    arguments.push_back("-m");
                    arguments.push_back("black");
                    arguments.push_back(wgSettingFileBSDFStd);
                    QString blackRad=mainFileName+"_allblack.rad";
                    xform->setStandardOutputFile(blackRad);
                    xform->setWorkingDirectory(model->projectFolder());
                    xform->setArguments(arguments);
                    xform->start();
                    if (!xform->waitForFinished(-1)){
                        ERROR("The xform command failed to convert layers to black.");
                        //I want to display the errors here if the standard error has any errors to show.
                        return false;
                    }
                    QStringList files;
                    files.append(blackRad);
                    QString blackOct=mainFileName+"_allblack.oct";
                    if(!createOctree(files,blackOct)){
                        return false;
                    }
                    QString sunsOct=baseFileName+"_suns.oct";
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
                    arguments.push_back("MF:"+QString().sprintf("%g",model->sunDivisions()));
                    arguments.push_back("-f");
                    arguments.push_back("klems_int.cal");
                    arguments.push_back("-b");
                    arguments.push_back("rbin");
                    arguments.push_back("-bn");
                    arguments.push_back("Nrbins");
                    arguments.push_back("-m");
                    arguments.push_back("solar");
                    arguments.push_back(sunsOct);
                    QString dirDSMX=mainFileName+"_5PH.dsmx";
                    QProcess *rcontrib=new QProcess(this);
                    rcontrib->setStandardOutputFile(dirDSMX);
                    rcontrib->setWorkingDirectory(model->projectFolder());
                    rcontrib->setStandardInputFile(model->dataFolder()+model->ptsFile());
                    rcontrib->setArguments(arguments);

                    rcontrib->start();
                    if (!rcontrib->waitForFinished(-1)){
                        ERROR("The rcontrib run for the 5-phase direct smx has failed with the following errors.");
                        //I want to display the errors here if the standard error has any errors to show.
                        return false;
                    }

                    //Create file names from base output

                    QString vmx=baseFileName+"_3PH.vmx";
                    QString dmx=baseFileName+"_3PH.dmx";
                    QString smx=baseFileName+"_3PH.smx";
                    QString dirDMX=baseFileName+"_3DIR.dmx";
                    QString dirVMX=baseFileName+"_3Dir.vmx";
                    QString dirSMX=baseFileName+"_3DIR.smx";
                    QString dir5PHsmx=baseFileName+"_5PH.smx";
                    //3Phase
                    //dctimestep | rcollate
                    QProcess *dctimestep=new QProcess(this);
                    QString dctimestepProgram="dctimestep.exe";
                    dctimestep->setProgram(dctimestepProgram);
                    arguments.clear();
                    arguments.append("-n");
                    arguments.append("8760");
                    arguments.append(vmx);
                    arguments.append(bsdfXML);
                    arguments.append(dmx);
                    arguments.append(smx);
                    dctimestep->setWorkingDirectory(model->projectFolder());

                    QProcess *rcollate=new QProcess(this);
                    QString rcollateProgram="rcollate.exe";
                    rcollate->setProgram(rcollateProgram);
                    dctimestep->setStandardOutputProcess(rcollate);
                    QStringList arguments2;
                    arguments2.append("-h");
                    arguments2.append("-fa");
                    arguments2.append("-oc");
                    arguments2.append("3");
                    rcollate->setArguments(arguments2);
                    QString threePhaseCollated=mainFileName+"_3ph.dat";
                    rcollate->setStandardOutputFile(threePhaseCollated);
                    rcollate->setWorkingDirectory(model->projectFolder());

                    dctimestep->start();
                    rcollate->start();

                    if(!rcollate->waitForFinished(-1)){
                        ERROR("The running of rcollate for the 3-phase has failed.");
                        //I want to display the errors here if the standard error has any errors to show.

                        return false;
                    }

                    //3Phase Direct
                    //dctimestep | rcollate
                    arguments.clear();
                    arguments.append("-n");
                    arguments.append("8760");
                    arguments.append(dirVMX);
                    arguments.append(bsdfXML);
                    arguments.append(dirDMX);
                    arguments.append(dirSMX);
                    dctimestep->setWorkingDirectory(model->projectFolder());

                    dctimestep->setStandardOutputProcess(rcollate);
                    arguments2.clear();
                    arguments2.append("-h");
                    arguments2.append("-fa");
                    arguments2.append("-oc");
                    arguments2.append("3");
                    rcollate->setArguments(arguments2);
                    QString threePhaseDirectCollated=mainFileName+"_3Dir.dat";
                    rcollate->setStandardOutputFile(threePhaseDirectCollated);
                    rcollate->setWorkingDirectory(model->projectFolder());

                    dctimestep->start();
                    rcollate->start();

                    if(!rcollate->waitForFinished(-1)){
                        ERROR("The running of rcollate for the 3-phase direct has failed.");
                        //I want to display the errors here if the standard error has any errors to show.

                        return false;
                    }

                    //5Phase
                    //dctimestep | rcollate
                    arguments.clear();
                    arguments.append("-n");
                    arguments.append("8760");
                    arguments.append(dirDSMX);
                    arguments.append(dir5PHsmx);
                    dctimestep->setWorkingDirectory(model->projectFolder());

                    dctimestep->setStandardOutputProcess(rcollate);
                    arguments2.clear();
                    arguments2.append("-h");
                    arguments2.append("-fa");
                    arguments2.append("-oc");
                    arguments2.append("3");
                    rcollate->setArguments(arguments2);
                    QString fivePhaseCollated=mainFileName+"_5PH.dat";
                    rcollate->setStandardOutputFile(fivePhaseCollated);
                    rcollate->setWorkingDirectory(model->projectFolder());

                    dctimestep->start();
                    rcollate->start();

                    if(!rcollate->waitForFinished(-1)){
                        ERROR("The running of rcollate for the 3-phase has failed.");
                        //I want to display the errors here if the standard error has any errors to show.

                        return false;
                    }

                    //Process final data into ill file
                    QProcess *rlam=new QProcess(this);
                    QString rlamProgram="rlam.exe";
                    rlam->setProgram(rlamProgram);
                    rlam->setWorkingDirectory(model->projectFolder());
                    arguments.clear();
                    arguments.append(threePhaseCollated);
                    arguments.append(threePhaseDirectCollated);
                    arguments.append(fivePhaseCollated);
                    rlam->setArguments(arguments);

                    QProcess *rcalc=new QProcess(this);
                    QString rcalcProgram="rcalc.exe";
                    rcalc->setProgram(rcalcProgram);
                    rcalc->setWorkingDirectory(model->projectFolder());
                    rlam->setStandardOutputProcess(rcalc);
                    arguments2.clear();
                    arguments2.append("-e");
                    arguments2.append("r=$1-$4+7;g=$2-$5+$8;b=$3-$6+$9");
                    arguments2.append("-e");
                    arguments2.append("ill=179*(.265*r+.670*g+.65*b)");
                    arguments2.append("-e");
                    arguments2.append("$1=floor(ill+.5)");
                    rcalc->setArguments(arguments2);

                    QStringList arguments3;
                    arguments3.append("-h");
                    arguments3.append("-fa");
                    arguments3.append("-or");
                    arguments3.append("8760");
                    arguments3.append("-t");
                    rcollate->setArguments(arguments3);
                    rcalc->setStandardOutputProcess(rcollate);
                    rcollate->setStandardOutputFile(mainFileName+".ill");

                    rlam->start();
                    rcalc->start();
                    rcollate->start();
                    if(!rcollate->waitForFinished(-1)){
                        ERROR("The running of rcollate for final illuminance has failed.");
                        //I want to display the errors here if the standard error has any errors to show.

                        return false;
                    }
                }
            }else{
                ERROR("Blind Group "+QString().sprintf("%g",blindGroupNum)+" setting "+QString().sprintf("%g",i)+ " does not contain a bsdf layer.");
                return false;
            }
            delete settingRad;
        }
    }
    return true;
}

bool Daylight::uniqueGlazingMaterials(Control *model){
    for (int i=0;i<model->windowGroups().size();i++){
        for (int j=0;j<model->windowGroups()[i]->glazingLayers().size();j++){
            for (int n=i;n<model->windowGroups().size();n++){
                for (int m=j+1;m<model->windowGroups()[n]->glazingLayers().size();m++){
                    if (model->windowGroups()[i]->glazingLayers()[j]==model->windowGroups()[n]->glazingLayers()[m]){
                        ERROR("The calculation cannot be performed because "+model->windowGroups()[i]->objectName()
                            +"\n\tcontains the same glazing layer as "+model->windowGroups()[n]->objectName()+".");
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
        if (model->windowGroups()[i]->isBSDF()){
            if (!setSimCase(i,2)){
                return false;
            }
            if (model->windowGroups()[i]->bsdfBaseLayers().size()!=0){					//If there are no BSDFs then this will remain material case 2
                bool GlazingBSDF=false;
                for (int j=0;j<model->windowGroups()[i]->bsdfBaseLayers().size();j++){
                    for (int k=0;k<model->windowGroups()[i]->glazingLayers().size();k++){
                        if (model->windowGroups()[i]->glazingLayers()[k]==model->windowGroups()[i]->bsdfBaseLayers()[j]){
                            GlazingBSDF=true;
                        }
                    }
                }
                if (model->windowGroups()[i]->bsdfSettingLayers().size()!=0){
                    if (model->windowGroups()[i]->shadeSettingGeometry().size()!=0){
                        for (int x=0;x<model->windowGroups()[i]->shadeSettingGeometry().size();x++){
                            for (int j=0;j<model->windowGroups()[i]->bsdfSettingLayers()[x].size();j++){
                                for (int k=0;k<model->windowGroups()[i]->glazingLayers().size();k++){
                                    if (model->windowGroups()[i]->glazingLayers()[k]==model->windowGroups()[i]->bsdfSettingLayers()[x][j]){
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
                }else if (model->windowGroups()[i]->shadeSettingGeometry().size()==0){
                    if (!setSimCase(i,6)){
                        return false;
                    }
                }else{
                    bool OnlyProxy=true;
                    bool OnlyGlazing=true;
                    for (int j=0;j<model->windowGroups()[i]->shadeSettingGeometry().size();j++){
                        QString tempString;
                        QFile iFile;
                        iFile.setFileName(QString(model->projectFolder()+model->windowGroups()[i]->shadeSettingGeometry()[j]));
                        iFile.open(QIODevice::ReadOnly | QIODevice::Text);
                        if (!iFile.exists()){
                            ERROR("The opening of the geometry file " +QString(model->projectFolder()+model->windowGroups()[i]->shadeSettingGeometry()[j]+" has failed."));
                            return false;
                        }
                        tempString=iFile.readLine();
                        iFile.close();
                        if (!tempString.contains("proxy")){				//If all files contain "# proxy" on the first line then it is material case 5
                            OnlyProxy=false;
                        }else if (!tempString.contains("exchange")){		//If all files contain "# exchange" on the first line then it is material case 6
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
}

bool Daylight::setSimCase(int setting, int simCase){
    if (setting>(m_SimCase.size()-1)){
        m_SimCase.reserve(setting+1);
    }else if (setting<0){
        ERROR("The setting of the simulation case failed.");
        return false;
    }
    m_SimCase.at(setting)=simCase;
    return true;
}

bool Daylight::writeSky(Control *model){
    QFile oFile;
    QString tmpFile=model->projectFolder()+model->tmpFolder()+"sky_white1.rad";
    oFile.setFileName(tmpFile);
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        ERROR("The opning of the file "+tmpFile +" has failed.");
        return false;
    }
    QTextStream out(&oFile);

    out<<"void glow sky_glow"<<endl<<"0 0 4 1 1 1 0"<<endl<<endl;
    out<<"sky_glow source sky"<<endl<<"0 0 4 0 0 1 180"<<endl<<endl;
    out<<"sky_glow source ground1"<<endl<<"0 0 4 0 0 -1 180"<<endl;
    oFile.close();

    tmpFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_suns.rad";
    oFile.setFileName(tmpFile);
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        ERROR("The opning of the file "+tmpFile +" has failed.");
        return false;
    }
    out.setDevice(&oFile);
    out<<"void light solar 0 0 3 1.00e+06 1.00e+06 1.00e+06";
    oFile.close();

}

bool Daylight::createBaseRadFiles(Control *model){
    RadFileData radModel;
    //Add the main material file to the primitive list
    radModel.addRad(model->projectFolder()+model->geoFolder()+model->matFile());
    RadPrimitive *black = new PlasticMaterial(&radModel);
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
        RadFileData *wgRadModel = new RadFileData(radModel.primitives(),this); // Careful! Stack allocation!
        //wgRadModel.addRad(tempFile);
        for (int j=0;j<model->windowGroups().size();j++){
            if (i!=j){
                if(!wgRadModel->addRad(model->projectFolder()+model->geoFolder()+model->windowGroups()[j]->baseGeometry())){
                    return false;
                }
                for (int k=0;k<model->windowGroups()[j]->glazingLayers().size();k++){
                    if(!wgRadModel->blackOutLayer(model->windowGroups()[j]->glazingLayers()[k])){
                        return false;
                    }
                }
            }
        }
        /*  This section not needed due to next line
        QString wgFile;
        wgFile=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_Main.rad";
        if (!wgRadModel->writeRadFile(wgFile)){
            return false;
        }
        */
        m_RadFiles.push_back(wgRadModel);
    }
    return true;
}

bool Daylight::createOctree(QStringList files, QString octreeName){
    QProcess *oconv=new QProcess(this);
    QString oconvProgram="oconv.exe";
    oconv->setProgram(oconvProgram);
    oconv->setArguments(files);
    oconv->setStandardOutputFile(octreeName);

    oconv->start();

    if (!oconv->waitForFinished(-1)){
        ERROR("The creation of the octree has failed with the following errors.");
        //I want to display the errors here if the standard error has any errors to show.

        return false;
    }
    return true;
}

bool Daylight::sumIlluminanceFiles(Control *model){
    QString FinalIllFileName;
    QString tempFileName;
    for (int i=0;i<model->windowGroups().size();i++){
        //Base Illuminance files
        FinalIllFileName=model->projectFolder()+model->resultsFolder()+model->projectName()+model->windowGroups()[i]->objectName()+"_base.ill";
        tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_base_ill.tmp";
        DaylightIlluminanceData *baseIll=new DaylightIlluminanceData(this);
        QFile checkFile(tempFileName);
        if (checkFile.exists()){
            if (!baseIll->parse(tempFileName,model->weaDataFile())){
                return false;
            }
            if (model->windowGroups()[i]->bsdfBaseLayers().size()>0){
                for (int j=0;j<model->windowGroups()[i]->bsdfBaseLayers().size();j++){
                    tempFileName==model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_base_bsdf"+j+".ill";
                    checkFile.setFileName(tempFileName);
                    if (checkFile.exists()){
                        if (!baseIll->addIllFile(tempFileName)){
                            return false;
                        }
                    }
                }
            }
            baseIll->writeIllFile(FinalIllFileName);
        }else{
            tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_base_bsdf0.ill";
            checkFile.setFileName(tempFileName);
            if (checkFile.exists()){
                if (!baseIll->parse(tempFileName, model->weaDataFile())){
                    return false;
                }
            }else{
                ERROR("The illuminance file "+tempFileName+" does not exist.");
                return false;
            }
            if (model->windowGroups()[i]->bsdfBaseLayers().size()>1){
                for (int j=1;j<model->windowGroups()[i]->bsdfBaseLayers().size();j++){
                    tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_base_bsdf"+j+".ill";
                    if (!baseIll->addIllFile(tempFileName)){
                        return false;
                    }
                }
            }
            baseIll->writeIllFile(FinalIllFileName);
        }
        for (int j=0;j<model->windowGroups()[i]->shadeSettingGeometry().size();j++){
            DaylightIlluminanceData *settingIll=new DaylightIlluminanceData(this);
            tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_set"+QString().sprintf("%g",(j+1))+"_ill_std.tmp";
            FinalIllFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_set"+QString().sprintf("%g",(j+1))+".ill";
            checkFile.setFileName(tempFileName);
            if (checkFile.exists()){
                if (!settingIll->parse(tempFileName,model->weaDataFile())){
                    return false;
                }
                if (model->windowGroups()[i]->bsdfSettingLayers()[j].size()!=0){
                    for (int k=0;k<model->windowGroups()[i]->bsdfSettingLayers()[j].size();k++){
                        tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_set"+j+"_bsdf"+QString().sprintf("%g",k)+".ill";
                        settingIll->addIllFile(tempFileName);
                    }
                }
                settingIll->writeIllFile(FinalIllFileName);
            }else{
                tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_set"+j+"_bsdf0.ill";
                checkFile.setFileName(tempFileName);
                if (checkFile.exists()){
                    if (!settingIll->parse(tempFileName,model->weaDataFile())){
                        return false;
                    }
                    if (model->windowGroups()[i]->bsdfSettingLayers()[j].size()!=1){
                        for (int k=1;k<model->windowGroups()[i]->bsdfSettingLayers()[j].size();k++){
                            tempFileName=model->projectFolder()+model->tmpFolder()+model->projectName()+"_"+model->windowGroups()[i]->objectName()+"_set"+j+"_bsdf"+QString().sprintf("%g",k)+".ill";
                            settingIll->addIllFile(tempFileName);
                        }
                    }
                    settingIll->writeIllFile(FinalIllFileName);
                }else{
                    ERROR("The illuminance file "+tempFileName+" does not exist.");
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
