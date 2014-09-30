#ifndef LUMBERJACK_H
#define LUMBERJACK_H

#include <string>

#include "stadicapi.h"

namespace stadic {
enum class Severity {Info, Warning, Error, Fatal};
}

void STADIC_API STADIC_ERROR(std::string mesg);
void STADIC_API STADIC_WARNING(std::string mesg);
void STADIC_API STADIC_LOG(stadic::Severity severity, std::string mesg);

#endif
