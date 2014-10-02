#ifndef OBJECTS_H
#define OBJECTS_H
#include <string>
#include <time.h>
#include "stadicapi.h"

#include <memory>
#include <vector>

//#define USE_QT

#ifdef USE_QT
#include <QProcess>
#else
#include <boost/process.hpp>
#endif

#include "stadicapi.h"

// This is a C++11 thing - if we can't use this then we're looking
// at a define or some other horrifying construct
template<typename T>
using shared_vector = std::vector<std::shared_ptr<T> >;

#ifdef _MSC_VER
//struct WIN32_FILE_ATTRIBUTE_DATA;
typedef struct _WIN32_FILE_ATTRIBUTE_DATA  WIN32_FILE_ATTRIBUTE_DATA;
//typedef struct _FILETIME FILETIME, *PFILETIME;
#endif

namespace stadic{

// Given the complexity of the Process class, it is probably a good idea to 
// include abasic explanation of the Process object. It is meant to be a 
// one-shot subprocess that provides a degree of access to the standard input,
// standard error, and standard input. The interface is modeled on QProcess
// and the class is a very thin wrapper around QProcess when USE_QT is
// defined. Otherwise, a modified form of the Boost.Process library is used.
//
// Boost.Process provides a "pipeline" feature that is used to connect a
// series of processes together:
//
//  Proc0 stdout -> Proc1 stdin, Proc1 stdout -> Proc2 stdin, ...
//
// and so on. This is used to emulate the QProcess connection feature. This is
// why the "start" function below is so complicated. None of the processes are
// actually started until the start function has been called on *all* of the
// processes. The usage of the class is demonstrated in the unit test program.
//
// To get this behavior, there are a lot of old-school linked list operations
// to move upstream and downstream in the pipeline. Modify the code with care.
class STADIC_API Process
{
public:
    Process(std::string program);
    Process(std::string program, std::vector<std::string> args);

    void start();
    bool wait();

    std::string error();
    std::string output();

    void setStandardOutputProcess(Process *destination);
    bool setStandardErrorFile(const std::string &fileName);
    bool setStandardInputFile(const std::string &fileName);
    bool setStandardOutputFile(const std::string &fileName);

private:
#ifdef USE_QT
    QProcess m_process;
#else
    enum ProcessState {Initialized, ReadyToRun, Running, RunCompleted, RunFailed};
    ProcessState m_state;     // Flag that describes the current state of the object
    Process *m_upstream;      // Upstream process whose stdout stream will be directed to this object's stdin
    Process *m_downstream;    // Downstream process whose stdin stream will get this object's stdout
    std::string m_program;    // Program name
    std::vector<std::string> m_args;  // Arguments: program, argv1, argv2, ...
    boost::process::children m_children;  // Vector of process objects
    std::string m_inputFile;  // File that standard input will be read from
    std::string m_outputFile; // File that standard output will be written to
    std::string m_errorFile;  // File that standard error will be written to
    unsigned m_index;  // Index of the corresponding process in m_children
#endif
};

class STADIC_API FilePath
{
public:
    FilePath(std::string path);
    ~FilePath();

    //Setters

    //Getters
    std::string toString();

    //Utilities
    bool isDir();
    bool isFile();
    bool exists();
    bool isUpdated();


private:
    std::string m_Path;
#ifdef _MSC_VER
    // This may be too much data - maybe just store a FILETIME?
    WIN32_FILE_ATTRIBUTE_DATA *m_fileAttr;  // Use a pointer to avoid including Windows.h here
#else
    struct tm *m_LastMod;
#endif
    void lastMod();

};

}
#endif // OBJECTS_H
