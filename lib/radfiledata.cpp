/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
 * Copyright (c) 2015-2016, Jason W. DeGraw
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

#include "radfiledata.h"
#include "functions.h"
#include "radparser.h"
#include "logging.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <math.h>


namespace stadic {

RadFileData::RadFileData()
{
}

RadFileData::RadFileData(const shared_vector<RadPrimitive> &primitives) : m_primitives(primitives)
{
}

//Setters
bool RadFileData::addRad(const std::string &file)
{
    std::ifstream data;
    data.open(file);
    if (!data.is_open()){
        STADIC_ERROR("The opening of the rad file '"+file+"' failed.");
        return false;
    }
    RadParser parser(data);
    shared_vector<RadPrimitive> primitives;
    while(!parser.endOfInput()) {
//    while (!data.eof()) {
        try {
            std::shared_ptr<RadPrimitive> primitive = RadPrimitive::fromRad(parser);
            if(primitive == nullptr) {
                break;
            }
            primitives.push_back(primitive);
        } catch(const std::runtime_error &) {
            data.close();
            STADIC_LOG(Severity::Warning, "Exception caught reading from '" + file + "'.");
            return false;
        }
    }
    data.close();
    if(primitives.size() == 0) {
        STADIC_LOG(Severity::Warning, "No primitives were read from '" + file + "'.");
        return false;
    }
    m_primitives.insert(m_primitives.end(),primitives.begin(),primitives.end());
    return true;
}

/*
QPair<RadFileData*,RadFileData*> RadFileData::split(bool (*f)(RadPrimitive*))
{
    std::vector<RadPrimitive*> in, out;
    for(RadPrimitive *primitive : m_Primitives) {
        if(f(primitive)) {
            in.push_back(primitive);
        } else {
            out.push_back(primitive);
        }
    }
    // Account for 0 size vectors
    RadFileData *first = nullptr;
    RadFileData *second = nullptr;
    if (in.size() > 0) {
        first = new RadFileData(in, this->parent());
    }
    if (out.size() > 0) {
        second = new RadFileData(out, this->parent());
    }
    return QPair<RadFileData*,RadFileData*>(first,second);
}
*/

std::shared_ptr<RadPrimitive> RadFileData::glazingsearch(std::string name)

{
	//shared_vector<RadPrimitive> in, out;
	std::shared_ptr<RadPrimitive> foundglazinglayer = nullptr;
	for (std::shared_ptr<RadPrimitive> primitive : m_primitives) {
		if (primitive->isMaterial()) {
			if (primitive->name() == name)
			{
				foundglazinglayer = primitive;
				return foundglazinglayer;
			}
		}
	}
	return foundglazinglayer;
}

std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive> >  RadFileData::split(const std::vector<std::string> &vector)
{
	shared_vector<RadPrimitive> in, out;
    for(std::shared_ptr<RadPrimitive> primitive : m_primitives) {
        if(primitive->isMaterial()) {
            if(std::find(vector.begin(),vector.end(),primitive->name()) != vector.end()) {
                in.push_back(primitive);
            } else {
                out.push_back(primitive);
            }
        } else if(primitive->isGeometry()) {
            if(std::find(vector.begin(),vector.end(),primitive->modifierName()) != vector.end()) {
                in.push_back(primitive);
            } else {
                out.push_back(primitive);
            }
        } else {
            out.push_back(primitive);
        }
    }
    // In this new version, the caller is responsible for checking that the vectors actually contain something
    return std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive>>(in, out);
}

bool RadFileData::writeRadFile(const std::string &file)
{
    std::ofstream oFile;
    oFile.open(file);
    if (!oFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the rad file named " + file + " has failed.");
    }

    // Write a header
    oFile << "## Radiance geometry file \"" + file + "\"" << std::endl;
    // It would be nice to write out more detail about what is going on here. A minimum would be
    // to write the date, time and the version of the library. We could also include the
    // filenames of any files that were added with addRad (but we'd need to start storing that).

    shared_vector<RadPrimitive> primitives=materials();
    size_t count = primitives.size();
    if(count > 0) {
        oFile << "## Materials" << std::endl;
        for(auto ptr : primitives) {
            oFile << ptr->toRad() << std::endl;
        }
    }

    // Write out the aliases - I'm not sure this is actually correct
    if(m_aliases.size()) {
        oFile << "## Aliases" << std::endl;
        for(auto pair : m_aliases) {
            oFile << "void alias " << pair.first->name() << " " << pair.second->name() << std::endl;
        }
    }

    primitives=geometry();
    count += primitives.size();
    if(primitives.size() > 0) {
        oFile << "## Geometry" << std::endl;
        for(auto ptr : primitives) {
            oFile << ptr->toRad() << std::endl;
        }
    }

    if(count != m_primitives.size()) {
        STADIC_LOG(Severity::Warning, "Only materials and geometry were written to " + file + ", "
            + toString(m_primitives.size() - count) + " primitives were not written out");
    }

    oFile << "## End of Radiance geometry file \"" + file + "\"" << std::endl;

    return true;
}

std::vector<double> RadFileData::surfaceNormal(const std::string &layer){
    std::vector<double> normalVector;
    for(int i=0;i<m_primitives.size();i++) {
        if(m_primitives[i]->modifierName()==layer && m_primitives[i]->typeString()=="Polygon") {
            std::vector<std::vector<double>> normalPoints;
            for (int j=0;j<m_primitives[i]->arg3().size();j++){
                std::vector<double> temp;
                for (int p=0;p<3;p++){
                    temp.push_back(atof(m_primitives[i]->arg3()[j].c_str()));
                    j++;
                }
                normalPoints.push_back(temp);
            }
            double x=(normalPoints[2][1]-normalPoints[1][1])*(normalPoints[0][2]-normalPoints[1][2])-(normalPoints[2][2]-normalPoints[1][2])*(normalPoints[0][1]-normalPoints[1][2]);
            double y=(normalPoints[2][0]-normalPoints[1][0])*(normalPoints[0][2]-normalPoints[1][2])-(normalPoints[2][2]-normalPoints[1][2])*(normalPoints[0][0]-normalPoints[1][0]);
            double z=(normalPoints[2][0]-normalPoints[1][0])*(normalPoints[0][1]-normalPoints[1][1])-(normalPoints[2][1]-normalPoints[1][1])*(normalPoints[0][0]-normalPoints[1][0]);
            double length=sqrt(x*x+y*y+z*z);
            x=x/length;
            y=y/length;
            z=z/length;
            normalVector.push_back(x);
            normalVector.push_back(y);
            normalVector.push_back(z);
            return normalVector;
        }
    }
    return normalVector;
}

std::shared_ptr<RadPrimitive> RadFileData::addPrimitive(RadPrimitive *primitive)
{
    m_primitives.push_back(std::shared_ptr<RadPrimitive>(primitive));
    return m_primitives.back();
}

std::shared_ptr<RadPrimitive> RadFileData::addPrimitive(std::shared_ptr<RadPrimitive> primitive)
{
    m_primitives.push_back(primitive);
    return primitive;
}

bool RadFileData::setPrimitives(const shared_vector<RadPrimitive> &primitives)
{
    m_primitives = primitives;
    return true;
}

//Getters
shared_vector<RadPrimitive> RadFileData::geometry() const
{
    shared_vector<RadPrimitive> primitives;
    for(auto primitive : m_primitives) {
        if(primitive->isGeometry()) {
            primitives.push_back(primitive);
        }
    }
    return primitives;
}

shared_vector<RadPrimitive> RadFileData::materials() const
{
    shared_vector<RadPrimitive> primitives;
    for(auto primitive : m_primitives) {
        if(primitive->isMaterial()) {
            primitives.push_back(primitive);
        }
    }
    return primitives;
}

shared_vector<RadPrimitive> RadFileData::primitives() const
{
    return m_primitives;
}

bool RadFileData::isConsistent()
{
    return RadPrimitive::checkModifierTree(m_primitives);
}

bool RadFileData::buildModifierTree()
{
    return RadPrimitive::buildModifierTree(m_primitives);
}

bool RadFileData::setAlias(std::shared_ptr<RadPrimitive> oldModifier, std::shared_ptr<RadPrimitive> newModifier)
{
    shared_vector<RadPrimitive> mats = materials();
    if (std::find(mats.begin(), mats.end(), oldModifier) == mats.end()) {
        return false; // Bail out, oldModifier is not in this file
    }
    if (std::find(mats.begin(), mats.end(), newModifier) == mats.end()) {
        return false; // Bail out, newModifier is not in this file
    }
    m_aliases.push_back(std::pair<std::shared_ptr<RadPrimitive>, std::shared_ptr<RadPrimitive>>(oldModifier, newModifier));
    return true;
}

std::vector<std::pair<std::shared_ptr<RadPrimitive>, std::shared_ptr<RadPrimitive>>> RadFileData::aliases() const
{
    return m_aliases;
}

bool RadFileData::setAliases(const std::vector<std::pair<std::shared_ptr<RadPrimitive>, std::shared_ptr<RadPrimitive>>> &aliases)
{
    shared_vector<RadPrimitive> mats = materials();
    for (auto pair : aliases) {
        if (std::find(mats.begin(), mats.end(), pair.first) == mats.end()) {
            return false; // Bail out, oldModifier is not in this file
        }
        if (std::find(mats.begin(), mats.end(), pair.second) == mats.end()) {
            return false; // Bail out, newModifier is not in this file
        }
    }
    m_aliases = aliases;
    return true;
}

std::shared_ptr<RadPrimitive> RadFileData::findPrimitive(std::function<bool(std::shared_ptr<RadPrimitive>)> predicate) const
{
    for (auto primitive : m_primitives) {
        if (predicate(primitive)) {
            return primitive;
        }
    }
    return nullptr;
}

std::shared_ptr<RadPrimitive> RadFileData::findMaterial(std::function<bool(std::shared_ptr<RadPrimitive>)> predicate) const
{
    for (auto primitive : m_primitives) {
        if (primitive->isMaterial()) {
            if (predicate(primitive)) {
                return primitive;
            }
        }
    }
    return nullptr;
}

std::shared_ptr<RadPrimitive> RadFileData::findGeometry(std::function<bool(std::shared_ptr<RadPrimitive>)> predicate) const
{
    for (auto primitive : m_primitives) {
        if (primitive->isGeometry()) {
            if (predicate(primitive)) {
                return primitive;
            }
        }
    }
    return nullptr;
}

}
