#include "functions.h"
#include <iostream>
#include <sstream>

namespace stadic{
//String splitting function based on a delimiter that returns a vector of standard strings
std::vector<std::string> split(std::string line, char delimiter)
{
    std::vector<std::string> stringList;
    std::stringstream stream(line);
    std::string string;
    while(std::getline(stream, string, delimiter)) {
        stringList.push_back(string);
    }
    return stringList;
    /*
    std::vector<std::string> stringList;
    std::string remain;
    if (line.find(delimiter)!=line.npos){
        stringList.push_back(trim(line.substr(0,line.find(delimiter))));
        remain=line.substr(line.find(delimiter)+1);
    }else{
        stringList.push_back(trim(line));
    }

    if (line.find(delimiter)!=line.npos){
        std::vector<std::string> stringList2;
        stringList2=split(remain,delimiter);
        stringList.insert(stringList.end(),stringList2.begin(), stringList2.end());
    }else{
        return stringList;
    }
    */
}

std::vector<std::string> trimmedSplit(std::string line, char delimiter)
{
    std::vector<std::string> stringList;
    std::stringstream stream(line);
    std::string string;
    while(std::getline(stream, string, delimiter)) {
        stringList.push_back(trim(string));
    }
    return stringList;
}

//String trimming function that clears the white space from the beginning or the end of a string
std::string trim(std::string string)
{
        std::size_t found=string.find_first_not_of(" \t\f\v\n\r");
        if (found!=string.npos){
            string.erase(string.begin(),string.begin()+found);
        }
        found=string.find_last_not_of(" \t\f\v\n\r");
        if (found!=string.npos){
            string.erase(string.begin()+found+1,string.end());
        }
    return string;
}

}
