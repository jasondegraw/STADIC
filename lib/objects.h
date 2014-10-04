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

#ifdef _MSC_VER
//struct WIN32_FILE_ATTRIBUTE_DATA;
typedef struct _WIN32_FILE_ATTRIBUTE_DATA  WIN32_FILE_ATTRIBUTE_DATA;
//typedef struct _FILETIME FILETIME, *PFILETIME;
#endif

namespace stadic{

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
