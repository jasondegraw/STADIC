/****************************************************************
 * Copyright © 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted for
 * personal and commercial purposes provided that the
 * following conditions are met:
 *
 * 1. Redistribution of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following Disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University
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

#include "filepath.h"
#include "logging.h"
#include <sstream>

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

#ifndef USE_QT
//#include <boost/process.hpp>
//namespace bp = ::boost::process;
#include <boost/filesystem.hpp>
#include <thread>
#include <fstream>
#endif


namespace stadic{
//*************************
//FilePath
//*************************
FilePath::FilePath(std::string path)
{
    m_Path = path;
#ifdef _MSC_VER
    m_fileAttr = new WIN32_FILE_ATTRIBUTE_DATA;
    GetFileAttributesEx(m_Path.c_str(), GetFileExInfoStandard, m_fileAttr);
#else
    if (isFile()){
        lastMod();
    }
#endif
}

FilePath::~FilePath()
{
#ifdef _MSC_VER
    delete m_fileAttr;
#endif
}

//Getters
std::string FilePath::toString(){
    return m_Path;
}

//Utilities
bool FilePath::isDir(){
#ifdef _MSC_VER
    //WIN32_FILE_ATTRIBUTE_DATA fileAttr;
    //if(GetFileAttributesEx(m_Path.c_str(), GetFileExInfoStandard, &fileAttr)) {
    //    return fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    //}
    DWORD result = GetFileAttributes(m_Path.c_str());
    if(result != INVALID_FILE_ATTRIBUTES) {
        return result & FILE_ATTRIBUTE_DIRECTORY;
    }
    return false;
#else //POSIX
    struct stat path;

    if (stat(m_Path.c_str(), &path)==0 && S_ISDIR(path.st_mode)){
        return true;
    }
    return false;
#endif
}

bool FilePath::isFile(){
#ifdef _MSC_VER
    DWORD result = GetFileAttributes(m_Path.c_str());
    if(result != INVALID_FILE_ATTRIBUTES) {
        return !(result & FILE_ATTRIBUTE_DIRECTORY);
    }
    return false;
#else //POSIX
    struct stat path;

    if (stat(m_Path.c_str(), &path)==0 && S_ISREG(path.st_mode)){
        return true;
    }
    return false;
#endif
}

bool FilePath::exists(){
    if(isDir()){
        return true;
    }
    return isFile();
    return false;
}

bool FilePath::isUpdated(){
#ifdef _MSC_VER
    FILETIME lastTime = m_fileAttr->ftLastWriteTime;
    if(GetFileAttributesEx(m_Path.c_str(), GetFileExInfoStandard, m_fileAttr)) {
        return CompareFileTime(&(m_fileAttr->ftLastWriteTime), &lastTime) > 0;
    }
    return false;
#else //POSIX
    if(isDir()) {
        return false;
    }
    struct tm* originalMod=m_LastMod;
    lastMod();
    if (difftime(mktime(m_LastMod),mktime(originalMod))>0){
        return true;
    }
    return false;
#endif
}

//Private
void FilePath::lastMod(){
#ifdef _MSC_VER
    // On Windows, this function is not needed.
#else
    struct stat path;
    if (isFile()){
        stat(m_Path.c_str(),&path);
        m_LastMod=localtime(&path.st_mtime);
    }
#endif
}

}
