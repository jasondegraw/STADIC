#include "objects.h"
//#include <sys/types.h>
#include <sys/stat.h>
#include "logging.h"
#include <iostream>

// Hack to get stuff compiling
//#define S_ISDIR(a) true
//#define S_ISREG(a) true

namespace stadic{
//*************************
//Dir
//*************************
Dir::Dir()
{

}
//Setters
bool Dir::setPath(std::string path){
    if (path.find_last_of('/')==path.length()-1){
        m_Path=path;
        return true;
    }
    if (path.find_last_of('\\')==path.length()-1){
        m_Path=path;
        return true;
    }
    std::cout<<path<<std::endl;
    ERROR("The path must end in a separator (ex. '/', '\\', etc.");
    return false;
}

//Getters
std::string Dir::toString(){
    return m_Path;
}

//Utilities
bool Dir::isDir(){
    struct stat path;

    if (stat(m_Path.c_str(), &path)==0 && S_ISDIR(path.st_mode)){
        return true;
    }
    return false;
}
bool Dir::exists(){
    if (isDir()){
        return true;
    }
    return false;
}

bool Dir::cdUp(){
    std::string tempPath=m_Path.erase(m_Path.length()-1);
    std::size_t found=tempPath.find_last_of('/');
    if (found!=m_Path.npos){
        m_Path.erase(m_Path.begin()+found+1,m_Path.end());
    }else{
        found=tempPath.find_last_of('\\');
        if (found!=m_Path.npos){
            m_Path.erase(m_Path.begin()+found+1,m_Path.end());
        }else{
            ERROR("There are no more directories up from the current directory.");
            return false;
        }
    }
    return true;
}
bool Dir::cd(){

    return true;
}
//*************************
//File
//*************************
File::File()
{

}
//Setters
void File::setPath(std::string path){
    m_Path=path;
}

//Getters
Dir File::dir(){
    Dir tempDir;
    std::string tempPath=m_Path;
    std::size_t found=tempPath.find_last_of('/');
    if (found!=tempPath.npos){
        tempPath.erase(tempPath.begin()+found+1,tempPath.end());
    }else{
        found=tempPath.find_last_of('\\');
        if (found!=tempPath.npos){
            tempPath.erase(tempPath.begin()+found+1,tempPath.end());
        }else{
            ERROR("There are no more directories up from the current directory.");
            return tempDir;
        }
    }
    tempDir.setPath(tempPath);
    return tempDir;
}
std::string File::toString(){
    return m_Path;
}
std::string File::name(){
    std::string tempName=m_Path;
    std::size_t found=tempName.find_last_of('/');
    if (found!=tempName.npos){
        tempName.erase(tempName.begin(),tempName.begin()+found+1);
    }else{
        found=tempName.find_last_of('\\');
        if (found!=tempName.npos){
            tempName.erase(tempName.begin(),tempName.begin()+found+1);
        }else{
            ERROR("The file name cannot be obtained.");
            tempName.clear();
            return tempName;
        }
    }
    return tempName;
}

//Utilities
bool File::exists(){
    if (isFile()){
        return true;
    }
    return false;
}
bool File::isFile(){
    struct stat path;

    if (stat(m_Path.c_str(), &path)==0 && S_ISREG(path.st_mode)){
        return true;
    }
    return false;
}
bool File::isUpdated(){
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
bool File::lastMod(){
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



Process::Process(std::string program, std::vector<std::string> args)
{
    m_process.setProgram(QString::fromStdString(program));
    QStringList arguments;
    for(std::string arg : args) {
        arguments << QString::fromStdString(arg);
    }
    m_process.setArguments(arguments);
}


void Process::start()
{
    m_process.start();
}




bool Process::wait()
{
    return m_process.waitForFinished(-1);
}

std::string Process::error()
{
    return QString(m_process.readAllStandardError()).toStdString();
}

std::string Process::output()
{
    return QString(m_process.readAllStandardOutput()).toStdString();
}

void Process::setStandardOutputProcess(Process *destination)
{
    m_process.setStandardOutputProcess(&(destination->m_process));
}

void Process::setStandardErrorFile(const std::string &fileName)
{
    m_process.setStandardErrorFile(QString::fromStdString(fileName));
}

void Process::setStandardInputFile(const std::string &fileName)
{
    m_process.setStandardInputFile(QString::fromStdString(fileName));
}

void Process::setStandardOutputFile(const std::string &fileName)
{
    m_process.setStandardOutputFile(QString::fromStdString(fileName));
}

}
