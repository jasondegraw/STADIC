#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <string>
#include <vector>
#include "stadicapi.h"
namespace stadic{

std::vector<std::string> STADIC_API split(std::string line, char delimiter);
std::vector<std::string> STADIC_API trimmedSplit(std::string line, char delimiter);
std::string STADIC_API trim(std::string string);

}
#endif // FUNCTIONS_H
