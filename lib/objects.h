#ifndef OBJECTS_H
#define OBJECTS_H

#include <QProcess>
#include <QStringList>

#include "stadicapi.h"

namespace stadic{

class STADIC_API Process
{
public:
  Process();

  void start();
  void setProgram(std::string prog);
  void setArguments(std::vector<std::string> args);
  bool waitForFinished(int msecs=30000); // Yuck.

  std::string error();
  std::string output();

  void setStandardOutputProcess(Process *destination);
  void setStandardErrorFile(const std::string &fileName);
  void setStandardInputFile(const std::string &fileName);
  void setStandardOutputFile(const std::string &fileName);

private:
  QProcess m_process;

};



}
#endif // OBJECTS_H
