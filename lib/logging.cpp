#include "logging.h"
#include <iostream>
#include <QString>

void ERROR(QString mesg){
    QString string;
    string ="ERROR: "+mesg;
    std::cerr << string.toStdString() << std::endl;
}
void WARNING(QString mesg){
    QString string;
    string="WARNING: "+mesg;
    std::cerr << string.toStdString() << std::endl;
}
