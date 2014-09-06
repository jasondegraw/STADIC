#ifndef RADFILEDATA_H
#define RADFILEDATA_H

#include <QPair>
#include "radprimitive.h"
#include "objects.h"
#include <vector>
#include <memory>

#include "stadicapi.h"

namespace stadic {

class STADIC_API RadFileData
{
public:
    explicit RadFileData();
    RadFileData(const shared_vector<RadPrimitive> &primitives);

    bool addRad(QString file); 
    bool removeLayer(const QString &layer, const QString &removing, const QString &rest);   //Function to remove a layer from the list to its own geometry file
    bool blackOutLayer(QString layer);
    bool writeRadFile(QString file);
    std::vector<double> surfaceNormal(QString layer);

    bool addPrimitive(RadPrimitive *primitive);

    //Getters
    shared_vector<RadPrimitive> geometry() const;
    shared_vector<RadPrimitive> materials() const;
    shared_vector<RadPrimitive> primitives() const;


    template<class T> shared_vector<T> getPrimitives();
    // Splitting is officially broken
    //QPair<RadFileData *, RadFileData *> split(bool (*f)(RadPrimitive*));
    //template <class T> QPair<RadFileData*, RadFileData*> split(bool(*f)(RadPrimitive*, const T&), const T &label);
    //QPair<RadFileData *, RadFileData *> split(const std::vector<std::string> &vector);

private:
    shared_vector<RadPrimitive> m_Primitives; //Vector to hold EVERYTHING

};

template<class T> shared_vector<T> RadFileData::getPrimitives()
{
    shared_vector<T> primitives;
    for(auto primitive : m_Primitives) {
        auto cast = std::dynamic_pointer_cast<T>(primitive);
        if(cast) {
            primitives.push_back(cast);
        }
    }
    return primitives;
}

/*
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
*/

}

#endif // RADFILEDATA_H
