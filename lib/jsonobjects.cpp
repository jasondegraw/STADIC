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

namespace stadic {

boost::optional<JsonObject> readJsonDocument(const std::string &filename)
{
    std::ifstream iFile;
    iFile.open(filename);
    if(!iFile.is_open()){
        STADIC_LOG(Severity::Warning, "Failed to open json file \"" + filename + "\".");
        return boost::none;
    }

    Json::Reader reader;
    JsonObject root;
    bool success = reader.parse(iFile, root);
    if(!success) {
        STADIC_LOG(Severity::Warning, "Failed to parse json file \"" + filename + "\".");
        return boost::none;
    }

    return boost::optional<JsonObject>(root);
}
boost::optional<double> STADIC_API getDouble(const JsonObject &json, const std::string &key){
    JsonObject value = json[key];
        if(value.isNull()) {
        return boost::none;
    } else if(value.isDouble()) {
        return boost::optional<double>(value.asDouble());
    }
    return boost::none;
}

boost::optional<double> getDouble(const JsonObject &json, const std::string &key, const std::string &errorMissing,
    const std::string &errorBad, Severity severity)
{
    JsonObject value = json[key];
    if(value.isNull()) {
        STADIC_LOG(severity, errorMissing);
    } else {
        if(value.isDouble()) {
            return boost::optional<double>(value.asDouble());
        } else {
            STADIC_LOG(severity, errorBad);
        }
    }
    return boost::none;
}
boost::optional<int> STADIC_API getInt(const JsonObject &json, const std::string &key){
    JsonObject value = json[key];
    if(value.isNull()) {
        return boost::none;
    } else if(value.isInt()) {
        return boost::optional<int>(value.asInt());
    }
    return boost::none;
}

boost::optional<int> getInt(const JsonObject &json, const std::string &key, const std::string &errorMissing,
    const std::string &errorBad, Severity severity)
{
    JsonObject value = json[key];
    if(value.isNull()) {
        STADIC_LOG(severity, errorMissing);
    } else {
        if(value.isInt()) {
            return boost::optional<int>(value.asInt());
        } else {
            STADIC_LOG(severity, errorBad);
        }
    }
    return boost::none;
}

boost::optional<std::string> getString(const JsonObject &json, const std::string &key)
{
  JsonObject value = json[key];
  if(value.isNull()) {
    return boost::none;
  } else if(value.isString()) {
      return boost::optional<std::string>(value.asString());
  }
  return boost::none;
}

boost::optional<std::string> getString(const JsonObject &json, const std::string &key, const std::string &errorMissing,
    const std::string &errorBad, Severity severity)
{
    JsonObject value = json[key];
    if(value.isNull()) {
        STADIC_LOG(severity, errorMissing);
    } else {
        if(value.isString()) {
            return boost::optional<std::string>(value.asString());
        } else {
            STADIC_LOG(severity, errorBad);
        }
    }
    return boost::none;
}
boost::optional<bool> STADIC_API getBool(const JsonObject &json, const std::string &key){
    JsonObject value=json[key];
    if (value.isNull()){
        return boost::none;
    }else if(value.isBool()){
        return boost::optional<bool>(value.asBool());
    }
    return boost::none;
}

boost::optional<bool> getBool(const JsonObject &json, const std::string &key, const std::string &errorMissing,
    const std::string &errorBad, Severity severity)
{
    JsonObject value = json[key];
    if(value.isNull()) {
        STADIC_LOG(severity, errorMissing);
    } else {
        if(value.isBool()) {
            return boost::optional<bool>(value.asBool());
        } else {
            STADIC_LOG(severity, errorBad);
        }
    }
    return boost::none;
}

boost::optional<bool> getBool(const JsonObject &json, const std::string &key, bool defaultValue,
    const std::string &errorBad, Severity severity)
{
    JsonObject value = json[key];
    if(value.isNull()) {
        return boost::optional<bool>(defaultValue);
    } else {
        if(value.isBool()) {
            return boost::optional<bool>(value.asBool());
        } else {
            STADIC_LOG(severity, errorBad);
        }
    }
    return boost::none;
}

boost::optional<JsonObject> getObject(const JsonObject &json, const std::string &key)
{
    JsonObject value = json[key];
    if(!value.isNull()) {
        return boost::optional<JsonObject>(value);
    }
    return boost::none;
}

boost::optional<JsonObject> getObject(const JsonObject &json, const std::string &key,
    const std::string &errorMissing, Severity severity)
{
    JsonObject value = json[key];
    if(!value.isNull()) {
        return boost::optional<JsonObject>(value);
    }
    STADIC_LOG(severity, errorMissing);
    return boost::none;
}

boost::optional<JsonObject> getArray(const JsonObject &json, const std::string &key)
{
    JsonObject value = json[key];
    if(!value.isNull()) {
        if(value.isArray()) {
            return boost::optional<JsonObject>(value);
        }
    }
    return boost::none;
}

boost::optional<JsonObject> getArray(const JsonObject &json, const std::string &key,
    const std::string &errorMissing, Severity severity)
{
    JsonObject value = json[key];
    if(!value.isNull()) {
        if(value.isArray()) {
            return boost::optional<JsonObject>(value);
        }
    }
    STADIC_LOG(severity, errorMissing);
    return boost::none;
}

boost::optional<double> asDouble(const JsonObject &json, const std::string &errorBad, Severity severity)
{
    if(json.isDouble()) {
        return boost::optional<double>(json.asDouble());
    }
    return boost::none;
}

boost::optional<int> asInt(const JsonObject &json, const std::string &errorBad, Severity severity)
{
    if(json.isInt()) {
        return boost::optional<int>(json.asInt());
    }
    return boost::none;
}

boost::optional<std::string> asString(const JsonObject &json, const std::string &errorBad, Severity severity)
{
    if(json.isString()) {
        return boost::optional<std::string>(json.asString());
    }
    return boost::none;
}

}
