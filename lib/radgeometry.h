#ifndef RADGEOMETRY_H
#define RADGEOMETRY_H

#include <QObject>

class RadGeometry : public QObject
{
    Q_OBJECT
public:
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
