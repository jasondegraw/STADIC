#include "daylight.h"
#include "stadiccontrol.h"
#include "logging.h"

daylight::daylight(QObject *parent) :
    QObject(parent)
{
}

bool daylight::simDaylight(QString fileName){
    StadicControl model;
    if (!model.parseJson(fileName)){
        return false;
    }

}
