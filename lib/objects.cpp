#include "objects.h"
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

//*************************
//Process
//*************************
Process::Process(std::string program)
{
#ifdef USE_QT
    m_process.setProgram(QString::fromStdString(program));
#else
    m_index = 0;
    m_state = Initialized;
    m_upstream = nullptr;
    m_downstream = nullptr;
    m_program = boost::process::find_executable_in_path(program);
#endif
}

Process::Process(std::string program, std::vector<std::string> args)
{
#ifdef USE_QT
    m_process.setProgram(QString::fromStdString(program));
    QStringList arguments;
    for(std::string arg : args) {
        arguments << QString::fromStdString(arg);
    }
    m_process.setArguments(arguments);
#else
    m_index = 0;
    m_state = Initialized;
    m_upstream = nullptr;
    m_downstream = nullptr;
    m_program = boost::process::find_executable_in_path(program);
    m_args = args;
#endif
}


void Process::start()
{
#ifdef USE_QT
    m_process.start();
#else
    if(m_state == Initialized) {
        if(m_upstream == nullptr && m_downstream == nullptr) { // This is the simple case
            m_state = Running;
            boost::process::context ctx;
            ctx.stdout_behavior = boost::process::capture_stream();
            ctx.stderr_behavior = boost::process::capture_stream();
            try {
                m_children.push_back(boost::process::launch(m_program, m_args, ctx));
            }
            catch(boost::system::system_error &ex) {
                STADIC_ERROR(ex.what());
                m_state = RunFailed;
            }
            // Launch threads here to handle stdin, stdout, stderr
        } else { // This is a pipeline situation, so we'll only run if all children are ready
            m_state = ReadyToRun;
            //std::vector<boost::process::pipeline_entry> entries;
            std::deque<boost::process::pipeline_entry> downstream;
            std::deque<boost::process::pipeline_entry> upstream;
            
            // Set up some contexts
            boost::process::context ctxin;
            ctxin.stdin_behavior = boost::process::capture_stream();
            ctxin.stderr_behavior = boost::process::capture_stream();

            boost::process::context ctxout;
            ctxout.stdout_behavior = boost::process::inherit_stream();
            ctxout.stderr_behavior = boost::process::capture_stream();

            boost::process::context ctxlast;
            ctxlast.stdout_behavior = boost::process::capture_stream();
            ctxlast.stderr_behavior = boost::process::capture_stream();

            // Check the downstream direction
            Process *last = this;
            while(last->m_downstream != nullptr) {
                if(last->m_downstream->m_state == ReadyToRun) {
                    downstream.push_back(boost::process::pipeline_entry(last->m_program, last->m_args, ctxout));
                    last = last->m_downstream;
                } else {
                    return; // Bail out if any one is not ready to go
                }
            }
            // If we made it to here, all of the downstream processes are ready to go
            // We still need to set up the last one so that output can be captured
            downstream.push_back(boost::process::pipeline_entry(last->m_program, last->m_args, ctxlast));

            // Now check the upstream direction - rather than get tricky, add the current process again
            Process *first = this;
            while(first != nullptr) {
                if(first->m_upstream->m_state == ReadyToRun) {
                    upstream.push_front(boost::process::pipeline_entry(first->m_program, first->m_args, ctxout));
                    first = first->m_upstream;
                } else {
                    return; // Bail out if any one is not ready to go
                }
            }
            // If we made it to here, all of the upstream processes are ready to go
            // We still need to set up the first one so that input can be handled
            upstream.push_back(boost::process::pipeline_entry(first->m_program, first->m_args, ctxin));
            // Combine the two lists, skipping the first downstream element (it is double counted to save logic)
            upstream.insert(upstream.end(), downstream.begin()+1, downstream.end());
            boost::process::children subprocesses = boost::process::launch_pipeline(upstream);
            // Finish up by giving everyone a copy of the children and assigning indices
            unsigned index = 0;
            Process *current = first;
            while(current != nullptr) {
                current->m_index = index;
                current->m_children = subprocesses;
                index++;
            }
        }
    }
#endif
}

bool Process::wait()
{
#ifdef USE_QT
    return m_process.waitForFinished(-1);
#else
    if(m_state==RunCompleted || m_state==RunFailed) {
        return false;
    }
    if(m_upstream == nullptr && m_downstream == nullptr) { // This is the simple case
        boost::process::status result = m_children[0].wait();
        m_state = RunFailed;
        // Figure out what happened
        if(result.exited()) {
            if(result.exit_status() == 0) { // Hopefully this is correct
                m_state = RunCompleted;
                return true;
            }
        }
    } else {
        boost::process::status result = boost::process::wait_children(m_children);
        // Figure out what happened
        if(result.exited()) {
            return result.exit_status() == 0; // Hopefully this is correct
        }
    }
    return false;
#endif
}

std::string Process::error()
{
#ifdef USE_QT
    return QString(m_process.readAllStandardError()).toStdString();
#else
    if(m_state == Running || m_state == RunCompleted) {
        wait();
        std::stringstream stream;
        boost::process::pistream &is = m_children[m_index].get_stderr();
        std::string line;
        while(std::getline(is, line)) {
            stream << line << std::endl;
        }
        return stream.str();
    }
    return std::string();
#endif
}

std::string Process::output()
{
#ifdef USE_QT
    return QString(m_process.readAllStandardOutput()).toStdString();
#else
    if((m_state == Running || m_state == RunCompleted) && m_outputFile.empty()) {
        wait();
        unsigned index = m_children.size()-1;
        std::stringstream stream;
        boost::process::pistream &is = m_children[index].get_stdout();
        std::string line;
        while(std::getline(is, line)) {
            stream << line << std::endl;
        }
        return stream.str();
    }
    return std::string();
#endif
}

void Process::setStandardOutputProcess(Process *destination)
{
#ifdef USE_QT
    m_process.setStandardOutputProcess(&(destination->m_process));
#else
    if(destination->m_upstream) { // The target process already has an upstream process
        destination->m_upstream->m_downstream = nullptr; // disconnect
    }
    destination->m_upstream = this;
    m_downstream = destination;
#endif
}

bool Process::setStandardErrorFile(const std::string &fileName)
{
#ifdef USE_QT
    m_process.setStandardErrorFile(QString::fromStdString(fileName));
    return true;
#else
    if(m_state == Initialized) {
        if(boost::filesystem::native(fileName)) {
            m_errorFile = fileName;
            return true;
        }
    }
    return false;
#endif
}

bool Process::setStandardInputFile(const std::string &fileName)
{
#ifdef USE_QT
    m_process.setStandardInputFile(QString::fromStdString(fileName));
    return true;
#else
    if(m_state == Initialized) {
        if(boost::filesystem::native(fileName)) {
            m_inputFile = fileName;
            return true;
        }
    }
    return false;
#endif
}

bool Process::setStandardOutputFile(const std::string &fileName)
{
#ifdef USE_QT
    m_process.setStandardOutputFile(QString::fromStdString(fileName));
    return true;
#else
    if(m_state == Initialized) {
        if(boost::filesystem::native(fileName)) {
            m_outputFile = fileName;
            return true;
        }
    }
    return false;
#endif
}

}
