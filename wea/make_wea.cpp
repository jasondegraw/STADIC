#include "makewea.h"
#include "logging.h"

int main (int argc, char *argv[]){
    QString fileName=argv[1];
    QString resultsFile=argv[2];
    MakeWea wea;
    if (!wea.parseWeather(fileName)){
        EXIT_FAILURE;
    }
    if (!wea.writeWea(resultsFile)){
        EXIT_FAILURE;
    }
    EXIT_SUCCESS;
}