#ifndef LUMBERJACK_H
#define LUMBERJACK_H
#include <QString>

#include "stadicapi.h"

void STADIC_API ERROR(QString mesg);
void STADIC_API WARNING(QString mesg);
void STADIC_API ERROR(std::string mesg);
void STADIC_API WARNING(std::string mesg);
void STADIC_API ERROR(const char mesg[]);
void STADIC_API WARNING(const char mesg[]);

#endif
