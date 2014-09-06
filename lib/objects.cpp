#include "objects.h"

namespace stadic{

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