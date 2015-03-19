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

#ifndef RAYTRACE_H
#define RAYTRACE_H

#include <istream>
#include <string>
#include <sstream>
#include <functional>
#include <queue>
#include <boost/optional.hpp>

#include "stadicapi.h"
#include "radprimitive.h"
#include "radfiledata.h" // For shared vector

namespace stadic {

class STADIC_API RayTrace
{
public:
    //RayTrace();
    RayTrace(RayTrace *parent, std::shared_ptr<RadPrimitive> &primitive);
    std::shared_ptr<RadPrimitive> primitive() const;
    std::vector<RayTrace*> children() const;

    void read(const std::string &filename);

private:
    RayTrace *m_parent;
    std::shared_ptr<RadPrimitive> m_primitive;
    std::vector<RayTrace*> m_children; // Maybe should use a smart pointer here?

};

}

#endif // RAYTRACE_H
