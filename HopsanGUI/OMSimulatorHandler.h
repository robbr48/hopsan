/*-----------------------------------------------------------------------------

 Copyright 2017 Hopsan Group

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 The full license is available in the file GPLv3.
 For details about the 'Hopsan Group' or information about Authors and
 Contributors see the HOPSANGROUP and AUTHORS files that are located in
 the Hopsan source code root directory.

-----------------------------------------------------------------------------*/

//!
//! @file   OMSimulatorHandler.h
//! @author Robert Braun <robert.braun@liu.se>
//! @date   2020-02-26
//!
//! @brief Contains the handler for OMSimulator library
//!
//$Id$

#ifndef OMSIMULATORHANDLER_H
#define OMSIMULATORHANDLER_H

#include <QString>
#include <QPointF>
#include "/home/robbr48/git/OpenModelica/OMSimulator/src/OMSimulatorLib/Types.h"

enum class OMSimulatorType {None=0, TLM=1, WeaklyCoupled=2, StronglyCoupled=3, FMU=4, ExternalModel=5, InputConnector=6, OutputConnector=7};
enum class OMSimulatorCausality {Input=0, Output=2, Parameter=3, Undefined=4};
enum class OMSimulatorDataType {Real=0, Integer=1, String=2, Boolean=3, Undefined=4};
enum class OMSimulatorState {Virgin=0,EnterInstantiation=1,Instantiated=2,Initialization=3,Simulation=4,Error=5,Undefined=6};

struct OMSimulatorConnection {
    QString startPort;
    QString endPort;
    QVector<QPointF> points;
};

typedef char* (*oms_getVersion_t)();
typedef int (*oms_newModel_t)(const char*);
typedef int (*oms_addSystem_t)(const char*, int);
typedef int (*oms_addSubModel_t)(const char*, const char*);
typedef int (*oms_list_t)(const char*, char**);
typedef int (*oms_setElementGeometry_t)(const char*, const ssd_element_geometry_t*);
typedef int (*oms_getElement_t)(const char*, oms_element_t** element);
typedef int (*oms_getConnector_t)(const char*, oms_connector_t** element);
typedef int (*oms_getReal_t)(const char*, double*);
typedef void (*oms_setLoggingCallback_t)(void (*)(oms_message_type_enu_t, const char*));
typedef int (*oms_getModelState_t)(const char*, oms_modelState_enu_t*);
typedef int (*oms_reset_t)(const char*);
typedef int (*oms_instantiate_t)(const char*);
typedef int (*oms_initialize_t)(const char*);
typedef int (*oms_setLogFile_t)(const char*);
typedef int (*oms_addConnection_t)(const char*, const char*);
typedef int (*oms_deleteConnection_t)(const char*, const char*);
typedef int (*oms_delete_t)(const char*);
typedef int (*oms_simulate_t)(const char*);
typedef int (*oms_stepUntil_t)(const char*, double);
typedef int (*oms_terminate_t)(const char*);
typedef int (*oms_setResultFile_t)(const char*, const char*, int);
typedef int (*oms_setLoggingInterval_t)(const char*, double);
typedef int (*oms_setStartTime_t)(const char*, double);
typedef int (*oms_setStopTime_t)(const char*, double);
typedef int (*oms_setFixedStepSize_t)(const char*, double);
typedef int (*oms_export_t)(const char*, const char*);
typedef int (*oms_importFile_t)(const char*, char**);
typedef int (*oms_getElements_t)(const char*, oms_element_t***);
typedef int (*oms_getSystemType_t)(const char*, oms_system_enu_t*);
typedef int (*oms_getComponentType_t)(const char*, oms_component_enu_t*);
typedef int (*oms_getFMUInfo_t)(const char*, const oms_fmu_info_t**);
typedef int (*oms_getConnections_t)(const char*, oms_connection_t ***);
typedef int (*oms_setConnectionGeometry_t)(const char*, const char*, const ssd_connection_geometry_t*);
typedef int (*oms_addConnector_t)(const char*, oms_causality_enu_t, oms_signal_type_enu_t);

class OMSimulatorHandler
{
public:
    OMSimulatorHandler();
    bool connect(QString filePath);
    bool isConnected() { return mIsConnected; }

    bool getVersion(QString &version);
    bool newModel(QString name);
    bool addSystem(QString name, OMSimulatorType type);
    bool addSubModel(QString name, QString path);
    bool list(QString ref, QString &contents);
    bool setElementGeometry(QString ref, double x, double y, double w, double h);
    bool getPortNames(QString ref, QStringList &portNames);
    bool getPortCausality(QString ref, OMSimulatorCausality &causality);
    bool getPortDataType(QString ref, OMSimulatorDataType &dataType);
    double getReal(QString ref, bool &ok);
    bool getModelState(QString ref, OMSimulatorState &state);
    bool reset(QString name);
    bool instantiate(QString name);
    bool initialize(QString name);
    bool setLogFile(QString file);
    bool addConnection(QString port1, QString port2);
    bool deleteConnection(QString port1, QString port2);
    bool deleteSubModelOrSystem(QString name);
    bool simulate(QString name);
    bool stepUntil(QString name, double stopTime);
    bool terminate(QString name);
    bool setResultFile(QString modelName, QString fileName);
    bool setLoggingInterval(QString model, double value);
    bool setStartTime(QString model, double value);
    bool setStopTime(QString model, double value);
    bool setFixedStepSize(QString model, double value);
    bool importModel(QString fileName, QString &modelName);
    bool exportModel(QString modelName, QString fileName);
    bool getRootSystemName(QString modelName, QString &rootSystemName);
    bool getSubElementNames(QString ref, QStringList &elementNames);
    bool getElementType(QString ref, OMSimulatorType &type);
    bool getElementGeometry(QString ref, double &x, double &y, double &w, double &h);
    bool getFMUFileInfoPath(QString ref, QFileInfo &fileInfo);
    bool getConnections(QString ref, QList<OMSimulatorConnection> &connections);
    bool setConnectionGeometry(QString refA, QString refB, QVector<QPointF> points);
    bool addConnector(QString ref, OMSimulatorCausality causality);

private:
    void* loadFunction(QString name);

    void *mpLibrary;

    bool mIsConnected=false;

    oms_getVersion_t mGetVersion;
    oms_newModel_t mNewModel;
    oms_addSystem_t mAddSystem;
    oms_addSubModel_t mAddSubModel;
    oms_list_t mList;
    oms_setElementGeometry_t mSetElementGeometry;
    oms_getElement_t mGetElement;
    oms_getConnector_t mGetConnector;
    oms_setLoggingCallback_t mSetLoggingCallback;
    oms_getReal_t mGetReal;
    oms_getModelState_t mGetModelState;
    oms_reset_t mReset;
    oms_instantiate_t mInstantiate;
    oms_initialize_t mInitialize;
    oms_setLogFile_t mSetLogFile;
    oms_addConnection_t mAddConnection;
    oms_deleteConnection_t mDeleteConnection;
    oms_delete_t mDelete;
    oms_simulate_t mSimulate;
    oms_stepUntil_t mStepUntil;
    oms_terminate_t mTerminate;
    oms_setResultFile_t mSetResultFile;
    oms_setLoggingInterval_t mSetLoggingInterval;
    oms_setStartTime_t mSetStartTime;
    oms_setStopTime_t mSetStopTime;
    oms_setFixedStepSize_t mSetFixedStepSize;
    oms_importFile_t mImportFile;
    oms_export_t mExport;
    oms_getElements_t mGetElements;
    oms_getSystemType_t mGetSystemType;
    oms_getComponentType_t mGetComponentType;
    oms_getFMUInfo_t mGetFMUInfo;
    oms_getConnections_t mGetConnections;
    oms_setConnectionGeometry_t mSetConnectionGeometry;
    oms_addConnector_t mAddConnector;
};

#endif // OMSIMULATORHANDLER_H
