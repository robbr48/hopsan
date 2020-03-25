#include <QFileInfo>
#include "MessageHandler.h"
#include "OMSimulatorHandler.h"
#include "global.h"
#include <QDebug>

#ifdef _WIN32
#else
#include "dlfcn.h"
#endif

#define CHECK_OMS_CONNECTION     \
    if(!mIsConnected) \
    { \
        gpMessageHandler->addErrorMessage("OMSimulator not connected"); return false; \
    }

void loggingCallback(oms_message_type_enu_t msgtype, const char* msg) {
    QString message = "OMSimulator: "+QString(msg);
    QString type = "OMSimulator";
    if(msgtype == oms_message_info) {
        gpMessageHandler->addInfoMessage(message,type);
    }
    else if(msgtype == oms_message_warning) {
        gpMessageHandler->addWarningMessage(message,type);
    }
    else if(msgtype == oms_message_error) {
        gpMessageHandler->addErrorMessage(message,type);
    }
    else if(msgtype == oms_message_debug || msgtype == oms_message_trace) {
        gpMessageHandler->addDebugMessage(message,type);
    }
}

OMSimulatorHandler::OMSimulatorHandler()
{
}

bool OMSimulatorHandler::connect(QString filePath)
{
    mIsConnected = false;

#ifdef _WIN32
    gpMessageHandler->addErrorMessage("OMSimulator is not yet supported in Windows.");
    return false;
#else
    //First open the lib
    QFileInfo path(filePath);
    mpLibrary = dlopen(path.filePath().toStdString().c_str(), RTLD_NOW);
    if (!mpLibrary) {
        gpMessageHandler->addErrorMessage("Failed to load OMSimulator library: "+path.filePath()+", Error: "+dlerror());
        return mIsConnected;
    }
    else {
        mIsConnected=true;
    }
#endif

    mGetVersion = (oms_getVersion_t)loadFunction("oms_getVersion");
    mNewModel = (oms_newModel_t)loadFunction("oms_newModel");
    mAddSystem = (oms_addSystem_t)loadFunction("oms_addSystem");
    mAddSubModel = (oms_addSubModel_t)loadFunction("oms_addSubModel");
    mList = (oms_list_t)loadFunction("oms_list");
    mSetElementGeometry = (oms_setElementGeometry_t)loadFunction("oms_setElementGeometry");
    mGetElement = (oms_getElement_t)loadFunction("oms_getElement");
    mGetConnector = (oms_getConnector_t)loadFunction("oms_getConnector");
    mSetLoggingCallback = (oms_setLoggingCallback_t)loadFunction("oms_setLoggingCallback");
    mGetReal = (oms_getReal_t)loadFunction("oms_getReal");
    mGetModelState = (oms_getModelState_t)loadFunction("oms_getModelState");
    mReset = (oms_reset_t)loadFunction("oms_reset");
    mInstantiate = (oms_instantiate_t)loadFunction("oms_instantiate");
    mInitialize = (oms_initialize_t)loadFunction("oms_initialize");
    mSetLogFile = (oms_setLogFile_t)loadFunction("oms_setLogFile");
    mAddConnection = (oms_addConnection_t)loadFunction("oms_addConnection");
    mDeleteConnection = (oms_deleteConnection_t)loadFunction("oms_deleteConnection");
    mDelete = (oms_delete_t)loadFunction("oms_delete");
    mSimulate = (oms_simulate_t)loadFunction("oms_simulate");
    mStepUntil = (oms_stepUntil_t)loadFunction("oms_stepUntil");
    mTerminate = (oms_terminate_t)loadFunction("oms_terminate");
    mSetResultFile = (oms_setResultFile_t)loadFunction("oms_setResultFile");
    mSetLoggingInterval = (oms_setLoggingInterval_t)loadFunction("oms_setLoggingInterval");
    mSetStartTime = (oms_setStartTime_t)loadFunction("oms_setStartTime");
    mSetStopTime = (oms_setStopTime_t)loadFunction("oms_setStopTime");
    mSetFixedStepSize = (oms_setFixedStepSize_t)loadFunction("oms_setFixedStepSize");
    mExport = (oms_export_t)loadFunction("oms_export");
    mImportFile = (oms_importFile_t)loadFunction("oms_importFile");
    mGetElements = (oms_getElements_t)loadFunction("oms_getElements");
    mGetSystemType = (oms_getSystemType_t)loadFunction("oms_getSystemType");
    mGetComponentType = (oms_getComponentType_t)loadFunction("oms_getComponentType");
    mGetFMUInfo = (oms_getFMUInfo_t)loadFunction("oms_getFMUInfo");
    mGetConnections = (oms_getConnections_t)loadFunction("oms_getConnections");
    mSetConnectionGeometry = (oms_setConnectionGeometry_t)loadFunction("oms_setConnectionGeometry");
    mAddConnector = (oms_addConnector_t)loadFunction("oms_addConnector");


    if(mIsConnected) {
        gpMessageHandler->addInfoMessage("Loaded OMSimulator version: "+QString(mGetVersion()));
    }

    mSetLoggingCallback(&loggingCallback);

    return mIsConnected;
}

bool OMSimulatorHandler::getVersion(QString &version)
{
    CHECK_OMS_CONNECTION;
    version = QString(mGetVersion());
    return true;
}

bool OMSimulatorHandler::newModel(QString name)
{
    CHECK_OMS_CONNECTION;
    int status = mNewModel(name.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::addSystem(QString name, OMSimulatorType type)
{
    CHECK_OMS_CONNECTION;
    int status = mAddSystem(name.toStdString().c_str(),(int)type);
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::addSubModel(QString name, QString path)
{
    CHECK_OMS_CONNECTION;
    int status = mAddSubModel(name.toStdString().c_str(), path.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::list(QString ref, QString &contents)
{
    CHECK_OMS_CONNECTION;

    char* contents_str;
    int status = mList(ref.toStdString().c_str(),&contents_str);
    contents = QString(contents_str);

    return (oms_status_ok == status);
}

bool OMSimulatorHandler::setElementGeometry(QString ref, double x, double y, double w, double h)
{
    CHECK_OMS_CONNECTION;

    ssd_element_geometry_t geometry;
    geometry.x1 = x;
    geometry.y1 = y;
    geometry.x2 = x+w;
    geometry.y2 = y+h;
    geometry.rotation = 0.0;
    geometry.iconSource = nullptr;
    geometry.iconFlip = false;
    geometry.iconFixedAspectRatio = false;

    int status = mSetElementGeometry(ref.toStdString().c_str(),&geometry);
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::getPortNames(QString ref, QStringList &portNames)
{
    CHECK_OMS_CONNECTION;

    oms_element_t* element = nullptr;
    int status = mGetElement(ref.toStdString().c_str(),&element);
    if(status == oms_status_ok) {
        for (int i=0; element->connectors[i]; i++) {
            portNames << element->connectors[i]->name;
        }
    }

    return (oms_status_ok == status);
}

bool OMSimulatorHandler::getPortCausality(QString ref, OMSimulatorCausality &causality)
{
    CHECK_OMS_CONNECTION;

    oms_connector_t* connector = nullptr;
    causality = OMSimulatorCausality::Undefined;
    int status = mGetConnector(ref.toStdString().c_str(),&connector);
    if(status == oms_status_ok) {
        if(connector->causality == oms_causality_input) {
            causality = OMSimulatorCausality::Input;
        }
        else if(connector->causality == oms_causality_output) {
            causality = OMSimulatorCausality::Output;
        }
        else if(connector->causality == oms_causality_parameter) {
            causality = OMSimulatorCausality::Parameter;
        }
    }

    return (oms_status_ok == status);
}

bool OMSimulatorHandler::getPortDataType(QString ref, OMSimulatorDataType &dataType)
{
    CHECK_OMS_CONNECTION;

    oms_connector_t* connector = nullptr;
    dataType = OMSimulatorDataType::Undefined;
    int status = mGetConnector(ref.toStdString().c_str(),&connector);
    if(status == oms_status_ok) {
        if(connector->type == oms_signal_type_real) {
            dataType = OMSimulatorDataType::Real;
        }
        else if(connector->type == oms_signal_type_integer) {
            dataType = OMSimulatorDataType::Integer;
        }
        else if(connector->type == oms_signal_type_boolean) {
            dataType = OMSimulatorDataType::Boolean;
        }
        else if(connector->type == oms_signal_type_integer) {
            dataType = OMSimulatorDataType::String;
        }
    }

    return (oms_status_ok == status);
}


double OMSimulatorHandler::getReal(QString ref, bool &ok)
{
    if(!mIsConnected) {
        gpMessageHandler->addErrorMessage("OMSimulator not connected");
        ok = false;
        return 0;
    }

    double value = 0;
    int status = mGetReal(ref.toStdString().c_str(),&value);
    ok = (status==0);
    return value;
}

bool OMSimulatorHandler::getModelState(QString ref, OMSimulatorState &state)
{
    CHECK_OMS_CONNECTION;

    oms_modelState_enu_t oms_state;
    int status = mGetModelState(ref.toStdString().c_str(),&oms_state);
    if(status == oms_status_ok) {
        if(oms_state == oms_modelState_virgin) {
            state = OMSimulatorState::Virgin;
        }
        else if(oms_state == oms_modelState_enterInstantiation) {
            state = OMSimulatorState::EnterInstantiation;
        }
        else if(oms_state == oms_modelState_instantiated) {
            state = OMSimulatorState::Instantiated;
        }
        else if(oms_state == oms_modelState_initialization) {
            state = OMSimulatorState::Initialization;
        }
        else if(oms_state == oms_modelState_simulation) {
            state = OMSimulatorState::Simulation;
        }
        else if(oms_state == oms_modelState_error) {
            state = OMSimulatorState::Error;
        }
    }
    else {
        state = OMSimulatorState::Undefined;
    }

    return (oms_status_ok == status);
}

bool OMSimulatorHandler::reset(QString name)
{
    CHECK_OMS_CONNECTION;
    int status = mReset(name.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::instantiate(QString name)
{
    CHECK_OMS_CONNECTION;
    int status = mInstantiate(name.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::initialize(QString name)
{
    CHECK_OMS_CONNECTION;
    int status = mInitialize(name.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::setLogFile(QString file)
{
    CHECK_OMS_CONNECTION;
    int status = mSetLogFile(file.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::addConnection(QString port1, QString port2)
{
    CHECK_OMS_CONNECTION;
    int status = mAddConnection(port1.toStdString().c_str(), port2.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::deleteConnection(QString port1, QString port2)
{
    CHECK_OMS_CONNECTION;
    int status = mDeleteConnection(port1.toStdString().c_str(), port2.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::deleteSubModelOrSystem(QString name)
{
    CHECK_OMS_CONNECTION;
    int status = mDelete(name.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::simulate(QString name)
{
    CHECK_OMS_CONNECTION;
    int status = mSimulate(name.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::stepUntil(QString name, double stopTime)
{
    CHECK_OMS_CONNECTION;
    int status = mStepUntil(name.toStdString().c_str(), stopTime);
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::terminate(QString name)
{
    CHECK_OMS_CONNECTION;
    int status = mTerminate (name.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::setResultFile(QString modelName, QString fileName)
{
    CHECK_OMS_CONNECTION;
    int status = mSetResultFile(modelName.toStdString().c_str(), fileName.toStdString().c_str(), 1);
    return (oms_status_ok == status);
}

bool OMSimulatorHandler::setLoggingInterval(QString model, double value)
{
    CHECK_OMS_CONNECTION;
    int status = mSetLoggingInterval(model.toStdString().c_str(), value);
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::setStartTime(QString model, double value)
{
    CHECK_OMS_CONNECTION;
    int status = mSetStartTime(model.toStdString().c_str(), value);
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::setStopTime(QString model, double value)
{
    CHECK_OMS_CONNECTION;
    int status = mSetStopTime(model.toStdString().c_str(), value);
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::setFixedStepSize(QString model, double value)
{
    CHECK_OMS_CONNECTION;
    int status = mSetFixedStepSize(model.toStdString().c_str(), value);
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::importModel(QString fileName, QString &modelName)
{
    CHECK_OMS_CONNECTION;
    char* model = nullptr;
    int status = mImportFile(fileName.toStdString().c_str(), &model);
    if(oms_status_ok==status) {
        modelName = QString(model);
    }
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::exportModel(QString modelName, QString fileName)
{
    CHECK_OMS_CONNECTION;
    int status = mExport(modelName.toStdString().c_str(), fileName.toStdString().c_str());
    return (oms_status_ok==status);
}

bool OMSimulatorHandler::getRootSystemName(QString modelName, QString &rootSystemName)
{
    CHECK_OMS_CONNECTION;
    oms_element_t** pElements = nullptr;
    int status = mGetElements(modelName.toStdString().c_str(), &pElements);
    if(oms_status_ok == status) {
        rootSystemName = QString(pElements[0]->name);
    }
    return oms_status_ok == status;
}

bool OMSimulatorHandler::getSubElementNames(QString ref, QStringList &elementNames)
{
    CHECK_OMS_CONNECTION;
    oms_element_t *pElement = nullptr;
    int status = mGetElement(ref.toStdString().c_str(), &pElement);
    if(oms_status_ok == status) {
        for(int i=0; pElement->elements[i]; ++i) {
            elementNames.append(pElement->elements[i]->name);
        }
    }
    return oms_status_ok == status;
}

bool OMSimulatorHandler::getElementType(QString ref, OMSimulatorType &type)
{
    CHECK_OMS_CONNECTION;
    oms_element_t* pElement = nullptr;
    type = OMSimulatorType::None;
    int status = mGetElement(ref.toStdString().c_str(), &pElement);
    if(oms_status_ok == status) {
        if(pElement->type == oms_element_system) {
            oms_system_enu_t systemType;
            int status = mGetSystemType(ref.toStdString().c_str(), &systemType);
            if(oms_status_ok == status) {
                if(systemType == oms_system_tlm) {
                    type = OMSimulatorType::TLM;
                }
                else if(systemType == oms_system_wc) {
                    type = OMSimulatorType::WeaklyCoupled;
                }
                else if(systemType == oms_system_sc) {
                    type = OMSimulatorType::StronglyCoupled;
                }
            }
        }
        else if(pElement->type == oms_element_component) {
            oms_component_enu_t componentType;
            int status = mGetComponentType(ref.toStdString().c_str(), &componentType);
            if(oms_status_ok == status) {
                if(componentType == oms_component_fmu) {
                    type = OMSimulatorType::FMU;
                }
                else if(componentType == oms_component_external) {
                    type = OMSimulatorType::ExternalModel;
                }
                else if(componentType == oms_component_table) {
                    gpMessageHandler->addWarningMessage("Tables in OMSimulator models are not yet supported.");
                    type = OMSimulatorType::None;
                }
            }
        }
    }

    return (oms_status_ok == status);
}

bool OMSimulatorHandler::getElementGeometry(QString ref, double &x, double &y, double &w, double &h)
{
    CHECK_OMS_CONNECTION;
    oms_element_t* element = nullptr;
    int status = mGetElement(ref.toStdString().c_str(),&element);
    if(status == oms_status_ok) {
        x = element->geometry->x1;
        y = element->geometry->y1;
        w = element->geometry->x2-x;
        h = element->geometry->y2-y;
    }
    return (oms_status_ok == status);
}

bool OMSimulatorHandler::getFMUFileInfoPath(QString ref, QFileInfo &fileInfo)
{
    CHECK_OMS_CONNECTION;
    const oms_fmu_info_t* pFMUInfo = nullptr;
    int status = mGetFMUInfo(ref.toStdString().c_str(), &pFMUInfo);
    if(oms_status_ok == status) {
        fileInfo.setFile(pFMUInfo->path);
    }
    return (status == oms_status_ok);
}

bool OMSimulatorHandler::getConnections(QString ref, QList<OMSimulatorConnection> &connections)
{
    CHECK_OMS_CONNECTION;
    oms_connection_t** pConnections = nullptr;
    int status = mGetConnections(ref.toStdString().c_str(), &pConnections);
    if(oms_status_ok == status) {
        for(int i=0; pConnections[i]; ++i) {
            OMSimulatorConnection connection;
            connection.startPort = pConnections[i]->conA;
            connection.endPort = pConnections[i]->conB;
            for(int j=0; j<pConnections[i]->geometry->n; ++j) {
                double x = pConnections[i]->geometry->pointsX[j];
                double y = pConnections[i]->geometry->pointsY[j];
                connection.points.append(QPointF(x,y));
            }
            connections.append(connection);
        }
    }
    return (oms_status_ok == status);
}

bool OMSimulatorHandler::setConnectionGeometry(QString refA, QString refB, QVector<QPointF> points)
{
    CHECK_OMS_CONNECTION;
    ssd_connection_geometry_t geometry;
    std::vector<double> vx,vy;
    for(const QPointF &point : points) {
        vx.push_back(point.x());
        vy.push_back(point.y());
    }
    geometry.n = points.size();
    geometry.pointsX = &vx[0];
    geometry.pointsY = &vy[0];
    int status = mSetConnectionGeometry(refA.toStdString().c_str(), refB.toStdString().c_str(), &geometry);
    return (oms_status_ok == status);
}

bool OMSimulatorHandler::addConnector(QString ref, OMSimulatorCausality causality)
{
    CHECK_OMS_CONNECTION;
    int status = mAddConnector(ref.toStdString().c_str(), oms_causality_enu_t(causality), oms_signal_type_real);
    return (oms_status_ok==status);
}

void *OMSimulatorHandler::loadFunction(QString name)
{
    char *dlsym_error;
    void* retval = dlsym(mpLibrary, name.toStdString().c_str());
    dlsym_error = dlerror();
    if (dlsym_error) {
        gpMessageHandler->addErrorMessage("Failed to load symbol '"+name+", Error: "+dlsym_error);
        mIsConnected=false;
    }
    return retval;
}
