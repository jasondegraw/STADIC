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
        }
        else if(std::string("-R") == argv[1]) {
            std::string string;
            while(std::getline(std::cin, string)) {
                std::cout << "Input:" << string << std::endl;
                std::cerr << "Error:" << string << std::endl;
                if(string.find("STOP") != std::string::npos) {
                    break;
                }
            }
        }
    }

    return 0;
}
