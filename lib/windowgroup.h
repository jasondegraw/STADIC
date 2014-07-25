#ifndef WINDOWGROUP_H
#define WINDOWGROUP_H

#include <QObject>
#include <vector>
#include "shadecontrol.h"

namespace stadic {

class WindowGroup : public QObject
{
    Q_OBJECT
public:
    explicit WindowGroup(QObject *parent = 0);
    bool parseJson(const QJsonObject &object);

    //  Setters
    void setBSDF(bool isBSDF);
    void setBaseGeometry(QString file);

    //  Getters
    bool isBSDF();
    QString baseGeometry();
    std::vector<QString> bsdfBaseLayers();
    std::vector<QString> glazingLayers();
    std::vector<QString> shadeSettingGeometry();
    std::vector<std::vector<QString> > bsdfSettingLayers();
    ShadeControl *shadeControl();

private:
    bool m_BSDF;
    QString m_BaseGeometry;
    std::vector<QString> m_BSDFBaseLayers;
    std::vector<QString> m_GlazingLayers;
    std::vector<QString> m_ShadeSettingGeometry;
    std::vector<std::vector<QString> > m_BSDFSettingLayers;
    ShadeControl *m_ShadeControl;


signals:
    
public slots:
    
};

}

#endif // WINDOWGROUPS_H
