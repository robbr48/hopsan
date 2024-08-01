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
//! @file   OpsWorkerSimulatedAnnealing.h
//! @author Robert Braun <robert.braun@liu.se>
//! @date   2024-08-01
//!
//! @brief Contains the optimization worker class for the simulatedannealing algorithm
//!
//$Id$
#ifndef OPSWORKERSIMULATEDANNEALING_H
#define OPSWORKERSIMULATEDANNEALING_H

#include "OpsWorker.h"

namespace Ops {

//enum OpsInertiaStrategy {InertiaConstant, InertiaLinearDecreasing};

class OPS_DLLAPI WorkerSimulatedAnnealing : public Worker
{
public:
    WorkerSimulatedAnnealing(Evaluator *pEvaluator, MessageHandler *pMessageHandler);

    AlgorithmT getAlgorithm();

    void initialize();
    void run();

    void setNumberOfPoints(size_t value);

//    void moveParticles();

//    void setOptVar(const std::string &var, const std::string &value);
//    double getOptVar(const std::string &var, bool &ok);

    void setInitialTemperature(double value);
    void setCoolingFactor(double value);
    void setPerturbationStep(double value); //Percentage of range for each parameter
//    void setOmega2(double value);
//    void setC1(double value);
//    void setC2(double value);
//    void setVmax(double value);
//    void setInertiaStrategy(OpsInertiaStrategy strategy);
    void setNumberOfParameters(size_t value);
protected:

    //OpsInertiaStrategy mInertiaStrategy;
    double mT0, mAlpha, mDx;
//    double mOmega, mOmega1, mOmega2, mC1, mC2;
//    bool mPrintLogOutput;
//    std::vector<std::string> mLogOutput;
//    std::vector< std::vector<double> > mVelocities, mLocalBestPoints;
//    std::vector<double> mLocalBestObjectives, mBestPoint;
//    double mBestObjective;
//    double mVmax;

private:
    void generateNeighbors();
//    void moveParticle(int p);
protected:
//    double mRandomFactor;

//    std::vector<double> mCentroidPoint;
};

}


#endif // OPSWORKERSIMULATEDANNEALING_H
