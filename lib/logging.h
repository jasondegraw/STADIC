#ifndef LUMBERJACK_H
#define LUMBERJACK_H
#include <QString>

#include "stadicapi.h"

void STADIC_API STADIC_ERROR(QString mesg);
void STADIC_API STADIC_WARNING(QString mesg);
void STADIC_API STADIC_ERROR(std::string mesg);
void STADIC_API STADIC_WARNING(std::string mesg);
void STADIC_API STADIC_ERROR(const char mesg[]);
void STADIC_API STADIC_WARNING(const char mesg[]);

#endif
