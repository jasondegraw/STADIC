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
 *    and the following Disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University
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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
#include <vector>
#include <sstream>
#include "stadicapi.h"
#include <boost/property_tree/ptree.hpp>
#include "logging.h"
namespace stadic{

std::vector<std::string> STADIC_API split(std::string line, char delimiter);                    //Function that splits a string given a delimiter
std::vector<std::string> STADIC_API trimmedSplit(std::string line, char delimiter);             //Function that trims and splits a string given a delimiter
std::string STADIC_API trim(std::string string);                                                //Function that removes whitespace from either end of a string
double STADIC_API toDouble(const std::string &string, bool *ok = nullptr);                      //Function that takes a string and returns a double
int STADIC_API toInteger(const std::string &string, bool *ok = nullptr);
template <typename T> std::string toString(T value)  //Function that takes a double and returns a string
{
    std::stringstream stream;
    stream << value;
    return stream.str();
}


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
