#include "weatherdata.h"
#include "logging.h"

int main (int argc, char *argv[]){
    QString fileName=argv[1];
    QString resultsFile=argv[2];
    stadic::WeatherData wea;
    if (!wea.parseWeather(fileName)){
        return EXIT_FAILURE;
    }
    if (!wea.writeWea(resultsFile)){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
