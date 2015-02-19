/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission of the
 *    respective copyright holder or contributor.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *****************************************************************************/

#include <iostream>
#include <string>

// This is a test program for testing the Process object. It should be replaced later
// with using one of the other real programs, but this will do for now.

int main(int argc, char *argv[])
{
    if(argc < 2) {
        std::cout << "This is the standard output" << std::endl;
        std::cerr << "This is the standard error" << std::endl;
    } else if(argc == 2) {
        if(std::string("-x") == argv[1]) {
            std::cout << "Here are some standard output letters: xxxxxxxxx" << std::endl;
            std::cerr << "Here are some standard error letters: xxxxxxxxx" << std::endl;
        } else if(std::string("-r") == argv[1]) {
            std::string string;
            while(std::getline(std::cin, string)) {
                std::cout << "Input:" << string << std::endl;
                if(string.find("STOP") != std::string::npos) {
                    break;
                }
            }
        } else if(std::string("-R") == argv[1]) {
            std::string string;
            while(std::getline(std::cin, string)) {
                std::cout << "Input:" << string << std::endl;
                std::cerr << "Error:" << string << std::endl;
                if(string.find("STOP") != std::string::npos) {
                    break;
                }
            }
        } else if(std::string("-d") == argv[1]) {
            std::string string;
            while(std::getline(std::cin, string)) {
                if(string.find("STOP") != std::string::npos) {
                    break;
                }
            }
        } else if(std::string("-B") == argv[1]) {
            for(int i=0; i<10000; i++) {
                std::cout << "x";
            }
            std::cout << "STOP";
        }
    }

    return 0;
}
