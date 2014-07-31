#ifndef DAYLIGHT_H
#define DAYLIGHT_H

#include <QObject>
#include "stadiccontrol.h"
#include <vector>

namespace stadic {
class daylight : public QObject
{
    Q_OBJECT
public:
    explicit daylight(QObject *parent = 0);
    bool simDaylight(stadic::Control &model);

private:
    bool simBSDF(int blindGroupNum, int setting, int bsdfNum);
    bool simStandard(int blindGroupNum, int setting);
    bool uniqueGlazingMaterials(stadic::Control &model);
    bool testSimCase(stadic::Control &model);
    bool setSimCase(int setting, int simCase);
    bool writeSky(stadic::Control &model);
    bool createBaseRadFiles(stadic::Control &model);

    std::vector<int> m_SimCase;
signals:

public slots:

};

}
#endif // DAYLIGHT_H
