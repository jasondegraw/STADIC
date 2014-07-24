#ifndef RADGEOMETRY_H
#define RADGEOMETRY_H

#include <QObject>
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
               Brighttext, Mixfunc, Mixdata, Mixpict, Mixtext};
    explicit RadGeometry(QObject *parent = 0);

    //Setters
    void setModifier(QString modifier);
    void setType(QString type);
    void setName(QString name);
    void setArg1(QString arg);
    void setArg2(QString arg);
    void setArg3(QString arg);

    //Getters
    QString modifier();
    QString type();
    QString name();
    std::vector<QString> arg1();
    std::vector<QString> arg2();
    std::vector<QString> arg3();


private:
    static std::array<QString,50> s_typeStrings;
    static Type typeFromString(QString string);

    QString m_Modifier;
    QString m_Type;
    QString m_Name;
    std::vector<QString> m_Arg1;
    std::vector<QString> m_Arg2;
    std::vector<QString> m_Arg3;

signals:

public slots:

};

#endif // RADGEOMETRY_H
