#ifndef RADGEOMETRY_H
#define RADGEOMETRY_H

#include <QObject>

namespace stadic {

class RadGeometry : public QObject
{
    Q_OBJECT
public:
    explicit RadGeometry(QObject *parent = 0);

    //Setters
    void setModifier(QString modifier);
    void setType(QString type);
    void setName(QString name);
    bool setArg1(std::vector<QString> vals);
    bool setArg1(QString arg, int position);
    bool setArg2(std::vector<QString> vals);
    bool setArg2(QString arg, int position);
    bool setArg3(std::vector<QString> vals);
    bool setArg3(QString arg, int position);

    //Getters
    QString modifier();
    QString type();
    QString name();
    std::vector<QString> arg1();
    std::vector<QString> arg2();
    std::vector<QString> arg3();


private:
    virtual bool validateArg(int number, QString value, int position) {return true;}

    QString m_Modifier;
    QString m_Type;
    QString m_Name;
    std::vector<QString> m_Arg1;
    std::vector<QString> m_Arg2;
    std::vector<QString> m_Arg3;

signals:

public slots:

};

}

#endif // RADGEOMETRY_H
