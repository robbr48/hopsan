/*-----------------------------------------------------------------------------

 Copyright 2017 Hopsan Group

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.


 The full license is available in the file LICENSE.
 For details about the 'Hopsan Group' or information about Authors and
 Contributors see the HOPSANGROUP and AUTHORS files that are located in
 the Hopsan source code root directory.

-----------------------------------------------------------------------------*/

//!
//! @file   OMSimulatorComponents.hpp
//! @author Robert Braun <robert.braun@liu.se>
//! @date   2020-03-24
//!
//! @brief Contains components for OMSimulator support
//!
//$Id$


#ifndef OMSIMULATORCOMPONENTS_HPP_INCLUDED
#define OMSIMULATORCOMPONENTS_HPP_INCLUDED

#include "ComponentEssentials.h"

namespace hopsan {

class OMSimulatorFMU : public ComponentSignal
{
private:
    HString mPorts, mCausalities, mParameterNames, mParameterDefaults;
    HVector<double> mRealParameterValues;
public:
    static Component* Creator(){ return new OMSimulatorFMU(); }
    void configure()
    {
        if(!this->hasParameter("ports"))
        {
            this->addConstant("ports", "Port names", "-", "", mPorts);
            this->addConstant("causalities", "Port causalities", "-", "", mCausalities);
            this->addConstant("parameters", "Parameter names", "-", "", mParameterNames);
            this->addConstant("defaults", "Default parameter values", "-", "", mParameterDefaults);
            addDebugMessage("Configuring (first time)!");
        }
        else
        {
            addDebugMessage("Configuring!");
            //Generate list of new ports, based on string parameter
            HVector<HString> portNames = mPorts.split(',');
            HVector<HString> causalities = mCausalities.split(',');
            if((portNames.size() != causalities.size()) ||
               (mPorts.empty() == !mCausalities.empty()) ||
               (!mPorts.empty() == mCausalities.empty())) {
                return; //Not necessarily an error, we will end up here anyway since we need to set one variable before the other
            }

            //Remove old ports if they do not exist in list of new ports
            for(size_t p=0; p<getPortNames().size(); ++p)
            {
                bool found=false;
                for(size_t j=0; j<portNames.size(); ++j)
                {
                    if(portNames[j] == getPortNames().at(p))
                    {
                        found = true;       //Old port exists in list of new ports, so keep it
                    }
                }
                if(!found)
                {
                    removePort(getPortNames().at(p)); //Old port does not exist among new ports, so remove it
                    --p;
                }
            }

            //Add new ports if they do not already exist
            for(size_t p=0; p<portNames.size(); ++p)
            {
                if(!getPort(portNames[p]))
                {
                    if(causalities[p] == "input") {
                        addReadPort(portNames[p], "NodeSignal", "");
                    }
                    else if(causalities[p] == "output") {
                        addWritePort(portNames[p], "NodeSignal", "");
                    }
                    else {
                        addErrorMessage("Illegal port causality "+causalities[p]);
                    }
                }
            }

            //Generate list of constants
            HVector<HString> newConstants= mParameterNames.split(',');
            HVector<HString> defaults= mParameterDefaults.split(',');
            if(mParameterNames.empty())
            if(mParameterDefaults.empty()) {
                defaults.clear();
            }
            if((newConstants.size() != defaults.size()) ||
               (mParameterNames.empty() == !mParameterDefaults.empty()) ||
               (!mParameterNames.empty() == mParameterDefaults.empty())) {
                return; //Not necessarily an error, we will end up here anyway since we need to set one variable before the other
            }

            //Unregister old constants if they do not exist in list of new constants
            std::vector<HString> oldConstants;
            getParameterNames(oldConstants);
            size_t numOldConstants = 0;
            for(size_t c=0; c<oldConstants.size(); ++c) {
                bool found=false;
                for(size_t j=0; j<newConstants.size(); ++j) {
                    if(newConstants[j] == oldConstants.at(c) ||
                       oldConstants.at(c) == "ports" ||
                       oldConstants.at(c) == "causalities" ||
                       oldConstants.at(c) == "parameters" ||
                       oldConstants.at(c) == "defaults") {
                        found = true;       //Old port exists in list of new ports, so keep it
                    }
                }
                if(!found) {
                    unRegisterParameter(oldConstants.at(c));
                }
            }

            //Add new ports if they do not already exist
            mRealParameterValues.resize(newConstants.size());
            for(size_t p=0; p<newConstants.size(); ++p) {
                if(!newConstants[p].empty() && !getParameter(newConstants[p])) {
                    bool ok;    //Dummy
                    double value = defaults[p].toDouble(&ok);
                    addConstant(newConstants[p], "-", "-", value, mRealParameterValues[p]);
                }
            }
        }
    }
};

class OMSimulatorExternalModel : public ComponentSignal
{
public:
    static Component* Creator(){ return new OMSimulatorExternalModel(); }
    void configure() {}
};

class OMSimulatorInputConnector : public ComponentSignal
{
public:
    static Component* Creator(){ return new OMSimulatorInputConnector(); }
    void configure() {}
};

class OMSimulatorOutputConnector : public ComponentSignal
{
public:
    static Component* Creator(){ return new OMSimulatorOutputConnector(); }
    void configure() {}
};

}

#endif // OMSIMULATORCOMPONENTS_HPP_INCLUDED
