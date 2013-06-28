#ifndef HYDRAULICCENTRIFUGALPUMP_HPP_INCLUDED
#define HYDRAULICCENTRIFUGALPUMP_HPP_INCLUDED

#include <iostream>
#include "ComponentEssentials.h"
#include "ComponentUtilities.h"
#include "math.h"

//!
//! @file HydraulicCentrifugalPump.hpp
//! @author Petter Krus <petter.krus@liu.se>
//! @date Fri 28 Jun 2013 13:04:38
//! @brief Centrifugal pump
//! @ingroup HydraulicComponents
//!
//==This code has been autogenerated using Compgen==
//from 
/*{, C:, HopsanTrunk, HOPSAN++, CompgenModels}/HydraulicComponents.nb*/

using namespace hopsan;

class HydraulicCentrifugalPump : public ComponentQ
{
private:
     double kl;
     double b;
     double d;
     double beta2;
     double Ap;
     double rho;
     double Kcp;
     double Bp;
     Port *mpP1;
     Port *mpP2;
     double delayParts1[9];
     double delayParts2[9];
     double delayParts3[9];
     double delayParts4[9];
     Matrix jacobianMatrix;
     Vec systemEquations;
     Matrix delayedPart;
     int i;
     int iter;
     int mNoiter;
     double jsyseqnweight[4];
     int order[4];
     int mNstep;
     //Port P1 variable
     double p1;
     double q1;
     double T1;
     double dE1;
     double c1;
     double Zc1;
     //Port P2 variable
     double p2;
     double q2;
     double T2;
     double dE2;
     double c2;
     double Zc2;
//==This code has been autogenerated using Compgen==
     //inputVariables
     double omegap;
     //outputVariables
     double q2e;
     double torp;
     double Pin;
     double Pout;
     //Expressions variables
     //Port P1 pointer
     double *mpND_p1;
     double *mpND_q1;
     double *mpND_T1;
     double *mpND_dE1;
     double *mpND_c1;
     double *mpND_Zc1;
     //Port P2 pointer
     double *mpND_p2;
     double *mpND_q2;
     double *mpND_T2;
     double *mpND_dE2;
     double *mpND_c2;
     double *mpND_Zc2;
     //Delay declarations
//==This code has been autogenerated using Compgen==
     //inputVariables pointers
     double *mpomegap;
     //inputParameters pointers
     double *mpkl;
     double *mpb;
     double *mpd;
     double *mpbeta2;
     double *mpAp;
     double *mprho;
     double *mpKcp;
     double *mpBp;
     //outputVariables pointers
     double *mpq2e;
     double *mptorp;
     double *mpPin;
     double *mpPout;
     Delay mDelayedPart10;
     Delay mDelayedPart20;
     EquationSystemSolver *mpSolver;

public:
     static Component *Creator()
     {
        return new HydraulicCentrifugalPump();
     }

     void configure()
     {
//==This code has been autogenerated using Compgen==

        mNstep=9;
        jacobianMatrix.create(4,4);
        systemEquations.create(4);
        delayedPart.create(5,6);
        mNoiter=2;
        jsyseqnweight[0]=1;
        jsyseqnweight[1]=0.67;
        jsyseqnweight[2]=0.5;
        jsyseqnweight[3]=0.5;


        //Add ports to the component
        mpP1=addPowerPort("P1","NodeHydraulic");
        mpP2=addPowerPort("P2","NodeHydraulic");
        //Add inputVariables to the component
            addInputVariable("omegap","Speed","rad/s",157.,&mpomegap);

        //Add inputParammeters to the component
            addInputVariable("kl", "Flow loss koeff.", "", 1.,&mpkl);
            addInputVariable("b", "outlet axial width", "m", 0.02,&mpb);
            addInputVariable("d", "Diameter", "m", 0.26,&mpd);
            addInputVariable("beta2", "Outlet flow angle", "rad", \
1.59,&mpbeta2);
            addInputVariable("Ap", "outlet flow area", "m2", 0.0004,&mpAp);
            addInputVariable("rho", "Fluid density", "kg/m2", 860,&mprho);
            addInputVariable("Kcp", "Leakage coeff", "m3/s/Pa", \
1.e-9,&mpKcp);
            addInputVariable("Bp", "Visc friction coeff", "N/m/s", 1.,&mpBp);
        //Add outputVariables to the component
            addOutputVariable("q2e","uncorrected flow","m3/s",0.,&mpq2e);
            addOutputVariable("torp","torque","Nm",0.,&mptorp);
            addOutputVariable("Pin","input power","W",0.,&mpPin);
            addOutputVariable("Pout","output power","W",0.,&mpPout);

//==This code has been autogenerated using Compgen==
        //Add constantParameters
        mpSolver = new EquationSystemSolver(this,4);
     }

    void initialize()
     {
        //Read port variable pointers from nodes
        //Port P1
        mpND_p1=getSafeNodeDataPtr(mpP1, NodeHydraulic::Pressure);
        mpND_q1=getSafeNodeDataPtr(mpP1, NodeHydraulic::Flow);
        mpND_T1=getSafeNodeDataPtr(mpP1, NodeHydraulic::Temperature);
        mpND_dE1=getSafeNodeDataPtr(mpP1, NodeHydraulic::HeatFlow);
        mpND_c1=getSafeNodeDataPtr(mpP1, NodeHydraulic::WaveVariable);
        mpND_Zc1=getSafeNodeDataPtr(mpP1, NodeHydraulic::CharImpedance);
        //Port P2
        mpND_p2=getSafeNodeDataPtr(mpP2, NodeHydraulic::Pressure);
        mpND_q2=getSafeNodeDataPtr(mpP2, NodeHydraulic::Flow);
        mpND_T2=getSafeNodeDataPtr(mpP2, NodeHydraulic::Temperature);
        mpND_dE2=getSafeNodeDataPtr(mpP2, NodeHydraulic::HeatFlow);
        mpND_c2=getSafeNodeDataPtr(mpP2, NodeHydraulic::WaveVariable);
        mpND_Zc2=getSafeNodeDataPtr(mpP2, NodeHydraulic::CharImpedance);

        //Read variables from nodes
        //Port P1
        p1 = (*mpND_p1);
        q1 = (*mpND_q1);
        T1 = (*mpND_T1);
        dE1 = (*mpND_dE1);
        c1 = (*mpND_c1);
        Zc1 = (*mpND_Zc1);
        //Port P2
        p2 = (*mpND_p2);
        q2 = (*mpND_q2);
        T2 = (*mpND_T2);
        dE2 = (*mpND_dE2);
        c2 = (*mpND_c2);
        Zc2 = (*mpND_Zc2);

        //Read inputVariables from nodes
        omegap = (*mpomegap);

        //Read inputParameters from nodes
        kl = (*mpkl);
        b = (*mpb);
        d = (*mpd);
        beta2 = (*mpbeta2);
        Ap = (*mpAp);
        rho = (*mprho);
        Kcp = (*mpKcp);
        Bp = (*mpBp);

        //Read outputVariables from nodes
        q2e = (*mpq2e);
        torp = (*mptorp);
        Pin = (*mpPin);
        Pout = (*mpPout);

//==This code has been autogenerated using Compgen==


        //Initialize delays

        delayedPart[1][1] = delayParts1[1];
        delayedPart[2][1] = delayParts2[1];
        delayedPart[3][1] = delayParts3[1];
        delayedPart[4][1] = delayParts4[1];
     }
    void simulateOneTimestep()
     {
        Vec stateVar(4);
        Vec stateVark(4);
        Vec deltaStateVar(4);

        //Read variables from nodes
        //Port P1
        T1 = (*mpND_T1);
        c1 = (*mpND_c1);
        Zc1 = (*mpND_Zc1);
        //Port P2
        T2 = (*mpND_T2);
        c2 = (*mpND_c2);
        Zc2 = (*mpND_Zc2);

        //Read inputVariables from nodes
        omegap = (*mpomegap);

        //LocalExpressions

        //Initializing variable vector for Newton-Raphson
        stateVark[0] = q2;
        stateVark[1] = torp;
        stateVark[2] = p1;
        stateVark[3] = p2;

        //Iterative solution using Newton-Rapshson
        for(iter=1;iter<=mNoiter;iter++)
        {
         //CentrifugalPump
         //Differential-algebraic system of equation parts

          //Assemble differential-algebraic equations
          systemEquations[0] =q2 - (onNegative(q2) + onPositive(p1))*(Kcp*(p1 \
- p2) + Ap*signedSquareL((2*(p1 - p2 + (omegap*rho*(0.25*b*Power(d,2)*omegap \
- 0.159155*q2*rho*Cot(beta2)))/b))/(kl*rho),10.));
          systemEquations[1] =(Power(Ap,2)*(Bp*omegap*(0.001 + 1.*omegap) + \
1.*Kcp*Power(p1,2) - 2.*Kcp*p1*p2 + 1.*Kcp*Power(p2,2) - 1.*p1*q2 + 1.*p2*q2) \
+ kl*(-0.5*Kcp*p1 + 0.5*Kcp*p2 + \
0.5*q2)*Power(q2,2)*rho)/(Power(Ap,2)*(-0.001 - 1.*omegap)) + torp;
          systemEquations[2] =p1 - lowLimit(c1 - q2*Zc1,0);
          systemEquations[3] =p2 - lowLimit(c2 + q2*Zc2,0);

          //Jacobian matrix
          jacobianMatrix[0][0] = 1 + \
(0.31831*Ap*omegap*rho*Cot(beta2)*dxSignedSquareL((2*(p1 - p2 + \
(omegap*rho*(0.25*b*Power(d,2)*omegap - \
0.159155*q2*rho*Cot(beta2)))/b))/(kl*rho),10.)*(onNegative(q2) + \
onPositive(p1)))/(b*kl);
          jacobianMatrix[0][1] = 0;
          jacobianMatrix[0][2] = -((Kcp + (2*Ap*dxSignedSquareL((2*(p1 - p2 + \
(omegap*rho*(0.25*b*Power(d,2)*omegap - \
0.159155*q2*rho*Cot(beta2)))/b))/(kl*rho),10.))/(kl*rho))*(onNegative(q2) + \
onPositive(p1)));
          jacobianMatrix[0][3] = -((-Kcp - (2*Ap*dxSignedSquareL((2*(p1 - p2 \
+ (omegap*rho*(0.25*b*Power(d,2)*omegap - \
0.159155*q2*rho*Cot(beta2)))/b))/(kl*rho),10.))/(kl*rho))*(onNegative(q2) + \
onPositive(p1)));
          jacobianMatrix[1][0] = (Power(Ap,2)*(0. - 1.*p1 + 1.*p2) + \
2*kl*(-0.5*Kcp*p1 + 0.5*Kcp*p2 + 0.5*q2)*q2*rho + \
0.5*kl*Power(q2,2)*rho)/(Power(Ap,2)*(-0.001 - 1.*omegap));
          jacobianMatrix[1][1] = 1;
          jacobianMatrix[1][2] = 0. + (Power(Ap,2)*(0. + 2.*Kcp*p1 - \
2.*Kcp*p2 - 1.*q2) + (0. - 0.5*Kcp)*kl*Power(q2,2)*rho)/(Power(Ap,2)*(-0.001 \
- 1.*omegap));
          jacobianMatrix[1][3] = 0. + (Power(Ap,2)*(0. - 2.*Kcp*p1 + \
2.*Kcp*p2 + 1.*q2) + (0. + 0.5*Kcp)*kl*Power(q2,2)*rho)/(Power(Ap,2)*(-0.001 \
- 1.*omegap));
          jacobianMatrix[2][0] = Zc1*dxLowLimit(c1 - q2*Zc1,0);
          jacobianMatrix[2][1] = 0;
          jacobianMatrix[2][2] = 1;
          jacobianMatrix[2][3] = 0;
          jacobianMatrix[3][0] = -(Zc2*dxLowLimit(c2 + q2*Zc2,0));
          jacobianMatrix[3][1] = 0;
          jacobianMatrix[3][2] = 0;
          jacobianMatrix[3][3] = 1;
//==This code has been autogenerated using Compgen==

          //Solving equation using LU-faktorisation
          mpSolver->solve(jacobianMatrix, systemEquations, stateVark, iter);
          q2=stateVark[0];
          torp=stateVark[1];
          p1=stateVark[2];
          p2=stateVark[3];
          //Expressions
          q1 = -q2;
          Pin = omegap*torp;
          Pout = (-p1 + p2)*q2;
        }

        //Calculate the delayed parts

        delayedPart[1][1] = delayParts1[1];
        delayedPart[2][1] = delayParts2[1];
        delayedPart[3][1] = delayParts3[1];
        delayedPart[4][1] = delayParts4[1];

        //Write new values to nodes
        //Port P1
        (*mpND_p1)=p1;
        (*mpND_q1)=q1;
        (*mpND_dE1)=dE1;
        //Port P2
        (*mpND_p2)=p2;
        (*mpND_q2)=q2;
        (*mpND_dE2)=dE2;
        //outputVariables
        (*mpq2e)=q2e;
        (*mptorp)=torp;
        (*mpPin)=Pin;
        (*mpPout)=Pout;

        //Update the delayed variabels

     }
    void deconfigure()
    {
        delete mpSolver;
    }
};
#endif // HYDRAULICCENTRIFUGALPUMP_HPP_INCLUDED
