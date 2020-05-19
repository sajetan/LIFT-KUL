# LIFT
Implementation of Secure communication between Drone and Control Center. \
This project is undertaken at the Department of Electrical Engineering, KU Leuven, Belgium.

### Specifications: 
* **Key Establishment Protocol** : Station-to-Station Protocol
* **Encryption** : Symmetric key encryption using Chacha20 cipher
* **AEAD** : Chacha20-Poly1305 AEAD scheme by DJB
* **Signature** : ECDSA over elliptic curve P-256 by NIST
* **Hash** : SHA-256 Keccak Implementation


### Implementation Details: 
##### For source code: 
[LIFT/src] (https://github.com/sajetan/PnD-ICT-Security/tree/master/LIFT/src)

##### For code benchmarking: 
[LIFT/benchmark] (https://github.com/sajetan/PnD-ICT-Security/tree/master/LIFT/benchmark)

##### To build source code: 
cd LIFT/ --> make clean --> make 

Executables are **control_launcher** and **drone_launcher**, open two terminals and execute them. \
**control_launcher** - Control Center launcher, this is always the initiator. \
**drone_launcher** - Drone is always the responder. 

On executing **control_launcher**, an user can make the below choices: 
* ***Establish Drone Connection*** - choosing this, control center will initiate the key establishment protocol with the drone.
* ***Close the System*** - will exit the terminal

Below are a few choices when the *Key Establishment Phase Successful*: 
* ***Get Drone Status*** - sends a command request to drone to fetch its status. Status menu will be shown to select the type of satus (like GPS coordinates, Temperature, Battery Level etc) 
* ***Send Command to Drone*** - sends commands to drone to make control decisions. Control menu will be shown to select the type of control (like move left/right/up/down) 
* ***Start Video Stream from Drone*** - this will initiate the video stream from drone to the control center, in the current architecture, video stream is just a binary stream of data which the control center will receive and process in background in a separate thread (Control center can still communicate with the Drone while video is streaming in the background) 
* ***Terminate Video Stream from Drone*** - this will make a request to Drone to stop the video stream, in the end it displays the total number of frames received with the size.  
* ***Restart Session*** - this will reinitate the session. 
* ***Close the system*** - this will exit the terminal.


### NOTE: 
* For changing any performance related actions like enabling debug logs, or changing parameters, please change in *LIFT/src/config.h*
* For changing IP addresses and ports of control center and drone, change in *LIFT/src/control/control_launcher.c* and *LIFT/src/drone/drone_launcher.c*


**For more details contact:** 
* Ferdinand Hannequart (*ferdinand.hannequart@student.kuleuven.be*)
* Lien Wouters (*lien.wouters@student.kuleuven.be*)
* Tejas Narayana (*tejas.narayana@student.kuleuven.be*)
