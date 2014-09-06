#ifndef DAYLIGHT_H
#define DAYLIGHT_H

#include <QObject>
#include "stadiccontrol.h"
#include <vector>
#include <QStringList>
#include "radfiledata.h"

#include "stadicapi.h"

namespace stadic {
class STADIC_API Daylight : public QObject
{
    Q_OBJECT
public:
    explicit Daylight(Control *model, QObject *parent = 0);
    bool simDaylight();

private:
    bool simBSDF(int blindGroupNum, int setting, int bsdfNum,QString bsdfRad,QString remainingRad,std::vector<double> normal,QString thickness,QString bsdfXML, QString bsdfLayer, Control *model);
    bool simStandard(int blindGroupNum, int setting, Control *model);
    bool simCase1(int blindGroupNum, Control *model);
    bool simCase2(int blindGroupNum, Control *model);
    bool simCase3(int blindGroupNum, Control *model);
    bool simCase4(int blindGroupNum, Control *model);
    bool simCase5(int blindGroupNum, Control *model);
    bool simCase6(int blindGroupNum, Control *model);
    bool uniqueGlazingMaterials(Control *model);
    bool testSimCase(Control *model);
    bool setSimCase(int setting, int simCase);
    bool writeSky(Control *model);
    bool createBaseRadFiles(Control *model);
    bool createOctree(QStringList files, QString octreeName);
    bool sumIlluminanceFiles(Control *model);

    std::vector<int> m_SimCase;
    Control *m_Model;
    std::vector<RadFileData* > m_RadFiles;

signals:

public slots:

};

}
#endif // DAYLIGHT_H
