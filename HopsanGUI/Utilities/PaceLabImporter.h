#ifndef PACELABIMPORTER_H
#define PACELABIMPORTER_H

#include "Widgets/ModelWidget.h"

#include <QString>

namespace pacelab {
namespace csv {
constexpr auto name = "Name";
constexpr auto type = "Type";
constexpr auto source = "Source";
constexpr auto sourcetype = "SourceType";
constexpr auto target = "Target";
constexpr auto targettype = "TargetType";
namespace component {
constexpr auto reservoir = "Pace.SysArc.Components.Hydraulic.Reservoir";
constexpr auto centrifugalpump = "Pace.SysArc.Components.Hydraulic.CentrifugalPump";
constexpr auto pressuremodule = "Pace.SysArc.Components.Hydraulic.PressureModule";
}
}
typedef struct
{
    QString name;
    QString type;
    QStringList downstream;
    QStringList upstream;
} component;

typedef struct
{
    QString source;
    QString sourceType;
    QString target;
    QString targetType;
} connection;
}

class PaceLabImporter
{
public:
    PaceLabImporter(const QString &componentsFilePath, const QString &connectionsFilePath);
    void populateModel(ModelWidget *pModel);

private:
    void addComponent(ModelWidget *pModel, pacelab::component *pComponent, int x, int y);
    bool mIsValid = false;
    QMap<QString, pacelab::component*> mComponents;
    QStringList mUsedComponents;
    QChar sep = ';';
    int dist = 100;
};

#endif // PACELABIMPORTER_H
