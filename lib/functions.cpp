

#include "functions.h"
namespace stadic{
//String splitting function based on a delimiter that returns a vector of standard strings
std::vector<std::string> split(std::string line, char delimiter){
    std::vector<std::string> stringList;
    std::string remain;
    if (line.find(delimiter)!=line.npos){
        stringList.push_back(line.substr(0,line.find(delimiter)));
        remain=line.substr(line.find(delimiter)+1);
    }else{
        stringList.push_back(line);
    }

    if (line.find(delimiter)!=line.npos){
        std::vector<std::string> stringList2;
        stringList2=split(remain,delimiter);
        stringList.insert(stringList.end(),stringList2.begin(), stringList2.end());
    }else{
        return stringList;
    }
}

//String trimming function that clears the white space from the beginning or the end of a string
std::string trim(std::string string){
    if (string.find_first_not_of(' ')!=string.npos){
        string.erase(string.find_first_not_of(' '));
    }

    if (string.find_last_not_of(' ')!=string.npos){
        string.erase(string.find_last_not_of(' '));
    }
    return string;
}

}
