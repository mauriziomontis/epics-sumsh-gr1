#!/bin/bash

MOTOR=GR1:Ax1_Mtr

#0) limits to arbitrary numbers
caput ${MOTOR}.LLM -6546546544
caput ${MOTOR}.HLM 654654654654

# Step 1: Go to zero position
caput ${MOTOR}.VAL 10000
sleep 1
while [[ $(caget -t ${MOTOR}.MOVN) == 1 ]]; do sleep 0.2; done

# Step 2: Set VAL to 0
caput ${MOTOR}.SET 1
caput ${MOTOR}.VAL 0
caput ${MOTOR}.SET 0

# move away from switch
caput ${MOTOR}.VAL -400
sleep 1
while [[ $(caget -t ${MOTOR}.MOVN) == 1 ]]; do sleep 0.2; done

# set new 0 (home) and limits
caput ${MOTOR}.SET 1
caput ${MOTOR}.VAL 0
caput ${MOTOR}.SET 0
caput ${MOTOR}.LLM -4000
caput ${MOTOR}.HLM 0

echo "Calibration complete. Max position: 4000 steps / 20 mm"