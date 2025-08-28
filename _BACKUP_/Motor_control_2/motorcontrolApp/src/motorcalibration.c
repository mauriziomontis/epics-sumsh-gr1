#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <subRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>

long initCalibrate(subRecord *prec) {
    return 0;  // No initialization needed
}

long runCalibrate(subRecord *prec) {
    int status = system("/home/iocadm/workspace/Motor_control_1/motorcontrolApp/src/calibration.sh");

    if (status == -1) {
        printf("Failed to execute script.\n");
        return -1;
    } else {
        printf("Calibration script returned %d\n", status);
        return 0;
    }
}

// Register the functions
epicsRegisterFunction(runCalibrate);
epicsRegisterFunction(initCalibrate);