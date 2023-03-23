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
//! @file   InterpolationTable.h
//! @author Robert Braun <robert.braun@liu.se>
//! @date   2022-04-04
//!
//! @brief Contains an interpolation table utility based on a circular buffer
//!
//$Id$

#ifndef INTERPOLATIONTABLE_H
#define INTERPOLATIONTABLE_H

#include "win32dll.h"
#include <stddef.h>
#include <stdio.h>
#include <vector>
#if __cplusplus >= 201103L
#include <mutex>
#endif

namespace hopsan {

class HOPSANCORE_DLLAPI InterpolationTable
{
public:
    void initialize(const double windowLength, const double startTime, const double initValue);
    void append(const double time, const double value);
    double interpolate(const double time);
    void clear();

private:
    size_t mBaseSize, mNewest, mOldest;
    double mWindowLength;
    std::vector<double> mTime;
    std::vector<double> mData;
#if __cplusplus >= 201103L
    std::mutex mMutex;
#endif
};

}

#endif // INTERPOLATIONTABLE_H
