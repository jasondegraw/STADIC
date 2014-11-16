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

#include "jsonobjects.h"

#include <fstream>
#include <boost/property_tree/json_parser.hpp>

namespace stadic {

boost::optional<JsonObject> readJsonDocument(const std::string &filename)
{
    std::ifstream iFile;
    iFile.open(filename);
    if(!iFile.is_open()){
        return boost::none;
    }
    iFile.close();

    boost::property_tree::ptree json;
    boost::property_tree::read_json(filename, json);
    if(json.empty()){
        STADIC_LOG(Severity::Fatal, "The json file is empty");
        return boost::none;
    }

    return boost::optional<JsonObject>(json);
}

boost::optional<double> getDouble(const JsonObject &json, const std::string &key, const std::string &errorMissing,
    const std::string &errorBad, Severity severity)
{
    boost::optional<double> dVal;
    try{
        dVal = json.get<double>(key);
    } catch(const boost::property_tree::ptree_bad_path &){
        STADIC_LOG(severity, errorMissing);
    } catch(const boost::property_tree::ptree_bad_data &){
        STADIC_LOG(severity, errorBad);
    }
    return dVal;
}

boost::optional<int> getInt(const JsonObject &json, const std::string &key, const std::string &errorMissing,
    const std::string &errorBad, Severity severity)
{
    boost::optional<int> iVal;
    try{
        iVal = json.get<int>(key);
    } catch(const boost::property_tree::ptree_bad_path &){
        STADIC_LOG(severity, errorMissing);
    } catch(const boost::property_tree::ptree_bad_data &){
        STADIC_LOG(severity, errorBad);
    }
    return iVal;
}

boost::optional<std::string> getString(const JsonObject &json, const std::string &key, const std::string &errorMissing,
    const std::string &errorBad, Severity severity)
{
    boost::optional<std::string> sVal;
    try{
        sVal = json.get<std::string>(key);
    } catch(const boost::property_tree::ptree_bad_path &){
        STADIC_LOG(severity, errorMissing);
    } catch(const boost::property_tree::ptree_bad_data &){
        STADIC_LOG(severity, errorBad);
    }
    return sVal;
}

boost::optional<bool> getBool(const JsonObject &json, const std::string &key, const std::string &errorMissing,
    const std::string &errorBad, Severity severity)
{
    boost::optional<bool> bVal;
    try{
        bVal = json.get<bool>(key);
    } catch(const boost::property_tree::ptree_bad_path &){
        STADIC_LOG(severity, errorMissing);
    } catch(const boost::property_tree::ptree_bad_data &){
        STADIC_LOG(severity, errorBad);
    }
    return bVal;
}

boost::optional<bool> getBool(const JsonObject &json, const std::string &key, bool defaultValue, const std::string &errorBad, Severity severity)
{
    boost::optional<bool> bVal;
    try{
        bVal = json.get<bool>(key);
    } catch(const boost::property_tree::ptree_bad_path &){
        return boost::optional<bool>(defaultValue);
    } catch(const boost::property_tree::ptree_bad_data &){
        STADIC_LOG(severity, errorBad);
        return boost::optional<bool>(defaultValue);
    }
    return bVal;
}

boost::optional<JsonObject> getObject(const JsonObject &json, const std::string &key)
{
    boost::property_tree::ptree treeVal;
    try {
        treeVal = json.get_child(key);
    } catch(const boost::property_tree::ptree_bad_path &) {
        return boost::none;
    }
    return boost::optional<boost::property_tree::ptree>(treeVal);
}

boost::optional<JsonObject> getObject(const JsonObject &json, const std::string &key,
    const std::string &errorMissing, Severity severity)
{
    boost::property_tree::ptree treeVal;
    try {
        treeVal = json.get_child(key);
    } catch(const boost::property_tree::ptree_bad_path &) {
        STADIC_LOG(severity, errorMissing);
        return boost::none;
    }
    return boost::optional<boost::property_tree::ptree>(treeVal);
}

boost::optional<double> asDouble(const JsonObject &json, const std::string &errorBad, Severity severity)
{
    boost::optional<double> dVal;
    try{
        dVal = json.get<double>("");
    } catch(const boost::property_tree::ptree_bad_data &){
        STADIC_LOG(severity, errorBad);
    }
    return dVal;
}

boost::optional<int> asInt(const JsonObject &json, const std::string &errorBad, Severity severity)
{
    boost::optional<int> iVal;
    try{
        iVal = json.get<int>("");
    } catch(const boost::property_tree::ptree_bad_data &){
        STADIC_LOG(severity, errorBad);
    }
    return iVal;
}

boost::optional<std::string> asString(const JsonObject &json, const std::string &errorBad, Severity severity)
{
    boost::optional<std::string> sVal;
    try{
        sVal = json.get<std::string>("");
    } catch(const boost::property_tree::ptree_bad_data &) {
        STADIC_LOG(severity, errorBad);
    }
    return sVal;
}

}
