#include "weatherdata.h"
#include "logging.h"

#include <iostream>

void usage()
{
    std::cout << "dxmakewea - Convert TMY or EPW files to WEA" << std::endl;
    std::cout << "usage: dxmakewea <input file> <output file>" << std::endl;
}

int main (int argc, char *argv[]){
    if (argc < 3){
        usage();
        return EXIT_FAILURE;
    }
    stadic::WeatherData wea;
    if (!wea.parseWeather(argv[1])) {
        return EXIT_FAILURE;
    }
    if (!wea.writeWea(argv[2])) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
