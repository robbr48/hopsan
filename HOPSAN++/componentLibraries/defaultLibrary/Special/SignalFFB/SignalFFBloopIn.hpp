#ifndef SIGNALFFBLOOPIN_HPP_INCLUDED
#define SIGNALFFBLOOPIN_HPP_INCLUDED

#include <iostream>
#include "ComponentEssentials.h"
#include "ComponentUtilities.h"
#include "math.h"

//!
//! @file SignalFFBloopIn.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @date Fri 28 Jun 2013 13:01:40
//! @brief FFBD Loop in
//! @ingroup SignalComponents
//!
//==This code has been autogenerated using Compgen==
//from 
/*{, C:, HopsanTrunk, HOPSAN++, CompgenModels}/SignalFFBDcomponents.nb*/

using namespace hopsan;

class SignalFFBloopIn : public ComponentSignal
{
private:
     int mNstep;
//==This code has been autogenerated using Compgen==
     //inputVariables
     double in0;
     double in1;
     //outputVariables
     double state;
     double exiting;
     //InitialExpressions variables
     double oldIn0;
     double oldIn1;
     double oldExiting;
     //Expressions variables
     double state0;
     double state1;
     double oldState0;
     double oldState1;
     //Delay declarations
//==This code has been autogenerated using Compgen==
     //inputVariables pointers
     double *mpin0;
     double *mpin1;
     //inputParameters pointers
     //outputVariables pointers
     double *mpstate;
     double *mpexiting;
     EquationSystemSolver *mpSolver;

public:
     static Component *Creator()
     {
        return new SignalFFBloopIn();
     }

     void configure()
     {
//==This code has been autogenerated using Compgen==

        mNstep=9;

        //Add ports to the component
        //Add inputVariables to the component
            addInputVariable("in0","Input 0","",0.,&mpin0);
            addInputVariable("in1","Input 1","",0.,&mpin1);

        //Add inputParammeters to the component
        //Add outputVariables to the component
            addOutputVariable("state","State activated","",0.,&mpstate);
            addOutputVariable("exiting","exiting to alt 0","",0.,&mpexiting);

//==This code has been autogenerated using Compgen==
        //Add constantParameters
     }

    void initialize()
     {
        //Read port variable pointers from nodes

        //Read variables from nodes

        //Read inputVariables from nodes
        in0 = (*mpin0);
        in1 = (*mpin1);

        //Read inputParameters from nodes

        //Read outputVariables from nodes
        state = (*mpstate);
        exiting = (*mpexiting);

//==This code has been autogenerated using Compgen==
        //InitialExpressions
        oldIn0 = in0;
        oldIn1 = in1;
        oldExiting = exiting;


        //Initialize delays

     }
    void simulateOneTimestep()
     {
        //Read variables from nodes

        //Read inputVariables from nodes
        in0 = (*mpin0);
        in1 = (*mpin1);

        //LocalExpressions

          //Expressions
          state0 = onPositive(-0.5 - onPositive(-0.5 + oldExiting) + \
2*onPositive(-0.5 + in0 - oldIn0) + onPositive(-0.5 + oldState0));
          state1 = onPositive(-0.5 - onPositive(-0.5 + oldExiting) + \
2*onPositive(-0.5 + in1 - oldIn1) + onPositive(-0.5 + oldState1));
          exiting = onPositive(-0.5 + oldState0 + oldState1);
          state = onPositive(-0.5 + onPositive(-0.5 + state0) + \
onPositive(-0.5 + state1));
          oldState0 = state0;
          oldState1 = state1;
          oldIn0 = in0;
          oldIn1 = in1;
          oldExiting = exiting;

        //Calculate the delayed parts


        //Write new values to nodes
        //outputVariables
        (*mpstate)=state;
        (*mpexiting)=exiting;

        //Update the delayed variabels

     }
    void deconfigure()
    {
        delete mpSolver;
    }
};
#endif // SIGNALFFBLOOPIN_HPP_INCLUDED
