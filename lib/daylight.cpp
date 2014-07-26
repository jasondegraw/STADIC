#include "daylight.h"
#include "stadiccontrol.h"
#include "logging.h"

Daylight::Daylight(QObject *parent) :
    QObject(parent)
{
}

bool Daylight::simDaylight(QString fileName){
    stadic::Control model;
    if (!model.parseJson(fileName)){
        return false;
    }

}
