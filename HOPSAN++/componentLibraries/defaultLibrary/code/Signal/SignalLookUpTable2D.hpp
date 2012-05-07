/*-----------------------------------------------------------------------------
 This source file is part of Hopsan NG

 Copyright (c) 2011 
    Mikael Axin, Robert Braun, Alessandro Dell'Amico, Björn Eriksson,
    Peter Nordin, Karl Pettersson, Petter Krus, Ingo Staack

 This file is provided "as is", with no guarantee or warranty for the
 functionality or reliability of the contents. All contents in this file is
 the original work of the copyright holders at the Division of Fluid and
 Mechatronic Systems (Flumes) at Linköping University. Modifying, using or
 redistributing any part of this file is prohibited without explicit
 permission from the copyright holders.
-----------------------------------------------------------------------------*/

//!
//! @file   SignalLookUpTable2D.hpp
//! @author Björn Eriksson <bjorn.eriksson@liu.se>
//! @date   2010-11-03
//!
//! @brief Contains a Look Up Table 2D
//!
//$Id$

#ifndef SIGNALLOOKUPTABLE2D_HPP_INCLUDED
#define SIGNALLOOKUPTABLE2D_HPP_INCLUDED

#include "ComponentEssentials.h"
#include "ComponentUtilities.h"
#include <algorithm>
#include <sstream>

namespace hopsan {

    //!
    //! @brief The data csv file should be  on , separated form or ; separated form
    //!
    //! @ingroup SignalComponents
    //!
    class SignalLookUpTable2D : public ComponentSignal
    {

    private:
        Port *mpIn, *mpOut;
        double *mpND_in, *mpND_out;

        std::string mDataCurveFileName;
        CSVParser *myDataCurve;

    public:
        static Component *Creator()
        {
            return new SignalLookUpTable2D();
        }

        SignalLookUpTable2D() : ComponentSignal()
        {
            mpIn = addReadPort("in", "NodeSignal", Port::NOTREQUIRED);
            mpOut = addWritePort("out", "NodeSignal", Port::NOTREQUIRED);

            mDataCurveFileName = "File name";
            registerParameter("filename", "Abs. path to data curve", "", mDataCurveFileName);

            myDataCurve = 0;
        }


        void initialize()
        {
            bool success=false;
            if (myDataCurve!=0)
            {
                delete myDataCurve;
                myDataCurve=0;
            }

            myDataCurve = new CSVParser(success, mDataCurveFileName);
            if(!success)
            {
                std::stringstream ss;
                ss << "Unable to initialize CSV file: " << mDataCurveFileName << ", " << myDataCurve->getErrorString();
                addErrorMessage(ss.str());
                stopSimulation();
            }
            else
            {
                success = success && myDataCurve->isInDataOk(0);
                if(!success)
                {
                    std::stringstream ss;
                    ss << "Unable to initialize CSV file: " << mDataCurveFileName << ", " << myDataCurve->getErrorString();
                    addErrorMessage(ss.str());
                    stopSimulation();
                }
            }

            if(success)
            {
                //                std::stringstream ss;
                //                ss << mGain << "  " << myDataCurve->interpolate(mGain);
                //                addInfoMessage(ss.str());

                mpND_in = getSafeNodeDataPtr(mpIn, NodeSignal::VALUE, 0);
                mpND_out = getSafeNodeDataPtr(mpOut, NodeSignal::VALUE);
            }
        }


        void simulateOneTimestep()
        {
//            (*mpND_out) = myDataCurve->interpolate_old(*mpND_in, 1);
//            (*mpND_out) = myDataCurve->interpolate(*mpND_in, 1);
//            (*mpND_out) = myDataCurve->interpolateInc(*mpND_in, 1);
            (*mpND_out) = myDataCurve->interpolateIncSubDiv(*mpND_in, 1);
        }

        void finalize()
        {
            //! @todo actually this is only needed in destructor to cleanup
            //Cleanup data curve
            if (myDataCurve!=0)
            {
                delete myDataCurve;
                myDataCurve=0;
            }
        }
    };
}

#endif // SIGNALLOOKUPTABLE2D_HPP_INCLUDED
