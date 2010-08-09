/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Linköping University,
 * Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 
 * AND THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S  
 * ACCEPTANCE OF THE OSMC PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Linköping University, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*
 * HopsanGUI
 * Fluid and Mechatronic Systems, Department of Management and Engineering, Linköping University
 * Main Authors 2009-2010:  Robert Braun, Björn Eriksson, Peter Nordin
 * Contributors 2009-2010:  Mikael Axin, Alessandro Dell'Amico, Karl Pettersson, Ingo Staack
 */

//!
//! @file   SimulationThread.cpp
//! @author Björn Eriksson <bjorn.eriksson@liu.se>
//! @date   2010-03-09
//!
//! @brief Contains a class for simulation in a separate class
//!
//$Id$


#include "SimulationThread.h"
#include "ProjectTabWidget.h"
#include "GUIRootSystem.h"
#include "MainWindow.h"


//! @class SimulationThread
//! @brief The SimulationThread class implement a class to simulate a core model in a separate thread
//!
//! One reason to implement a simulation in a separate thread is to enable a progress bar.
//!


//! Constructor.
//! @param pComponentSystem is a pointer to the system to simulate.
//! @param startTime is the start time for the simulation.
//! @param finishTime is the finish time for the simulation.
//! @param parent is the parent of the thread, the a ProjectTabWidget
SimulationThread::SimulationThread(GUIRootSystem *pGUIRootSystem, double startTime, double finishTime, ProjectTabWidget *parent)
{
    mpParentProjectTabWidget = parent;

    mpGUIRootSystem = pGUIRootSystem;

    mStartTime = startTime;
    mFinishTime = finishTime;

}


//! Implements the task for the thread.
void SimulationThread::run()
{
    QTime simTimer;
    simTimer.start();
    mpGUIRootSystem->simulate(mStartTime, mFinishTime);
    qDebug() << "Simulation time: " << (simTimer.elapsed()) << " ms";
    mpGUIRootSystem->finalize(mStartTime, mFinishTime);


    //exec(); //Is used if one want to run an event loop in this thread.
}
