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

#include "stadicprocess.h"
#include "logging.h"
#include <sstream>

#ifndef USE_QT
#include <stdlib.h>
#endif

#include <iostream>

namespace stadic{
//*************************
//Process
//*************************
Process::Process(const std::string &program)
{
#ifdef USE_QT
    m_process.setProgram(QString::fromStdString(program));
#else
    m_state = Initialized;
    m_inputProcess = nullptr;
    m_outputProcess = nullptr;
    m_program = program;
#endif
}

Process::Process(const std::string &program, const std::vector<std::string> &args)
{
#ifdef USE_QT
    m_process.setProgram(QString::fromStdString(program));
    QStringList arguments;
    for(std::string arg : args) {
        arguments << QString::fromStdString(arg);
    }
    m_process.setArguments(arguments);
#else
    m_state = Initialized;
    m_inputProcess = nullptr;
    m_outputProcess = nullptr;
    m_program = program;
    m_args = args;
#endif
}

#ifndef USE_QT
static void processStreamToFile(boost::process::pistream &stream, const std::string &fileName)
{
    std::ofstream output(fileName);
    if(output.is_open()) {
        std::string line;
        while(std::getline(stream, line)) {
            output << line << std::endl;
        }
        output.close();
    }
}
#endif

bool Process::run()
{
#ifdef USE_QT
    m_process.start();
#else
    if(m_state == Initialized) {
        std::string command;
        Process *first = this;
        if(m_inputProcess == nullptr && m_outputProcess == nullptr) { // This is the simple case
            command = commandLine();
        } else { // This is a pipeline situation, run everything
            // Get to the first command
            while(first->m_inputProcess) {
                first = first->m_inputProcess;
            }
            command = first->commandLine();
            Process *current = first->m_outputProcess;
            while(current) {
                command += " | " + current->commandLine();
                current = current->m_outputProcess;
            }
        }
        // Run the command line
        int returnCode = system(command.c_str());
        // Figure out what happened
        m_state = RunCompleted;
        if(returnCode != 0) {
            m_state = RunFailed;
        }
        if(m_inputProcess || m_outputProcess) {
            while(first) {
                first->m_state = m_state;
                first = first->m_outputProcess;
            }
        }
        return m_state == RunCompleted;
    }
#endif
    return true;
}

void Process::start()
{
#ifdef USE_QT
    m_process.start();
#else
    run();
#endif
}

bool Process::wait()
{
#ifdef USE_QT
    return m_process.waitForFinished(-1);
#else
    return m_state == RunCompleted;
#endif
}

/*
std::string Process::error()
{
#ifdef USE_QT
    return QString(m_process.readAllStandardError()).toStdString();
#else
    return std::string();
#endif
}

std::string Process::output()
{
#ifdef USE_QT
    return QString(m_process.readAllStandardOutput()).toStdString();
#else
    return std::string();
#endif
}
*/

void Process::setStandardOutputProcess(Process *destination)
{
#ifdef USE_QT
    m_process.setStandardOutputProcess(&(destination->m_process));
#else
    if(destination->m_inputProcess) { // The target process already has an upstream process
        destination->m_inputProcess->m_outputProcess = nullptr; // disconnect
    }
    destination->m_inputProcess = this;
    m_outputProcess = destination;
#endif
}

bool Process::setStandardErrorFile(const std::string &fileName)
{
#ifdef USE_QT
    m_process.setStandardErrorFile(QString::fromStdString(fileName));
    return true;
#else
    if(m_state == Initialized) {
        m_errorFile = fileName;
        return true;
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
        m_inputFile = fileName;
        if(m_inputProcess) {
            m_inputProcess->m_outputProcess = nullptr;
            m_inputProcess = nullptr;
        }
        return true;
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
        m_outputFile = fileName;
        if(m_outputProcess) {
            m_outputProcess->m_inputProcess = nullptr;
            m_outputProcess = nullptr;
        }
        return true;
    }
    return false;
#endif
}

std::string Process::commandLine()
{
#ifdef USE_QT
    return std::string;
#else
    std::string command = m_program;
    for(unsigned i = 0; i<m_args.size(); i++) {
        command += " " + m_args[i];
    }
    if(!m_inputFile.empty()) {
        command += " < " + m_inputFile;
    }
    if(!m_outputFile.empty()) {
        command += " > " + m_outputFile;
    }
    if(!m_errorFile.empty()) {
        command += " 2> " + m_errorFile;
    }
    return command;
#endif
}

}
