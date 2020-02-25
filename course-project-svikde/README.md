# Home Automation using Bluetooth Mesh 
## ECEN 5823

## Team Members

  - [Souvik De](https://github.com/CU-ECEN-5823/course-project-svikde) (Author/Low Power Node - A)
  - [Isha Sawant](https://github.com/CU-ECEN-5823/course-project-IshaS27) (Low Power Node - B)
  - [Devansh Mittal](https://github.com/CU-ECEN-5823/course-project-devmittal) (Friend Node)

## About
### Overview
This project aims to automate a home by providing a level of convenience and security to the user. This project focuses on two pivotal areas of a home - the kitchen and the bedroom. In the kitchen, one node of the mesh is responsible for detecting fire and gas leakage. In case of a fire or gas leakage, a buzzer is sounded to alert the user to take action to prevent the fire/gas leakage. In the bedroom, a motion detector and a light intensity sensor on a low power node are used to automate the intensity of an LED located in the same room, thereby making a user’s life convenient as they do not have to set the intensity of the light manually.

### Sub-System Overview
My part of the project pertains to designing one of the low power nodes that would host the PIR Motion detector and the Luminosity or the Ambient Light sensor. As part of the Home Automation mesh, the low power node would detect the presence or absence of a user in a room. Additionally, it would also be able to detect the net flux in the room (natural light + Incandescent sources) using the ambient light sensor. The friend node would utilize this data to modulate incandescent light sources in the room for judicious utilization of energy and thereby save electricity. BLE services in the low power node would publish Motion Detection (On-Off) data and luminosity (level) values for the friend node.

### Sub-System High Level Design
The light & motion detection sensor would automate a system that would utilize energy with prudence in the absence of a user thereby decreasing monthly electricity bills. Such a framework could be extended to multifarious home appliances with robust sensing devices. This would allow users to enter and exit their apartments without the need to be concerned with the switching of common lighting and heating/cooling appliances. Succinctly, smart energy savings with user convenience. 

In addition to the Ambient Light & Motion Sensor, the embedded LCD & the flash memory of the system would also be utilized. The display would be used to display relevant data & states in the system. The flash memory will be used to store persistent sensor data to be later used to compare and transmit meaningful data to the friend node and keep track of local occurrences.


## Project Status / Summary
This low power node assumes the role of a smart energy-saving and monitoring system in a mesh-based home automation system. The goal was to design a low power node that could detect motion in a given room and send luminosity values from the room to automate light sources in the room controlled by the friend node. This would not only minimize energy utilization but would be becoming of a convenient and smart home automation network. 

Low Power Node-A hosts an ambient light and PIR motion detection sensor. The motion detection sensor is responsible for detecting the occupancy state of a given room. Depending on the motion values interpreted by the system, the light sensor calculates the light intensity in the room. The lumens thus detected are only published if they make sensible enough difference to modulate the light source at the friend node in the presence of a user. The on-board persistent storage is utilized for this purpose which serves as a reference for the system to publish the lumen values. The persistent storage also stores the number of times a room was occupied for statistical purposes. Using a non-volatile memory also makes the system smoother in exceptional cases, where sensor data might not be available. The system loads the last stored values from the flash on bootup to ensure smooth and continuous operation. 

All proposed deliverables have been accomplished as part of the final project submission with minor changes to how the flash memory was used and assimilated as part of the system. Changes were made to best fit the system utility and its intended purpose.

Code checked in to the repository consists of the following elements:
- I2C interrupt mechanism for Ambient Light Sensor.
- Integrated PIR Motion Detection Sensor.
- Ambient Light Sensor state machine with Load Power Management.
- Devised a custom filter to control sentivity of the PIR sensor.
- Devised a method to detect an empty room after a period of inactivity.
- Motion Data Published usign Generic On/Off Client Model.
- Lumen Data Published using Generic Level Client Model.
- Integrated on-borad Flash (Persistent) storage.
- Timestamp support in logging.
- LETIMER interrupt code.
- BLE Mesh client code (Low Power). 
- Updated LCD support.

## Google Drive Documentation
- [Group Folder](https://drive.google.com/drive/u/2/folders/1Xr8iJkr0jLGmn8_0hWulgEYgaV0E8OCy)
- [Individual Folder](https://drive.google.com/drive/u/2/folders/1CWqCTIoTYblcMowGw_Ub_jY9n1wHcCmE)
- [Validation Plan](https://drive.google.com/open?id=1eStyrzkKugRVDpPUWN5skwlgMMWItw95KXlyspTW2zY)

