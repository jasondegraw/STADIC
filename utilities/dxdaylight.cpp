#include "daylight.h"
#include "logging.h"

#include <iostream>

void usage()
{
    std::cout << "dxdaylight - Simulate the daylight for a space model" << std::endl;
    std::cout << "usage: dxdaylight <STADIC Control File>" << std::endl;
}


int main (int argc, char *argv[]){
    if (argc < 2){
        usage();
        return EXIT_FAILURE;
    }
    std::string fileName=argv[1];
    stadic::Control model;
    if (!model.parseJson(fileName)){
        return EXIT_FAILURE;
    }
    stadic::Daylight sim(&model);
    if (!sim.simDaylight()){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
