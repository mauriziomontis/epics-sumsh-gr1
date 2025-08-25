# CAEN ELS Easy Driver - EPICS driver


Version 0.1.35

This software is compatible with:

- Easy Driver

The driver is inspired on the SY3634 EPICS driver (A36xx Norum driver).



## Configuration:

Modify the **configure/RELEASE** file and put the correct ASYN and EPICS_BASE paths.

The script was tested with:

- **base 3.14.12.5** and 
- **asyn 4.18**

To compile the driver, execute the **make** command from the top folder.



To configure the IP of the connected Easy Driver, edit the **./st.cmd ** from the iocBoot/iocEasyDriverTest folder. Now it is possible to execute the **./st.cmd** script  to run the easy driver ioc.

