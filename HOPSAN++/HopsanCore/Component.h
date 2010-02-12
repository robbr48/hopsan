//!
//! @file   Component.h
//! @author <FluMeS>
//! @date   2009-12-20
//!
//! @brief Contains Component base classes as well as Component Parameter class
//!
//$Id$

#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED

#include "Node.h"
#include "CoreUtilities/ClassFactory.h"
#include "win32dll.h"
#include <string>

using namespace std;

class DLLIMPORTEXPORT CompParameter
{
    friend class Component;

private:
    CompParameter(const string name, const string description, const string unit, double &rValue); //should maybe be a description field as well

    double getValue();
    void setValue(const double value);

    ///TODO: getting strings can (probably) be speed up by returning const references instead of copying strings
    string getName();
    string getDesc();
    string getUnit();

    string mName;
    string mDescription;
    string mUnit;
    double* mpValue;
};

class ComponentSystem; //Forward declaration

class DLLIMPORTEXPORT Component
{
    friend class ComponentSystem;

public:
    //=====Public functions
    //Virtual functions
    virtual void initialize(const double startT, const double stopT);
    virtual void simulate(const double startT, const double Ts);

    //Name and type
    void setName(string name);
    const string &getName();
    const string &getType();
    const string &getTypeName();
    const string &getTypeCQS();

    //Parameters
    void listParametersConsole();
    double getParameter(const string name);
    void setParameter(const string name, const double value);
    map<string, double> getParameterList();

    //Ports
    vector<Port*> getPortPtrVector();
    Port &getPort(const string portname);

    //System parent
    ComponentSystem &getSystemparent();

    // Component type identification
    bool isComponentC();
    bool isComponentQ();
    bool isComponentSystem();
    bool isComponentSignal();

    //void setTimestep(const double timestep); ///TODO: Should it be possible to set timestep of a component? Should only be possible for a Systemcomponent
    //double getTimestep();

protected:
    //=====Protected member functions
    //Constructor - Destructor
    Component(string name="DefaultComponentName", double timestep=0.001);
    virtual ~Component(){};

    //Virtual functions
    virtual void initialize(); ///TODO: Default values are hard set
    virtual void simulateOneTimestep();
    virtual void setTimestep(const double timestep);

    //Parameter functions
    void registerParameter(const string name, const string description, const string unit, double &rValue);

    //Port functions
    Port* addPort(const string portname, const string porttype, const NodeTypeT nodetype, const int id=-1);
    Port* addPowerPort(const string portname, const string nodetype, const int id=-1);
    Port* addReadPort(const string portname, const string nodetype, const int id=-1);
    Port* addWritePort(const string portname, const string nodetype, const int id=-1);
    //void addMultiPort(const string portname, const string nodetype, const size_t nports, const size_t startctr=0);
    bool getPort(const string portname, Port* &prPort);
    Port &getPortById(const size_t port_idx);

    //=====Protected member variables
    string mTypeCQS;
    string mTypeName;
    double mTimestep, mDesiredTimestep;
    double mTime;
    bool mIsComponentC;
    bool mIsComponentQ;
    bool mIsComponentSystem;
    bool mIsComponentSignal;
    vector<Port*> mPortPtrs;

private:
    //Private member functions
    void setSystemparent(ComponentSystem &rComponentSystem);
    //Port* addInnerPortSetNode(const string portname, const string porttype, Node* pNode);
    void addSubNode(Node* node_ptr);

    //Private member variables
    string mName;
    vector<Node*> mSubNodePtrs;
    vector<CompParameter> mParameters;
    ComponentSystem* mpSystemparent;
};


class DLLIMPORTEXPORT ComponentSignal :public Component
{
protected:
    ComponentSignal(string name, double timestep=0.001);
};


class DLLIMPORTEXPORT ComponentC :public Component
{
protected:
    ComponentC(string name, double timestep=0.001);
};


class DLLIMPORTEXPORT ComponentQ :public Component
{
protected:
    ComponentQ(string name, double timestep=0.001);
};


class DLLIMPORTEXPORT ComponentSystem :public Component
{
public:
    ComponentSystem(string name="DefaultComponentSystemName", double timestep=0.001);
    void addComponents(vector<Component*> components);
    void addComponent(Component &rComponent);
    void addComponent(Component *pComponent);

    void connect(Component &rComponent1, const string portname1, Component &rComponent2, const string portname2);
    void connect(Component *pComponent1, const string portname1, Component *pComponent2, const string portname2);
    void connect(Port &rPort1, Port &rPort2);
    void simulate(const double startT, const double stopT);
    void initialize(const double startT, const double stopT);

    Port* addSystemPort(const string portname);
    void setTypeCQS(const string cqs_type);
    void setDesiredTimestep(const double timestep);

    Component* getComponent(string name);
    ComponentSystem* getComponentSystem(string name);
    map<string, string> getComponentNames();

private:
    void logAllNodes(const double time);

    void setTimestep(const double timestep);
    void adjustTimestep(double timestep, vector<Component*> componentPtrs);
    void preAllocateLogSpace(const double startT, const double stopT);
    //void addInnerPortSetNode(const string portname, const string porttype, Node* pNode);
    bool connectionOK(Node *pNode, Port *pPort1, Port *pPort2);

    //vector<Component*> mSubComponentPtrs; //Problems with inheritance and casting?
    vector<Component*> mComponentSignalptrs;
    vector<Component*> mComponentQptrs;
    vector<Component*> mComponentCptrs;
    map<string, string> mComponentNames;

    NodeFactory mpNodeFactory;
};

typedef ClassFactory<string, Component> ComponentFactory;
extern ComponentFactory gCoreComponentFactory;
DLLIMPORTEXPORT ComponentFactory* getCoreComponentFactoryPtr();

#endif // COMPONENT_H_INCLUDED
