#ifndef DAYILL_H
#define DAYILL_H

#include "stadicapi.h"
#include <vector>
#include <string>

namespace stadic {

class STADIC_API TemporalIlluminance
{
public:
    TemporalIlluminance();
    TemporalIlluminance(int month, int day, double hour, const std::vector<double> &illuminance);

    bool add(std::vector<double> newIll);                                       //Function to add a vector of illuminance values to the current object


    //Getters
    std::vector<double> lux();                                                  //Function that returns the temporal illuminance as a vector in lux
    std::vector<double> fc();                                                   //Function that returns the temporal illuminance as a vector in fc
    int month();                                                                //Function that returns the month as an int
    int day();                                                                  //Function that returns the day as an int
    double hour();                                                              //Function that returns the hour as a double

private:
    std::vector<double> m_Illuminance;                                          //Vector holding the illuminance values for a given interval
    int m_Month;                                                                //Variable holding the month as an int
    int m_Day;                                                                  //Variable holding the day as an int
    double m_Hour;                                                              //Variable holding the hour as an int
};


class STADIC_API DaylightIlluminanceData
{
public:
    explicit DaylightIlluminanceData();
    bool parse(std::string fileName,std::string weaFile);                       //Function to parse an illuminance file that doesn't contain time values
    bool parseTimeBased(std::string fileName);                                  //Function to parse an illuminance file that contains time values
    bool addIllFile(std::string fileName);                                      //Function to add the illuminance of a file that doesn't contain time values to the object
    bool addTimeBasedIll(std::string fileName);                                 //Function to add the illuminance of a file that contains time values to the object
    bool writeIllFileLux(std::string fileName);                                    //Function to write the illuminance file in lux
    bool writeIllFileFC(std::string fileName);                                  //Function to write the illuminance file in fc
    //Getters
    std::vector<TemporalIlluminance> illuminance();                             //Function that returns the illuminance values in a vector

    //int hoursGreaterThan(double value, int point);

private:
    std::vector<TemporalIlluminance> m_data;                                    //Vector holding the illuminance values

};

}

#endif // DAYILL_H
