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
#include <boost/filesystem.hpp>
#include <thread>
#include <fstream>
#endif


namespace stadic{
//*************************
//Process
//*************************
Process::Process(const std::string &program)
{
#ifdef USE_QT
    m_process.setProgram(QString::fromStdString(program));
#else
    m_index = 0;
    m_state = Initialized;
    m_upstream = nullptr;
    m_downstream = nullptr;
    setProgram(program);
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
    m_index = 0;
    m_state = Initialized;
    m_upstream = nullptr;
    m_downstream = nullptr;
    setProgram(program);
    if(m_state == Initialized) {
         m_args.insert(m_args.end(), args.begin(), args.end());
    }
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
            ctx.environment = boost::process::self::get_environment();
            if(!m_inputFile.empty()) {
                ctx.stdin_behavior = boost::process::capture_stream();
            }
            try {
                m_children.push_back(boost::process::launch(m_program, m_args, ctx));
            }
            catch(boost::system::system_error &ex) {
                STADIC_ERROR(ex.what());
                m_state = RunFailed;
            }
            // Launch threads here to handle stdout, stderr
            /*
            if(!m_outputFile.empty()) {
                std::thread thread = std::thread(processStreamToFile, std::ref(m_children[0].get_stdout()), std::ref(m_outputFile));
                thread.detach(); // This could be very unwise
            }
            if(!m_errorFile.empty()) {
                std::thread thread = std::thread(processStreamToFile, std::ref(m_children[0].get_stderr()), std::ref(m_errorFile));
                thread.detach(); // This could be very unwise
            }*/
            // Handle input
            if(!m_inputFile.empty()) {
                std::ifstream input(m_inputFile);
                boost::process::postream &stream = m_children[0].get_stdin();
                if(input.is_open()) { // Should this cause the whole thing to fail?
                    std::string line;
                    while(std::getline(input, line)) {
                        stream << line << std::endl;
                    }
                }
            }
        } else { // This is a pipeline situation, so we'll only run if all children are ready
            m_state = ReadyToRun;
            //std::vector<boost::process::pipeline_entry> entries;
            std::deque<boost::process::pipeline_entry> downstream;
            std::deque<boost::process::pipeline_entry> upstream;
            
            // Set up some contexts
            boost::process::context ctxin;
            ctxin.stdin_behavior = boost::process::capture_stream();
            ctxin.stderr_behavior = boost::process::capture_stream();
            ctxin.environment = boost::process::self::get_environment();

            boost::process::context ctxout;
            //ctxout.stdout_behavior = boost::process::inherit_stream();
            ctxout.stdout_behavior = boost::process::close_stream();
            ctxout.stderr_behavior = boost::process::capture_stream();
            ctxout.environment = boost::process::self::get_environment();

            boost::process::context ctxlast;
            ctxlast.stdout_behavior = boost::process::capture_stream();
            ctxlast.stderr_behavior = boost::process::capture_stream();
            ctxlast.environment = boost::process::self::get_environment();

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
            while(first->m_upstream != nullptr) {
                if(first->m_upstream->m_state == ReadyToRun) {
                    upstream.push_front(boost::process::pipeline_entry(first->m_program, first->m_args, ctxout));
                    first = first->m_upstream;
                } else {
                    return; // Bail out if any one is not ready to go
                }
            }
            // If we made it to here, all of the upstream processes are ready to go
            // We still need to set up the first one so that input can be handled
            upstream.push_front(boost::process::pipeline_entry(first->m_program, first->m_args, ctxin));
            // Combine the two lists, keeping in mind that the element we started with is in both (it is double counted to save logic)
            if(upstream.size() < 2) {
                downstream.pop_front();
                upstream.insert(upstream.end(), downstream.begin(), downstream.end());
            } else {
                upstream.pop_back();
                upstream.insert(upstream.end(), downstream.begin(), downstream.end());
            }
            boost::process::children subprocesses;
            try {
                subprocesses = boost::process::launch_pipeline(upstream);
            }
            catch(boost::system::system_error &ex) { // Something has gone wrong!
                STADIC_ERROR(ex.what());
                Process *current = first;
                while(current != nullptr) {
                    current->m_state = RunFailed;
                }
            }
            // Give everyone a copy of the children, assign indices, set state
            unsigned index = 0;
            Process *current = first;
            while(current != nullptr) {
                current->m_index = index;
                current->m_children = subprocesses;
                current->m_state = Running;
                current = current->m_downstream;
                index++;
            }
            index--; // This the index of the last child
            // Launch threads here to handle stdout, stderr
            /*
            if(!last->m_outputFile.empty()) {
                std::thread thread = std::thread(processStreamToFile, std::ref(m_children[index].get_stdout()), std::ref(m_outputFile));
                thread.detach(); // This could be very unwise
            }
            */
            // Need to loop through the whole list for this
            current = first;
            while(current != nullptr) {
                /*
                if(!current->m_errorFile.empty()) {
                    std::thread thread = std::thread(processStreamToFile, std::ref(m_children[current->m_index].get_stderr()), std::ref(current->m_errorFile));
                    thread.detach(); // This could be very unwise
                }*/
                current = current->m_downstream;
            }
            // Handle input
            if(!first->m_inputFile.empty()) {
                std::ifstream input(first->m_inputFile);
                boost::process::postream &stream = m_children[0].get_stdin();
                if(input.is_open()) { // Should this cause the whole thing to fail?
                    std::string line;
                    while(std::getline(input, line)) {
                        stream << line << std::endl;
                    }
                }
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
    if(m_state==RunCompleted || m_state==RunFailed || m_state==BadProgram) {
        return false;
    }
    if(m_upstream == nullptr && m_downstream == nullptr) { // This is the simple case
        boost::process::status result = m_children[0].wait();
        m_state = RunFailed;
        // Figure out what happened
        if(result.exited()) {
            if(result.exit_status() == 0) { // Hopefully this is correct
                m_state = RunCompleted;
                writeFiles();
                return true;
            }
        }
    } else {
        boost::process::status result = boost::process::wait_children(m_children);
        // Figure out what happened
        m_state = RunFailed;
        if(result.exited()) {
            if(result.exit_status() == 0) { // Hopefully this is correct
                m_state = RunCompleted;
            }
        }
        // Propagate this result to all of the other processes
        // Upstream first
        Process *current = m_upstream;
        while(current != nullptr) {
            current->m_state = m_state;
            current->writeFiles();
            current = current->m_upstream;
        }
        // Now downstream
        current = m_downstream;
        while(current != nullptr) {
            current->m_state = m_state;
            current->writeFiles();
            current = current->m_downstream;
        }
        return m_state == RunCompleted;
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

void Process::setProgram(const std::string &program)
{
#ifdef USE_QT
    // This function is not used with Qt
#else
    try {
        m_program = boost::process::find_executable_in_path(program);
        m_args.push_back(program);
    } catch (const boost::filesystem::filesystem_error&) {
#ifndef WIN32
        try {
            m_program = boost::process::find_executable_in_path(program,".");
            m_args.push_back(program);
        } catch (const boost::filesystem::filesystem_error&) {
            m_state = BadProgram;
        }
#else
        m_state = BadProgram;
#endif
    }
#endif
}

void Process::writeFiles()
{
#ifdef USE_QT
    // This function is not used with Qt
#else
    if(m_state == RunCompleted) {
        if(!m_outputFile.empty()) {
            if(m_children.size() == 1 || m_index == m_children.size()-1) {
                // Only do this for single processes or the last one in the pipeline
                boost::process::pistream &stream = m_children[m_index].get_stdout();
                processStreamToFile(stream, m_outputFile);
            }
        }
        if(!m_errorFile.empty()) {
            boost::process::pistream &stream = m_children[m_index].get_stderr();
            processStreamToFile(stream, m_errorFile);
        }
    }
#endif
}

}
