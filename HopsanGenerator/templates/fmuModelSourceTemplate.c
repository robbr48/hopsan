// Define class name and unique id
    #define MODEL_IDENTIFIER <<<0>>>
    #define MODEL_GUID "<<<1>>>"

    // Define model size
    #define NUMBER_OF_REALS <<<2>>>
    #define NUMBER_OF_INTEGERS 0
    #define NUMBER_OF_BOOLEANS 0
    #define NUMBER_OF_STRINGS 0
    #define NUMBER_OF_STATES <<<3>>>
    #define NUMBER_OF_EVENT_INDICATORS 0

    // Include fmu header files, typedefs and macros
    #include "fmuTemplate.h"
    #include "HopsanFMU.h"

    // Define all model variables and their value references
>>>4>>>   #define <<<varname>>>_ <<<varref>>>
<<<4<<<
    // Define state vector as vector of value references
    #define STATES { <<<5>>> }

    //Set start values
    void setStartValues(ModelInstance *comp) 
    {
        initializeHopsanWrapperFromBuiltInModel();
>>>6>>>        r(<<<varname>>>_) = 0;
<<<6<<<    }

    //Initialize
    void initialize(ModelInstance* comp, fmiEventInfo* eventInfo)
    {
        eventInfo->upcomingTimeEvent = fmiTrue;
        eventInfo->nextEventTime = comp->time;
    }

    //Return variable of real type
    fmiReal getReal(ModelInstance* comp, fmiValueReference vr)
    {
        switch (vr) 
       {
<<<8>>>           default: return 1;
       }
    }

    void setReal(ModelInstance* comp, fmiValueReference vr, fmiReal value)
    {
        switch (vr) 
       {
<<<9>>>           default: return;
       }
    }

    //Update at time event
    void eventUpdate(ModelInstance* comp, fmiEventInfo* eventInfo)
    {
        while(comp->time > getFmuTime() + 0.5*getTimeStep())
            simulateOneStep();
        eventInfo->upcomingTimeEvent = fmiTrue;
        eventInfo->nextEventTime = comp->time+getTimeStep();
    }

    // Include code that implements the FMI based on the above definitions
    #include "fmuTemplate.c"
