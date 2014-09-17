#include "logging.h"
#include <iostream>
#include <QString>

void STADIC_ERROR(QString mesg)
{
    QString string;
    string ="ERROR: "+mesg;
    std::cerr << string.toStdString() << std::endl;
}

void STADIC_WARNING(QString mesg)
{
    QString string;
    string="WARNING: "+mesg;
    std::cerr << string.toStdString() << std::endl;
}

void STADIC_ERROR(std::string mesg)
{
    std::string string ="ERROR: "+mesg;
    std::cerr << string << std::endl;
}

void STADIC_WARNING(std::string mesg)
{
    std::string string="WARNING: "+mesg;
    std::cerr << string << std::endl;
}

void STADIC_ERROR(const char mesg[])
{
  std::string string = "ERROR: " + std::string(mesg);
  std::cerr << string << std::endl;
}

void STADIC_WARNING(const char mesg[])
{
  std::string string = "WARNING: " + std::string(mesg);
  std::cerr << string << std::endl;
}
