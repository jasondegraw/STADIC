#ifndef OBJECTS_H
#define OBJECTS_H
#include <string>
#include <time.h>
#include "stadicapi.h"

#include <memory>
#include <vector>
#include <QProcess>
#include <QStringList>

#include <boost/process.hpp>

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
    void setStandardErrorFile(const std::string &fileName);
    void setStandardInputFile(const std::string &fileName);
    void setStandardOutputFile(const std::string &fileName);

private:
    QProcess m_process;

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
