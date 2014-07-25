#ifndef RADGEOMETRY_H
#define RADGEOMETRY_H

#include <QObject>
#include <QFile>
#include<array>

class RadGeometry : public QObject
{
    Q_OBJECT
public:
    enum Type {Source, Sphere, Bubble, Polygon, Cone, Cup, Cylinder, Tube, Ring, Instance,
               Mesh, Light, Illum, Glow, Spotlight, Mirror, Prism1, Prism2, Plastic, Metal,
               Trans, Plastic2, Metal2, Trans2, Mist, Dielectric, Interface, Glass, Plasfunc,
               Metfunc, Transfunc, BRTDfunc, Plasdata, Metdata, Transdata, BSDF, Antimatter,
               Texfunc, Texdata, Colorfunc, Brightfunc, Colordata, Brightdata, Colorpict, Colortext,
               Brighttext, Mixfunc, Mixdata, Mixpict, Mixtext, Unknown};
    explicit RadGeometry(QObject *parent = 0);

    //Setters
    void setModifier(QString modifier);
    void setType(QString type);
    void setName(QString name);
    void setArg1(QString arg);
    void setArg2(QString arg);
    void setArg3(QString arg);

    virtual bool setArg(int number, QString value, int position);

    //Getters
    QString modifier();
    Type type();
    QString typeString();
    QString name();
    std::vector<QString> arg1();
    std::vector<QString> arg2();
    std::vector<QString> arg3();

    virtual QString getArg1(int position) const;
    virtual QString getArg2(int position) const;
    virtual QString getArg3(int position) const;
    virtual QString getArg(int number, int position) const;

    static RadGeometry *fromRad(QFile file, QObject *parent = 0);

protected:
    void initArg(int number, std::vector<QString> arg);

private:
    virtual bool validateArg(int number, QString value, int position) {return true;}
    static std::array<QString,51> s_typeStrings;
    static Type typeFromString(QString string);

    QString m_Modifier;
    Type m_Type;
    QString m_TypeString;
    QString m_Name;
    std::vector<QString> m_Arg1;
    std::vector<QString> m_Arg2;
    std::vector<QString> m_Arg3;

signals:

public slots:

};

class Plastic : public RadGeometry
{
public:
    explicit Plastic(QObject *parent=0);
    Plastic(double red, double green, double blue, double spec, double rough, QObject *parent=0);

    // Setters
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
   bool validateArg(int number, QString value, int position);
};

#endif // RADGEOMETRY_H
