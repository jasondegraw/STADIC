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

#ifndef RADFILEDATA_H
#define RADFILEDATA_H

#include "radprimitive.h"
#include <vector>
#include <memory>
#include <utility>

#include "stadicapi.h"

// This is a C++11 thing - if we can't use this then we're looking
// at a define or some other horrifying construct
template<typename T>
using shared_vector = std::vector<std::shared_ptr<T> >;

namespace stadic {

class STADIC_API RadFileData
{
public:
    explicit RadFileData();
    RadFileData(const shared_vector<RadPrimitive> &primitives);

    bool addRad(std::string file);                                      //Function to add rad primitives from a rad file
    //bool removeLayer(const QString &layer, const QString &removing, const QString &rest);   //Function to remove a layer from the list to its own geometry file
    bool blackOutLayer(std::string layer);                              //Function to black out a layer
    bool writeRadFile(std::string file);                                //Function to write the rad file from the list of primitives
    std::vector<double> surfaceNormal(std::string layer);               //Function that returns the surface normal as a vector of doubles

    bool addPrimitive(RadPrimitive *primitive);                         //Function to add a rad primitive to the list of primitives

    //Getters
    shared_vector<RadPrimitive> geometry() const;                       //Function to get just the geometry primitives as a vector
    shared_vector<RadPrimitive> materials() const;                      //Function to get just the material primitives as a vector
    shared_vector<RadPrimitive> primitives() const;                     //Function that returns all of the primitives as a vector


    template<class T> shared_vector<T> getPrimitives();
    // Splitting is officially broken
    //QPair<RadFileData *, RadFileData *> split(bool (*f)(RadPrimitive*));
    //template <class T> QPair<RadFileData*, RadFileData*> split(bool(*f)(RadPrimitive*, const T&), const T &label);
    // This one is the one that is most critical, but it needs to be redone
    std::pair<shared_vector<RadPrimitive>, shared_vector<RadPrimitive> > split(const std::vector<std::string> &vector);

private:
    shared_vector<RadPrimitive> m_Primitives; //Vector to hold EVERYTHING

};

template<class T> shared_vector<T> RadFileData::getPrimitives()
{
    shared_vector<T> primitives;
    for(auto primitive : m_Primitives) {
        auto cast = std::dynamic_pointer_cast<T>(primitive);
        if(cast) {
            primitives.push_back(cast);
        }
    }
    return primitives;
}

/*
template <class T> QPair<RadFileData*, RadFileData*> RadFileData::split(bool(*f)(RadPrimitive*, const T&), const T &label)
{
    std::vector<RadPrimitive*> in, out;
    for (RadPrimitive *primitive : m_Primitives) {
        if (f(primitive, label)) {
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
    return QPair<RadFileData*, RadFileData*>(first, second);
}
*/

}

#endif // RADFILEDATA_H
