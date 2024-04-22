# **2-Player Casino Game**
## **Overview**
This project is a 2-player casino game implemented on a Cypress programmable SoC using C programming language. The game utilizes an ADC (Analog-to-Digital Converter) and an LCD module for player input and output. Each player takes turns turning the ADC knob, which converts the player's move into a power move. If a player lands on a prime number, they hit a master move; otherwise, they make a normal move. To efficiently detect prime numbers, the Sieve of Eratosthenes competitive programming algorithm is used, reducing computation time. 

## **Features**
- 2-player casino game with power move mechanics.
- ADC for player input.
- LCD module for displaying game status and messages.
- Detection of prime numbers using the Sieve of Eratosthenes algorithm for efficient gameplay.

## **Requirements**
- Cypress programmable SoC development board.
- ADC module compatible with the development board.
- LCD module compatible with the development board.
- Development environment for programming the Cypress SoC (e.g., Cypress PSoC Creator).
- USB cable for programming and power supply.

## **Installation and Setup**
1. Clone or download the project repository from GitHub.
2. Open the project in Cypress PSoC Creator or your preferred development environment.
3. Compile the source code to generate the binary file.
4. Flash the binary file onto the Cypress programmable SoC.
5. Connect the necessary hardware components (ADC, LCD module) to the development board.
6. Power on the development board.

## **Flow Diagram**
![Flow Diagram](https://github.com/shantanu49001/EMB_PROJECTS_PSOC/blob/main/CYPRESS_PSOC_01CASINO/proj/flow.png)

<!-- Add space here for pin image -->

## **Pin Configuration**
![Pin Configuration](https://github.com/shantanu49001/EMB_PROJECTS_PSOC/blob/main/CYPRESS_PSOC_01CASINO/proj/PINS.png)

<!-- Add space here for schematic image -->

## **Schematic Diagram**
![Schematic Diagram](https://github.com/shantanu49001/EMB_PROJECTS_PSOC/blob/main/CYPRESS_PSOC_01CASINO/proj/Schematic.png)

<!-- Add space here for output images -->

## **Output Screenshots**
![Screenshot 1](https://github.com/shantanu49001/EMB_PROJECTS_PSOC/blob/main/CYPRESS_PSOC_01CASINO/proj/o1.jpg)
![Screenshot 2](https://github.com/shantanu49001/EMB_PROJECTS_PSOC/blob/main/CYPRESS_PSOC_01CASINO/proj/o2.jpg)
![Screenshot 3](https://github.com/shantanu49001/EMB_PROJECTS_PSOC/blob/main/CYPRESS_PSOC_01CASINO/proj/o3.jpg)
![Screenshot 4](https://github.com/shantanu49001/EMB_PROJECTS_PSOC/blob/main/CYPRESS_PSOC_01CASINO/proj/o4.jpg)

## **Usage**
1. Power on the Cypress programmable SoC.
2. The LCD module will display the game status and messages.
3. Players take turns turning the ADC knob to make their moves.
4. If a player lands on a prime number, they hit a master move; otherwise, they make a normal move.
5. The game continues until a player reaches a predetermined winning condition or until the game is manually stopped.
6. Enjoy playing the 2-player casino game!
