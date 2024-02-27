#ifndef HEATFLOWCALCULATOR_H
#define HEATFLOWCALCULATOR_H

class HeatFlowCalculator
{
public:
    inline void calculateHeatFlows(double p1, double q1, double T1, double &Qdot1, double p2, double q2, double T2, double &Qdot2) const
    {
        if(q2>0) {
            Qdot1 = q1*mrho*mcp*T1;
            Qdot2 = (p1-p2)*q2 - Qdot1;
        }
        else {
            Qdot2 = q2*mrho*mcp*T2;
            Qdot1 = (p2-p1)*q1 - Qdot2;
        }
    }

    inline void appendHeatFlows(double p1, double q1, double T1, double &Qdot1, double p2, double q2, double T2, double &Qdot2) const
    {
        if(q2>0) {
            Qdot1 += q1*mrho*mcp*T1;
            Qdot2 += (p1-p2)*q2 - q1*mrho*mcp*T1;
        }
        else {
            Qdot2 += q2*mrho*mcp*T2;
            Qdot1 += (p2-p1)*q1 - q2*mrho*mcp*T2;
        }
    }

    inline void setDensity(const double rho)
    {
        mrho = rho;
    }

    inline void setHeatCapacity(const double cp)
    {
        mcp = cp;
    }


private:
    double mrho = 880;
    double mcp = 1670;
};

#endif // HEATFLOWCALCULATOR_H
