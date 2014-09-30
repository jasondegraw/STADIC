#include "logging.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>

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

void STADIC_LOG(stadic::Severity severity, std::string mesg)
{
    std::string string;
    switch(severity) {
    case stadic::Severity::Info: // Not an error, write out information
        string = "INFO: "+mesg;
        std::cerr << string << std::endl;
        break;
    case stadic::Severity::Warning: // Something is wrong, but can continue
        string = "WARNING: "+mesg;
        std::cerr << string << std::endl;
        break;
    case stadic::Severity::Error: // Something is wrong, cannot continue
        string = "ERROR: "+mesg;
        std::cerr << string << std::endl;
        throw std::runtime_error(mesg);
    default: //case stadic::Severity::Fatal:  // Something is really wrong, stop now!
        string = "FATAL: "+mesg;
        std::cerr << string << std::endl;
        abort();
    }
}

