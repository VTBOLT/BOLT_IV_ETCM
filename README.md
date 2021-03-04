# BOLT_IV_ETCM TestBench

This TestBench is made to read in values from a file, then send it to the C2000. The C2000 has code
to model each of the inputs to the ETCM using various peripherals on the F28379D board.

To setup your system to run the python script to interface with the C2000, you will need the following

        - pip install pyserial
        - Figure out what COM port the C2000 is on using device manager or another tool

To create a CSV for inputting into the C2000, follow the list of columns below
 (There's no limit to the number of rows you can have)

        - Front Wheel Speed (RPM, 3 Digits, 0 - 459)
        - Back Wheel Speed (RPM, 3 Digits, 0 - 459)
        - Front Wheel Suspension (cm, 3 Digits, 0 - 150) -> This only goes up to 3V
        - Back Wheel Suspension (cm, 3 Digits, 0 - 150) -> This only goes up to 3V
        - Front Brake Switch 1 (1 or 0)
        - Back Brake Switch 2 (1 or 0)
        - Profile Switch 1 (1 or 0)
        - Profile Switch 2 (1 or 0)
        - Profile Switch 3 (1 or 0)
        - Throttle Switch (1 or 0)
        - Yaw Calibration (999.99 - 0) (Need to find units and range)
        - Pitch Calibration (999.99 - 0) (Need to find units and range)
        - Roll Calibration (999.99 - 0) (Need to find units and range)
        - Yaw (999.99 - 0) (Need to find units and range)
        - Pitch (999.99 - 0) (Need to find units and range)
        - Roll (999.99 - 0) (Need to find units and range)

Once you have a CSV, start up the CCS project on the C2000 and start up the Python Script
BOLT_TestBench_Input.py and follow the prompts. (COM port, and input file).

The script will then send serial values to the C2000 and wait for a return value. This data will
print the values sent to the C2000 with the value received from the C2000 below it to a file named
**BOLT_TestBench_Output.txt**

Note: For some reason if you stop the python script in the middle of execution, you have to reflash
to the board before running again or the code will hang (I think because the UART buffer is still)
waiting. Let the entire python script run and you shouldn't have any issues

**The pin mappings are in the main file next to the sensor or switch that they model**
