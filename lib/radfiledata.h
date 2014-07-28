#ifndef PARSERAD_H
#define PARSERAD_H

#include <QObject>
#include <QPair>
#include "radprimitive.h"
#include <vector>

namespace stadic {

class RadFileData : public QObject
{
    Q_OBJECT
public:
    explicit RadFileData(QObject *parent = 0);
    RadFileData(const std::vector<RadPrimitive *> &primitives, QObject *parent = 0);

    bool addRad(QString file); 
    bool removeLayer(const QString &layer, const QString &removing, const QString &rest);                     //Function to remove a layer from the list to its own geometry file
    bool blackOutLayer(QString layer);
    bool writeRadFile(QString file);

    //Getters
    std::vector<RadPrimitive *> geometry() const;
    std::vector<RadPrimitive *> materials() const;
    std::vector<RadPrimitive *> primitives() const;


    template<class T> std::vector<T*> getPrimitives();
    QPair<RadFileData *, RadFileData *> split(bool (*f)(RadPrimitive*,const QString&), const QString &label);

private:
    //std::vector<RadPrimitive *> m_RadGeo;                //Vector to hold the radiance polygons
    //std::vector<RadPrimitive *> m_RadMat;                //Vector to hold the radiance materials
    std::vector<RadPrimitive *> m_Primitives;            //Vector to hold EVERYTHING

signals:

public slots:

};

template<class T> std::vector<T*> RadFileData::getPrimitives()
{
    std::vector<T*> primitives;
    for(RadPrimitive *primitive : m_Primitives) {
        T* cast = dynamic_cast<T*>(primitive);
        if(cast) {
            primitives.push_back(cast);
        }
    }
    return primitives;
}

}

#endif // PARSERAD_H
