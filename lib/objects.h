#ifndef OBJECTS_H
#define OBJECTS_H

#include <memory>
#include <vector>
#include <QProcess>
#include <QStringList>

#include "stadicapi.h"

// This is a C++11 thing - if we can't use this then we're looking
// at a define or some other horrifying construct
template<typename T>
using shared_vector = std::vector<std::shared_ptr<T> >;

namespace stadic{

class STADIC_API Process
{
public:
    Process(std::string program, std::vector<std::string> args);

    void start();
    bool wait();

    std::string error();
    std::string output();

    void setStandardOutputProcess(Process *destination);
    void setStandardErrorFile(const std::string &fileName);
    void setStandardInputFile(const std::string &fileName);
    void setStandardOutputFile(const std::string &fileName);

private:
    QProcess m_process;

};



}
#endif // OBJECTS_H
