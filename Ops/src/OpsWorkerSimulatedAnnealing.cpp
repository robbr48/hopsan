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
//! @brief Contains the optimization worker class for the simulated annealing algorithm
//!
//$Id$

#include "OpsWorkerSimulatedAnnealing.h"
#include "OpsEvaluator.h"
#include "OpsMessageHandler.h"
#include <math.h>
#include <algorithm>

using namespace Ops;

//! @brief Initializes a particle swarm optimization
WorkerSimulatedAnnealing::WorkerSimulatedAnnealing(Evaluator *pEvaluator, MessageHandler *pMessageHandler)
    : Worker(pEvaluator, pMessageHandler)
{
    //mInertiaStrategy = InertiaLinearDecreasing;
}

AlgorithmT WorkerSimulatedAnnealing::getAlgorithm()
{
    return SimulatedAnnealing;
}



void WorkerSimulatedAnnealing::initialize()
{
    Worker::initialize();

//    for(size_t p=0; p<mNumPoints; ++p)
//    {
//        for(size_t i=0; i<mNumParameters; ++i)
//        {
//            //Initialize velocities
//            double minVel = -fabs(mParameterMax[i]-mParameterMin[i]);
//            double maxVel = fabs(mParameterMax[i]-mParameterMin[i]);
//            double r = opsRand();
//            mVelocities[p][i] = minVel + r*(maxVel-minVel);
//        }
//    }
}


//! @brief Executes a particle swarm algorithm. optParticleInit() must be called before this one.
void WorkerSimulatedAnnealing::run()
{
    if(mNumCandidates != mNumPoints)
    {
        mpMessageHandler->printMessage("Error: Simulated annealing algorithm requires same number of candidates and points.");
        return;
    }

    mpMessageHandler->printMessage("Running optimization with simualted annealing algorithm.");

    distributePoints();

    //Evaluate initial objective values
    mpEvaluator->evaluateAllPointsWithSurrogateModel();
    mpMessageHandler->objectivesChanged();

    double T = mT0;

    mIterationCounter=0;
    for(; mIterationCounter<mnMaxIterations && !mpMessageHandler->aborted(); ++mIterationCounter)
    {
        generateNeighbors();
        mpMessageHandler->candidatesChanged();

        mpEvaluator->evaluateAllCandidatesWithSurrogateModel();
        mpMessageHandler->objectivesChanged();

        for(size_t i=0; i<mNumPoints; ++i) {
            double dE = mCandidateObjectives[i]-mObjectives[i];
            double p = exp(-dE/T);
            if(mCandidateObjectives[i] < mObjectives[i] || opsRand() < p) {
                mPoints[i] = mCandidatePoints[i];
                mObjectives[i] = mCandidateObjectives[i];
                mpMessageHandler->pointChanged(i);
            }
        }

        T = T/(1+mAlpha);

        //Check convergence
        if(checkForConvergence()) break;      //Use complex method, it's the same principle

        mpMessageHandler->stepCompleted(mIterationCounter);
    }

    if(mpMessageHandler->aborted())
    {
        mpMessageHandler->printMessage("Optimization was aborted after "+std::to_string(mIterationCounter)+" iterations.");
    }
    else if(mIterationCounter == mnMaxIterations)
    {
        mpMessageHandler->printMessage("Optimization failed to converge after "+std::to_string(mIterationCounter)+" iterations");
    }
    else
    {
        mpMessageHandler->printMessage("Optimization converged in parameter values after "+std::to_string(mIterationCounter)+" iterations.");
    }

    // Clean up
    finalize();

    return;
}


void WorkerSimulatedAnnealing::setNumberOfPoints(size_t value)
{
    Worker::setNumberOfPoints(value);

//    mVelocities.resize(value);
//    mLocalBestPoints.resize(value);
//    mLocalBestObjectives.resize(value);
//    for(size_t i=0; i<value; ++i)
//    {
//        mVelocities[i].resize(mNumPoints);
//        mLocalBestPoints[i].resize(mNumPoints);
//    }
}


void WorkerSimulatedAnnealing::setNumberOfParameters(size_t value)
{
    Worker::setNumberOfParameters(value);

//    mBestPoint.resize(value);
//    for(size_t i=0; i<mNumPoints; ++i)
//    {
//        mVelocities[i].resize(mNumPoints);
//        mLocalBestPoints[i].resize(mNumPoints);
    //    }
}

void WorkerSimulatedAnnealing::generateNeighbors()
{
    for(size_t i=0; i<mNumPoints; ++i) {
        for(size_t j=0; j<mNumParameters; ++j) {


            mCandidatePoints[i][j] = mPoints[i][j] + mDx*(mParameterMax[j]-mParameterMin[j])*(1-2*opsNormalDistribution(0.5*getMaxPercentalParameterDiff()));
            if(mCandidatePoints[i][j] <= mParameterMin[j]) {
                mCandidatePoints[i][j] = mParameterMin[j];
            }
            if(mCandidatePoints[i][j] >= mParameterMax[j]) {
                mCandidatePoints[i][j] = mParameterMax[j];
            }
        }
    }
}



void WorkerSimulatedAnnealing::setInitialTemperature(double value)
{
    mT0 = value;
}

void WorkerSimulatedAnnealing::setPerturbationStep(double value)
{
    mDx = value;
}


void WorkerSimulatedAnnealing::setCoolingFactor(double value)
{
    mAlpha = value;
}



