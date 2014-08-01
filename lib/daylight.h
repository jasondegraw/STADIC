#ifndef DAYLIGHT_H
#define DAYLIGHT_H

#include <QObject>
#include "stadiccontrol.h"
#include <vector>
#include <QStringList>
#include "radfiledata.h"

namespace stadic {
class Daylight : public QObject
{
    Q_OBJECT
public:
    explicit Daylight(Control *model, QObject *parent = 0);
    bool simDaylight();

private:
    bool simBSDF(int blindGroupNum, int setting, int bsdfNum);
    bool simStandard(int blindGroupNum, int setting, Control *model);
    bool uniqueGlazingMaterials(Control *model);
    bool testSimCase(Control *model);
    bool setSimCase(int setting, int simCase);
    bool writeSky(Control *model);
    bool createBaseRadFiles(Control *model);
    bool createOctree(QStringList files, QString octreeName);

    std::vector<int> m_SimCase;
    Control *m_Model;
    std::vector<RadFileData* > m_RadFiles;
signals:

public slots:

};

}
#endif // DAYLIGHT_H
