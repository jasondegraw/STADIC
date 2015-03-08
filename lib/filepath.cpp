/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
 * Copyright (c) 2015, Jason W. DeGraw
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
#include "functions.h"
#include <sstream>
#include <iostream>
#ifdef _MSC_VER
#include <Windows.h>
#else
#include <sys/stat.h>
#include <string.h>
#endif

#ifdef _MSC_VER
#define UNLINK _unlink
#else
#define UNLINK unlink
#endif

// Will need to fix later
#include <direct.h>

#ifdef _WIN32
#define PATHSEPARATOR '\\'
#define MKDIR _mkdir
#else
#define PATHSEPARATOR '/'
#define MKDIR mkdir
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

PathName::PathName(const std::string &path) : m_isFile(false)
{
    if(path.size() > 0) {
        m_parts = split(path, '/');
        if(m_parts.size() > 0) {
            if(*(path.end() - 1) != '/'){
                m_isFile = true;
                m_filename = *(m_parts.end() - 1);
                m_parts.pop_back();
            }
        }
    }
}

bool PathName::create() const
{
    if(m_parts.size() > 0){
        std::string current = m_parts[0];
        MKDIR(current.c_str());
        for(unsigned i = 1; i<m_parts.size(); ++i){
            current += PATHSEPARATOR + m_parts[i];
            if(!stadic::exists(current)) {
                MKDIR(current.c_str());
            }
        }
        if(m_isFile) {
            std::string name = toString();
            FILE* fp = fopen(name.c_str(), "w");
            fclose(fp);
        }
        return true;
    }
    return false;
}

bool PathName::remove() const
{
    std::vector<std::string> paths;
    std::string result;
    if(m_parts.size() > 0) {
        result = m_parts[0] + PATHSEPARATOR;
        paths.push_back(result);
        for(unsigned i = 1; i<m_parts.size(); ++i) {
            result += m_parts[i] + PATHSEPARATOR;
            paths.push_back(result);
        }
    }
    if(m_isFile) {
        result += m_filename;
#ifdef _WIN32
        if(!DeleteFile(result.c_str())) {
            return false;
        }
#else
        ADD POSIX VERSION HERE
#endif
    }
    //for(auto string : paths) {
    //    std::cout << string << std::endl;
    //}
    for(auto iter = paths.rbegin(); iter != paths.rend(); iter++) {
        //std::cout << *iter << std::endl;
#ifdef _WIN32
        if(!RemoveDirectory(iter->c_str())) {
            return false;
        }
#else
        ADD POSIX VERSION HERE
#endif
        if(stadic::exists(*iter)) {
            return false;
        }
    }
    return true;
}

bool PathName::isFile() const
{
    return m_isFile;
}

std::string PathName::toString() const
{
    std::string result;
    if(m_parts.size() > 0) {
        result = m_parts[0] + PATHSEPARATOR;
        for(unsigned i = 1; i<m_parts.size(); ++i) {
            result += m_parts[i] + PATHSEPARATOR;
        }
    }
    if(m_isFile) {
        result += m_filename;
    }
    return result;
}

bool PathName::exists() const
{
    std::string file = toString();
    return stadic::exists(file);
}

}
