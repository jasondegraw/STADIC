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

