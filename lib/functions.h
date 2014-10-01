#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
#include <vector>
#include "stadicapi.h"
#include <boost/property_tree/ptree.hpp>
#include "logging.h"
namespace stadic{

std::vector<std::string> STADIC_API split(std::string line, char delimiter);                    //Function that splits a string given a delimiter
std::vector<std::string> STADIC_API trimmedSplit(std::string line, char delimiter);             //Function that trims and splits a string given a delimiter
std::string STADIC_API trim(std::string string);                                                //Function that removes whitespace from either end of a string
double STADIC_API toDouble(const std::string &string, bool *ok = nullptr);                      //Function that takes a string and returns a double
std::string STADIC_API toString(double value);                                                  //Function that takes a double and returns a string

//******************
//Boost ptree functions
//******************
boost::optional<double> getDouble(boost::property_tree::ptree json, std::string key, std::string errorMissing, std::string errorBad, Severity severity);
boost::optional<int> getInt(boost::property_tree::ptree json, std::string key, std::string errorMissing, std::string errorBad, Severity severity);
boost::optional<std::string> getString(boost::property_tree::ptree json, std::string key, std::string errorMissing, std::string errorBad, Severity severity);
boost::optional<bool> getBool(boost::property_tree::ptree json, std::string key, std::string errorMissing, std::string errorBad, Severity severity);
boost::optional<boost::property_tree::ptree> getTree(boost::property_tree::ptree json, std::string key, std::string errorMissing, Severity severity);



}
#endif // FUNCTIONS_H
