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
    if (!closestSun()){
        return false;
    }
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

    std::vector<double> pvec;
    std::vector<double> svec;
    std::vector<double> tempvec;
    double sda;         //Solar Declination Angle
    double sta;         //Solar Time Adjustment
    double altitude;
    double azimuth;
    double dprod;
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
                            azi_prev=azimuth;
                            alt_prev=altitude;
                            pvec=pos(alt_prev,azi_prev);
                        }
                    }
                }
            }
        }
    }
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
    return 0.170*sin((4*PI/373)*(julianDate-80))-0.129*sin((2*PI/355)*(julianDate-8))+12 *(degToRad(m_WeaData.timeZone())-degToRad(m_WeaData.longitude()))/PI;
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
    std::ofstream oFile;
    oFile.open("c:/001/BracketValues.txt");
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
            oFile<<"julianDate="<<julian_date<<" time= "<<hr+sta<<" sta="<<sta<<" PI="<<PI;
            oFile<<" sda "<<sda<< " altitude "<<altitude<<" azimuth "<<azimuth<<" svec= "<<svec[0]<< ","<<svec[1]<< ","<<svec[2];
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
            }else {
                if (m_ClosestSun.size()<hr_count){
                    m_ClosestSun.push_back(-1);
                }else{
                    m_ClosestSun[hr_count-1]=-1;
                }
            }
            oFile<<" ClosestSun="<<m_ClosestSun[hr_count-1]<<std::endl;
        }
    }

    oFile.close();

    return true;
}

bool Analemma::genSunMtx()
{
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
    for (int i=0;i<m_WeaData.hour().size();i++){
        if (m_ClosestSun[i]>-1){
            std::vector<double> tempVec;
            tempVec.push_back(m_WeaData.directIlluminance()[i]/6.797e-05);
            tempVec.push_back(m_WeaData.directIlluminance()[i]/6.797e-05);
            tempVec.push_back(m_WeaData.directIlluminance()[i]/6.797e-05);
            m_SunVal[i][m_ClosestSun[i]]=tempVec;
        }
    }
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
                smx<<m_SunVal[j][i][0]<<"\t"<<m_SunVal[j][i][1]<<"\t"<<m_SunVal[j][i][2]<<std::endl;
            }
        }
    }
    smx.close();
    return true;
}



}
