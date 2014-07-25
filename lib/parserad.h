#ifndef PARSERAD_H
#define PARSERAD_H

#include <QObject>
#include "radgeometry.h"
#include <vector>

namespace stadic {

class RadFileData : public QObject
{
    Q_OBJECT
public:
    explicit RadFileData(QObject *parent = 0);

    bool addRad(QString file); 
    bool removeLayer(QString layer, QString outFile);                     //Function to remove a layer from the list to its own geometry file
    bool blackOutLayer(QString layer);
    bool writeRadFile(QString file);

    //Getters
    std::vector<RadGeometry *> geometry();
    std::vector<RadGeometry *> materials();

private:
    std::vector<RadGeometry *> m_RadGeo;                //Vector to hold the radiance polygons
    std::vector<RadGeometry *> m_RadMat;                //Vector to hold the radiance materials

signals:

public slots:

};

}

#endif // PARSERAD_H
