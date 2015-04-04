# Introduction #

This Wiki outlines the 2010 HLO's, SR's and roles that the project must meet to satisfy the objectives.


# Details #

## High Level Objectives ##

> ### HLO-1 Platform ###

> A platform should be developed and maintained to facilitate flight and on board hardware intergration.

> ### HLO-2 Localisaton ###

> The system should be capable of determining its position with the aid of image processing within an indoor environment to an appropriate time resolution.

> ### HLO-3 State Estimation ###

> A method of estimating the states of the helicopter system should be designed and implemented. The resolution of the estimations should faciliate their employment in the control system design.

> ### HLO-4 Autonomous Hovering Flight ###

> An autopilot system should be developed to enable sustained indoor autonomous hovering flight. The control system should be designed to enable future ingress and egress manerourvers to longitudinal and hovering flight.

> ### HLO-5 Ground Control Station ###

> A ground control station that supports appropriate command and system setting input and data display and logging should be developed. The design should be derived from previous AHNS developments and enable future ground station developments.

> ### HLO-6 Communications ###

> The communications system should enable transfer of control, state and localisation data to the ground control station. It should provide with a flexible wireless datalink available on consumer-electronic devices.


---


## System Requirements ##

| **System Requirement** | **Definition** | **Acceptance Test** |
|:-----------------------|:---------------|:--------------------|
| SR-B-01 | The platform shall have the ability to be manually manoeuvred with a radio controller | AT-01 |
| SR-B-02 | The GCS shall enable autopilot flight mode switching between manual, stability augmented flight, and  autonomous station keeping. | AT-02 |
| SR-B-03 | The airborne system shall provide control updates at a minimum rate of 50Hz | AT-03 |
| SR-B-04 | The estimator shall provide Euler angle and rate estimation for the system at minimum rate of 50 Hz. | AT-04 |
| SR-B-05 | The estimator shall provide altitude estimation for the system at minimum rate of 50 Hz.  | AT-05 |
| SR-B-06 | The estimator shall provide x and y estimation in an Earth fixed co-ordinate system at minimum rate of 50 Hz. | AT-06 |
| SR-B-07 | The system shall use image processing to aid in state estimation of x and y in an Earth fixed co-ordinate system. | AT-07 |
| SR-B-08 | The autopilot system gain and reference parameters shall be updatable in flight using an 802.11g WLAN uplink from the GCS | AT-08 |
| SR-B-09 | The airborne system shall transmit telemetry data including state data to the GCS using 802.11g WLAN | AT-09 |
| SR-B-10 | The autopilot control methodology shall be based on cascaded PID control loops. | AT-10 |
| SR-D-01 | The platform shall be capable of maintaining controlled flight with a total payload of 400 grams. | AT-11 |
| SR-D-02 | A maintenance document shall be used to log airframe flight time, battery cycles and aircraft repairs.  | AT-12 |
| SR-D-03 | The autopilot shall provide stability augmented flight. | AT-13 |
| SR-D-04 | The autopilot shall provide autonomous station keeping capability within a 1 meter cubed volume of a desired position | AT-14 |
| SR-D-05 | The airborne system shall receive and process measurement data from the state estimation and localisation sensors; supporting IMU, Camera, IR, Ultrasonic and Magnetic compass devices. | AT-15 |
| SR-D-06 | The airborne system shall collect avionics system health monitoring information in the form of radio control link status, flight mode status and battery level | AT-16 |
| SR-D-07 | The airborne system shall transmit all actuator inputs, including radio control inputs, to the GCS. | AT-17 |
| SR-D-08 | The GCS shall log all telemetry and uplink data communications | AT-18 |
| SR-D-09 | Aircraft state data and control inputs received shall be displayable on the GCS along with appropriate time references. | AT-19 |
| SR-D-10 | The GCS shall provide display of avionics system health monitoring including telemetry, uplink, radio control link and battery level status read-outs. | AT-20 |



---


## Roles ##

**Michael Hamilton**

  * Project Manager
    * Project Management plan
    * Risk assessment
    * Budget - money
  * Communication
    * Wifi interface with GCS and onboard

**Michael Kincel**

  * Pilot
  * Airframe
    * Landing gear
    * Avionics payload mounting
    * Weight budget
    * Hardware integration
    * Batteries
    * Sensor hardware mounting

**Tim Molloy**

  * Control
    * Modeling
    * Simulation
    * Control software implementation
  * GCS
    * Software GUI and onboard/offboard implementation

**Liam O'Sullivan**

  * Localisation
    * Computer Vision, camera selection, integration
  * State Estimation
    * IMU software implementation
    * Sensor software implementation
  * GCS
    * Software GUI and onboard/offboard implementation