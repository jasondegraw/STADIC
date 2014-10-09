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
