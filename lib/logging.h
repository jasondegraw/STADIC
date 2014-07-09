#ifndef LUMBERJACK_H
#define LUMBERJACK_H
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

#endif
