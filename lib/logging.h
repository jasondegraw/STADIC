#ifndef LUMBERJACK_H
#define LUMBERJACK_H
#include <QString>

#include "stadicapi.h"

void STADIC_API ERROR(QString mesg);
void STADIC_API WARNING(QString mesg);
void STADIC_API ERROR(std::string mesg);
void STADIC_API WARNING(std::string mesg);

#endif
