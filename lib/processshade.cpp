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

#include "processshade.h"
#include "logging.h"
#include "dayill.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "functions.h"
#include "weatherdata.h"

namespace stadic {
ProcessShade::ProcessShade(BuildingControl *model) :
    m_Model(model)
{
}

bool ProcessShade::processShades()
{
    std::vector<std::shared_ptr<Control>> spaces=m_Model->spaces();    
    //populate the time interval vector
    DaylightIlluminanceData timeData(spaces[0].get()->illumUnits());
    if (!timeData.parseTimeBased(spaces[0].get()->spaceDirectory()+spaces[0].get()->resultsDirectory()+spaces[0].get()->spaceName()+"_"+spaces[0].get()->windowGroups()[0].name()+"_base.ill")){
        return false;
    }
    /*
    std::cout<<"setting up TimeIntervals"<<std::endl;
    m_Months.resize(timeData.illuminance().size());
    m_Days.resize(timeData.illuminance().size());
    m_Hours.resize(timeData.illuminance().size());
    for (int j=0;j<timeData.illuminance().size();j++){
        m_Months[j]=timeData.illuminance()[j].month();
        m_Days[j]=timeData.illuminance()[j].day();
        m_Hours[j]=timeData.illuminance()[j].hour();
    }
    std::cout<<"finished populating TimerIntervals"<<std::endl;
    */
    for (int i=0;i<spaces.size();i++){
        //start processing shade algorithms
        if (!makeShadeSched(spaces[i].get(), &timeData)){
            STADIC_LOG(Severity::Error, "The creation of the shade schedule for "+spaces[i].get()->spaceName()+" has failed.");
            return false;
        }
    }

    return true;
}
bool ProcessShade::writeSched(std::vector<std::vector<int>> shadeSched, std::string file, DaylightIlluminanceData *dayIll){
    std::ofstream oFile;
    oFile.open(file);
    if (!oFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the file "+ file + " has failed.");
        return false;
    }
    //Write out the shade schedule here.  M D H WG1Set WG2Set...
    int i=0;
    for (auto illum : dayIll->illuminance()){
    //for (int i=0;i<dayIll->illuminance().size();i++){
        oFile << illum.month() << " " << illum.day() << " " << illum.hour();
        //oFile<<dayIll->illuminance()[i].month()<<" "<<dayIll->illuminance()[i].day()<<" "<<dayIll->illuminance()[i].hour();
        for (int j=0;j<shadeSched.size();j++){
            oFile<<" "<<shadeSched[j][i];           //The shadeSched vector is filled with a fector of one window group first then the second...
        }
        oFile<<std::endl;
        ++i;
    }

    oFile.close();
    return true;
}

bool ProcessShade::makeShadeSched(Control *model, DaylightIlluminanceData *dayIll){
    std::vector<std::vector<int>> shadeSchedule;
	int imon[8760];
	int iday[8760];
	double ihour[8760];
	int i1, i2, i3, i4, i5, i6, jdd;

	for (int i = 0; i < 8760; i++){
		jdd = int((i - .5) / 24);
		i1 = 1768 + jdd;
		i2 = int((i1 - 1) / 1461);
		i3 = i1 - i2 * 1461;
		i4 = int((i3 - 1) / 365 - int(i3 / 1461));
		i5 = i3 - 365 * i4 + 30;
		i6 = int((80 * i5) / 2447);
		iday[i] = i5 - int((2447 * i6) / 80);
		imon[i] = i6 + 2 - 12 * int(i6 / 11);
		ihour[i] = i % 24 + 0.5;
	}
    //determine schedule for each window group
    for (int j=0;j<model->windowGroups().size();j++){
        STADIC_LOG(Severity::Info, "Now processing window group "+model->windowGroups()[j].name() +" from "+ model->spaceName()+".");
        std::vector<int> tempVector;
        tempVector.clear();
        if (model->windowGroups()[j].shadeControl()->controlMethod()=="automated_signal"){
            tempVector=automatedSignal(model, j);
            if (tempVector.size()>0){
                shadeSchedule.push_back(tempVector);
            }else{
                STADIC_LOG(Severity::Error, "The shade schedule for "+model->windowGroups()[j].name() +" from "+ model->spaceName()+ "is empty.");
                return false;
            }
        }else if (model->windowGroups()[j].shadeControl()->controlMethod()=="automated_profile_angle"){
            tempVector=automatedProfileAngle(model, j);
            if (tempVector.size()>0){
                shadeSchedule.push_back(tempVector);
            }else{
                STADIC_LOG(Severity::Error, "The shade schedule for "+model->windowGroups()[j].name() +" from "+ model->spaceName()+ "is empty.");
                return false;
            }
        }else if (model->windowGroups()[j].shadeControl()->controlMethod()=="automated_profile_angle_signal"){
            tempVector=automatedProfileAngleSignal(model, j);
            if (tempVector.size()>0){
                shadeSchedule.push_back(tempVector);
            }else{
                STADIC_LOG(Severity::Error, "The shade schedule for "+model->windowGroups()[j].name() +" from "+ model->spaceName()+ "is empty.");
                return false;
            }
        }else{
            STADIC_LOG(Severity::Error, "The shade control algortithm for "+model->windowGroups()[j].name() +" from "+ model->spaceName()+" is not valid.");
            return false;
        }
    }
    STADIC_LOG(Severity::Info, "Now writing out the schedule.");
    //Write out completed schedule
    if (!writeSched(shadeSchedule, model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_shade.sch", dayIll)){
        return false;
    }
    STADIC_LOG(Severity::Info, "Combining the final illuminance file.");
    //determine final illuminance values using completed schedule
    std::vector<DaylightIlluminanceData> baseIlls;
    std::vector<std::vector<DaylightIlluminanceData>> settingIlls;
    for (int i=0;i<model->windowGroups().size();i++){
        DaylightIlluminanceData illBase(model->illumUnits());
        illBase.parseTimeBased(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_base.ill");
        baseIlls.push_back(illBase);
        std::vector<DaylightIlluminanceData> tempSettingIlls;
        for (int j=0;j<model->windowGroups()[i].shadeSettingGeometry().size();j++){
            DaylightIlluminanceData illSetting(model->illumUnits());
            illSetting.parseTimeBased(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[i].name()+"_set"+toString(model->sDAwgSettings()[i])+".ill");
            tempSettingIlls.push_back(illSetting);
        }
        settingIlls.push_back(tempSettingIlls);
    }
    DaylightIlluminanceData finalIlluminance(model->illumUnits());
    std::vector<double> finalTemporalIll;
    finalTemporalIll.resize(baseIlls[0].illuminance()[0].lux().size());
    for (int i=0;i<shadeSchedule[0].size();i++){               //Loop over the entire year
        //finalTemporalIll.clear();
        //for (int j=0;j<baseIlls[0].illuminance()[0].lux().size();j++){      //Set the illuminance vector to zero for all points
        //    finalTemporalIll.push_back(0);
        //}
        std::fill(finalTemporalIll.begin(), finalTemporalIll.end(), 0);
        for (int j=0;j<shadeSchedule.size();j++){        //Loop over window groups
            if (shadeSchedule[j][i]==0){           //use base condition
               // if (!baseIlls[j].illuminance()[i].allZeros()){
                    int k=0;
                    for (auto illum : baseIlls[j].illuminance()[i].lux()){
                        finalTemporalIll[k]=finalTemporalIll[k] + illum;
                        k++;
                    }
                    /*
                    for (int k=0;k<baseIlls[j].illuminance()[i].lux().size();k++){
                        finalTemporalIll[k]=finalTemporalIll[k] + baseIlls[j].illuminance()[i].lux()[k];
                    }
                    */
               // }
            }else{                              //shades employed
                int k=0;
                for (auto illum : settingIlls[j][shadeSchedule[j][i]-1].illuminance()[i].lux()){
                    finalTemporalIll[k]=finalTemporalIll[k] + illum;
                    k++;
                }
                /*
                for (int k=0;k<settingIlls[j][shadeSchedule[j][i]-1].illuminance()[i].lux().size();k++){
                    finalTemporalIll[k]=finalTemporalIll[k] + settingIlls[j][shadeSchedule[j][i]-1].illuminance()[i].lux()[k];
                }
                */
            }
        }
		TemporalIlluminance dataPoint(imon[i], iday[i], ihour[i], finalTemporalIll);
        finalIlluminance.addDataPoint(dataPoint);
    }
    STADIC_LOG(Severity::Info, "Now writing out the final illuminance file.");
    //Write Illuminance File
    if (model->illumUnits()=="lux"){
        finalIlluminance.writeIllFileLux(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_final.ill");
    }else{
        finalIlluminance.writeIllFileFC(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_final.ill");
    }

    return true;
}
//Shade Control Algorithms
std::vector<int> ProcessShade::automatedSignal(Control *model, int windowGroup){
    std::vector<int> shadeSchedule;
    DaylightIlluminanceData shadeSignal(model->illumUnits());
    if(!shadeSignal.parseTimeBased(model->spaceDirectory()+model->resultsDirectory()+model->spaceName()+"_"+model->windowGroups()[windowGroup].name()+"_shade.sig")){
        return shadeSchedule;
    }
    for (int j=0;j<shadeSignal.illuminance().size();j++){
        if (shadeSignal.illuminance()[j].lux()[0]==0){
            shadeSchedule.push_back(0);
        }else{
            bool settingFound=false;
            if (model->illumUnits()=="lux"){
                for (int i=0;i<model->windowGroups()[windowGroup].shadeControl()->signalSettings().size();i++){
                    if (shadeSignal.illuminance()[j].lux()[0]>model->windowGroups()[windowGroup].shadeControl()->signalSettings()[i]&&settingFound==false){
                        shadeSchedule.push_back(i+1);
                        settingFound=true;
                    }
                }
            }else{
                for (int i=0;i<model->windowGroups()[windowGroup].shadeControl()->signalSettings().size();i++){
                    if (shadeSignal.illuminance()[j].fc()[0]>model->windowGroups()[windowGroup].shadeControl()->signalSettings()[i]&&settingFound==false){
                        shadeSchedule.push_back(i+1);
                        settingFound=true;
                    }
                }
            }
            if (settingFound==false){
                shadeSchedule.push_back(0);
            }
        }
    }
    return shadeSchedule;
}

std::vector<int> ProcessShade::automatedProfileAngle(Control *model, int windowGroup){
    std::vector<int> shadeSchedule;
    WeatherData weaData;
    if (!weaData.parseWeather(model->weaFile())){
        return shadeSchedule;
    }

    std::vector<double> profAngle;
    profAngle=weaData.getProfileAngles(model->windowGroups()[windowGroup].shadeControl()->elevationAzimuth());
	for (int j = 0; j < profAngle.size(); j++){
        if (profAngle[j]==-1){
            shadeSchedule.push_back(0);
        }else{

            bool settingFound=false;
            for (int i=0;i<model->windowGroups()[windowGroup].shadeControl()->angleSettings().size();i++){

                if (profAngle[j]<model->windowGroups()[windowGroup].shadeControl()->angleSettings()[i]&&settingFound==false){
                    shadeSchedule.push_back(i+1);

                    settingFound=true;
                }
            }
            if (settingFound==false){
            //int ccc = int(profAngle[j]);
                shadeSchedule.push_back(0);
            }
        }
    }
    return shadeSchedule;
}
std::vector<int> ProcessShade::automatedProfileAngleSignal(Control *model, int windowGroup){
    std::vector<int> shadeSchedule;
    std::vector<int> profSchedule;
	DaylightIlluminanceData shadeSignal(model->illumUnits());
	if (!shadeSignal.parseTimeBased(model->spaceDirectory() + model->resultsDirectory() + model->spaceName() + "_" + model->windowGroups()[windowGroup].name() + "_shade.sig")){
		return shadeSchedule;
	}
    profSchedule=automatedProfileAngle(model, windowGroup);
    std::vector<int> sigSchedule;
    sigSchedule=automatedSignal(model, windowGroup);
    for (int i=0;i<sigSchedule.size();i++){
        if (sigSchedule[i]==0 || profSchedule[i]==0){
            shadeSchedule.push_back(0);
        }else{
            /*if (sigSchedule[i]>profSchedule[i]){
                shadeSchedule.push_back(sigSchedule[i]);
			}
			else{*/
				bool settingFound = false;
				for (int ii = profSchedule[i]; 0; i--){

					if (model->illumUnits() == "lux"){
						if (shadeSignal.illuminance()[i].lux()[0] > model->windowGroups()[windowGroup].shadeControl()->signalSettings()[ii] && settingFound == false){
						STADIC_LOG(Severity::Info, "Now writing out the schedule." + toString(i) + "." + toString(ii));
							//if ( settingFound == false){
							shadeSchedule.push_back(ii+1 );
							settingFound = true;
						}
					}

					else{
							if (shadeSignal.illuminance()[i].fc()[0]>model->windowGroups()[windowGroup].shadeControl()->signalSettings()[ii] && settingFound == false){
								shadeSchedule.push_back(ii + 1);
								settingFound = true;
							}
						
					}
				}
					if (settingFound == false) {
						shadeSchedule.push_back(0);
					}
           //* }
        }
    }
    return shadeSchedule;
}
	


}
