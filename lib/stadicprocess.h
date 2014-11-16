/****************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted for
 * personal and commercial purposes provided that the
 * following conditions are met:
 *
 * 1. Redistribution of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University.
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

#ifndef PROCESS_H
#define PROCESS_H
#include <string>
#include <time.h>
#include "stadicapi.h"

#include <memory>
#include <vector>
#include <string>

//#define USE_QT

#ifdef USE_QT
#include <QProcess>
#else
//#include <boost/process.hpp>
#endif

#include "stadicapi.h"

#ifdef _MSC_VER
//struct WIN32_FILE_ATTRIBUTE_DATA;
typedef struct _WIN32_FILE_ATTRIBUTE_DATA  WIN32_FILE_ATTRIBUTE_DATA;
//typedef struct _FILETIME FILETIME, *PFILETIME;
#endif

namespace stadic{

// Given the complexity of the Process class, it is probably a good idea to 
// include a basic explanation of the Process object. It is meant to be a 
// one-shot subprocess that provides a degree of access to the standard input,
// standard error, and standard input. The interface is modeled on QProcess
// and the class is a very thin wrapper around QProcess when USE_QT is
// defined. The QProcess version is a bit out of date, so it should be tested
// before it is used (by defining USE_QT).
//
// The non-Qt version uses the C standard library system function. A command
// line is generated for the program using the standard shell constructs. 
// Processes that are connected together (using setStandardOutputProcess) are 
// run all at once (with pipes in between) and are started once start is
// called for one of the processes. 
//
// To get this behavior, there are a lot of old-school linked list operations
// to move around in the process pipeline. Modify the code with care.
//
// The usage of the class is demonstrated in the unit test program.
//

class STADIC_API Process
{
public:
    enum ProcessState { BadProgram, Initialized, ReadyToRun, Running, RunCompleted, RunFailed };

    Process(const std::string &program);
    Process(const std::string &program, const std::vector<std::string> &args);

    std::string commandLine();
    static std::string quote(const std::string &string);

    bool run();
    void start();
    bool wait();

    //std::string error();
    //std::string output();

    void setStandardOutputProcess(Process *destination);
    bool setStandardErrorFile(const std::string &fileName);
    bool setStandardInputFile(const std::string &fileName);
    bool setStandardOutputFile(const std::string &fileName);

    //static bool findProgram(const std::string &program);
    ProcessState state() const
    {
#ifdef USE_QT
        return RunFailed; // If the Qt version survives, then this will need to be written
#else
        return m_state;
#endif
    }

private:
#ifdef USE_QT
    QProcess m_process;
#else
    ProcessState m_state;     // Flag that describes the current state of the object
    Process *m_inputProcess;  // Upstream process whose stdout stream will be directed to this object's stdin
    Process *m_outputProcess; // Downstream process whose stdin stream will get this object's stdout
    std::string m_program;    // Program name
    std::vector<std::string> m_args;  // Arguments: program, argv1, argv2, ...
    //boost::process::children m_children;  // Vector of process objects
    std::string m_inputFile;  // File that standard input will be read from
    std::string m_outputFile; // File that standard output will be written to
    std::string m_errorFile;  // File that standard error will be written to
    //unsigned m_index;  // Index of the corresponding process in m_children
#endif
};

}
#endif // OBJECTS_H
