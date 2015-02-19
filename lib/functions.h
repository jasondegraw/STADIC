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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
#include <vector>
#include <sstream>
#include "stadicapi.h"
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
std::string STADIC_API wrapAtN(const std::string &text, unsigned N = 72, unsigned indent = 0,
    bool hangingIndent = false);

}
#endif // FUNCTIONS_H
