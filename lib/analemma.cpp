/****************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted for
 * personal and commercial purposes provided that the
 * following conditions are met:
 *
 * 1. Redistribution of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PENNSYLVANIA STATE UNIVERSITY
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************/

#include "analemma.h"
#include "logging.h"
#include "functions.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <iomanip>

const double PI=3.1415926535897932;

namespace stadic{

Analemma::Analemma(std::string file)
{
    m_WeatherFile=file;
    m_SunLoc.clear();
    m_Rotation=0;
    m_numSuns=0;
    m_ClosestSun.clear();


}

//Setters
void Analemma::setRotation(double val)
{
    m_Rotation=val;
}
void Analemma::setMatFile(std::string file)
{
    m_MatFile=file;
}

void Analemma::setGeoFile(std::string file)
{
    m_GeoFile=file;
}

void Analemma::setSMXFile(std::string file){
    m_SMXFile=file;
}


//Getters


//Functions
bool Analemma::genSun()
{
    if(!parseWeather()){
        return false;
    }
    if (!getSunPos()){
        return false;
    }
    /*
    if (!closestSun()){
        return false;
    }
    */
    if (!genSunMtx()){
        return false;
    }
    return true;
}


//Private Functions
bool Analemma::parseWeather()
{
    if (!m_WeaData.parseWeather(m_WeatherFile)){
        return false;
    }
    return true;
}

bool Analemma::getSunPos()
{
    std::ofstream matFile;
    matFile.open(m_MatFile);
    if (!matFile.is_open()){
        STADIC_ERROR("There was a problem opening the output material file \""+m_MatFile+"\".");
        return false;
    }
    std::ofstream geoFile;
    geoFile.open(m_GeoFile);
    if (!geoFile.is_open()){
        STADIC_ERROR("There was a problem opening the output geometry file \""+m_GeoFile+"\".");
        return false;
    }
    /*
    std::ofstream debugFile;     //Debugging
    debugFile.open("c:/001/SpeedUpData.txt");     //Debugging
    if (!debugFile.is_open()){
        std::clog<<"The debug file could not be opened."<<std::endl;
    }

    */
    std::vector<double> pvec;
    std::vector<double> svec;
    std::vector<double> qvec;
    qvec.resize(3);
    std::vector<std::vector<double> > over_vec;
    std::vector<int> overlap_Sun;
    std::vector<double> tempvec;
    double sda;         //Solar Declination Angle
    double sta;         //Solar Time Adjustment
    double altitude;
    double azimuth;
    double dprod;
    int nskip=0;
    m_ClosestSun.resize(8760);
    temporarySun.resize(8760);


    // Loop through all hours of the day
    for (int i=0;i<24;i++){
        double hr=i+.5;
        double alt_prev=1.57080;
        double azi_prev=0.0;
        int n_first;
        if (m_SunLoc.empty()){
            n_first=0;
        }else{
            n_first=m_SunLoc.size();
        }
        bool first_of_hour=true;
        pvec=pos(alt_prev,azi_prev);
        int n_overlap=0;
        over_vec.clear();
        overlap_Sun.clear();
        // Loop through all days of the year at the hour being considered.
        for (int julianDate=1;julianDate<366;julianDate++){
            //std::clog<<"julianDate="<<julianDate<<" hour="<<hr<<std::endl;
            //if (julianDate==1&&hr==17.5){
            //    std::clog<<"Found"<<std::endl;
            //}
            double over_dprod=0;
            //if (julianDate==6&&i==12){
              //  std::clog<<"found"<<std::endl;
            //}
            sda = solarDec(julianDate);
            sta=solarTimeAdj(julianDate);
            altitude=solarAlt(sda,hr+sta);
            azimuth=solarAz(sda,hr+sta)+PI-degToRad(m_Rotation);
            // If sun is above the horizon
            if (altitude>0.00278){
                svec=pos(altitude,azimuth);
                dprod=dotProd(pvec,svec);
                // If the Julian day is in the overlap zone coming back over previous suns

                if(238<julianDate && julianDate<249){
                    m_ClosestSun[(julianDate-1)*24+i]=m_numSuns-1;
                    // Test sun positions that are in the overlap sun from earlier in the year.
                    for (int jd = 0; jd < n_overlap; jd++){
                        //int jd_tmp=overlap_Sun(jd);

                        qvec[0] = over_vec[jd][0];
                        qvec[1] = over_vec[jd][1];
                        qvec[2] = over_vec[jd][2];
                        over_dprod=dotProd(qvec,svec);


                        if (over_dprod>dprod){
                            dprod=over_dprod;
                            m_ClosestSun[(julianDate-1)*24+i]=overlap_Sun[jd];
                            temporarySun[(julianDate-1)*24+i]="Overlap";     //Debugging
                        }
                    }
                }

                // If the current sun doesnt overlap any previous suns
                if (dprod<0.999954){
                    // If this sun is the first one of the year at this hour.
                    if (first_of_hour==true){
                        tempvec.clear();
                        tempvec.push_back(svec[0]);
                        tempvec.push_back(svec[1]);
                        tempvec.push_back(svec[2]);
                        m_SunLoc.push_back(tempvec);
                        m_numSuns++;
                        matFile<<"void light solar"<<m_numSuns<<" 0 0 3 1.0 1.0 1.0" <<std::endl;
                        geoFile<<"solar" << m_numSuns << " source sun"<<m_numSuns <<" "<<"0 0 4 " <<svec[0]<< " " << svec[1] << " " << svec[2]  <<" 0.533" << std::endl;
                        first_of_hour=false;
                        azi_prev=azimuth;
                        alt_prev=altitude;
                        pvec=pos(alt_prev,azi_prev);
                        m_ClosestSun[(julianDate-1)*24+i]=m_numSuns-1;  //Added m_numSuns-1 to fix the array bounds
                        temporarySun[(julianDate-1)*24+i]="julianDate: "+toString(julianDate)+" Hour: "+toString(hr);     //Debugging

                    }else{      // If this sun is NOT the first one of the year at this hour
                        //if (m_numSuns>900 && i==12){
                        //    std::clog<<"jdate="<<julianDate<< " hour"<<hr<<std::endl;
                        //}
                                // If some suns were skipped due to an overlap with previously created suns, then the second half of the skipped suns must point to this sun position.
                        if (nskip>0){
                            // For an odd number of suns, test the middle sun to determine if the previous or current sun is closest.
                            if(nskip%2 ==1){
                                int midDate=julianDate - (nskip+1)/2;
                                sda = solarDec(midDate);
                                sta=solarTimeAdj(midDate);
                                double altitude2=solarAlt(sda,hr+sta);
                                double azimuth2=solarAz(sda,hr+sta)+PI-degToRad(m_Rotation);
                                qvec=pos(altitude2,azimuth2);
                                double q_dprod=dotProd(qvec,svec);
                                if (m_ClosestSun[(midDate-1)*24+i]==-1){
                                    std::cerr<<"BUGGGGG:  Array went out of bounds."<<std::endl;
                                    EXIT_FAILURE;
                                }
                                pvec[0]=m_SunLoc [m_ClosestSun [(midDate-1)*24+i]] [0];
                                pvec[1]=m_SunLoc [m_ClosestSun [(midDate-1)*24+i]] [1];
                                pvec[2]=m_SunLoc [m_ClosestSun [(midDate-1)*24+i]] [2];
                                double p_dprod=dotProd(qvec,pvec);
                                if (q_dprod > p_dprod){
                                    m_ClosestSun[(midDate-1)*24+i] = m_numSuns;   //Removed the -1
                                    //temporarySun[(midDate-1)*24+i]="julianDate: "+toString(midDate)+" Hour: "+toString(hr);     //Debugging
                                }
                            }
                            // Set the second half for both odd and even number of skipped suns to the current sun.
                            for (int aa = julianDate-int((nskip+1))/2; aa < julianDate+1; aa++) {
                                //  Test against the current closest if within the overlap zone.
                                if(238<aa && aa <249){
                                    sda = solarDec(aa);
                                    sta=solarTimeAdj(aa);
                                    double altitude3=solarAlt(sda,hr+sta);
                                    double azimuth3=solarAz(sda,hr+sta)+PI-degToRad(m_Rotation);
                                    qvec=pos(altitude3,azimuth3);
                                    double q_dprod=dotProd(qvec,svec);
                                    if (m_ClosestSun[(aa-1)*24+i]==-1){
                                        std::cerr<<"BUGGGGG:  Array went out of bounds."<<std::endl;
                                        EXIT_FAILURE;
                                    }
                                    pvec[0]=m_SunLoc [m_ClosestSun [(aa-1)*24+i]] [0];
                                    pvec[1]=m_SunLoc [m_ClosestSun [(aa-1)*24+i]] [1];
                                    pvec[2]=m_SunLoc [m_ClosestSun [(aa-1)*24+i]] [2];
                                    double p_dprod=dotProd(qvec,pvec);

                                    // If the closest sun is the current sun, put this sun into the ClosestSun array.
                                    if (q_dprod > dprod){
                                        m_ClosestSun[(aa-1)*24+i] = m_numSuns;    //Removed the -1       //Added m_numSuns-1 to fix the array bounds
                                        temporarySun[(aa-1)*24+i]="julianDate: "+toString(aa)+" Hour: "+toString(hr);     //Debugging
                                     }
                                }else{
                                    m_ClosestSun [(aa-1)*24+i] = m_numSuns;    //Removed the -1            //Added m_numSuns-1 to fix the array bounds
                                    temporarySun[(aa-1)*24+i]="julianDate: "+toString(aa)+" Hour: "+toString(hr);     //Debugging
                                }
                            }
                        }
                        nskip = 0;

                        std::vector<double> tempvec;
                        //bool exclude=false;

                        // count and save the suns that lie in the first pass through the overlap zone of the analemma path.
                        tempvec.clear();
                        tempvec.push_back(svec[0]);
                        tempvec.push_back(svec[1]);
                        tempvec.push_back(svec[2]);

                        if(98<julianDate && julianDate<110){
                            n_overlap++;
                            over_vec.push_back(tempvec);
                            //over_vec[n_overlap][1]= svec[1];
                            //over_vec[n_overlap][2]= svec[2];
                            overlap_Sun.push_back(m_numSuns);     //Removed the -1               //Added m_numSuns-1 to fix the array bounds
                        }
                        m_SunLoc.push_back(tempvec);
                        m_numSuns++;
                        // write out current sun position that does not overlap with previous suns.
                        matFile<<"void light solar"<<m_numSuns<<" 0 0 3 1.0 1.0 1.0" <<std::endl;
                        geoFile<<"solar" << m_numSuns << " source sun"<<m_numSuns <<" "<<"0 0 4 " <<svec[0]<< " " << svec[1] << " " << svec[2]  <<" 0.533" << std::endl;
                        azi_prev=azimuth;
                        alt_prev=altitude;
                        pvec=pos(alt_prev,azi_prev);
                    }
                }else{  // The current sun overlaps, so it is skipped.
                    if (over_dprod==0){
                        m_ClosestSun[(julianDate-1)*24+i]=m_numSuns-1;      //Added m_numSuns-1 to fix the array bounds
                        temporarySun[(julianDate-1)*24+i]="julianDate: "+toString(julianDate)+" Hour: "+toString(hr);     //Debugging
                    }else{
                        over_dprod=0;
                    }
                        nskip++;
                }
            }else{
            //The current sun position is below the horizon.
                if (nskip>0){
                    for (int aa=julianDate-nskip;aa<julianDate;aa++){
                        m_ClosestSun[(aa-1)*24+i]=m_numSuns-1;
                    }
                    nskip=0;
                }
                m_ClosestSun[(julianDate-1)*24+i]=-1;
            }
        }
        if (nskip>0){
            for (int aa=366-nskip;aa<366;aa++){
                m_ClosestSun[(aa-1)*24+i]=m_numSuns-1;
            }
            nskip=0;
        }
        n_overlap=0;
    }
    /*
    std::ofstream debugFile;
    debugFile.open("c:/001/closestSun.txt");
    int countHour=0;
    for (int i=0;i<365;i++){
        for (int j=0;j<24;j++){
            countHour++;
            if (m_ClosestSun[countHour-1]>-1){
                debugFile<<"julianDate="<<i+1<<" hour="<<j<<" Closest Sun: "<<m_ClosestSun[countHour-1]<<" from:"<<temporarySun[countHour-1]<<std::endl;
            }
        }
    }

    debugFile.close();
    */


    /*
    for (int i=0;i<24;i++){
        double hr=i+.5;
        double alt_prev=1.57080;
        double azi_prev=0.0;
        int n_first;
        if (m_SunLoc.empty()){
            n_first=0;
        }else{
            n_first=m_SunLoc.size();
        }
        bool first_of_hour=true;
        pvec=pos(alt_prev,azi_prev);
        for (int julianDate=1;julianDate<366;julianDate++){
            sda = solarDec(julianDate);
            sta=solarTimeAdj(julianDate);
            altitude=solarAlt(sda,hr+sta);
            azimuth=solarAz(sda,hr+sta)+PI-degToRad(m_Rotation);
            if (altitude>0.00278){
                svec=pos(altitude,azimuth);
                dprod=dotProd(pvec,svec);
                if (dprod<0.99994){
                    if (first_of_hour==true){
                        tempvec.clear();
                        tempvec.push_back(svec[0]);
                        tempvec.push_back(svec[1]);
                        tempvec.push_back(svec[2]);
                        m_SunLoc.push_back(tempvec);
                        m_numSuns++;
                        matFile<<"void light solar"<<m_numSuns<<" 0 0 3 1.0 1.0 1.0" <<std::endl;
                        geoFile<<"solar" << m_numSuns << " source sun"<<m_numSuns <<" "<<"0 0 4 " <<svec[0]<< " " << svec[1] << " " << svec[2]  <<" 0.533" << std::endl;
                        //temporarySun.push_back("julianDate: "+toString(julianDate)+" Hour: "+toString(hr));
                        first_of_hour=false;
                        azi_prev=azimuth;
                        alt_prev=altitude;
                        pvec=pos(alt_prev,azi_prev);
                    }else{
                        std::vector<double> tempvec;
                        bool exclude=false;
                        for (int j=n_first;j<=(m_SunLoc.size()-1);j++){
                            tempvec.clear();
                            tempvec.push_back(m_SunLoc[j][0]);
                            tempvec.push_back(m_SunLoc[j][1]);
                            tempvec.push_back(m_SunLoc[j][2]);
                            dprod=dotProd(tempvec,svec);
                            if (dprod>0.999954){
                                exclude=true;
                            }
                        }
                        if (!exclude){
                            tempvec.clear();
                            tempvec.push_back(svec[0]);
                            tempvec.push_back(svec[1]);
                            tempvec.push_back(svec[2]);
                            m_SunLoc.push_back(tempvec);
                            m_numSuns++;
                            matFile<<"void light solar"<<m_numSuns<<" 0 0 3 1.0 1.0 1.0" <<std::endl;
                            geoFile<<"solar" << m_numSuns << " source sun"<<m_numSuns <<" "<<"0 0 4 " <<svec[0]<< " " << svec[1] << " " << svec[2]  <<" 0.533" << std::endl;
                            //temporarySun.push_back("julianDate: "+toString(julianDate)+" Hour: "+toString(hr));
                            azi_prev=azimuth;
                            alt_prev=altitude;
                            pvec=pos(alt_prev,azi_prev);
                        }
                    }
                }
            }
        }
    }
    */
    matFile.close();
    geoFile.close();
    return true;
}

std::vector<double> Analemma::pos(double altitude, double azimuth)
{
    std::vector<double> tempVec;
    tempVec.push_back(cos(altitude)*sin(azimuth));
    tempVec.push_back(cos(altitude)*cos(azimuth));
    tempVec.push_back(sin(altitude));
    return tempVec;
}

double Analemma::degToRad(double val)
{
    return val*PI/180.0;
}

double Analemma::degToRad(std::string val)
{
    return stadic::toDouble(val)*PI/180.0;
}

double Analemma::solarDec(int julianDate)
{
    return 0.4093*sin((2*PI/368)*(julianDate-81));
}

double Analemma::solarTimeAdj(int julianDate)
{
    return 0.170*sin((4*PI/373)*(julianDate-80))-0.129*sin((2*PI/355)*(julianDate-8))+12 *(degToRad(m_WeaData.timeZoneDeg())-degToRad(m_WeaData.longitude()))/PI;
}

double Analemma::solarAlt(double solarDeclination, double time)
{
    return asin(sin(degToRad(m_WeaData.latitude()))*sin(solarDeclination)-cos(degToRad(m_WeaData.latitude()))*cos(solarDeclination)*cos(PI*time/12));
}

double Analemma::solarAz(double solarDeclination, double time)
{
    return -atan2(cos(solarDeclination)*sin(time*(PI/12)),-cos(degToRad(m_WeaData.latitude()))*sin(solarDeclination)-sin(degToRad(m_WeaData.latitude()))*cos(solarDeclination)*cos(time*(PI/12)));
}

double Analemma::dotProd(std::vector<double> vec1,std::vector<double> vec2)
{
    return vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2];
}

bool Analemma::closestSun()
{
    //std::ofstream debugFile;
    //debugFile.open("c:/001/SpeedUpData.txt");
    int hr_count=0;
    double hr=0;
    double sda;         //Solar Declination Angle
    double sta;         //Solar Time Adjustment
    double altitude;
    double azimuth;
    double dprod;
    std::vector<double> svec;
    for (int julian_date=1;julian_date<366;julian_date++){
        for (int hri=0;hri<24; hri++){
            hr=hri+.5;
            hr_count=hr_count+1;
            sda = solarDec(julian_date);
            sta = solarTimeAdj(julian_date);
            altitude = solarAlt(sda, hr+sta);
            azimuth = solarAz(sda, hr+sta) + PI - degToRad(m_Rotation);
            svec=pos(altitude,azimuth);
            double dp_closest=0;
            if(altitude > 0){
              for (unsigned int j=0; j<m_SunLoc.size(); j++) {
                std::vector<double> tempvec;
                tempvec.push_back(m_SunLoc[j][0]);
                tempvec.push_back(m_SunLoc[j][1]);
                tempvec.push_back(m_SunLoc[j][2]);
                dprod = dotProd(tempvec,svec);
                if(dprod>dp_closest){
                    if (m_ClosestSun.size()<hr_count){
                        m_ClosestSun.push_back(j);
                    }else{
                        m_ClosestSun[hr_count-1]=j;
                    }
                    dp_closest=dprod;
                }
              }
              //debugFile<<"Time: "<<hr<<" julianDate: "<<julian_date<<" ClosestSun: "<<temporarySun[m_ClosestSun[hr_count-1]]<<std::endl;
            }else {
                if (m_ClosestSun.size()<hr_count){
                    m_ClosestSun.push_back(-1);
                }else{
                    m_ClosestSun[hr_count-1]=-1;
                }
            }
        }
    }
    //debugFile.close();
    return true;
}

bool Analemma::genSunMtx()
{
    std::ofstream smx;
    smx.open(m_SMXFile);
    if (!smx.is_open()){
        STADIC_ERROR("There was a problem opening the smx file \""+m_SMXFile+"\".");
        return false;
    }
    //smx.setf(std::ios::scientific);
    //smx.setf(std::ios::fixed);
    //smx.precision(6);
    for (int j=0;j<m_numSuns;j++){
        for (int i=0;i<8760;i++){
            if (m_ClosestSun[i]==j){
                smx<<m_WeaData.directIlluminance()[i]/6.797e-05<<"\t"<<m_WeaData.directIlluminance()[i]/6.797e-05<<"\t"<<m_WeaData.directIlluminance()[i]/6.797e-05<<std::endl;
            }else{
                smx<<"0\t0\t0\n";
            }
        }
    }
    smx.close();

    /*
    std::clog<<"Resizing Sun Matrix."<<std::endl;
    m_SunVal.resize(8760);
    for (int i=0;i<8760;i++){
        m_SunVal[i].resize(m_numSuns);
        for (int j=0;j<m_numSuns;j++){
            m_SunVal[i][j].resize(3);
            for (int k=0;k<3;k++){
                m_SunVal[i][j][k]=0;
            }
        }
    }
    std::clog<<"Finished resizing Sun Matrix."<<std::endl;
    for (int i=0;i<m_WeaData.hour().size();i++){
        if (m_ClosestSun[i]>-1){
            std::vector<double> tempVec;
            tempVec.push_back(m_WeaData.directIlluminance()[i]/6.797e-05);
            tempVec.push_back(m_WeaData.directIlluminance()[i]/6.797e-05);
            tempVec.push_back(m_WeaData.directIlluminance()[i]/6.797e-05);
            m_SunVal[i][m_ClosestSun[i]]=tempVec;
        }
    }
    std::clog<<"Writing Sun Matrix."<<std::endl;
    //Write out SMX matrix
    std::ofstream smx;
    smx.open(m_SMXFile);
    if (!smx.is_open()){
        STADIC_ERROR("There was a problem opening the smx file \""+m_SMXFile+"\".");
        return false;
    }
    smx.setf(std::ios::scientific);
    smx.setf(std::ios::fixed);
    smx.precision(3);
    for (int i=0;i<m_SunLoc.size();i++){
        for (int j=0;j<8760;j++){
            if (m_SunVal[j][i][0]==0){
                smx<<"0\t0\t0"<<std::endl;
            }else{
                //Write exponential format
                smx<<m_SunVal[j][i][0]<<"\t"<<m_SunVal[j][i][1]<<"\t"<<m_SunVal[j][i][2]<<"\n";
            }
        }
    }
    smx.close();
    std::clog<<"Finished writing Sun Matrix."<<std::endl;
    */
    return true;
}



}
