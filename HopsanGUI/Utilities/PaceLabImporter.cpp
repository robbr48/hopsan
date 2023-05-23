#include "PaceLabImporter.h"
#include "global.h"
#include "MessageHandler.h"
#include "GUIObjects/GUIContainerObject.h"
#include <QTextStream>

PaceLabImporter::PaceLabImporter(const QString &componentsFilePath, const QString &connectionsFilePath)
{
    gpMessageHandler->addInfoMessage("Generating Hopsan model from PaceLab CSV");

    //Read components file
    {
        QFile componentsFile(componentsFilePath);
        componentsFile.open(QFile::ReadOnly);
        QTextStream componentsStream(&componentsFile);
        int numColumns, nameIndex, typeIndex;
        bool headerRow = true;
        while(!componentsStream.atEnd()) {
            QStringList row = componentsStream.readLine().split(sep);
            if(headerRow) {
                if(!row.contains(pacelab::csv::name) || !row.contains(pacelab::csv::type)) {
                    gpMessageHandler->addErrorMessage("Wrong headers in components CSV file");
                    mIsValid = false;
                    return;
                }
                numColumns = row.size();
                nameIndex = row.indexOf(pacelab::csv::name);
                gpMessageHandler->addInfoMessage("Name index: "+QString::number(nameIndex));
                typeIndex = row.indexOf(pacelab::csv::type);
                gpMessageHandler->addInfoMessage("Type index: "+QString::number(typeIndex));
                headerRow = false;
                continue;
            }
            if(row.size() != numColumns) {
                gpMessageHandler->addErrorMessage("Malformed component CSV file.");
                mIsValid = false;
                return;
            }
            pacelab::component *component = new pacelab::component;
            component->name = row.at(nameIndex);
            component->type = row.at(typeIndex);
            mComponents[row.at(nameIndex)] = component;
            gpMessageHandler->addInfoMessage("Adding component: "+row.at(nameIndex));
        }
        componentsFile.close();
    }

    //Read connections file
    {
        QFile connectionsFile(connectionsFilePath);
        connectionsFile.open(QFile::ReadOnly);
        QTextStream connectionsStream(&connectionsFile);
        int numColumns, sourceIndex, sourceTypeIndex, targetIndex, targetTypeIndex;
        bool headerRow = true;
        while(!connectionsStream.atEnd()) {
            QStringList row = connectionsStream.readLine().split(sep);
            if(headerRow) {
                if(!row.contains(pacelab::csv::source) ||
                    !row.contains(pacelab::csv::sourcetype) ||
                    !row.contains(pacelab::csv::target) ||
                    !row.contains(pacelab::csv::targettype)) {
                    gpMessageHandler->addErrorMessage("Wrong headers in components CSV file");
                    mIsValid = false;
                    return;
                }
                numColumns = row.size();
                sourceIndex = row.indexOf(pacelab::csv::source);
                gpMessageHandler->addInfoMessage("Source index: "+QString::number(sourceIndex));
                sourceTypeIndex = row.indexOf(pacelab::csv::sourcetype);
                gpMessageHandler->addInfoMessage("Source type index: "+QString::number(sourceTypeIndex));
                targetIndex = row.indexOf(pacelab::csv::target);
                gpMessageHandler->addInfoMessage("Target index: "+QString::number(targetIndex));
                targetTypeIndex = row.indexOf(pacelab::csv::targettype);
                gpMessageHandler->addInfoMessage("Target type index: "+QString::number(targetTypeIndex));
                headerRow = false;
                continue;
            }
            if(row.size() != numColumns) {
                gpMessageHandler->addErrorMessage("Malformed connections CSV file.");
                mIsValid = false;
                return;
            }
            pacelab::connection connection;
            QString source = row.at(sourceIndex);
            //connection.sourceType = row.at(sourceTypeIndex);
            QString target = row.at(targetIndex);
            //connection.targetType = row.at(targetTypeIndex);

            QString sourceComponent = source.left(source.lastIndexOf("."));
            QString targetComponent = target.left(target.lastIndexOf("."));

            gpMessageHandler->addInfoMessage("Looking for component "+sourceComponent+" and " +targetComponent);

            mComponents[sourceComponent]->upstream.push_back(targetComponent);
            mComponents[targetComponent]->downstream.push_back(sourceComponent);
        }
        connectionsFile.close();
    }

    mIsValid = true;
}


void PaceLabImporter::populateModel(ModelWidget *pModel)
{
    if(!mIsValid) {
        gpMessageHandler->addErrorMessage("Cannot populate model: Invalid components or connections specification");
        return;
    }

    gpMessageHandler->addInfoMessage("Populating model!");

    int x = 1000;
    QMapIterator<QString, pacelab::component*> i(mComponents);
    while(i.hasNext()) {
        i.next();
        if(mUsedComponents.contains(i.key())) {
            continue;
        }
        //gpMessageHandler->addInfoMessage("Adding component \""+i.key()+"\" with type \""+i.value()->type+"\"");
        addComponent(pModel, i.value(), 1000, x);
        x += 500;
    }
}

void PaceLabImporter::addComponent(ModelWidget *pModel, pacelab::component *pComponent, int x, int y) {
    if(mUsedComponents.contains(pComponent->name)) {
        return;
    }

    mUsedComponents.push_back(pComponent->name);
    ModelObject *pObject = nullptr;
    if(pComponent->type == pacelab::csv::component::reservoir) {
        QPointF pos = QPointF(x, y);
        pObject = pModel->getTopLevelSystemContainer()->addModelObject("HydraulicTankC", pos, 0);
    }
    else if(pComponent->type == pacelab::csv::component::centrifugalpump) {
        QPointF pos = QPointF(x, y);
        pObject = pModel->getTopLevelSystemContainer()->addModelObject("HydraulicCentrifugalPump", pos, 0);
    }
    else if(pComponent->type == pacelab::csv::component::pressuremodule) {
        QPointF pos = QPointF(x, y);
        pObject = pModel->getTopLevelSystemContainer()->addModelObject("HydraulicPressureReducingValve", pos, 0);
    }
    if(!pObject) {
        return;
    }
    pModel->getViewContainerObject()->renameModelObject(pObject->getName(), pComponent->name);

    int dx = x;
    for(const auto &upstream : qAsConst(pComponent->upstream))
    {
        addComponent(pModel, mComponents[upstream], x+dx, y+dist);
        x += dist;
    }
    dx = x;
    for(const auto &downstream : qAsConst(pComponent->downstream))
    {
        addComponent(pModel, mComponents[downstream], x+dx, y-dist);
        x += dist;
    }
}
