/******************************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
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

#include "filepath.h"
#include "logging.h"
#include <sstream>
#include <iostream>
#ifdef _MSC_VER
#include <Windows.h>
#else
#include <sys/stat.h>
#include <string.h>
#endif

namespace stadic{

bool isDir(const std::string &dir)
{
#ifdef _MSC_VER
    DWORD result = GetFileAttributes(dir.c_str());
    if(result != INVALID_FILE_ATTRIBUTES) {
        return result & FILE_ATTRIBUTE_DIRECTORY;
    }
    return false;
#else //POSIX
    struct stat path;

    if(stat(dir.c_str(), &path)==0 && S_ISDIR(path.st_mode)){
        return true;
    }
    return false;
#endif
}

bool isFile(const std::string &file)
{
#ifdef _MSC_VER
    DWORD result = GetFileAttributes(file.c_str());
    if(result != INVALID_FILE_ATTRIBUTES) {
        return !(result & FILE_ATTRIBUTE_DIRECTORY);
    }
    return false;
#else //POSIX
    struct stat path;

    if(stat(file.c_str(), &path)==0 && S_ISREG(path.st_mode)){
        return true;
    }
    return false;
#endif
}

bool exists(const std::string &path)
{
    if(isDir(path)){
        return true;
    }
    return isFile(path);
}

}
