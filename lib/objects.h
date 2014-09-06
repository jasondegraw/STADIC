#ifndef OBJECTS_H
#define OBJECTS_H
#include <string>
#include <time.h>
#include "stadicapi.h"


namespace stadic{
class STADIC_API FileSystem
{
public:
    FileSystem();

    // Setters
    void setPath(std::string path);
    // Getters

    //Utilities
    bool exists();
    bool isDir();
    bool isFile();
    bool isUpdated();
    bool lastMod();

private:
    std::string m_Path;
    struct tm *m_LastMod;


};




}
#endif // OBJECTS_H
