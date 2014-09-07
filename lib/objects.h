#ifndef OBJECTS_H
#define OBJECTS_H
#include <string>
#include <time.h>
#include "stadicapi.h"


namespace stadic{

class STADIC_API Dir
{
public:
    Dir();

    //Setters
    bool setPath(std::string path);

    //Getters
    std::string toString();

    //Utilities
    bool isDir();
    bool exists();
    bool cdUp();
    bool cd();


private:
    std::string m_Path;


};

class STADIC_API File
{
public:
    File();

    // Setters
    void setPath(std::string path);
    // Getters
    Dir dir();
    std::string toString();
    std::string name();

    //Utilities
    bool exists();
    bool isFile();
    bool isUpdated();
    bool lastMod();

private:
    std::string m_Path;
    struct tm *m_LastMod;


};

}
#endif // OBJECTS_H
