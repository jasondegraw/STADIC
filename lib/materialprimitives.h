#ifndef MATERIALPRIMITIVES_H
#define MATERIALPRIMITIVES_H
#include "radprimitive.h"

namespace stadic{
class PlasticMaterial : public RadPrimitive
{
public:
    explicit PlasticMaterial(QObject *parent=0);
    PlasticMaterial(double red, double green, double blue, double spec, double rough, QObject *parent=0);

    // Setters
    bool setType(QString){return false;}
    bool setRed(double value);
    bool setGreen(double value);
    bool setBlue(double value);
    bool setSpecularity(double value);
    bool setRoughness(double value);
    // Getters
    double red() const;
    double green() const;
    double blue() const;
    double specularity() const;
    double roughness() const;
private:
   virtual bool validateArg(int number, QString value, int position) const;
   virtual bool validateArg(int number, std::vector<QString> value) const;
};

}
#endif // MATERIALPRIMITIVES_H
