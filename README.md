# EPICS Summer School - Magnetic Field Measurement - Group 1


## Tutor Information

**Tutors:** Maurizio Montis  
**E-Mail:** maurizio.montis@lnl.infn.it

## Participants


## Netwoek

LAN network: 172.30.84.0/24

Device:
- raspberry-pi: 172.30.84.235
- power supply: 172.30.84.111
- motor driver: 172.30.84.151




## Project Description

In this laboratory, students will develop and implement a control system to measure the magnetic field produced by a steerer, a critical component of a linear accelerator. Understanding and precisely controlling the magnetic field is essential for the optimal operation of the accelerator, as it directly impacts the trajectory and focus of the particle beam. These systems consist of electromagnets, called steerers, that generate magnetic fields to adjust the beam’s path.

By fine-tuning the magnetic field, the steerer ensures the beam remains on its intended course, thereby preventing collisions with the accelerator walls and ensuring that the particles reach their target with the desired energy and precision.

The laboratory setup for this exercise includes a steerer composed of four coils, a high current power supply remotely controlled via Ethernet, a teslameter for measuring the magnetic field also remotely controlled via Ethernet, and a stepper motor coupled with a motor driver. The motor driver is already equipped with an EPICS IOC, which facilitates integration and automation. The stepper motor is responsible for moving a magnetic field sensor through a dedicated kinematic transmission system, allowing precise spatial measurements of the magnetic field.

Students are tasked with creating a control system to automate the magnetic field measurements using the provided equipment. This involves programming the power supply to adjust the current through the steerer coils, interfacing with the teslameter to record magnetic field data, and coordinating the stepper motor to move the magnetic field sensor systematically. Through this exercise, students will gain practical experience in control systems, data acquisition, and the integration of various laboratory instruments, all essential skills in accelerator physics and engineering.

### Required Components

#### Hardware components

- 1× Steerer (2 coils – horizontal OR vertical)  
- 1× High Current Power Supply 10 V / 20 A  
- 1× Teslameter  (simulated with Raspberry Pi)
- 1× Magnetic Field Sensor (Hall Sensor)  
- 1× Motor Driver  
- 1× Stepper Motor  
- 1× Kinematic transmission  
- 1× Network switch  
- (optional) 1× USB-to-RS232c converter  

#### Software components

- VM and EPICS toolkit  
- Phoebus application  
- BlueSky and Ophyd applications  
- Server with VxWorks OS and VSFTP service (for the motor driver)  

## Project Objectives

- Demonstrate correct communication between hardware components  
- Configure and test VME-based and EPICS IOC for motor drivers  
- Verify the kinematic transmission with the stepper motor and the mechanical setup  
- Implement EPICS IOC for the power supply and the teslameter  
- Create the proper device support to interface the power supply and teslameter  
  - **Teslameter:**  
    - Enable / disable measurements  
    - Read **magnet filed** (in Gauss)  
  - **Power supply:**  
    - enable power supply command and readback  
    - current setpoint command  
    - current and voltage readbacks  
    - (optional) power supply fault status  
- Create Phoebus Control Panels devoted to supervising the entire setup  
- Describe management and organization strategy  
- Map the setup in a BlueSky service and define proper experimental operations  

## Task Structure

| Responsible   | Topic                              |
|---------------|------------------------------------|
| Student Name  | Hardware setup                     |
| Student Name  | EPICS layer                        |
| Student Name  | Phoebus Screen and Archiver        |
| Student Name  | Ophyd + Bluesky                    |

### Tasks Description

#### Hardware Setup

The experiment requires a dedicated LAN network to work properly: every device communicates via an Ethernet port.

The principal characteristics of every element are indicated below:

**Power Supply:**  
- Electrical connections with the steerer  
- Communication via Ethernet link  
- Serial and network communication setup via the web interface is required (requires info authentication to the tutors)  

**Teslameter:**  
- Connection to the magnetic field sensor  
- Communication via Ethernet link  
- Network communication setup is required  

**Motor Driver:**  
- Electrical connection with the stepper motor (DB25 to DB15 transition required)  
- Network connection via Ethernet link; network configuration at OS level is required and done via serial communication (USB-to-RS232 converter required)  

**Network Switch:**  
- Devices properly connected in the LAN  

**Stepper Motor:**  
- Electrical connection with the motor driver (DB15 to DB25 transition required)  
- Kinematic transmission required to convert rotational movement into longitudinal movement  
- Limit switches required to prevent dangerous sensor positions  

#### EPICS Layer

Every single device must be interfaced in EPICS, except for the motor driver, which already provides an IOC. There must be two IOCs: one for the power supply and one for the teslameter. Each IOC requires dedicated device support to interface the hardware in the EPICS environment.

**Power Supply:**  
- Device support with `streamDevice`  
- Use commands from the manual  
- Controlled in current; appropriate EPICS records must be implemented for control and readbacks  
- (Optional) Provide device diagnostics at the EPICS level  

**Teslameter:**  
- Device support with `streamDevice`  
- Use commands from the python code inside Raspberry Pi  
- Provides measurements of the magnetic field in three directions; proper EPICS records must be established  

**Motor Driver:**  
- IOC already provided; analyze the list of PVs and identify key candidates for control  

**softIOC (optional):**  
- An additional soft IOC to provide data and calculations required by the GUI (e.g., plot data) can be implemented if suitable for the control system architecture  

#### Phoebus Screen and Additional Services

The control system’s GUI consists of several panels displaying various information and controls:

**Main OPI must include:**  
- Information about principal devices (power supply, teslameter, motor driver) such as serial number, firmware version, etc., if available  
- Main parameters (commands and readbacks) for each device, with both sliders and text input for analog commands; specify logic placement (IOC level or Phoebus level)  
- Navigation buttons to open sub-panels; choose positions and behavior of buttons  
- A graph representing the magnetic field distribution based on acquired measurements  

**Sub-panels:**  
- Power supply details (complete control with commands, readbacks, diagnostics, etc.)  
- Teslameter details  
- Stepper motor details (based on PVs from IOC)  

**Color coding using LEDs/text (as needed):**  
- Green: enabled / OK  
- Yellow / Orange: warning  
- Red: error / fault  

#### Ophyd + Bluesky

Create an Ophyd device for the teslameter, motor axis, and power supply. Each device must include the appropriate variables and data required to prepare the following Bluesky experiment.

**Bluesky experiment stages:**

- **Initialization:**  
  - Set home position for motor axis  
  - Zero-reset for sensor probe  

- **Measurement 1:**  
  - Set a current for the power supply  
  - Scan procedure to acquire measurements at different axis positions  

- **Measurement 2:**  
  - Set a motor position for the probe  
  - Scan procedure to acquire measurements at different power supply current setpoints  

## Final Presentation

Prepare a slideshow presentation, approximately **45 minutes** long. Each person responsible (as defined in the Task Structure) must present their work in detail. At the end of the presentation, the group must demonstrate a working demo of their project.

