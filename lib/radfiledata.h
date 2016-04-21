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

#ifndef RADFILEDATA_H
#define RADFILEDATA_H

#include "radprimitive.h"
#include <vector>
#include <memory>
#include <utility>

#include "sharedvector.h"

#include "stadicapi.h"

namespace stadic {

class STADIC_API RadFileData
{
public:
    RadFileData();
    explicit RadFileData(const shared_vector<RadPrimitive> &primitives);

    bool addRad(const std::string &file);                                      //Function to add rad primitives from a rad file
    bool writeRadFile(const std::string &file);                                //Function to write the rad file from the list of primitives
    std::vector<double> surfaceNormal(const std::string &layer);               //Function that returns the surface normal as a vector of doubles

    std::shared_ptr<RadPrimitive> addPrimitive(RadPrimitive *primitive);    //!< Add a rad primitive to the list of primitives
    std::shared_ptr<RadPrimitive> addPrimitive(std::shared_ptr<RadPrimitive> primitive);  //!< Add a rad primitive to the list of primitives
    bool setPrimitives(const shared_vector<RadPrimitive> &primitives);  //!< Replace the contents of the primitives list
    std::shared_ptr<RadPrimitive> RadFileData::glazingsearch(std::string name);

    std::shared_ptr<RadPrimitive> findPrimitive(std::function<bool(std::shared_ptr<RadPrimitive>)> predicate) const;    //!< Find a particular primitive
    std::shared_ptr<RadPrimitive> findMaterial(std::function<bool(std::shared_ptr<RadPrimitive>)> predicate) const;    //!< Find a particular material
    std::shared_ptr<RadPrimitive> findGeometry(std::function<bool(std::shared_ptr<RadPrimitive>)> predicate) const;    //!< Find a particular geometry

    //Getters
    shared_vector<RadPrimitive> geometry() const;  //!< Returns all geometry primitives as a vector
    shared_vector<RadPrimitive> materials() const;  //!< Returns all material primitives as a vector
    shared_vector<RadPrimitive> primitives() const;  //!< Returns all of the primitives as a vector
    std::vector<std::pair<std::shared_ptr<RadPrimitive>, std::shared_ptr<RadPrimitive>>> aliases() const;  //!< Returns all of the aliases as a vector of pairs

    // Aliasing
    bool setAlias(std::shared_ptr<RadPrimitive> newModifier, std::shared_ptr<RadPrimitive> oldModifier);
    bool setAliases(const std::vector<std::pair<std::shared_ptr<RadPrimitive>, std::shared_ptr<RadPrimitive>>> &aliases);

    // Consistency
    bool isConsistent();
    bool buildModifierTree();

    template<class T> shared_vector<T> getPrimitives();
    // This one is the one that is most critical, but it needs to be redone or possibly removed
    std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive> > split(const std::vector<std::string> &vector);
    std::vector < std::pair<std::shared_ptr<RadPrimitive>, std::shared_ptr<RadPrimitive>>> m_aliases;

private:
    shared_vector<RadPrimitive> m_primitives; //Vector to hold EVERYTHING
    //std::vector < std::pair<std::shared_ptr<RadPrimitive>, std::shared_ptr<RadPrimitive>>> m_aliases; MOVED TO PUBLIC  Stop-gap to get this done now

};

template<class T> shared_vector<T> RadFileData::getPrimitives()
{
    shared_vector<T> primitives;
    for(auto primitive : m_primitives) {
        auto cast = std::dynamic_pointer_cast<T>(primitive);
        if(cast) {
            primitives.push_back(cast);
        }
    }
    return primitives;
}

}

#endif // RADFILEDATA_H
