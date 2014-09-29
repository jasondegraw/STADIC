#include "logging.h"
#include <iostream>
#include <QString>

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

