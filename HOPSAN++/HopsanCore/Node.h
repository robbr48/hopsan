//! @file   Node.h
//! @author <FluMeS>
//! @date   2009-12-20
//!
//! @brief Contains Node base classes
//!
//$Id$

#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <vector>
#include <string>
#include "CoreUtilities/ClassFactory.h"
#include "win32dll.h"

using namespace std;

typedef string NodeTypeT;

class Port; //forward declaration

class DLLIMPORTEXPORT Node
{
    friend class Port;
    friend class Component;
    friend class ComponentSystem;

///TODO: Nodes should know their ports so a check can be performed by the node at connection time, the check should be virtual and implement different checks at different nodes.
public:
    //The user should never bother about Nodes

protected:
    Node();
    NodeTypeT &getNodeType();

    void preAllocateLogSpace(const size_t nSlots);
    void logData(const double time);
    void saveLogData(string filename);
    void setData(const size_t data_type, double data);
    double getData(const size_t data_type);
    double &getDataRef(const size_t data_type);
    NodeTypeT mNodeType;
    vector<double> mDataVector;
    vector<Port*> mPortPtrs;

private:
    string mName;
    vector<double> mTimeStorage;
    vector<vector<double> > mDataStorage;
    bool mLogSpaceAllocated;
    size_t mLogCtr;
    void setPort(Port *pPort);
    bool connectedToPort(Port *pPort);

};

typedef ClassFactory<NodeTypeT, Node> NodeFactory;
extern NodeFactory gCoreNodeFactory;
DLLIMPORTEXPORT NodeFactory* getCoreNodeFactoryPtr();

#endif // NODE_H_INCLUDED
