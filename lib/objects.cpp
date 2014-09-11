#include "objects.h"
//#include <sys/types.h>
#ifdef _MSVC

#else
#include <sys/stat.h>
#endif

#include "logging.h"
#include <iostream>

namespace stadic{
//*************************
//FilePath
//*************************
FilePath::FilePath(std::string path)
{
    m_Path=path;
    if (isFile()){
        lastMod();
    }
}
//Setters


//Getters
std::string FilePath::toString(){
    return m_Path;
}

//Utilities
bool FilePath::isDir(){
#ifdef __MSVC

#else //POSIC
    struct stat path;

    if (stat(m_Path.c_str(), &path)==0 && S_ISDIR(path.st_mode)){
        return true;
    }
    return false;
#endif
}

bool FilePath::isFile(){
#ifdef _MSVC

#else //POSIX
    struct stat path;

    if (stat(m_Path.c_str(), &path)==0 && S_ISREG(path.st_mode)){
        return true;
    }
    return false;
#endif
}
bool FilePath::exists(){
    if (isDir()){
        return true;
    }
    if (isFile()){
        return true;
    }
    return false;
}

bool FilePath::isUpdated(){
#ifdef _MSVC

#else //POSIX
    struct tm* originalMod=m_LastMod;
    if (!lastMod()){
        ERROR("There was an error checking the file.");
        return false;
    }
    if (difftime(mktime(m_LastMod),mktime(originalMod))>0){
        return true;
    }
    return false;
#endif
}

//Private
void File::lastMod(){
    struct stat path;
    if (isFile()){
        stat(m_Path.c_str(),&path);
        m_LastMod=localtime(&path.st_mtime);
    }
}

//*************************
//Process
//*************************
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
