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
//! @file   Ops.h
//! @author Robert Braun <robert.braun@liu.se>
//! @date   2021-04-07
//!
//! @brief Contains the optimization worker class for the Simplex Swarm algorithm
//!
//$Id$

#include "OpsWorkerSimplexSwarm.h"
#include "OpsEvaluator.h"
#include "OpsMessageHandler.h"
#include <math.h>
#include <limits.h>
#include <algorithm>

using namespace Ops;

Ops::WorkerSimplexSwarm::WorkerSimplexSwarm(Ops::Evaluator *pEvaluator, Ops::MessageHandler *pMessageHandler)
    : WorkerSimplex(pEvaluator, pMessageHandler)
{
    mRandomFactor=0;
}


AlgorithmT WorkerSimplexSwarm::getAlgorithm()
{
    return SimplexSwarm;
}


void WorkerSimplexSwarm::run()
{
    mpMessageHandler->printMessage("Running optimization with Nelder-Mead algorithm.");

    distributePoints();

    calculateBestAndWorstId();

    //Evaluate initial objective values
    mpEvaluator->evaluateAllPoints();
    mpMessageHandler->objectivesChanged();



    //Run optimization loop
    mIterationCounter=0;
    for(; mIterationCounter<mnMaxIterations && !mpMessageHandler->aborted(); ++mIterationCounter)
    {
        //Check convergence
        if(checkForConvergence()) break;

        std::vector<size_t> usedPoints;
        mCandidatePoints.clear();
        while(mCandidatePoints.size() < mNumCandidates) {
            //Pick three random points
            size_t id1, id2, id3;
            id1 = std::min(size_t(opsRand()*mNumPoints),mNumPoints-1);
            id2 = id1;
            while(id2 == id1) {
                id2 = std::min(size_t(opsRand()*mNumPoints),mNumPoints-1);
            }
            id3 = id2;
            while(id3 == id1 || id3 == id2) {
                id3 = std::min(size_t(opsRand()*mNumPoints),mNumPoints-1);
            }

            if(mObjectives[id2] > mObjectives[id1] && mObjectives[id2] > mObjectives[id3]) {
                std::swap(id1,id2); //id2 is worst
            }
            else if(mObjectives[id3] > mObjectives[id1] && mObjectives[id3] > mObjectives[id2]) {
                std::swap(id1,id3); //id3 is worst
            }

            if(std::find(usedPoints.begin(), usedPoints.end(), id1) != usedPoints.end()) {
                continue;   //Only reflect each point once at each iteration
            }
            std::vector<std::vector<double> > others;
            others.push_back(mPoints[id2]);
            others.push_back(mPoints[id3]);
            findCentroidPoint(others);
            double alpha = mAlphaMin + opsRand()*(mAlphaMax-mAlphaMin);
            mCandidatePoints.push_back(reflect(mPoints[id1],mCentroidPoint, alpha));
            usedPoints.push_back(id1);
        }

        mpMessageHandler->candidatesChanged();

        mpEvaluator->evaluateAllCandidates();

        calculateBestAndWorstId();
        size_t worst = getWorstId();
        for(size_t i=0; i<mNumCandidates; ++i) {
            if(mCandidateObjectives[i] < mObjectives[worst]) {
                mPoints[worst] = mCandidatePoints[i];
                mObjectives[worst] = mCandidateObjectives[i];
                calculateBestAndWorstId();
                worst = getWorstId();
            }
        }
        mpMessageHandler->pointsChanged();
        mpMessageHandler->objectivesChanged();

        //Check convergence
        if(checkForConvergence()) break;

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

void WorkerSimplexSwarm::setMinReflectionFactor(double value)
{
    mAlphaMin = value;
}

void WorkerSimplexSwarm::setMaxReflectionFactor(double value)
{
    mAlphaMax = value;
}

void WorkerSimplexSwarm::getNearestNeighbours(size_t id, size_t &id1, size_t &id2)
{
    id1 = id;
    while(id1 == id) {
        id1 = std::min(size_t(opsRand()*mNumPoints),mNumPoints-1);
    }
    id2 = id;
    while(id2 == id || id2 == id1) {
        id2 = std::min(size_t(opsRand()*mNumPoints),mNumPoints-1);
    }
    /*
    double dist1 = std::numeric_limits<double>::max();
    double dist2 = std::numeric_limits<double>::max();
    for(size_t i=0; i<mNumPoints; ++i) {
        if(i == id) {
            continue;
        }
        double dist = getDistanceBetweenPoints(id, i);
        if(dist < dist1) {
            id2 = id1;
            dist2 = dist1;
            id1 = i;
            dist1 = dist;
        }
        else if(dist < dist2) {
            id2 = i;
            dist2 = dist;
        }
    }*/
}

double WorkerSimplexSwarm::getDistanceBetweenPoints(size_t id1, size_t id2)
{
    double sum=0;
    for(size_t i=0; i<mNumParameters; ++i) {
        sum += (mPoints[id1][i]-mPoints[id2][i])*(mPoints[id1][i]-mPoints[id2][i]);
    }
    return sqrt(sum);
}
