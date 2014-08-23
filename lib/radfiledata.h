#ifndef PARSERAD_H
#define PARSERAD_H

#include <QObject>
#include <QPair>
#include "radprimitive.h"
#include <vector>

#include "stadicapi.h"

namespace stadic {

class STADIC_API RadFileData : public QObject
{
    Q_OBJECT
public:
    explicit RadFileData(QObject *parent = 0);
    RadFileData(const std::vector<RadPrimitive *> &primitives, QObject *parent = 0);

    bool addRad(QString file); 
    bool removeLayer(const QString &layer, const QString &removing, const QString &rest);                     //Function to remove a layer from the list to its own geometry file
    bool blackOutLayer(QString layer);
    bool writeRadFile(QString file);

    bool addPrimitive(RadPrimitive *primitive);

    //Getters
    std::vector<RadPrimitive *> geometry() const;
    std::vector<RadPrimitive *> materials() const;
    std::vector<RadPrimitive *> primitives() const;


    template<class T> std::vector<T*> getPrimitives();
    template <class T> QPair<RadFileData*, RadFileData*> split(bool(*f)(RadPrimitive*, const T&), const T &label);
	QPair<RadFileData *, RadFileData *> split(bool(*f)(RadPrimitive*));

private:
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

template <class T> QPair<RadFileData*, RadFileData*> RadFileData::split(bool(*f)(RadPrimitive*, const T&), const T &label)
{
    std::vector<RadPrimitive*> in, out;
    for (RadPrimitive *primitive : m_Primitives) {
        if (f(primitive, label)) {
          in.push_back(primitive);
        } else {
            out.push_back(primitive);
        }
    }
    // Account for 0 size vectors
    RadFileData *first = nullptr;
    RadFileData *second = nullptr;
    if (in.size() > 0) {
      first = new RadFileData(in, this->parent());
    }
    if (out.size() > 0) {
      second = new RadFileData(out, this->parent());
    }
    return QPair<RadFileData*, RadFileData*>(first, second);
}

}

#endif // PARSERAD_H
