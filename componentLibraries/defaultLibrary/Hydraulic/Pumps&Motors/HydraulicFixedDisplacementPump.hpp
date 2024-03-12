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
//! @file   HydraulicFixedDisplacementPump.hpp
//! @author Robert Braun <robert.braun@liu.se>
//! @date   2010-01-07
//!
//! @brief Contains a hydraulic pump with fixed displacement component
//!
//$Id$

#ifndef HYDRAULICFIXEDDISPLACEMENTPUMP_HPP_INCLUDED
#define HYDRAULICFIXEDDISPLACEMENTPUMP_HPP_INCLUDED

#include <iostream>
#include <cmath>
#include "ComponentEssentials.h"

namespace hopsan {

    //!
    //! @brief
    //! @ingroup HydraulicComponents
    //!
    class HydraulicFixedDisplacementPump : public ComponentQ
    {
    private:
        double *mpND_p1, *mpND_q1, *mpND_Qdot1, *mpND_c1, *mpND_Zc1, *mpND_T1, *mpND_p2, *mpND_q2, *mpND_Qdot2, *mpND_c2, *mpND_Zc2, *mpND_T2, *mpND_a;
        double *mpN, *mpDp, *mpClp, *mpEtahm;
        Port *mpP1, *mpP2;
        double rho, cp;

    public:
        static Component *Creator()
        {
            return new HydraulicFixedDisplacementPump();
        }

        void configure()
        {
            addConstant("rho", "Density", "kg/m^3", 880, rho);
            addConstant("cp", "Specific heat capacity", "J/kgK", 1670, cp);

            mpP1 = addPowerPort("P1", "NodeHydraulic");
            mpP2 = addPowerPort("P2", "NodeHydraulic");

            addOutputVariable("a", "Angle", "Angle", 0.0, &mpND_a);
            addInputVariable("n_p", "Angular Velocity", "AngularVelocity", 250.0, &mpN);
            addInputVariable("D_p", "Displacement", "Displacement", 0.00005, &mpDp);
            addInputVariable("C_lp", "Leakage coefficient", "LeakageCoefficient", 1e-12, &mpClp);
            addInputVariable("eta_hm", "Hydromechanical efficiency", "", 0.9, &mpEtahm);
        }


        void initialize()
        {
            mpND_p1 = getSafeNodeDataPtr(mpP1, NodeHydraulic::Pressure);
            mpND_q1 = getSafeNodeDataPtr(mpP1, NodeHydraulic::Flow);
            mpND_Qdot1 = getSafeNodeDataPtr(mpP1, NodeHydraulic::HeatFlow);
            mpND_c1 = getSafeNodeDataPtr(mpP1, NodeHydraulic::WaveVariable);
            mpND_Zc1 = getSafeNodeDataPtr(mpP1, NodeHydraulic::CharImpedance);
            mpND_T1 = getSafeNodeDataPtr(mpP1, NodeHydraulic::Temperature);

            mpND_p2 = getSafeNodeDataPtr(mpP2, NodeHydraulic::Pressure);
            mpND_q2 = getSafeNodeDataPtr(mpP2, NodeHydraulic::Flow);
            mpND_Qdot2 = getSafeNodeDataPtr(mpP2, NodeHydraulic::HeatFlow);
            mpND_c2 = getSafeNodeDataPtr(mpP2, NodeHydraulic::WaveVariable);
            mpND_Zc2 = getSafeNodeDataPtr(mpP2, NodeHydraulic::CharImpedance);
            mpND_T2 = getSafeNodeDataPtr(mpP2, NodeHydraulic::Temperature);

            (*mpND_a) = 0;
        }


        void simulateOneTimestep()
        {
            //Declare local variables
            double p1, q1, Qdot1, c1, Zc1, T1, p2, q2, Qdot2, c2, Zc2, T2;
            bool cav = false;

            double n = (*mpN);
            double dp = (*mpDp);
            double Clp = (*mpClp);

            //Get variable values from nodes
            c1 = (*mpND_c1);
            Zc1 = (*mpND_Zc1);
            T1 = (*mpND_T1);
            c2 = (*mpND_c2);
            Zc2 = (*mpND_Zc2);
            T2 = (*mpND_T2);

            //Fixed Displacement Pump equations
            q2 = ( dp*n/(2.0*pi) + Clp*(c1-c2) ) / ( (Zc1+Zc2)*Clp+1 );
            q2 = std::fmin(std::fmax(q2, -c2/Zc2), c1/Zc1); //Limit flow to indirectly limit pressures
            q1 = -q2;
            p2 = c2 + Zc2*q2;
            p1 = c1 + Zc1*q1;

            /* Cavitation Check */
            if (p1 < 0.0)
            {
                c1 = 0.0;
                Zc1 = 0.0;
                cav = true;
            }
            if (p2 < 0.0)
            {
                c2 = 0.0;
                Zc2 = 0.0;
                cav = true;
            }
            if (cav)
            {
                q2 = ( dp*n/(2.0*pi) + Clp*(c1-c2) ) / ( (Zc1+Zc2)*Clp+1 );

                p1 = c1 + Zc1 * q1;
                p2 = c2 + Zc2 * q2;
                if (p1 <= 0.0)
                {
                    p1 = 0.0;
                    q2 = std::min(q2, 0.0);
                }
                if (p2 <= 0.0)
                {
                    p2 = 0.0;
                    q2 = std::max(q2, 0.0);
                }
                q1 = -q2;
            }

            if(q2>0) {
                Qdot1 = q1*rho*cp*T1;
                Qdot2 = (q1/(*mpEtahm) - q1 + Clp*(p1-p2))*(p1-p2) - Qdot1;
            }
            else {
                Qdot2 = q2*rho*cp*T2;
                Qdot1 = (q2/(*mpEtahm) - q2 + Clp*(p2-p1))*(p2-p1) - Qdot2;
            }

            //Write new values to nodes
            (*mpND_p1) = p1;
            (*mpND_q1) = q1;
            (*mpND_p2) = p2;
            (*mpND_q2) = q2;
            (*mpND_Qdot1) = Qdot1;
            (*mpND_Qdot2) = Qdot2;
            (*mpND_a) += n*mTimestep;
        }
    };
}

#endif // HYDRAULICFIXEDDISPLACEMENTPUMP_HPP_INCLUDED
