/******************************************************************************
 * Copyright (c) 2015, Alliance for Sustainable Energy
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

#include "raytrace.h"
#include "functions.h"
#include <iostream>
#include <fstream>

namespace stadic {

RayTrace::RayTrace(RayTrace *parent, std::shared_ptr<RadPrimitive> &primitive) : m_parent(parent),
    m_primitive(primitive)
{
    if(m_parent) {
        m_parent->m_children.push_back(this);
    }
}

RayTrace::~RayTrace()
{
    for(auto child : m_children) {
        delete child;
    }
}

std::shared_ptr<RadPrimitive> RayTrace::primitive() const
{
    return m_primitive;
}

std::vector<RayTrace*> RayTrace::children() const
{
    return m_children;
}

bool RayTrace::read(const std::string &filename, shared_vector<RadPrimitive> &geometry)
{
    // Clear any previous content
    m_primitive.reset();
    for(auto child : m_children) {
        delete child;
    }
    m_raysTerminate = true;
    // Read a file with this as the root
    std::ifstream input(filename);
    std::string line;
    // Read the header
    do {
        std::getline(input, line);
    } while(!line.empty());
    if(input.eof()){
        return false;
    }
    // Read the name of the root
    std::getline(input, line);
    line = trim(line);
    if(line.empty()) {
        return false;
    }
    // Find the primitive
    for(auto primitive : geometry) {
        if(primitive->name() == line) {
            m_primitive = primitive;
        }
    }
    // Probably easiest to do this recursively with a function that reads the children of the current top-level

    return true;
}

bool RayTrace::raysTerminate()
{
    return m_raysTerminate;
}

}
