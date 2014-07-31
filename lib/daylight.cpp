#include "daylight.h"
#include "logging.h"
#include <QFile>
#include <QTextStream>
#include "radfiledata.h"

namespace stadic {

Daylight::Daylight(QObject *parent) :
    QObject(parent)
{
}

bool Daylight::simDaylight(stadic::Control &model){
    if (!uniqueGlazingMaterials(model)){
        return false;
    }

    if (!testSimCase(model)){
        return false;
    }

    bool BSDFs=false;
    for (int i=0;i<m_SimCase.size();i++){
        if (m_SimCase[i]>0){
            BSDFs=true;
        }
    }
    if (!writeSky(model)){
        return false;
    }

}

//Private
bool Daylight::simBSDF(int blindGroupNum, int setting, int bsdfNum){
    return true;
}

bool Daylight::simStandard(int blindGroupNum, int setting){
    return true;
}

bool Daylight::uniqueGlazingMaterials(stadic::Control &model){
    for (int i=0;i<model.windowGroups().size();i++){
        for (int j=0;j<model.windowGroups()[i]->glazingLayers().size();j++){
            for (int n=i;n<model.windowGroups().size();n++){
                for (int m=j+1;m<model.windowGroups()[n]->glazingLayers().size();m++){
                    if (model.windowGroups()[i]->glazingLayers()[j]==model.windowGroups()[n]->glazingLayers()[m]){
                        ERROR("The calculation cannot be performed because "+model.windowGroups()[i]->objectName()
                            +"\n\tcontains the same glazing layer as "+model.windowGroups()[n]->objectName()+".");
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool Daylight::testSimCase(stadic::Control &model){
    //	Simulation cases are based on whether the control file contains a BSDF for a particular window group
    //	The following loop will change the simulation case for each window group.
    //  Simulation Case 1 will be for window groups that do not contain BSDFs
    //	Simulation case 2 will be for window groups that contain BSDFs, but not in the base case
    //	Simulation case 3 will be for window groups that contain BSDFs even in the base case, but the glazing layers are not BSDFs
    //	Simulation case 4 will be for window groups that have shade materials in addition to the glazing layer
    //	Simulation case 5 will be for window groups that have added geometry, but it is a proxy geometry
    //	Simulation case 6 will be for window groups that only have the glazing layer as a BSDF

    for (int i=0;i<model.windowGroups().size();i++){
        if (model.windowGroups()[i]->isBSDF()){
            if (!setSimCase(i,2)){
                return false;
            }
            if (model.windowGroups()[i]->bsdfBaseLayers().size()!=0){					//If there are no BSDFs then this will remain material case 2
                bool GlazingBSDF=false;
                for (int j=0;j<model.windowGroups()[i]->bsdfBaseLayers().size();j++){
                    for (int k=0;k<model.windowGroups()[i]->glazingLayers().size();k++){
                        if (model.windowGroups()[i]->glazingLayers().at(k)==model.windowGroups()[i]->bsdfBaseLayers().at(j)){
                            GlazingBSDF=true;
                        }
                    }
                }
                if (GlazingBSDF==false){					//If the glazing layer is not a BSDF then this will be material case 3
                    if (!setSimCase(i,3)){
                        return false;
                    }
                }else if (model.windowGroups()[i]->shadeSettingGeometry().size()==0){
                    if (!setSimCase(i,6)){
                        return false;
                    }
                }else{
                    bool OnlyProxy=true;
                    bool OnlyGlazing=true;
                    for (int j=0;j<model.windowGroups()[i]->shadeSettingGeometry().size();j++){

                        QString tempString;
                        QFile iFile;
                        iFile.setFileName(QString(model.projectFolder()+model.windowGroups()[i]->shadeSettingGeometry()[j]));
                        iFile.open(QIODevice::ReadOnly | QIODevice::Text);
                        if (!iFile.exists()){
                            ERROR("The opening of the geometry file " +QString(model.projectFolder()+model.windowGroups()[i]->shadeSettingGeometry()[j]+" has failed."));
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

bool Daylight::writeSky(stadic::Control &model){
    QFile oFile;
    QString tmpFile=model.projectFolder()+model.tmpFolder()+"sky_white1.rad";
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

    tmpFile=model.projectFolder()+model.tmpFolder()+model.projectName()+"_suns.rad";
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

bool Daylight::createBaseRadFiles(stadic::Control &model){
    stadic::RadFileData radModel;
    //Add the main material file to the primitive list
    radModel.addRad(model.projectFolder()+model.geoFolder()+model.matFile());
    //Create a file of a black material
    QFile oFile;
    QString tempFile=model.projectFolder()+model.geoFolder()+"blackmat.rad";
    oFile.setFileName(tempFile);
    oFile.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!oFile.exists()){
        ERROR("There was an error opening the file "+tempFile+".");
        return false;
    }
    QTextStream out(&oFile);
    out<<"void plastic black"<<endl;
    out<<"0"<<endl<<"0"<<endl<<"5 0 0 0 0 0"<<endl;
    oFile.close();
    //Add the black material to the primitive list
    radModel.addRad(model.projectFolder()+model.geoFolder()+"blackmat.rad");
    //Add the main geometry file to the primitive list
    radModel.addRad(model.projectFolder()+model.geoFolder()+model.geoFile());

    //Create main rad files for each of the window groups
        //The window group rad file will contain the base rad files and each of the other
        //base rad files except their own.  The glazing layers for the other groups will
        //be blacked out.
    for (int i=0;i<model.windowGroups().size();i++){
        /*stadic::RadFileData wgRadModel=radModel;
        for (int j=0;j<model.windowGroups().size();j++){
            if (i!=j){
                if(!wgRadModel.addRad(model.projectFolder()+model.geoFolder()+model.windowGroups()[j]->baseGeometry())){
                    return false;
                }
                for (int k=0;k<model.windowGroups()[j]->glazingLayers().size();k++){
                    if(!wgRadModel.blackOutLayer(model.windowGroups()[j]->glazingLayers()[k])){
                        return false;
                    }
                }
            }
        }
        tempFile=model.projectFolder()+model.tmpFolder()+model.projectName()+"_"+model.windowGroups()[i]->objectName()+"_Main.rad";
        if (!wgRadModel.writeRadFile(tempFile)){
            return false;
        }
        */
    }
    return true;

    /*
    //*******************************************************************
    //	Section of the code for creating the main rad file
    //*******************************************************************
    RadIn.open(string(material_file));														//	open the main material file for placing at the beginning of the main dds rad file
    if (RadIn.fail()){																		//	test that the file exists
        cout << "The main material file could not be opened." << endl;
        cout << "The program will assume the materials are in the geometry file."<<endl;
    }
    MainRadFileName=string(project_directory)+"tmp/"+string(project_name)+"_base.rad";
    RadOut.open(MainRadFileName);														//	open the output file for the main dds rad file
    //black material for the BSDF runs.
    RadOut<<"void plastic black"<<endl;
    RadOut<<"0"<<endl<<"0"<<endl<<"5 0 0 0 0 0"<<endl;

    while (getline(RadIn,PassString)){  //	Pulls each line out one by one
        ss << PassString;				//	Places the string into the string stream
        while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
            RadOut<<tempchar;
        }
        ss.clear();
        RadOut<< endl;
    }
    RadIn.close();

    RadIn.open(string(geometry_file));														//	open the main geometry file for placing after the main material file
    if (RadIn.fail()){																		//	test that the file exists
        cout << "The main geometry file could not be opened." << endl;
        cout<< "Please specify your geometry before running the sDA analysis." << endl;
        cout<< "The program will now close.";
        exit(1);
    }
    while (getline(RadIn,PassString)){  //	Pulls each line out one by one
        ss << PassString;				//	Places the string into the string stream
        while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
            RadOut<<tempchar;
        }
        ss.clear();
        RadOut<< endl;
    }
    RadIn.close();
    RadOut.close();
    #pragma endregion

    #pragma region "Window Group Main Geometry Files"
    //Section of the code to create the main radiance file for each of the window groups
    //These files contain the main base geometry + the base case geometry from each of the other window groups (the other window groups will be blacked out)
    for (int i=1;i<=NumBlindSettings;i++){														//	loop through each of the base case rad files
        RadIn.open(MainRadFileName);
        cout<<"Attempting to open the Rad In file."<<endl;
        if (RadIn.fail()){
            cout<<"The opening of the temporary main rad file for has failed."<<endl;
            cout<<"The program will now close."<<endl;
            exit(1);
        }
        RadOut.open(string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.tmp");
        cout<<"Attempting to Open the Rad Out file."<<endl;
        if (RadOut.fail()){
            cout<<"The first attempt at opening the temporary main rad file for "<<string(BlindGroupName[i])<<" has failed."<<endl;
            cout<<"The program will now attempt to close any open streams and re-open the file."<<endl;
            RadOut.close();
            RadOut.open(string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.tmp");
            if (RadOut.fail()){
                cout<<"The opening of the temporary main rad file for "<<string(BlindGroupName[i])<<" has still failed."<<endl;
                cout<<"The program will now close."<<endl;
                exit(1);
            }
        }
        while (getline(RadIn,PassString)){  //	Pulls each line out one by one
            ss << PassString;				//	Places the string into the string stream
            while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
                RadOut<<tempchar;
            }
            ss.clear();
            RadOut<< endl;
        }
        RadIn.close();
        cout<<"The Rad In file has been closed."<<endl;

        for (int j=1;j<=NumBlindSettings;j++){
            if (i!=j){
                RadInFileName=string(project_directory)+string(BlindGroupGeometryInRadiance[0][j]);		//	get the file name for the base case rad file
                RadIn.open(RadInFileName);																//	open the rad file for placing into the main dds rad file
                cout<<"Attempting to open the Rad In file."<<endl;
                if (RadIn.fail()){																		//	test that the file exists
                    cout << "The geometry file " << RadInFileName << " could not be opened." << endl;
                    cout<< "Please specify your geometry before the simulation." << endl;
                    cout<< "The program will now close.";
                    exit(1);
                }
                while (getline(RadIn,PassString)){  //	Pulls each line out one by one
                    ss << PassString;				//	Places the string into the string stream
                    while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
                    RadOut<<tempchar;
                    }
                    ss.clear();
                RadOut<< endl;
                }
                RadIn.close();
                cout<<"The Rad In file has been closed."<<endl;
            }
            //RadOut.close();			//Changed the location of this to two lines below.
        }
        RadOut.close();
        cout<<"The Rad Out file has been closed."<<endl;
        RadIn.open(string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.tmp");
        cout<<"Attempting to open the Rad In file."<<endl;
        if (RadIn.fail()){
            cout<<"The opening of the temporary main rad file for "<<string(BlindGroupName[i])<<" has failed."<<endl;
            cout<<"The program will now close."<<endl;
            exit(1);
        }
        RadOut.open(string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad");

        RadIn>>PassString;
        while (!RadIn.eof()){
            //RadIn>> PassString;
            if (PassString.find("#") != string::npos){
                getline(RadIn, PassString);
            }else{
                if (PassString=="void"){
                    MatFound=false;
                    RadIn>>PassString;
                    RadIn>>PassString2;
                    for (int n=1;n<=NumBlindSettings;n++){							//loop through window groups
                        if (n!=i){													//skip the window group that this is being created for
                            for (int j=1;j<=NumGlazingMaterials[i]; j++){			//loop through glazing materials in those groups
                                if (PassString2==GlazingMaterials[n][j]){			//if the layer in the material file equals the glazing layer of another window group
                                    MatFound=true;
                                    RadOut<<"void plastic "<<PassString2<<endl;
                                    RadOut<<"0 0 5 0 0 0 0 0"<<endl;
                                }
                            }
                        }
                    }
                    if (MatFound==false){
                        RadOut<<"void "<<PassString<<" "<<PassString2<<endl;
                        RadIn>>PassString;
                        RadOut<<PassString;
                        if (PassString!="0"){									// Section to read the first line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                                RadOut<<" "<<PassString2;
                            }
                        }
                        RadOut<<endl;
                        RadIn>>PassString;
                        RadOut<<PassString;
                        if (PassString!="0"){									// Section to read the second line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                                RadOut<<" "<<PassString2;
                            }
                        }
                        RadOut<<endl;
                        RadIn>>PassString;
                        RadOut<<PassString;
                        if (PassString!="0"){									// Section to read the third line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                                RadOut<<" "<<PassString2;
                            }
                        }
                        RadOut<<endl;
                    }else{
                        RadIn>>PassString;
                        if (PassString!="0"){									// Section to read the first line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                            }
                        }
                        RadIn>>PassString;
                        if (PassString!="0"){									// Section to read the second line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                            }
                        }
                        RadIn>>PassString;
                        if (PassString!="0"){									// Section to read the third line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                            }
                        }
                    }
                }else{
                    RadOut<<PassString<<" ";		//Write Modifier
                    RadIn>>PassString;				//Read Material Type
                    RadOut<<PassString<<" ";		//Write Material Type
                    RadIn>>PassString;				//Read Identifier
                    RadOut<<PassString<<endl;		//Write Identifier
                    RadIn>>PassString;
                    RadOut<<PassString;
                    if (PassString!="0"){									// Section to read the first line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                            RadOut<<" "<<PassString2;
                        }
                    }
                    RadOut<<endl;
                    RadIn>>PassString;
                    RadOut<<PassString;
                    if (PassString!="0"){									// Section to read the second line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                            RadOut<<" "<<PassString2;
                        }
                    }
                    RadOut<<endl;
                    RadIn>>PassString;
                    RadOut<<PassString;
                    if (PassString!="0"){									// Section to read the third line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                            RadOut<<" "<<PassString2;
                        }
                    }
                    RadOut<<endl;
                }
            }
            RadIn>>PassString;
        }
        RadIn.close();
    }
    */
}

}
/*
//*************************************************************************************************************
//	Craig Casey
//	The Pennsylvania State University
//
//	Description:		This program produces a batch file to run the daylight illuminance calculations
//	Input:				This program takes the following input (Files names include file path and extension):
//							(1) Header file
//	Process:			The program will produce header files based on the static system runs for each shade
//							condition.  These will each be ran individually through a batch file.
//	Output:				The program returns the illuminance files for each setting for each window group
//**************************************************************************************************************

# include <iostream>		//	Pre-processor directive to allow for input output stream
# include <fstream>			//	Pre-processor directive to allow for streaming data to and from files
# include <string>			//	Pre-processor directive to allow for use of strings
# include <stdlib.h>		//	Pre-processor directive to allow for type casting from string to doubles or integers
# include <sstream>			//	Pre-processor directive to allow for string streams
# include <cstdlib>			//	Pre-processor directive to allow for the exit command
# include <iomanip>			//	Pre-processor directive to allow for manipulation of the input/output stream
using namespace std;

extern "C"{
# include "c:/CPrograms/lib/read_in_header.h"
}
extern "C"{
    void read_in_header(char *header_file);
}
//Function Prototypes
string WriteBSDFBat(int BlindGroupNum, int Setting, string RadFileName, int BSDFNum, string GlassRad, string BSDFRad, string HeaName);
void RemoveLayerFunc(string InRadName, string OutRadName, string LayerName, string OutLayerName);
void AddTwoRadFiles(string inRadOne, string inRadTwo, string OutRadName);
void BlackOutLayer(string InRadName, string OutRadName, string LayerName);

//MAIN PROGRAM
int main(int argc, char *argv[])
{
    string HeaderFileName(argv[1]);						//	Name of the header file
    //read in header.c
    read_in_header((char*)HeaderFileName.c_str());
    cout<<"The header file has been read in."<<endl;
    cout<<"The number of Blind Groups is "<<NumBlindSettings<<"."<<endl;
    if (NumBlindSettings>10){
        cout<< "The maximum number of window groups allowed for the calculation is 10."<< endl;
        cout<< "The program will now close.";
        exit(1);
    }

#pragma region "Variale Declaration"
    //*******************************************************************
    //	Section of the code for variable declaration
    //*******************************************************************
    cout<<"I will begin declaring the variables."<<endl;
    //Integers and Doubles
    int NumIntervals((60/time_step)*8760);	//	Number of time intervals in the year
    cout<<"Number of intervals is "<<NumIntervals<<"."<<endl;
    double ** Illuminance;								//	Variable for holding the illuminance for each sensor for summing each setting
    Illuminance= new double * [NumIntervals];
    for (int i=0;i<NumIntervals;i++){
        Illuminance[i]=new double [number_of_sensors];
    }
    double TempIllum;
    cout<<"The integers and doubles are declared."<<endl;
    int *NewMaterialCase;								//	Holds the new material case for BSDFs and Material Case 1
    NewMaterialCase= new int [NumBlindSettings+1];

    //Booleans
    bool EndOfHeader=false;								//	Boolean for testing whether the end of the header file has been reached.
    bool MatFound=false;
    bool GlazingBSDF=false;
    bool OnlyProxy=false;								//	Boolean for testing whether the shade setting radiance file is only proxy geometry
    bool OnlyGlazing=false;								//	Boolean for testing whether the shade setting radiance file is only a new BSDF material for a window
    bool BSDFs=false;									//	Boolean for testing whether the model uses BSDFs

    cout<<"The booleans are declared."<<endl;

    //Strings
    string PassString;									//	Variable to pass lines to from the header file.
    string PassString2;
    string DCFileNames;
    string IllFileNames;
    string FileName;									//	Variable for filenames
    string MainGeometry;								//	Variable for holding the main geometry file name with the base cases attached to it
    string DDSNewFileName;								//	Variable for holding the DDS file names for the last two lines of the header files
    string CorrectFileName;								//	Variable to correct the filenames
    string TestFileName;								//	Name of the test files for the TestFiles Stream object
    string MainRadFileName;								//
    string RadOutFileName;
    string RadInFileName;
    string AddToBat;
    string File1;
    string File2;
    string File3;
    string **DateTime;
    DateTime= new string * [NumIntervals];
    for (int i=0;i<NumIntervals;i++){
        DateTime[i]= new string [3];
    }

    cout<<"The strings are declared."<<endl;

    //Chars
    char tempchar;										//	Variable for fixing the file path slash direction


    cout<<"The chars are declared."<<endl;

    //Objects
    ifstream HeaFileIn;									//	Input file stream for the header file
    ifstream RadIn;										//	Input file stream for the rad files
    ofstream RadOut;									//	Ouptut file stream for the rad files
    ofstream HeaFileOut;								//	Output file stream for the header file
    ofstream BatchFileOut;								//	Output file stream for the batch file
    stringstream ss;									//	String stream to pass each part of a line from
    stringstream FixString;								//	String stream to fix file path slash directions
    ofstream FivePhaseOut;								//	Output file stream for five phase
    ofstream GlassRadOut;								//	Output file stream for the glazing materials
    ifstream TempIn;
    ofstream TempOut;

    cout<<"The streams are declared."<<endl;

# pragma endregion

    cout<<"Finished declaring variables."<<endl;
# pragma region "Test for duplicate glazing material names"
    cout<<"Testing for duplicate glazing material names."<<endl;
    for (int i=0;i<NumBlindSettings;i++){
        for (int j=0;j<NumGlazingMaterials[i];j++){
            for (int n=i;n<NumBlindSettings;n++){
                for (int m=j+1;m<NumGlazingMaterials[n];m++){
                    if (GlazingMaterials[i][j]==GlazingMaterials[n][m]){
                        cout<< "The calculation cannot be performed because " << string(BlindGroupName[i+1]) << " setting #" << i+1 << endl;
                        cout << " material name was the same as " << string(BlindGroupName[n+1]) << " setting #" << n+1 << "."<< endl;
                        cout<< "The program will now close.";
                        exit(1);
                    }
                }
            }
        }
    }
#pragma endregion
    cout<<"No duplicates found."<<endl;

#pragma region "Test for BSDF Material Case"
    cout<<"Testing for material case."<<endl;
    //	Material cases in the header file determine whether there are any BSDFs present with 2 meaning there are BSDFs and 1 meaning there are not
    //	The following loop will change the material case for each window group while this program runs.
    //	Material case 2 will be for window groups that contain BSDFs, but not in the base case
    //	Material case 3 will be for window groups that contain BSDFs even in the base case, but the glazing layers are not BSDFs
    //	Material case 4 will be for window groups that have shade materials in addition to the glazing layer
    //	Material case 5 will be for window groups that have added geometry, but it is a proxy geometry
    //	Material case 6 will be for window groups that only have the glazing layer as a BSDF
    cout<<"The number of blind groups is "<<NumBlindSettings<<"."<<endl;

    for (int i=1;i<=NumBlindSettings;i++){
        cout<<"The material case is matl_case "<<MaterialCase[i]<<"."<<endl;
        if (MaterialCase[i]==2){
            NewMaterialCase[i]=2;
            BSDFs=true;
            cout<<"The material case is matl_case "<<NewMaterialCase[i]<<"."<<endl;
            cout<<"The number of BSDF materials is "<<NumBSDFMaterials[i][0]<<"."<<endl;
            if (NumBSDFMaterials[i][0]!=0){					//If there are no BSDFs then this will remain material case 2
                GlazingBSDF=false;
                for (int j=0;j<NumBSDFMaterials[i][0];j++){
                    for (int k=0;k<NumGlazingMaterials[i];k++){
                        if (string(GlazingMaterials[i][k])==string(BSDFExchangeMaterials[i][0][j])){
                            GlazingBSDF=true;
                        }
                    }
                }
                if (GlazingBSDF==false){					//If the glazing layer is not a BSDF then this will be materical case 3
                    NewMaterialCase[i]=3;
                }else if (NumberOfSettingsInBlindgroup[i]==0){
                    NewMaterialCase[i]=6;
                }else{
                    for (int j=0;j<NumberOfSettingsInBlindgroup[i];j++){
                        OnlyProxy=true;
                        OnlyGlazing=true;
                        TempIn.open(BlindGroupGeometryInRadiance[j+1][i]);
                        TempIn>>PassString;
                        TempIn>>PassString;
                        if (PassString!="proxy"){				//If all files contain "# proxy" on the first line then it is material case 5
                            OnlyProxy=false;
                        }else if (PassString!="exchange"){		//If all files contain "# exchange" on the first line then it is material case 6
                            OnlyGlazing=false;
                        }
                        TempIn.close();
                        if (OnlyProxy==true){
                            NewMaterialCase[i]=5;					//If there is added geometry, but only within the thickness
                        }else if (OnlyGlazing==true){
                            NewMaterialCase[i]=6;					//If only the glazing layers are BSDFs
                        }else{
                            NewMaterialCase[i]=4;					//If there is added geometry outside of the thickness
                        }
                    }
                }
            }
        }else{
            cout<<"The material case is case 1."<<endl;
            NewMaterialCase[i]=1;
        }
        cout<<"The final material case for this group is "<<NewMaterialCase[i]<<"."<<endl;
    }
#pragma endregion


#pragma region "Write sky and ground rad files if needed"
    if (BSDFs==true){
            RadOut.open(string(project_directory)+"tmp/sky_white1.rad");
            if (RadOut.fail()){
                cout<<"The creation of the sky geometry file has failed.";
            }
            RadOut<<"void glow sky_glow"<<endl<<"0 0 4 1 1 1 0"<<endl<<endl;
            RadOut<<"sky_glow source sky"<<endl<<"0 0 4 0 0 1 180"<<endl<<endl;
            RadOut<<"sky_glow source ground1"<<endl<<"0 0 4 0 0 -1 180"<<endl;
            RadOut.close();

            RadOut.open(string(project_directory)+"tmp/"+string(project_name)+"_suns.rad");
            if (RadOut.fail()){
                cout<<"The creation of the sun geometry file has failed.";
            }
            RadOut<<"void light solar 0 0 3 1.00e+06 1.00e+06 1.00e+06";
            RadOut.close();
    }

#pragma endregion


#pragma region "Create base rad files"
    cout<<"The program is creating the main radiance files."<<endl<<endl;
    #pragma region "Main Rad File"
    //*******************************************************************
    //	Section of the code for creating the main rad file
    //*******************************************************************
    RadIn.open(string(material_file));														//	open the main material file for placing at the beginning of the main dds rad file
    if (RadIn.fail()){																		//	test that the file exists
        cout << "The main material file could not be opened." << endl;
        cout << "The program will assume the materials are in the geometry file."<<endl;
    }
    MainRadFileName=string(project_directory)+"tmp/"+string(project_name)+"_base.rad";
    RadOut.open(MainRadFileName);														//	open the output file for the main dds rad file
    //black material for the BSDF runs.
    RadOut<<"void plastic black"<<endl;
    RadOut<<"0"<<endl<<"0"<<endl<<"5 0 0 0 0 0"<<endl;

    while (getline(RadIn,PassString)){  //	Pulls each line out one by one
        ss << PassString;				//	Places the string into the string stream
        while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
            RadOut<<tempchar;
        }
        ss.clear();
        RadOut<< endl;
    }
    RadIn.close();

    RadIn.open(string(geometry_file));														//	open the main geometry file for placing after the main material file
    if (RadIn.fail()){																		//	test that the file exists
        cout << "The main geometry file could not be opened." << endl;
        cout<< "Please specify your geometry before running the sDA analysis." << endl;
        cout<< "The program will now close.";
        exit(1);
    }
    while (getline(RadIn,PassString)){  //	Pulls each line out one by one
        ss << PassString;				//	Places the string into the string stream
        while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
            RadOut<<tempchar;
        }
        ss.clear();
        RadOut<< endl;
    }
    RadIn.close();
    RadOut.close();
    #pragma endregion

    #pragma region "Window Group Main Geometry Files"
    //Section of the code to create the main radiance file for each of the window groups
    //These files contain the main base geometry + the base case geometry from each of the other window groups (the other window groups will be blacked out)
    for (int i=1;i<=NumBlindSettings;i++){														//	loop through each of the base case rad files
        RadIn.open(MainRadFileName);
        cout<<"Attempting to open the Rad In file."<<endl;
        if (RadIn.fail()){
            cout<<"The opening of the temporary main rad file for has failed."<<endl;
            cout<<"The program will now close."<<endl;
            exit(1);
        }
        RadOut.open(string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.tmp");
        cout<<"Attempting to Open the Rad Out file."<<endl;
        if (RadOut.fail()){
            cout<<"The first attempt at opening the temporary main rad file for "<<string(BlindGroupName[i])<<" has failed."<<endl;
            cout<<"The program will now attempt to close any open streams and re-open the file."<<endl;
            RadOut.close();
            RadOut.open(string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.tmp");
            if (RadOut.fail()){
                cout<<"The opening of the temporary main rad file for "<<string(BlindGroupName[i])<<" has still failed."<<endl;
                cout<<"The program will now close."<<endl;
                exit(1);
            }
        }
        while (getline(RadIn,PassString)){  //	Pulls each line out one by one
            ss << PassString;				//	Places the string into the string stream
            while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
                RadOut<<tempchar;
            }
            ss.clear();
            RadOut<< endl;
        }
        RadIn.close();
        cout<<"The Rad In file has been closed."<<endl;

        for (int j=1;j<=NumBlindSettings;j++){
            if (i!=j){
                RadInFileName=string(project_directory)+string(BlindGroupGeometryInRadiance[0][j]);		//	get the file name for the base case rad file
                RadIn.open(RadInFileName);																//	open the rad file for placing into the main dds rad file
                cout<<"Attempting to open the Rad In file."<<endl;
                if (RadIn.fail()){																		//	test that the file exists
                    cout << "The geometry file " << RadInFileName << " could not be opened." << endl;
                    cout<< "Please specify your geometry before the simulation." << endl;
                    cout<< "The program will now close.";
                    exit(1);
                }
                while (getline(RadIn,PassString)){  //	Pulls each line out one by one
                    ss << PassString;				//	Places the string into the string stream
                    while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
                    RadOut<<tempchar;
                    }
                    ss.clear();
                RadOut<< endl;
                }
                RadIn.close();
                cout<<"The Rad In file has been closed."<<endl;
            }
            //RadOut.close();			//Changed the location of this to two lines below.
        }
        RadOut.close();
        cout<<"The Rad Out file has been closed."<<endl;
        RadIn.open(string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.tmp");
        cout<<"Attempting to open the Rad In file."<<endl;
        if (RadIn.fail()){
            cout<<"The opening of the temporary main rad file for "<<string(BlindGroupName[i])<<" has failed."<<endl;
            cout<<"The program will now close."<<endl;
            exit(1);
        }
        RadOut.open(string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad");

        RadIn>>PassString;
        while (!RadIn.eof()){
            //RadIn>> PassString;
            if (PassString.find("#") != string::npos){
                getline(RadIn, PassString);
            }else{
                if (PassString=="void"){
                    MatFound=false;
                    RadIn>>PassString;
                    RadIn>>PassString2;
                    for (int n=1;n<=NumBlindSettings;n++){							//loop through window groups
                        if (n!=i){													//skip the window group that this is being created for
                            for (int j=1;j<=NumGlazingMaterials[i]; j++){			//loop through glazing materials in those groups
                                if (PassString2==GlazingMaterials[n][j]){			//if the layer in the material file equals the glazing layer of another window group
                                    MatFound=true;
                                    RadOut<<"void plastic "<<PassString2<<endl;
                                    RadOut<<"0 0 5 0 0 0 0 0"<<endl;
                                }
                            }
                        }
                    }
                    if (MatFound==false){
                        RadOut<<"void "<<PassString<<" "<<PassString2<<endl;
                        RadIn>>PassString;
                        RadOut<<PassString;
                        if (PassString!="0"){									// Section to read the first line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                                RadOut<<" "<<PassString2;
                            }
                        }
                        RadOut<<endl;
                        RadIn>>PassString;
                        RadOut<<PassString;
                        if (PassString!="0"){									// Section to read the second line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                                RadOut<<" "<<PassString2;
                            }
                        }
                        RadOut<<endl;
                        RadIn>>PassString;
                        RadOut<<PassString;
                        if (PassString!="0"){									// Section to read the third line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                                RadOut<<" "<<PassString2;
                            }
                        }
                        RadOut<<endl;
                    }else{
                        RadIn>>PassString;
                        if (PassString!="0"){									// Section to read the first line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                            }
                        }
                        RadIn>>PassString;
                        if (PassString!="0"){									// Section to read the second line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                            }
                        }
                        RadIn>>PassString;
                        if (PassString!="0"){									// Section to read the third line of arguments
                            for (int m=0;m<atoi(PassString.c_str());m++){
                                RadIn>>PassString2;
                            }
                        }
                    }
                }else{
                    RadOut<<PassString<<" ";		//Write Modifier
                    RadIn>>PassString;				//Read Material Type
                    RadOut<<PassString<<" ";		//Write Material Type
                    RadIn>>PassString;				//Read Identifier
                    RadOut<<PassString<<endl;		//Write Identifier
                    RadIn>>PassString;
                    RadOut<<PassString;
                    if (PassString!="0"){									// Section to read the first line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                            RadOut<<" "<<PassString2;
                        }
                    }
                    RadOut<<endl;
                    RadIn>>PassString;
                    RadOut<<PassString;
                    if (PassString!="0"){									// Section to read the second line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                            RadOut<<" "<<PassString2;
                        }
                    }
                    RadOut<<endl;
                    RadIn>>PassString;
                    RadOut<<PassString;
                    if (PassString!="0"){									// Section to read the third line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                            RadOut<<" "<<PassString2;
                        }
                    }
                    RadOut<<endl;
                }
            }
            RadIn>>PassString;
        }
        RadIn.close();
    }

    #pragma endregion

#pragma endregion


#pragma region "Create the temporary header files and batch files"
    cout<<"The program is creating the temporary header files."<<endl<<endl;

    //*******************************************************************
    //	Section of the code for creating the header files and BSDF batch files
    //*******************************************************************
    BatchFileOut.open(string(project_directory)+"SimDaylight.bat");			//Main Simulation Batch File
    if (BatchFileOut.fail()){
        cout<<"*******************************"<<"The opening of the main simulation batch file has failed."<<endl<<"*******************************"<<endl;
    }
    cout<<"The program is creating the batch file for running the simulation."<<endl<<endl;

    for (int i=1;i<=NumBlindSettings;i++){
        cout<<"Now processing Window Group "<<i<<"."<<endl;
        RadInFileName=string(project_directory)+"tmp/"+ string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.rad";
        if (NewMaterialCase[i]==1){				//Standard Radiance run, no BSDFs
            cout<<"i am in matl_case1"<<endl;

#pragma region "MaterialCase 1"
            //This case requires that the base case and all shade settings get a .htmp file written and added to the main simulation batch file.
            #pragma region "Base Case"
            File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";
            cout<<"File1: "<<File1<<endl;
            File2=string(project_directory)+string(BlindGroupGeometryInRadiance[0][i]);
            cout<<"File2: "<<File2<<endl;
            File3=string(project_directory)+"tmp/"+ string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.rad";
            AddTwoRadFiles(File1, File2, File3);
            cout<<"I have added the two files together."<<endl;


            HeaFileIn.open(HeaderFileName);
            HeaFileOut.open(string(project_directory)+"tmp/"+string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.htmp");
            while (getline(HeaFileIn,PassString) && EndOfHeader==false){  //	Pulls each line out one by one
                HeaFileOut<<endl;
                PassString=PassString.substr(0,PassString.find_last_not_of(" ")+1);
                ss << PassString;					//	Places the string into the string stream
                if (EndOfHeader==false){
                    while (!ss.eof()){				//	Checks to make sure that the string stream isn't empty
                        ss>>PassString;
                        HeaFileOut<<PassString<< " ";
                        if (PassString=="geometry_file"){
                            ss>>PassString;
                            HeaFileOut<<"tmp/"+ string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.rad";
                        }
                        else if (PassString=="material"){
                            HeaFileOut<<"tmp/emptry.rad";
                        }
                        else if (PassString=="shading"){
                            ss>>PassString;
                            ss>>PassString;
                            HeaFileOut<<"1"<<endl;
                            //cout<<"I got to shading 1"<<endl;
                            HeaFileOut<<"static_system ";
                            //cout<<"I have written out static_system."<<endl;
                            //cout<<"The string name is :  "<<string(BlindGroupDiffDC_File[0][0])<<endl;
                            CorrectFileName="tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_base.dc";//substr(string(project_directory).length());
                            //cout<<"I have corrected the file name."<<endl;
                            HeaFileOut<< CorrectFileName<<" ";
                            //cout<<"I have written out the correct file name."<<endl;
                            CorrectFileName="res/"+string(project_name)+"_"+string(BlindGroupName[i])+"_base.ill";//.substr(string(project_directory).length());
                            HeaFileOut<< CorrectFileName<<endl<<endl;
                            HeaFileOut<< "ab "<<abPARAM<<endl;
                            HeaFileOut<< "ad "<<adPARAM<<endl;
                            HeaFileOut<< "as "<<asPARAM<<endl;
                            HeaFileOut<< "ar "<<arPARAM<<endl;
                            HeaFileOut<< "aa "<<aaPARAM<<endl;
                            HeaFileOut<< "lr "<<lrPARAM<<endl;
                            HeaFileOut<< "st "<<stPARAM<<endl;
                            HeaFileOut<< "sj "<<sjPARAM<<endl;
                            HeaFileOut<< "lw "<<lwPARAM<<endl;
                            HeaFileOut<< "dj "<<djPARAM<<endl;
                            HeaFileOut<< "ds "<<dsPARAM<<endl;
                            HeaFileOut<< "dr "<<drPARAM<<endl;
                            HeaFileOut<< "dp "<<dpPARAM<<endl<<endl;
                            HeaFileOut<< "DDS_sensor_file res/" +string(project_name)+"_base.dds"<<endl;
                            HeaFileOut<< "DDS_file res/" +string(project_name)+"_base.sen"<<endl;
                            ss.clear();
                            EndOfHeader=true;
                            PassString="";
                        }
                    }
                    ss.clear();
                }
            }
            HeaFileIn.close();
            HeaFileOut.close();
            //Add the previous entry to the main simulation batch file
            BatchFileOut<<"gen_dc " + string(project_directory)+"tmp/"+string(project_name)+"_"+ string(BlindGroupName[i])+"_base.htmp -dds"<<endl;
            BatchFileOut<<"ds_illum " + string(project_directory)+"tmp/"+string(project_name)+"_"+ string(BlindGroupName[i])+"_base.htmp -dds"<<endl<<endl;
            #pragma endregion

            #pragma region "Shade Settings"
            for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
                File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";
                File2=string(project_directory)+string(BlindGroupGeometryInRadiance[n+1][i]);
                ss<<string(project_directory)<<"tmp/"<< string(project_name)<<"_"<< string(BlindGroupName[i]) <<"_setting_"<<n+1<<".rad";
                ss>>File3;
                ss.clear();
                AddTwoRadFiles(File1, File2, File3);

                ss<<"tmp/"<< string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".rad";
                ss>>File3;
                ss.clear();

                EndOfHeader=false;
                HeaFileIn.open(HeaderFileName);
                ss<<string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_setting_"<<n+1<<".htmp";
                ss>>FileName;
                ss.clear();



                HeaFileOut.open(FileName);
                while (getline(HeaFileIn,PassString) && EndOfHeader==false){  //	Pulls each line out one by one
                    HeaFileOut<<endl;
                    PassString=PassString.substr(0,PassString.find_last_not_of(" ")+1);
                    ss << PassString;					//	Places the string into the string stream
                    if (EndOfHeader==false){
                        while (!ss.eof()){				//	Checks to make sure that the string stream isn't empty
                            ss>>PassString;
                            HeaFileOut<<PassString<< " ";
                            if (PassString=="tmp_directory"){
                                ss>>PassString;
                                HeaFileOut<< "tmp"<<i<<n+1<<"/";
                            }
                            else if (PassString=="geometry_file"){
                                ss>>PassString;
                                HeaFileOut<<File3;
                            }
                            else if (PassString=="shading"){
                                ss>>PassString;
                                ss>>PassString;
                                HeaFileOut<<"1"<<endl;
                                HeaFileOut<<"static_system ";
                                FixString.clear();
                                FixString<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_set"<<n+1<<".dc";
                                FixString>>CorrectFileName;
                                FixString.clear();
                                HeaFileOut<< CorrectFileName<<" ";
                                FixString<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_set"<<n+1<<".ill";
                                FixString>>CorrectFileName;
                                FixString.clear();
                                HeaFileOut<< CorrectFileName<<endl<<endl;
                                HeaFileOut<< "ab "<<abPARAM<<endl;
                                HeaFileOut<< "ad "<<adPARAM<<endl;
                                HeaFileOut<< "as "<<asPARAM<<endl;
                                HeaFileOut<< "ar "<<arPARAM<<endl;
                                HeaFileOut<< "aa "<<aaPARAM<<endl;
                                HeaFileOut<< "lr "<<lrPARAM<<endl;
                                HeaFileOut<< "st "<<stPARAM<<endl;
                                HeaFileOut<< "sj "<<sjPARAM<<endl;
                                HeaFileOut<< "lw "<<lwPARAM<<endl;
                                HeaFileOut<< "dj "<<djPARAM<<endl;
                                HeaFileOut<< "ds "<<dsPARAM<<endl;
                                HeaFileOut<< "dr "<<drPARAM<<endl;
                                HeaFileOut<< "dp "<<dpPARAM<<endl<<endl;
                                FixString.clear();
                                FixString<< "res/" <<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".dds";
                                DDSNewFileName="";
                                FixString>>DDSNewFileName;
                                FixString.clear();
                                HeaFileOut<< "DDS_sensor_file "<<DDSNewFileName<<endl;
                                FixString<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".sen";
                                DDSNewFileName="";
                                FixString>>DDSNewFileName;
                                FixString.clear();
                                HeaFileOut<<"DDS_file "<< DDSNewFileName<<endl;
                                DDSNewFileName="";
                                ss.clear();
                                EndOfHeader=true;
                                PassString="";
                            }
                        }
                        ss.clear();
                    }
                }
                HeaFileIn.close();
                HeaFileOut.close();

                //Add the previous entry to the main simulation batch file
                FileName="";
                FixString.clear();
                FixString << string(project_directory)<<"tmp"<<i<<n+1;			//	Places the string into the string stream
                while (FixString.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
                    if (tempchar=='/'){
                        FileName=FileName+"\\";
                    }else{
                        FileName=FileName+tempchar;
                    }
                }
                FixString.clear();
                BatchFileOut<<"mkdir "<<FileName<<endl;
                BatchFileOut<<"gen_dc \"" << string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_setting_"<<n+1<<".htmp\" -dds"<<endl;
                BatchFileOut<<"ds_illum \"" << string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_setting_"<<n+1<<".htmp\" -dds"<<endl<<endl;

            }
            #pragma endregion
            #pragma endregion
        }else if (NewMaterialCase[i]==2){			//No BSDFs in base case, but at least one BSDF is contained in a setting
            #pragma region "MaterialCase 2"
            //This case requires that a .htmp file be written for the base case and any shade settings that does not have BSDFs as a direct replacement for the glazing layer and a BSDF batch file be written for those that do contain BSDFs
            #pragma region "Base Case"
            File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";
            cout<<"File1: "<<File1<<endl;
            File2=string(project_directory)+string(BlindGroupGeometryInRadiance[0][i]);
            cout<<"File2: "<<File2<<endl;
            File3=string(project_directory)+"tmp/"+ string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.rad";
            AddTwoRadFiles(File1, File2, File3);

            HeaFileIn.open(HeaderFileName);
            HeaFileOut.open(string(project_directory)+"tmp/"+string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.htmp");
            ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.dc";
            ss>>DCFileNames;
            ss.clear();
            ss<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.ill";
            ss>>IllFileNames;
            ss.clear();
            //cout<<"I made it past the opening of the file."<<endl;
            while (getline(HeaFileIn,PassString) && EndOfHeader==false){  //	Pulls each line out one by one
                HeaFileOut<<endl;
                PassString=PassString.substr(0,PassString.find_last_not_of(" ")+1);
                ss << PassString;					//	Places the string into the string stream
                if (EndOfHeader==false){
                    while (!ss.eof()){				//	Checks to make sure that the string stream isn't empty
                        ss>>PassString;
                        HeaFileOut<<PassString<< " ";
                        if (PassString=="geometry_file"){
                            ss>>PassString;
                            HeaFileOut<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.rad";
                        }
                        else if (PassString=="material"){
                            HeaFileOut<<"tmp/empty.rad";
                        }
                        else if (PassString=="shading"){
                            ss>>PassString;
                            ss>>PassString;
                            HeaFileOut<<"1"<<endl;
                            HeaFileOut<<"static_system ";
                            HeaFileOut<< DCFileNames<<" ";
                            HeaFileOut<< IllFileNames<<endl<<endl;
                            HeaFileOut<< "ab "<<abPARAM<<endl;
                            HeaFileOut<< "ad "<<adPARAM<<endl;
                            HeaFileOut<< "as "<<asPARAM<<endl;
                            HeaFileOut<< "ar "<<arPARAM<<endl;
                            HeaFileOut<< "aa "<<aaPARAM<<endl;
                            HeaFileOut<< "lr "<<lrPARAM<<endl;
                            HeaFileOut<< "st "<<stPARAM<<endl;
                            HeaFileOut<< "sj "<<sjPARAM<<endl;
                            HeaFileOut<< "lw "<<lwPARAM<<endl;
                            HeaFileOut<< "dj "<<djPARAM<<endl;
                            HeaFileOut<< "ds "<<dsPARAM<<endl;
                            HeaFileOut<< "dr "<<drPARAM<<endl;
                            HeaFileOut<< "dp "<<dpPARAM<<endl<<endl;
                            HeaFileOut<< "DDS_sensor_file res/" +string(project_name)+"_base.dds"<<endl;
                            HeaFileOut<< "DDS_file res/" +string(project_name)+"_base.sen"<<endl;
                            ss.clear();
                            EndOfHeader=true;
                            PassString="";
                        }
                    }
                    ss.clear();
                }
            }
            HeaFileIn.close();
            HeaFileOut.close();

            //Add the previous entry to the main simulation batch file
            BatchFileOut<<"gen_dc " << string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.htmp -dds"<<endl;
            BatchFileOut<<"ds_illum " <<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.htmp -dds"<<endl<<endl;

            #pragma endregion

            #pragma region "Shade Settings"

            for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
                if (NumBSDFMaterials[i][n+1]>0){
                    for (int j=0;j<NumBSDFMaterials[i][n+1];j++){
                        //BSDF Layer should be blacked out for this part of the calculation.
                        //STEP 1
                        //Add building (Main) geometry to setting geometry and call it setting_#_.rad
                        //STEP 2
                        //Take that rad file and black out the BSDF layers and call it setting_#_standard.rad
                        //STEP 3
                        //For the 5 phase calculation pull out the BSDF and also glazing into their own .rads

                        //*********************** //
                        //STEP 1//
                        //*********************** //
                        File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";						//Main building geometry file
                        File2=string(project_directory)+string(BlindGroupGeometryInRadiance[n+1][i]);												//Setting additional geometry

                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".rad";		//Setting final geometry
                        ss>>FileName;
                        ss.clear();
                        AddTwoRadFiles(File1,File2,FileName);

                        //*********************** //
                        //STEP 2//
                        //*********************** //
                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.rad";	//Geometry file for the standard radiance run
                        ss>>File2;
                        ss.clear();

                        BlackOutLayer(FileName, File2,BSDFExchangeMaterials[i][n+1][0]);

                        //*********************** //
                        //STEP 3//
                        //*********************** //
                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF"<<j+1<<".rad";
                        ss>>PassString2;
                        ss.clear();

                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF"<<j+1<<"_Room.tmp";
                        ss>>PassString;
                        ss.clear();

                        RemoveLayerFunc(FileName, PassString, string(BSDFExchangeMaterials[i][n+1][0]), PassString2);

                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF"<<j+1<<"_Room.rad";
                        ss>>FileName;
                        ss.clear();

                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_Glass.rad";
                        ss>>File2;
                        ss.clear();

                        RemoveLayerFunc(PassString,FileName, GlazingMaterials[i][0],File2);

                        BatchFileOut<<"Call ";
                        AddToBat=WriteBSDFBat(i,n, FileName,0, File2,PassString2,HeaderFileName);
                        BatchFileOut<<AddToBat<<endl;



                        ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.rad";
                        ss>>File2;
                        ss.clear();
                    }
                }
                EndOfHeader=false;
                HeaFileIn.open(HeaderFileName);
                ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".htmp";
                ss>>FileName;
                ss.clear();
                ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.dc";
                ss>>DCFileNames;
                ss.clear();
                ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.ill";
                ss>>IllFileNames;
                ss.clear();
                HeaFileOut.open(FileName);
                while (getline(HeaFileIn,PassString) && EndOfHeader==false){  //	Pulls each line out one by one
                    HeaFileOut<<endl;
                    PassString=PassString.substr(0,PassString.find_last_not_of(" ")+1);
                    ss << PassString;					//	Places the string into the string stream
                    if (EndOfHeader==false){
                        while (!ss.eof()){				//	Checks to make sure that the string stream isn't empty
                            ss>>PassString;
                            HeaFileOut<<PassString<< " ";
                            if (PassString=="geometry_file"){
                                ss>>PassString;
                                HeaFileOut<<File2<<endl;
                            }
                            else if (PassString=="shading"){
                                ss>>PassString;
                                ss>>PassString;
                                HeaFileOut<<"1"<<endl;
                                HeaFileOut<<"static_system ";
                                HeaFileOut<< DCFileNames<<" ";
                                HeaFileOut<< IllFileNames<<endl<<endl;
                                HeaFileOut<< "ab "<<abPARAM<<endl;
                                HeaFileOut<< "ad "<<adPARAM<<endl;
                                HeaFileOut<< "as "<<asPARAM<<endl;
                                HeaFileOut<< "ar "<<arPARAM<<endl;
                                HeaFileOut<< "aa "<<aaPARAM<<endl;
                                HeaFileOut<< "lr "<<lrPARAM<<endl;
                                HeaFileOut<< "st "<<stPARAM<<endl;
                                HeaFileOut<< "sj "<<sjPARAM<<endl;
                                HeaFileOut<< "lw "<<lwPARAM<<endl;
                                HeaFileOut<< "dj "<<djPARAM<<endl;
                                HeaFileOut<< "ds "<<dsPARAM<<endl;
                                HeaFileOut<< "dr "<<drPARAM<<endl;
                                HeaFileOut<< "dp "<<dpPARAM<<endl<<endl;
                                FixString.clear();
                                FixString<< "res/" <<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".dds";
                                DDSNewFileName="";
                                FixString>>DDSNewFileName;
                                FixString.clear();
                                HeaFileOut<< "DDS_sensor_file "<<DDSNewFileName<<endl;
                                FixString<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".sen";
                                DDSNewFileName="";
                                FixString>>DDSNewFileName;
                                FixString.clear();
                                HeaFileOut<<"DDS_file "<< DDSNewFileName<<endl;
                                DDSNewFileName="";
                                ss.clear();
                                EndOfHeader=true;
                                PassString="";
                            }
                        }
                        ss.clear();
                    }
                }
                HeaFileIn.close();
                HeaFileOut.close();

                ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".htmp";
                ss>>FileName;
                ss.clear();

                //Add the previous entry to the main simulation batch file
                BatchFileOut<<"gen_dc " << FileName<<" -dds"<<endl;
                BatchFileOut<<"ds_illum "<< FileName<<" -dds"<<endl<<endl;
            }

            #pragma endregion
            #pragma endregion
        }else if (NewMaterialCase[i]==3){			//The glazing layer is not a BSDF
            #pragma region "MaterialCase 3"
            //This case requires that a .htmp file be written for the base case and all shade settings and a BSDF batch file be written for those settings containing BSDFs
            #pragma region "Base Case"
            if (NumBSDFMaterials[i][0]>0){
                for (int j=0;j<NumBSDFMaterials[i][0];j++){
                    //BSDF Layer should be blacked out for this part of the calculation.
                    //STEP 1
                    //Add building (Main) geometry to setting geometry and call it setting_#_.rad
                    //STEP 2
                    //Take that rad file and black out the BSDF layers and call it setting_#_standard.rad
                    //STEP 3
                    //For the 5 phase calculation pull out the BSDF and also glazing into their own .rads

                    //*********************** //
                    //STEP 1//
                    //*********************** //
                    File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";						//Main building geometry file
                    File2=string(project_directory)+string(BlindGroupGeometryInRadiance[0][i]);												//Setting additional geometry

                    ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.rad";		//base final geometry
                    ss>>FileName;
                    ss.clear();
                    AddTwoRadFiles(File1,File2,FileName);

                    //*********************** //
                    //STEP 2//
                    //*********************** //
                    ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_standard.rad";	//Geometry file for the standard radiance run
                    ss>>File2;
                    ss.clear();
                    BlackOutLayer(FileName, File2,BSDFExchangeMaterials[i][0][0]);

                    //*********************** //
                    //STEP 3//
                    //*********************** //
                    ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF"<<j+1<<".rad";
                    ss>>PassString2;
                    ss.clear();

                    ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF"<<j+1<<"_Room.tmp";
                    ss>>PassString;
                    ss.clear();

                    RemoveLayerFunc(FileName, PassString, string(BSDFExchangeMaterials[i][0][0]), PassString2);

                    ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF"<<j+1<<"_Room.rad";
                    ss>>FileName;
                    ss.clear();

                    ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_Glass.rad";
                    ss>>File2;
                    ss.clear();

                    RemoveLayerFunc(PassString,FileName, GlazingMaterials[i][0],File2);

                    BatchFileOut<<"Call ";
                    AddToBat=WriteBSDFBat(i,0, FileName,0, File2,PassString2,HeaderFileName);
                    BatchFileOut<<AddToBat<<endl;



                    ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_standard.rad";
                    ss>>File2;
                    ss.clear();
                }
            }
            EndOfHeader=false;
            HeaFileIn.open(HeaderFileName);
            ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.htmp";
            ss>>FileName;
            ss.clear();
            ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_standard.dc";
            ss>>DCFileNames;
            ss.clear();
            ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_standard.ill";
            ss>>IllFileNames;
            ss.clear();
            HeaFileOut.open(FileName);
            while (getline(HeaFileIn,PassString) && EndOfHeader==false){  //	Pulls each line out one by one
                HeaFileOut<<endl;
                PassString=PassString.substr(0,PassString.find_last_not_of(" ")+1);
                ss << PassString;					//	Places the string into the string stream
                if (EndOfHeader==false){
                    while (!ss.eof()){				//	Checks to make sure that the string stream isn't empty
                        ss>>PassString;
                        HeaFileOut<<PassString<< " ";
                        if (PassString=="geometry_file"){
                            ss>>PassString;
                            HeaFileOut<<File2<<endl;
                        }
                        else if (PassString=="shading"){
                            ss>>PassString;
                            ss>>PassString;
                            HeaFileOut<<"1"<<endl;
                            HeaFileOut<<"static_system ";
                            HeaFileOut<< DCFileNames<<" ";
                            HeaFileOut<< IllFileNames<<endl<<endl;
                            HeaFileOut<< "ab "<<abPARAM<<endl;
                            HeaFileOut<< "ad "<<adPARAM<<endl;
                            HeaFileOut<< "as "<<asPARAM<<endl;
                            HeaFileOut<< "ar "<<arPARAM<<endl;
                            HeaFileOut<< "aa "<<aaPARAM<<endl;
                            HeaFileOut<< "lr "<<lrPARAM<<endl;
                            HeaFileOut<< "st "<<stPARAM<<endl;
                            HeaFileOut<< "sj "<<sjPARAM<<endl;
                            HeaFileOut<< "lw "<<lwPARAM<<endl;
                            HeaFileOut<< "dj "<<djPARAM<<endl;
                            HeaFileOut<< "ds "<<dsPARAM<<endl;
                            HeaFileOut<< "dr "<<drPARAM<<endl;
                            HeaFileOut<< "dp "<<dpPARAM<<endl<<endl;
                            FixString.clear();
                            FixString<< "res/" <<string(project_name)<<"_base.dds";
                            DDSNewFileName="";
                            FixString>>DDSNewFileName;
                            FixString.clear();
                            HeaFileOut<< "DDS_sensor_file "<<DDSNewFileName<<endl;
                            FixString<<"res/"<<string(project_name)<<"_base.sen";
                            DDSNewFileName="";
                            FixString>>DDSNewFileName;
                            FixString.clear();
                            HeaFileOut<<"DDS_file "<< DDSNewFileName<<endl;
                            DDSNewFileName="";
                            ss.clear();
                            EndOfHeader=true;
                            PassString="";
                        }
                    }
                    ss.clear();
                }
            }
            HeaFileIn.close();
            HeaFileOut.close();

            ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.htmp";
            ss>>FileName;
            ss.clear();

            //Add the previous entry to the main simulation batch file
            BatchFileOut<<"gen_dc " << FileName<<" -dds"<<endl;
            BatchFileOut<<"ds_illum "<< FileName<<" -dds"<<endl<<endl;
            #pragma endregion

            #pragma region "Shade Settings"
            for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
                if (NumBSDFMaterials[i][n+1]>0){
                    for (int j=0;j<NumBSDFMaterials[i][n+1];j++){
                        //BSDF Layer should be blacked out for this part of the calculation.
                        //STEP 1
                        //Add building (Main) geometry to setting geometry and call it setting_#_.rad
                        //STEP 2
                        //Take that rad file and black out the BSDF layers and call it setting_#_standard.rad
                        //STEP 3
                        //For the 5 phase calculation pull out the BSDF and also glazing into their own .rads

                        //*********************** //
                        //STEP 1//
                        //*********************** //
                        File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";						//Main building geometry file
                        File2=string(project_directory)+string(BlindGroupGeometryInRadiance[n+1][i]);												//Setting additional geometry

                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".rad";		//Setting final geometry
                        ss>>FileName;
                        ss.clear();
                        AddTwoRadFiles(File1,File2,FileName);

                        //*********************** //
                        //STEP 2//
                        //*********************** //
                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.rad";	//Geometry file for the standard radiance run
                        ss>>File2;
                        ss.clear();

                        BlackOutLayer(FileName, File2,BSDFExchangeMaterials[i][n+1][0]);

                        //*********************** //
                        //STEP 3//
                        //*********************** //
                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF"<<j+1<<".rad";
                        ss>>PassString2;
                        ss.clear();

                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF"<<j+1<<"_Room.tmp";
                        ss>>PassString;
                        ss.clear();

                        RemoveLayerFunc(FileName, PassString, string(BSDFExchangeMaterials[i][n+1][0]), PassString2);

                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF"<<j+1<<"_Room.rad";
                        ss>>FileName;
                        ss.clear();

                        ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_Glass.rad";
                        ss>>File2;
                        ss.clear();

                        RemoveLayerFunc(PassString,FileName, GlazingMaterials[i][0],File2);

                        BatchFileOut<<"Call ";
                        AddToBat=WriteBSDFBat(i,n, FileName,0, File2,PassString2,HeaderFileName);
                        BatchFileOut<<AddToBat<<endl;

                        ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.rad";
                        ss>>File2;
                        ss.clear();
                    }
                }
                EndOfHeader=false;
                HeaFileIn.open(HeaderFileName);
                ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".htmp";
                ss>>FileName;
                ss.clear();
                ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.dc";
                ss>>DCFileNames;
                ss.clear();
                ss<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.ill";
                ss>>IllFileNames;
                ss.clear();
                HeaFileOut.open(FileName);
                while (getline(HeaFileIn,PassString) && EndOfHeader==false){  //	Pulls each line out one by one
                    HeaFileOut<<endl;
                    PassString=PassString.substr(0,PassString.find_last_not_of(" ")+1);
                    ss << PassString;					//	Places the string into the string stream
                    if (EndOfHeader==false){
                        while (!ss.eof()){				//	Checks to make sure that the string stream isn't empty
                            ss>>PassString;
                            HeaFileOut<<PassString<< " ";
                            //if (PassString=="tmp_directory"){
                            //	ss>>PassString;
                            //	HeaFileOut<< "tmp"<<i<<n+1<<"/";
                            //}
                            //else
                            if (PassString=="geometry_file"){
                                ss>>PassString;
                                HeaFileOut<<File2<<endl;
                            }
                            else if (PassString=="shading"){
                                ss>>PassString;
                                ss>>PassString;
                                HeaFileOut<<"1"<<endl;
                                HeaFileOut<<"static_system ";
                                HeaFileOut<< DCFileNames<<" ";
                                HeaFileOut<< IllFileNames<<endl<<endl;
                                HeaFileOut<< "ab "<<abPARAM<<endl;
                                HeaFileOut<< "ad "<<adPARAM<<endl;
                                HeaFileOut<< "as "<<asPARAM<<endl;
                                HeaFileOut<< "ar "<<arPARAM<<endl;
                                HeaFileOut<< "aa "<<aaPARAM<<endl;
                                HeaFileOut<< "lr "<<lrPARAM<<endl;
                                HeaFileOut<< "st "<<stPARAM<<endl;
                                HeaFileOut<< "sj "<<sjPARAM<<endl;
                                HeaFileOut<< "lw "<<lwPARAM<<endl;
                                HeaFileOut<< "dj "<<djPARAM<<endl;
                                HeaFileOut<< "ds "<<dsPARAM<<endl;
                                HeaFileOut<< "dr "<<drPARAM<<endl;
                                HeaFileOut<< "dp "<<dpPARAM<<endl<<endl;
                                FixString.clear();
                                FixString<< "res/" <<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".dds";
                                DDSNewFileName="";
                                FixString>>DDSNewFileName;
                                FixString.clear();
                                HeaFileOut<< "DDS_sensor_file "<<DDSNewFileName<<endl;
                                FixString<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".sen";
                                DDSNewFileName="";
                                FixString>>DDSNewFileName;
                                FixString.clear();
                                HeaFileOut<<"DDS_file "<< DDSNewFileName<<endl;
                                DDSNewFileName="";
                                ss.clear();
                                EndOfHeader=true;
                                PassString="";
                            }
                        }
                        ss.clear();
                    }
                }
                HeaFileIn.close();
                HeaFileOut.close();

                ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<".htmp";
                ss>>FileName;
                ss.clear();

                //Add the previous entry to the main simulation batch file
                BatchFileOut<<"gen_dc " << FileName<<" -dds"<<endl;
                BatchFileOut<<"ds_illum "<< FileName<<" -dds"<<endl<<endl;
            }
            #pragma endregion
            #pragma endregion
        }else if (NewMaterialCase[i]==4){			//The glazing layer is a BSDF and there is addded geometry in at least one of the settings
            #pragma region "MaterialCase 4"
            //This case requires that the BSDF be run with each new added geometry additions for each setting
            #pragma region "Base Case"

            //AddToBat=WriteBSDFBat(i,-1,RadInFileName,0);
            //BatchFileOut<<AddToBat;

            #pragma endregion

            #pragma region "Shade Settings"

            //for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
            //	ss<<"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_setting_"<<n+1<<".rad";
            //	ss>>RadInFileName;
            //	ss.clear();
            //	AddToBat=WriteBSDFBat(i,n,RadInFileName,0);
            //	BatchFileOut<<AddToBat;
            //}

            #pragma endregion
            #pragma endregion
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
        }else if (NewMaterialCase[i]==6){			//Only the glazing layer contains a BSDF and there is no additional geometry
            #pragma region "MaterialCase 6"
            //This case requires that only the base case have a BSDF batch file written for simulating.  The remainder of the settings will have a section written for the batch file to exchange only the BSDF xml file.
            #pragma region "Base Case"
            File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";
            File2=string(project_directory)+string(BlindGroupGeometryInRadiance[0][i]);
            File3=string(project_directory)+"tmp/"+ string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.tmp";
            AddTwoRadFiles(File1, File2, File3);

            File1=string(project_directory)+"tmp/"+ string(project_name)+"_"+ string(BlindGroupName[i]) +"_base.rad";
            File2=string(project_directory)+"tmp/"+ string(project_name)+"_"+string(BlindGroupName[i])+"_base_glass.rad";
            RemoveLayerFunc(File3, File1, GlazingMaterials[i][0], File2);

            AddToBat=WriteBSDFBat(i, -1, File1, 0, "null", File2, HeaderFileName);
            BatchFileOut<<"call "<<AddToBat<<endl;

            #pragma endregion

            #pragma region "Exchange BSDF matrix"
            for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
                File1=string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_Main.rad";
                File2=string(project_directory)+string(BlindGroupGeometryInRadiance[n+1][i]);
                ss<<string(project_directory)<<"tmp/"<< string(project_name)<<"_"<< string(BlindGroupName[i]) <<"_setting_"<<n+1<<".tmp";
                ss>>File3;
                ss.clear();
                AddTwoRadFiles(File1, File2, File3);

                ss<<string(project_directory)<<"tmp/"<< string(project_name)<<"_"<< string(BlindGroupName[i]) <<"_setting_"<<n+1<<".rad";
                ss>>File1;
                ss.clear();

                ss<<string(project_directory)<<"tmp/"<< string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_glass.rad";
                ss>>File2;
                ss.clear();
                RemoveLayerFunc(File3, File1, GlazingMaterials[i][0], File2);

                RadIn.open(File1);
                MatFound=false;
                while (!RadIn.eof() && MatFound==false){
                    RadIn>>PassString;
                    if (PassString=="void"){
                        RadIn>>PassString;
                        RadIn>>PassString;
                        if (PassString==BSDFExchangeMaterials[i][n+1][0]){
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
                //FivePhaseOut<<"REM Sky Divisions"<<endl;
                //FivePhaseOut<<"REM Sun Divisions"<<endl;
                //FivePhaseOut<<"REM MF1=145 ; MF2=577 ; MF3=1297 ; MF4=2305 ; MF5=3601 ; MF6=5185"<<endl;
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
                FivePhaseOut<<"oconv "<<string(project_directory)<<"tmp/black.rad "<< string(project_directory)<<"tmp/"<<string(project_name)<<"_suns.rad "<<File2<<" "<<string(project_directory)<<"tmp/suns_m1.rad > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF_"<<1<<"_suns.oct"<<endl<<endl;

                //FivePhaseOut<<"REM DCsun matrix"<<endl;
                FivePhaseOut<<"echo rcontrib ^< "<<string(sensor_file)<<" -I -ab 1 -ad 65000 -lw 1.52e-5 -dc 1 -dt 0 -dj 0 -st 1 -ss 0 -faa -e MF:%MFsun% -f reinhart.cal -b rbin -bn Nrbins -m solar "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF_"<<1<<"_suns.oct ^> "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_5PH.dsmx > DCsun.bat"<<endl;
                FivePhaseOut<<"echo exit >> DCsun.bat"<<endl;
                FivePhaseOut<<"start \"DCsun matrix\" /wait DCsun.bat"<<endl<<endl;

                FivePhaseOut<<"del Dcsun.bat"<<endl<<endl;

                FivePhaseOut<<"echo putting it all together..."<<endl;
                FivePhaseOut<<"dctimestep -n "<<(double(60)/time_step)*8760<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3PH.vmx "<<PassString<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3PH.dmx "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1.smx | rcollate -h -fa -oc 3 > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_3ph.dat"<<endl;
                FivePhaseOut<<"dctimestep -n "<<(double(60)/time_step)*8760<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3DIR.vmx "<<PassString<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3DIR.dmx "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_3DIR.smx | rcollate -h -fa -oc 3 > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_3DIR.dat"<<endl;
                FivePhaseOut<<"dctimestep -n "<<(double(60)/time_step)*8760<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_5PH.dsmx "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF1_5PH.smx | rcollate -h -fa -oc 3 > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_ds5ph.dat"<<endl;
                FivePhaseOut<<"rlam "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_3ph.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_3DIR.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_ds5ph.dat | rcalc -e \"r=$1-$4+$7;g=$2-$5+$8;b=$3-$6+$9\" -e \"ill=179*(.265*r +.670*g + .65*b)\" -e \"$1=floor(ill+.5)\" | rcollate -h -fa -or "<<(double(60)/time_step)*8760<<" -t > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF1.ill"<<endl;   //Took out the underline between BSDF and 1
                FivePhaseOut.close();

                BatchFileOut<<"call "<<File3<<endl;
            }


            #pragma endregion


            #pragma endregion
        }else{
            cout<<"The material case for window group "<<string(BlindGroupName[i])<<" has a value of "<<NewMaterialCase[i]<<" which is not recognized."<<endl;
            cout<<"The program will now close."<<endl;
            exit(1);
        }
    }

    BatchFileOut.close();
    cout<<"The program has finished writing to the batch file."<<endl;
    cout<<"The program will now run the batch file."<<endl;
    system((string(project_directory)+"SimDaylight.bat").c_str());
    cout<<"The program has finished running the batch file"<<endl;
    cout<<"The program will begin compiling the final illuminance files."<<endl;
#pragma endregion


#pragma region "Add Illuminance files together"
    for (int i=1;i<=NumBlindSettings;i++){
        cout<<"The material case is "<<NewMaterialCase[i]<<"."<<endl;
        if (NewMaterialCase[i]==1){

        }else if (NewMaterialCase[i]==2){
            for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
                for (int k=0;k<NumIntervals;k++){
                    for (int p=0;p<number_of_sensors;p++){
                        Illuminance[k][p]=0;
                    }
                }
                ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.ill";
                ss>>IllFileNames;
                ss.clear();
                cout<<"Attempting to open the standard radiance file "<<IllFileNames<<"."<<endl;
                TempIn.open(IllFileNames);
                if (TempIn.fail()){
                    cout<<"The opening of the illuminance file "<<IllFileNames<<" has failed."<<endl;
                    cout<<"The program will now close."<<endl;
                    exit(1);
                }
                for (int k=0;k<NumIntervals;k++){
                    TempIn>>DateTime[k][0];
                    TempIn>>DateTime[k][1];
                    TempIn>>DateTime[k][2];
                    for (int p=0;p<number_of_sensors;p++){
                        TempIn>>TempIllum;
                        Illuminance[k][p]=Illuminance[k][p]+TempIllum;
                    }
                }
                TempIn.close();

                for (int j=0;j<NumBSDFMaterials[i][n+1];j++){
                    ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_set"<<n+1<<"_BSDF"<<j+1<<".ill";
                    ss>>IllFileNames;
                    ss.clear();
                    cout<<"Attempting to open the BSDF radiance file "<<IllFileNames<<"."<<endl;
                    TempIn.open(IllFileNames);
                    if (TempIn.fail()){
                        cout<<"The opening of the illuminance file "<<IllFileNames<<" has failed."<<endl;
                        cout<<"The program will now close."<<endl;
                        exit(1);
                    }
                    for (int k=0;k<NumIntervals;k++){
                        for (int p=0;p<number_of_sensors;p++){
                            TempIn>>TempIllum;
                            Illuminance[k][p]=Illuminance[k][p]+TempIllum;
                        }
                    }
                    TempIn.close();
                }
                ss<<string(project_directory)<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_set"<<n+1<<".ill";
                ss>>File1;
                ss.clear();
                TempOut.open(File1);
                if (TempOut.fail()){
                    cout<<"The opening of the illuminance file "<<File1<<" has failed."<<endl;
                    cout<<"The program will now close."<<endl;
                    exit(1);
                }
                for (int k=0;k<NumIntervals;k++){
                    TempOut<<DateTime[k][0]<<" "<<DateTime[k][1]<<" "<<DateTime[k][2];
                    for (int p=0;p<number_of_sensors;p++){
                        TempOut<<" "<<Illuminance[k][p];
                    }
                    TempOut<<endl;
                }
                TempOut.close();
            }
        }else if (NewMaterialCase[i]==3){
            //Base Illuminance files
            for (int k=0;k<NumIntervals;k++){
                for (int p=0;p<number_of_sensors;p++){
                    Illuminance[k][p]=0;
                }
            }
            ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_standard.ill";
            ss>>IllFileNames;
            ss.clear();
            cout<<"Attempting to open the standard radiance file "<<IllFileNames<<"."<<endl;
            TempIn.open(IllFileNames);
            if (TempIn.fail()){
                cout<<"The opening of the illuminance file "<<IllFileNames<<" has failed."<<endl;
                cout<<"The program will now close."<<endl;
                exit(1);
            }
            for (int k=0;k<NumIntervals;k++){
                TempIn>>DateTime[k][0];
                TempIn>>DateTime[k][1];
                TempIn>>DateTime[k][2];
                for (int p=0;p<number_of_sensors;p++){
                    TempIn>>TempIllum;
                    Illuminance[k][p]=Illuminance[k][p]+TempIllum;
                }
            }
            TempIn.close();

            for (int j=0;j<NumBSDFMaterials[i][0];j++){
                ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF"<<j+1<<".ill";
                ss>>IllFileNames;
                ss.clear();
                cout<<"Attempting to open the BSDF radiance file "<<IllFileNames<<"."<<endl;
                TempIn.open(IllFileNames);
                if (TempIn.fail()){
                    cout<<"The opening of the illuminance file "<<IllFileNames<<" has failed."<<endl;
                    cout<<"The program will now close."<<endl;
                    exit(1);
                }
                for (int k=0;k<NumIntervals;k++){
                    for (int p=0;p<number_of_sensors;p++){
                        TempIn>>TempIllum;
                        Illuminance[k][p]=Illuminance[k][p]+TempIllum;
                    }
                }
                TempIn.close();
            }
            ss<<string(project_directory)<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.ill";
            ss>>File1;
            ss.clear();
            TempOut.open(File1);
            if (TempOut.fail()){
                cout<<"The opening of the illuminance file "<<File1<<" has failed."<<endl;
                cout<<"The program will now close."<<endl;
                exit(1);
            }
            for (int k=0;k<NumIntervals;k++){
                TempOut<<DateTime[k][0]<<" "<<DateTime[k][1]<<" "<<DateTime[k][2];
                for (int p=0;p<number_of_sensors;p++){
                    TempOut<<" "<<Illuminance[k][p];
                }
                TempOut<<endl;
            }
            TempOut.close();

            //Shade Setting Illuminance files
            for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
                for (int k=0;k<NumIntervals;k++){
                    for (int p=0;p<number_of_sensors;p++){
                        Illuminance[k][p]=0;
                    }
                }
                ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_standard.ill";
                ss>>IllFileNames;
                ss.clear();
                cout<<"Attempting to open the standard radiance file "<<IllFileNames<<"."<<endl;
                TempIn.open(IllFileNames);
                if (TempIn.fail()){
                    cout<<"The opening of the illuminance file "<<IllFileNames<<" has failed."<<endl;
                    cout<<"The program will now close."<<endl;
                    exit(1);
                }
                for (int k=0;k<NumIntervals;k++){
                    TempIn>>DateTime[k][0];
                    TempIn>>DateTime[k][1];
                    TempIn>>DateTime[k][2];
                    for (int p=0;p<number_of_sensors;p++){
                        TempIn>>TempIllum;
                        Illuminance[k][p]=Illuminance[k][p]+TempIllum;
                    }
                }
                TempIn.close();

                for (int j=0;j<NumBSDFMaterials[i][n+1];j++){
                    ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_set"<<n+1<<"_BSDF"<<j+1<<".ill";
                    ss>>IllFileNames;
                    ss.clear();
                    cout<<"Attempting to open the BSDF radiance file "<<IllFileNames<<"."<<endl;
                    TempIn.open(IllFileNames);
                    if (TempIn.fail()){
                        cout<<"The opening of the illuminance file "<<IllFileNames<<" has failed."<<endl;
                        cout<<"The program will now close."<<endl;
                        exit(1);
                    }
                    for (int k=0;k<NumIntervals;k++){
                        for (int p=0;p<number_of_sensors;p++){
                            TempIn>>TempIllum;
                            Illuminance[k][p]=Illuminance[k][p]+TempIllum;
                        }
                    }
                    TempIn.close();
                }
                ss<<string(project_directory)<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_set"<<n+1<<".ill";
                ss>>File1;
                ss.clear();
                TempOut.open(File1);
                if (TempOut.fail()){
                    cout<<"The opening of the illuminance file "<<File1<<" has failed."<<endl;
                    cout<<"The program will now close."<<endl;
                    exit(1);
                }
                for (int k=0;k<NumIntervals;k++){
                    TempOut<<DateTime[k][0]<<" "<<DateTime[k][1]<<" "<<DateTime[k][2];
                    for (int p=0;p<number_of_sensors;p++){
                        TempOut<<" "<<Illuminance[k][p];
                    }
                    TempOut<<endl;
                }
                TempOut.close();
            }
        }else if (NewMaterialCase[i]==4){

        }else if (NewMaterialCase[i]==5||NewMaterialCase[i]==6){
            for (int k=0;k<NumIntervals;k++){
                for (int p=0;p<number_of_sensors;p++){
                    Illuminance[k][p]=0;
                }
            }
            PassString=string(wea_data_short_file);
            TempIn.open(PassString);
            if (TempIn.fail()){
                cout<<"The opening of the .wea short file \""<<PassString<<"\" has failed."<<endl;
                cout<<"The program will now close."<<endl;
                exit(1);
            }
            for (int k=0;k<6;k++){
                getline(TempIn,PassString);
            }
            for (int k=0;k<NumIntervals;k++){
                TempIn>>DateTime[k][0];
                TempIn>>DateTime[k][1];
                TempIn>>DateTime[k][2];
                TempIn>>PassString;
                TempIn>>PassString;
            }
            TempIn.close();

            for (int j=0;j<NumBSDFMaterials[i][0];j++){
                ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base_BSDF"<<j+1<<".ill";
                ss>>File1;
                ss.clear();
                TempIn.open(File1);
                if (TempIn.fail()){
                    cout<<"The opening of the illuminance file "<<File1<<" has failed."<<endl;
                    cout<<"The program will now close."<<endl;
                    exit(1);
                }
                for (int k=0;k<NumIntervals;k++){
                    for (int p=0;p<number_of_sensors;p++){
                        TempIn>>TempIllum;
                        Illuminance[k][p]=Illuminance[k][p]+TempIllum;
                    }
                }
                TempIn.close();
            }
            ss<<string(project_directory)<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_base.ill";
            ss>>File1;
            ss.clear();
            TempOut.open(File1);
            if (TempOut.fail()){
                cout<<"The opening of the illuminance file "<<File1<<" has failed."<<endl;
                cout<<"The program will now close."<<endl;
                exit(1);
            }
            for (int k=0;k<NumIntervals;k++){
                TempOut<<DateTime[k][0]<<" "<<DateTime[k][1]<<" "<<DateTime[k][2];
                for (int p=0;p<number_of_sensors;p++){
                    TempOut<<" "<<Illuminance[k][p];
                }
                TempOut<<endl;
            }
            TempOut.close();
            for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
                for (int k=0;k<NumIntervals;k++){
                    for (int p=0;p<number_of_sensors;p++){
                        Illuminance[k][p]=0;
                    }
                }
                for (int j=0;j<NumBSDFMaterials[i][n+1];j++){
                    ss<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_setting_"<<n+1<<"_BSDF"<<j+1<<".ill";
                    ss>>File1;
                    ss.clear();
                    TempIn.open(File1);
                    if (TempIn.fail()){
                        cout<<"The opening of the illuminance file "<<File1<<" has failed."<<endl;
                        cout<<"The program will now close."<<endl;
                        exit(1);
                    }
                    for (int k=0;k<NumIntervals;k++){
                        for (int p=0;p<number_of_sensors;p++){
                            TempIn>>TempIllum;
                            Illuminance[k][p]=Illuminance[k][p]+TempIllum;
                        }
                    }
                    TempIn.close();
                }
                ss<<string(project_directory)<<"res/"<<string(project_name)<<"_"<<string(BlindGroupName[i])<<"_set"<<n+1<<".ill";
                ss>>File1;
                ss.clear();
                TempOut.open(File1);
                if (TempOut.fail()){
                    cout<<"The opening of the illuminance file "<<File1<<" has failed."<<endl;
                    cout<<"The program will now close."<<endl;
                    exit(1);
                }
                for (int k=0;k<NumIntervals;k++){
                    TempOut<<DateTime[k][0]<<" "<<DateTime[k][1]<<" "<<DateTime[k][2];
                    for (int p=0;p<number_of_sensors;p++){
                        TempOut<<" "<<Illuminance[k][p];
                    }
                    TempOut<<endl;
                }
                TempOut.close();
            }

        }
        //else if (NewMaterialCase[i]==6){
        //}



    }
    cout<<"The final illuminance files have been compiled."<<endl;
#pragma endregion


#pragma region "Creating the batch file for running the simulation"
        //cout<<"The program is creating the batch file for running the simulation."<<endl<<endl;


    //*******************************************************************
    //	Section of the code for writing and running the DDS batch files
    //*******************************************************************
    //BatchFileOut.open(string(project_directory)+"SimDaylight.bat");
    //BatchFileOut<<"gen_dc " + string(project_directory)+"tmp/"+string(project_name)+"_base.htmp -dds"<<endl;
    //BatchFileOut<<"ds_illum " + string(project_directory)+"tmp/"+string(project_name)+"_base.htmp -dds"<<endl<<endl;
    //for (int i=1;i<=NumBlindSettings;i++){
    //	for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){
    //		FileName="";
    //		FixString.clear();
    //		FixString << string(project_directory)<<"tmp"<<i<<n+1;			//	Places the string into the string stream
    //		while (FixString.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
    //			if (tempchar=='/'){
    //				FileName=FileName+"\\";
    //			}else{
    //				FileName=FileName+tempchar;
    //			}
    //		}
    //		FixString.clear();
    //		BatchFileOut<<"mkdir "<<FileName<<endl;
    //		BatchFileOut<<"gen_dc \"" << string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_setting_"<<n+1<<".htmp\" -dds"<<endl;
    //		BatchFileOut<<"ds_illum \"" << string(project_directory)+"tmp/"+string(project_name)+"_"+string(BlindGroupName[i])+"_setting_"<<n+1<<".htmp\" -dds"<<endl<<endl;
    //	}
    //}
    //BatchFileOut.close();


    //system((string(project_directory)+"SimDaylight.bat").c_str());
    //remove((string(project_directory)+"SimDaylight.bat").c_str());




#pragma endregion


#pragma region "Cleaning up the folders"

    //*******************************************************************
    //	Section of the code for writing and running the cleaning batch files
    //*******************************************************************
    //BatchFileOut.open(string(project_directory)+"Clean.bat");
    //for (int i=1;i<=NumBlindSettings;i++){
    //	for (int n=0;n<NumberOfSettingsInBlindgroup[i];n++){		//Changed starting value from 0 to -1
    //		TestFileName="";
    //		FixString << string(project_directory)<<"tmp"<<i<<n+1;			//	Places the string into the string stream
    //		while (FixString.get(tempchar)){										//	Checks to make sure that the string stream isn't empty
    //			if (tempchar=='/'){
    //				TestFileName=TestFileName+"\\";
    //			}else{
    //				TestFileName=TestFileName+tempchar;
    //			}
    //		}
    //		FixString.clear();
    //		BatchFileOut<<"DEL "<<TestFileName<<"\\*.dc"<<endl;
    //		BatchFileOut<<"DEL "<<TestFileName<<"\\*.rad"<<endl;
    //		BatchFileOut<<"DEL "<<TestFileName<<"\\*.oct"<<endl;
    //		BatchFileOut<<"DEL "<<TestFileName<<"\\*.dc_tmp"<<endl;
    //		BatchFileOut<<"rmdir "<< TestFileName<<"\\"<<endl<<endl;
    //	}
    //}
    //TestFileName="";
    //FixString << string(project_directory);			//	Places the string into the string stream
    //while (FixString.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
    //	if (tempchar=='/'){
    //		TestFileName=TestFileName+"\\";
    //	}else{
    //		TestFileName=TestFileName+tempchar;
    //	}
    //}
    //FixString.clear();
    //BatchFileOut<<"DEL "<<TestFileName<<"tmp\\*_base.rad"<<endl;
    //BatchFileOut<<"DEL "<<TestFileName<<"tmp\\*_base.htmp"<<endl;
    //BatchFileOut<<"DEL "<<TestFileName<<"tmp\\*_setting_*.rad"<<endl;
    //BatchFileOut<<"DEL "<<TestFileName<<"tmp\\*_setting_*.htmp"<<endl;

    //BatchFileOut.close();


    //system((string(project_directory)+"Clean.bat").c_str());
    //remove((string(project_directory)+"Clean.bat").c_str());


#pragma endregion


    return 0;
}


string WriteBSDFBat(int BlindGroupNum, int Setting, string RadFileName, int BSDFNum, string GlassRad, string BSDFRad, string HeaName){

    //The arguments being passed to this function are as follows:
    //BlindGroupNum:  This is the number associated with the position in the arrays for a particular blind group
    //Setting:		This is the setting number for the blind group.  (-1 means base, 0 means the first setting and so on)
    //RadFileName:	This is the radiance geometry file with all geometry excluding the glazing geometry and the BSDF geometry
    //BSDFNum:		This is the number associated with the BSDF used for a particular setting (up to 5 BSDFs per setting)
    //GlassRad:		This is the radiance geometry file with the glazing geometry (This is "null" if the glazing geometry is the BSDF)
    //BSDFRad:		This is the radiance geometry file with the BSDF geometry (if the glazing is null, then the glazing will be found in this file)


    read_in_header((char*)HeaName.c_str());
    stringstream ss;
    string PassString;
    string PassString2;
    string FivePhaseBatName;
    string SettingFileName;
    ofstream FivePhaseOut;
    ifstream RadIn;
    bool MatFound;
    double NormalPoints[3][3];	//[Vector][point]
    double NormalVector[3];
    double length;

    //Section to get normal vector facing in and calculate normal vector facing out.  Normal In assumes right hand rule for polygon.
    RadIn.open(BSDFRad);
    if (RadIn.fail()){
        cout<<"The opening of the geometry file "<<BSDFRad<<" which contains the BSDF has failed."<<endl;
        cout<<"The program will now close."<<endl;
        exit(1);
    }
    RadIn>>PassString;
    while (PassString!="Polygon"&&PassString!="polygon"&&!RadIn.eof()){
        RadIn>>PassString;
    }
    RadIn>>PassString;	//Takes in polygon name
    RadIn>>PassString;	//Takes in 0
    RadIn>>PassString;	//Takes in 0
    RadIn>>PassString;	//Takes in number of values describing the geometry
    for (int i=0;i<3;i++){
        for (int k=0;k<3;k++){
            RadIn>>NormalPoints[i][k];
        }
    }
    RadIn.close();

    NormalVector[0]=(NormalPoints[2][1]-NormalPoints[1][1])*(NormalPoints[0][2]-NormalPoints[1][2])-(NormalPoints[2][2]-NormalPoints[1][2])*(NormalPoints[0][1]-NormalPoints[1][2]);
    NormalVector[1]=(NormalPoints[2][0]-NormalPoints[1][0])*(NormalPoints[0][2]-NormalPoints[1][2])-(NormalPoints[2][2]-NormalPoints[1][2])*(NormalPoints[0][0]-NormalPoints[1][0]);
    NormalVector[2]=(NormalPoints[2][0]-NormalPoints[1][0])*(NormalPoints[0][1]-NormalPoints[1][1])-(NormalPoints[2][1]-NormalPoints[1][1])*(NormalPoints[0][0]-NormalPoints[1][0]);
    length=sqrt(NormalVector[0]*NormalVector[0]+NormalVector[1]*NormalVector[1]+NormalVector[2]*NormalVector[2]);	//Calculate the length of the normal vector
    NormalVector[0]=NormalVector[0]/length;	//normalize the normal vector  ...negative zero result may be causing a problem.  possible fix is by using a tolerance to make it zero. for the vector facing out
    NormalVector[1]=NormalVector[1]/length;	//normalize the normal vector
    NormalVector[2]=NormalVector[2]/length;	//normalize the normal vector

    if (Setting==-1){
        ss<<"base_BSDF"<<BSDFNum+1;
        ss>>SettingFileName;
        ss.clear();
    }else{
        ss<<"set"<<Setting+1<<"_BSDF"<<BSDFNum+1;
        ss>>SettingFileName;
        ss.clear();
    }

    ss<<string(project_directory)+"tmp/"+string(project_name)+"_"+ string(BlindGroupName[BlindGroupNum]) +"_"+SettingFileName+"_5Phase.bat";
    ss>>FivePhaseBatName;
    ss.clear();

    FivePhaseOut.open(FivePhaseBatName);
    FivePhaseOut<<"@echo off"<<endl;
    //FivePhaseOut<<"REM Sky Divisions"<<endl;
    //FivePhaseOut<<"REM Sun Divisions"<<endl;
    //FivePhaseOut<<"REM MF1=145 ; MF2=577 ; MF3=1297 ; MF4=2305 ; MF5=3601 ; MF6=5185"<<endl;
    FivePhaseOut<<"set MFsky="<<SkyDivisions<<endl;
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
    //FivePhaseOut<<"REM BSDF thickness"<<endl;
    RadIn.open(RadFileName);
    cout<<"Finding the thickness"<<endl;
    cout<<"The file name is "<<RadFileName<<endl;
    MatFound=false;
    while (!RadIn.eof() && MatFound==false){
        RadIn>>PassString;
        if (PassString=="void"){
            RadIn>>PassString;
            RadIn>>PassString;
            if (PassString==BSDFExchangeMaterials[BlindGroupNum][Setting+1][BSDFNum]){
                MatFound=true;
                RadIn>>PassString;
                RadIn>>PassString;			//Read the thickness
                RadIn>>PassString2;			//Read the xml file
            }
        }
    }
    if(MatFound==false){
        cout<<"The BSDF material was not found in the file."<<endl;
    }
    RadIn.close();
    FivePhaseOut<<"set thick="<<PassString<<endl<<endl<<endl<<endl;
    //FivePhaseOut<<"REM Models"<<endl<<endl;

    if (GlassRad=="null"){
        PassString=BSDFRad;
    }else{
        PassString=GlassRad+" "+BSDFRad;
    }
    FivePhaseOut<<"oconv "<<RadFileName<<" "<<PassString<<" "<<string(project_directory)<<"tmp/sky_white1.rad > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<".oct"<<endl;
    FivePhaseOut<<endl<<"xform -m black "<<RadFileName<<" > "<<string(project_directory)<<"tmp/black.rad"<<endl;
    FivePhaseOut<<"oconv "<<string(project_directory)<<"tmp/black.rad "<< PassString <<" "<<string(project_directory)<<"tmp/sky_white1.rad > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_allblack.oct"<<endl<<endl;

    FivePhaseOut<<"cnt %Nsuns% | rcalc -e MF:%MFsun% -f reinsrc.cal -e Rbin=recno -o \"solar source sun 0 0 4 ${ Dx } ${ Dy } ${ Dz } 0.533\" > "<<string(project_directory)<<"tmp/suns_m1.rad"<<endl;
    FivePhaseOut<<"oconv "<<string(project_directory)<<"tmp/black.rad "<< string(project_directory)<<"tmp/"<<string(project_name)<<"_suns.rad "<<PassString<<" "<<string(project_directory)<<"tmp/suns_m1.rad > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_suns.oct"<<endl<<endl;

    FivePhaseOut<<"echo running several parallel processes..."<<endl<<endl;

    //FivePhaseOut<<"REM V matrix"<<endl;
    FivePhaseOut<<"echo rcontrib -f klems_int.cal -b kbin("<<(-1)*NormalVector[0]<<","<<(-1)*NormalVector[1]<<","<<(-1)*NormalVector[2]<<",0,0,1) -bn Nkbins -m "<<GlazingMaterials[BlindGroupNum][0]<<" -I+ -ab 12 -ad 50000 -lw 2e-5 "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<".oct ^< "<<string(sensor_file)<<" ^> "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3PH.vmx > v.bat"<<endl;
    FivePhaseOut<<"echo exit >> v.bat"<<endl;
    FivePhaseOut<<"start \"V matrix\" v.bat"<<endl<<endl;

    //FivePhaseOut<<"REM D matrix"<<endl;
    FivePhaseOut<<"echo perl.exe genklemsamp.pl -vd "<<NormalVector[0]<<" "<<NormalVector[1]<<" "<<NormalVector[2]<<" -vo %thick% "<< PassString<<" ^| rcontrib -c 1000 -e MF:%MFsky% -f reinhart.cal -b rbin -bn Nrbins -m sky_glow -faa "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<".oct ^> "<< string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3PH.dmx > D.bat"<<endl;
    FivePhaseOut<<"echo exit >> D.bat"<<endl;
    FivePhaseOut<<"start \"D matrix\" D.bat"<<endl<<endl;

    //FivePhaseOut<<"REM S matrix"<<endl;
    FivePhaseOut<<"echo gendaymtx -m %MFsky% ";
    if (rotation_angle!=0){
        FivePhaseOut<<"-r "<<(-1)*rotation_angle<<" ";
    }
    FivePhaseOut<<"-c 1 1 1 "<<wea_data_short_file<<" ^> "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<".smx > S.bat"<<endl;
    FivePhaseOut<<"echo exit >> S.bat"<<endl;
    FivePhaseOut<<"start \"S matrix\" S.bat"<<endl<<endl;

    //FivePhaseOut<<"REM Dd matrix"<<endl;
    FivePhaseOut<<"echo perl.exe genklemsamp.pl -vd "<<NormalVector[0]<<" "<<NormalVector[1]<<" "<<NormalVector[2]<<" -vo %thick% "<<PassString<<" ^| rcontrib -c 1000 -e MF:%MFsky% -f reinhart.cal -b rbin -bn Nrbins -m sky_glow -faa "<< string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_allblack.oct ^> "<<string(project_directory) <<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3DIR.dmx > Dd.bat"<<endl;
    FivePhaseOut<<"echo exit >> Dd.bat"<<endl;
    FivePhaseOut<<"start \"Dd matrix\" Dd.bat"<<endl;

    //FivePhaseOut<<"REM Vd matrix"<<endl;
    FivePhaseOut<<"echo rcontrib ^< "<<string(sensor_file)<<" -f klems_int.cal -b kbin("<<(-1)*NormalVector[0]<<","<<(-1)*NormalVector[1]<<","<<(-1)*NormalVector[2]<<",0,0,1) -bn Nkbins -m "<<GlazingMaterials[BlindGroupNum][0]<<" -I+ -ab 1 -ad 50000 -lw 2e-5 "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_allblack.oct ^> "<<string(project_directory) <<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3DIR.vmx > Vd.bat"<<endl;
    FivePhaseOut<<"echo exit >> Vd.bat"<<endl;
    FivePhaseOut<<"start \"Vd matrix\" Vd.bat"<<endl<<endl;

    //FivePhaseOut<<"REM Sd matrix"<<endl;
    FivePhaseOut<<"echo gendaymtx -m %MFsky% ";
    if (rotation_angle!=0){
        FivePhaseOut<<"-r "<<(-1)*rotation_angle<<" ";
    }
    FivePhaseOut<<"-d "<<wea_data_short_file<<" ^> "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3DIR.smx > Sd.bat"<<endl;
    FivePhaseOut<<"echo exit >> Sd.bat"<<endl;
    FivePhaseOut<<"start \"Sd matrix\" sd.bat"<<endl<<endl;

    //FivePhaseOut<<"REM Ssun matrix"<<endl;
    FivePhaseOut<<"echo gendaymtx -m %MFsun% -5 ";
    if (rotation_angle!=0){
        FivePhaseOut<<"-r "<<(-1)*rotation_angle<<" ";
    }
    FivePhaseOut<<"-d "<<wea_data_short_file<<" ^> "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_5PH.smx > Ssun.bat"<<endl;
    FivePhaseOut<<"echo exit >> Ssun.bat"<<endl;
    FivePhaseOut<<"start \"Ssun matrix\" /wait Ssun.bat"<<endl<<endl;

    //FivePhaseOut<<"REM DCsun matrix"<<endl;
    FivePhaseOut<<"echo rcontrib ^< "<<string(sensor_file)<<" -I -ab 1 -ad 65000 -lw 1.52e-5 -dc 1 -dt 0 -dj 0 -st 1 -ss 0 -faa -e MF:%MFsun% -f reinhart.cal -b rbin -bn Nrbins -m solar "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_suns.oct ^> "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_5PH.dsmx > DCsun.bat"<<endl;
    FivePhaseOut<<"echo exit >> DCsun.bat"<<endl;
    FivePhaseOut<<"start \"DCsun matrix\" /wait DCsun.bat"<<endl<<endl;

    FivePhaseOut<<"del D.bat Dd.bat Dcsun.bat s.bat Sd.bat Ssun.bat v.bat Vd.bat"<<endl<<endl;

    FivePhaseOut<<"echo putting it all together..."<<endl;
    FivePhaseOut<<"dctimestep -n "<<(double(60)/time_step)*8760<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3PH.vmx "<<PassString2<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3PH.dmx "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<".smx | rcollate -h -fa -oc 3 > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_3ph.dat"<<endl;
    FivePhaseOut<<"dctimestep -n "<<(double(60)/time_step)*8760<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3DIR.vmx "<<PassString2<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3DIR.dmx "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_3DIR.smx | rcollate -h -fa -oc 3 > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_3DIR.dat"<<endl;
    FivePhaseOut<<"dctimestep -n "<<(double(60)/time_step)*8760<<" "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_5PH.dsmx "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<"_5PH.smx | rcollate -h -fa -oc 3 > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_ds5ph.dat"<<endl;
    FivePhaseOut<<"rlam "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_3ph.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_3DIR.dat "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_ds5ph.dat | rcalc -e \"r=$1-$4+$7;g=$2-$5+$8;b=$3-$6+$9\" -e \"ill=179*(.265*r +.670*g + .65*b)\" -e \"$1=floor(ill+.5)\" | rcollate -h -fa -or "<<(double(60)/time_step)*8760<<" -t > "<<string(project_directory)<<"tmp/"<<string(project_name)<<"_"<<string(BlindGroupName[BlindGroupNum])<<"_"<<SettingFileName<<".ill"<<endl;

    FivePhaseOut.close();


    return FivePhaseBatName;
}


void RemoveLayerFunc(string InRadName, string OutRadName, string LayerName, string OutLayerName){
    ifstream inRad;
    ofstream outRad;
    ofstream outLayerRad;

    string PassString;
    string PassString2;
    string PassString3;

    bool FoundBSDF=false;

    inRad.open(InRadName);
    if (inRad.fail()){
        cout<<"The opening of the rad file "<<InRadName<<" has failed."<<endl;
        cout<<"The program will now close."<<endl;
        exit(1);
    }
    outRad.open(OutRadName);
    if (outRad.fail()){
        cout<<"The opening of the rad file "<<OutRadName<<" has failed."<<endl;
        cout<<"The program will now close."<<endl;
        exit(1);
    }
    outLayerRad.open(OutLayerName);
    if (outLayerRad.fail()){
        cout<<"The opening of the rad file "<<OutLayerName<<" has failed."<<endl;
        cout<<"The program will now close."<<endl;
        exit(1);
    }


    while (!inRad.eof()){
        inRad>> PassString;
        if (PassString.find("#") != string::npos){
            getline(inRad, PassString);
        }
        else{
            if (!inRad.eof()){
                inRad>>PassString2;
                inRad>>PassString3;
                if (PassString==LayerName){			//Write this geometry out to the BSDF Rad file
                    FoundBSDF=true;
                    outLayerRad<<PassString<<" "<<PassString2<<" "<<PassString3<<endl;
                    inRad>>PassString;
                    outLayerRad<<PassString;
                    if (PassString!="0"){									// Section to read the first line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            inRad>>PassString2;
                            outLayerRad<<" "<<PassString2;
                        }
                    }
                    outLayerRad<<endl;

                    inRad>>PassString;
                    outLayerRad<<PassString;
                    if (PassString!="0"){									// Section read the second line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            inRad>>PassString2;
                            outLayerRad<<" "<<PassString2;
                        }
                    }
                    outLayerRad<<endl;

                    inRad>>PassString;
                    outLayerRad<<PassString;
                    if (PassString!="0"){									// Section to read the third line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            inRad>>PassString2;
                            outLayerRad<<" "<<PassString2;
                        }
                    }
                    outLayerRad<<endl;
                }else{					//Write this geometry out to the companion geometry file
                    outRad<<PassString<<" "<<PassString2<<" "<<PassString3<<endl;
                    inRad>>PassString;
                    outRad<<PassString;
                    if (PassString!="0"){									// Section to read the first line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            inRad>>PassString2;
                            outRad<<" "<<PassString2;
                        }
                    }
                    outRad<<endl;

                    inRad>>PassString;
                    outRad<<PassString;
                    if (PassString!="0"){									// Section read the second line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            inRad>>PassString2;
                            outRad<<" "<<PassString2;
                        }
                    }
                    outRad<<endl;

                    inRad>>PassString;
                    outRad<<PassString;
                    if (PassString!="0"){									// Section to read the third line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            inRad>>PassString2;
                            outRad<<" "<<PassString2;
                        }
                    }
                    outRad<<endl;
                }
            }
        }
    }
    inRad.close();
    outRad.close();
    outLayerRad.close();
    if (FoundBSDF=false){
        cout<<"The layer specified with name "<<LayerName<<" was not found in the geometry files."<<endl;
        cout<<"The program will now close."<<endl;
        exit(1);
    }
}


void AddTwoRadFiles(string inRadOne, string inRadTwo, string OutRadName){
    //The radiance file "inRadOne" will be added first
    ifstream RadIn;
    ofstream RadOut;
    stringstream ss;
    string PassString;
    char tempchar;

    RadOut.open(OutRadName);
    if (RadOut.fail()){
        cout<<"The opening of the rad file named "<<OutRadName<<" has failed."<<endl;
        cout<<"The program will now close."<<endl;
        exit(1);
    }
    RadIn.open(inRadOne);														//	open the first geometry file
    if (RadIn.fail()){																		//	test that the file exists
        cout << "The geometry file "<< inRadOne<<" failed to open." << endl;
        cout<< "Please specify your geometry before running simulation analysis." << endl;
        cout<< "The program will now close.";
        exit(1);
    }
    while (getline(RadIn,PassString)){  //	Pulls each line out one by one
        ss << PassString;				//	Places the string into the string stream
        while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
            RadOut<<tempchar;
        }
        ss.clear();
        RadOut<< endl;
    }
    RadIn.close();

    RadIn.open(inRadTwo);														//	open the first geometry file
    if (RadIn.fail()){																		//	test that the file exists
        cout << "The geometry file "<< inRadTwo<<" failed to open." << endl;
        cout<< "Please specify your geometry before running simulation analysis." << endl;
        cout<< "The program will now close.";
        exit(1);
    }
    while (getline(RadIn,PassString)){  //	Pulls each line out one by one
        ss << PassString;				//	Places the string into the string stream
        while (ss.get(tempchar)){				//	Checks to make sure that the string stream isn't empty
            RadOut<<tempchar;
        }
        ss.clear();
        RadOut<< endl;
    }
    RadIn.close();

    RadOut.close();
}


void BlackOutLayer(string InRadName, string OutRadName, string LayerName){

    string PassString;
    string PassString2;
    bool MatFound;

    ifstream RadIn;
    ofstream RadOut;

    RadIn.open(InRadName);
    if (RadIn.fail()){
        cout<<"The opening of the file named "<<InRadName<<" has failed."<<endl;
        cout<<"The program will now close."<<endl;
        exit(1);
    }
    RadOut.open(OutRadName);
        if (RadOut.fail()){
        cout<<"The opening of the file named "<<OutRadName<<" has failed."<<endl;
        cout<<"The program will now close."<<endl;
        exit(1);
    }
    RadIn>>PassString;
    while (!RadIn.eof()){
        if (PassString.find("#") != string::npos){
            getline(RadIn, PassString);
        }else{
            if (PassString=="void"){
                MatFound=false;
                RadIn>>PassString;
                RadIn>>PassString2;
                if (PassString2==LayerName){			//if the layer in the material file equals the glazing layer of another window group
                    MatFound=true;
                    RadOut<<"void plastic "<<PassString2<<endl;
                    RadOut<<"0 0 5 0 0 0 0 0"<<endl;
                    RadIn>>PassString;
                    if (PassString!="0"){									// Section to read the first line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                        }
                    }
                    RadIn>>PassString;
                    if (PassString!="0"){									// Section to read the second line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                        }
                    }
                    RadIn>>PassString;
                    if (PassString!="0"){									// Section to read the third line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                        }
                    }
                }
                if (MatFound==false){
                    RadOut<<"void "<<PassString<<" "<<PassString2<<endl;
                    RadIn>>PassString;
                    RadOut<<PassString;
                    if (PassString!="0"){									// Section to read the first line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                            RadOut<<" "<<PassString2;
                        }
                    }
                    RadOut<<endl;
                    RadIn>>PassString;
                    RadOut<<PassString;
                    if (PassString!="0"){									// Section to read the second line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                            RadOut<<" "<<PassString2;
                        }
                    }
                    RadOut<<endl;
                    RadIn>>PassString;
                    RadOut<<PassString;
                    if (PassString!="0"){									// Section to read the third line of arguments
                        for (int m=0;m<atoi(PassString.c_str());m++){
                            RadIn>>PassString2;
                            RadOut<<" "<<PassString2;
                        }
                    }
                    RadOut<<endl;
                }
            }else{
                RadOut<<PassString<<" ";		//Write Modifier
                RadIn>>PassString;				//Read Material Type
                RadOut<<PassString<<" ";		//Write Material Type
                RadIn>>PassString;				//Read Identifier
                RadOut<<PassString<<endl;		//Write Identifier
                RadIn>>PassString;
                RadOut<<PassString;
                if (PassString!="0"){									// Section to read the first line of arguments
                    for (int m=0;m<atoi(PassString.c_str());m++){
                        RadIn>>PassString2;
                        RadOut<<" "<<PassString2;
                    }
                }
                RadOut<<endl;
                RadIn>>PassString;
                RadOut<<PassString;
                if (PassString!="0"){									// Section to read the second line of arguments
                    for (int m=0;m<atoi(PassString.c_str());m++){
                        RadIn>>PassString2;
                        RadOut<<" "<<PassString2;
                    }
                }
                RadOut<<endl;
                RadIn>>PassString;
                RadOut<<PassString;
                if (PassString!="0"){									// Section to read the third line of arguments
                    for (int m=0;m<atoi(PassString.c_str());m++){
                        RadIn>>PassString2;
                        RadOut<<" "<<PassString2;
                    }
                }
                RadOut<<endl;
            }
        }
        RadIn>>PassString;
    }
    RadOut.close();
    RadIn.close();
}
*/
