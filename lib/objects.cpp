#include "objects.h"
//#include <sys/types.h>
#include <sys/stat.h>
#include "logging.h"

// Hack to get stuff compiling
#define S_ISDIR(a) true
#define S_ISREG(a) true

namespace stadic{

//FileSystem
FileSystem::FileSystem()
{

}
//Setters
void FileSystem::setPath(std::string path){
    m_Path=path;
}

//Getters


//Utilities
bool FileSystem::exists(){
    if(isDir()){
        return true;
    }
    if (isFile()){
        return true;
    }
    return false;
}
bool FileSystem::isDir(){
    struct stat path;

    if (stat(m_Path.c_str(), &path)==0 && S_ISDIR(path.st_mode)){
        return true;
    }
    return false;
}
bool FileSystem::isFile(){
    struct stat path;

    if (stat(m_Path.c_str(), &path)==0 && S_ISREG(path.st_mode)){
        return true;
    }
    return false;
}
bool FileSystem::isUpdated(){
    struct tm* originalMod=m_LastMod;
    if (!lastMod()){
        ERROR("There was an error checking the file.");
        return false;
    }
    if (difftime(mktime(m_LastMod),mktime(originalMod))>0){
        return true;
    }
    return false;
}
bool FileSystem::lastMod(){
    struct stat path;
    if (isFile()){
        stat(m_Path.c_str(),&path);
        m_LastMod=localtime(&path.st_mtime);
    }else{
        ERROR("The path provided is not a file.");
        return false;
    }
    return true;
}


//Private


}
