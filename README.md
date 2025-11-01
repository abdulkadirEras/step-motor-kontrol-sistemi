2.4 Inch Dwin HMI Display Epressif firmasının ESP32 denetleyicisi kullanarak 7 tane step motoru saat yönüne/tersine 
ve motorun hızını kontrol etmeye yarayan sistem tasarlanmıştır.

Sistemin yazılımı Cpp ile donanım tasarımı KiCad’ de tasarlanmıştır.

Kullanıcı ara yüzü ile operatör manuel olarak kontrol edebildiği gibi operatörün ayarladığı sürelerde istediği motor 
da otomatik çalışabiliyor.

Sistemde güç butonları step motor sürücüleri aktif veya pasif yapılabilir. Aktif veya pasif durumları 
EEPROM’ da kaydediliyor.

Ekrandaki Slide Bar ile Step Motorların hızları ayarlanabilir.
Record butonu ile de operatör motorların çalışma sürelerini ve motor dönüş yönlerini kaydederek otomatik olarak çalıştırabilir.

Operatörün kaydettiği çalışma şeklini soldaki ekrandan play butonuna dokunarak pause butonuna basana kadar çalıştırabilir. 

Acil stop butonuna operatör bastığında ise sistem tüm step motorlarını durdurur.

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
A control system was designed using a 2.4 Inch Dwin HMI Display and an Espressif ESP32 microcontroller to regulate the speed and direction (clockwise/counter-clockwise) of seven stepper motors.

The system's software was developed using C++, and the hardware was designed in KiCad.

Via the user interface, the operator can not only control the motors manually but also enable automatic operation for specific motors at durations set by the operator.

The system includes power buttons that allow the stepper motor drivers to be actively turned on or off. Their active/inactive status is saved in EEPROM.

Motor speeds can be adjusted using the Slide Bar on the display. Furthermore, the Record button allows the operator to save custom sequences of motor run times and rotation directions for automatic playback.

The operator can execute the recorded sequence by tapping the Play button on the left side of the screen, and the sequence will run until the Pause button is pressed.

If the operator presses the Emergency Stop button, the system immediately halts all stepper motors.

---
# Step Motor Kontrol Sistemi

## Description

This project implements a system for controlling up to 7 stepper motors using a 2.4-inch DWIN HMI display and an ESP32 controller. The system allows for manual control of the motors (clockwise/counter-clockwise direction, speed) as well as the ability to record and automatically execute pre-defined motor sequences with specific durations and directions for each motor.

This project was designed using C++ for the software, and KiCad for the hardware design.

## Key Features & Benefits

*   **DWIN HMI Integration:** User-friendly interface for real-time control and monitoring of stepper motors.
*   **Manual Control:** Operators can manually control each motor's direction and speed.
*   **Automated Sequences:** The system allows users to record motor movements (direction and duration) and play them back automatically.
*   **Independent Motor Control:** Each of the 7 motors can be controlled independently.
*   **Power Control:** Individual power buttons to activate/deactivate stepper motor drivers.
*   **ESP32 Based:** Leverages the powerful ESP32 for real-time control and communication.

## Technologies

*   **Language:** C++
*   **Hardware:** ESP32, 2.4 inch DWIN HMI Display
*   **Design Software:** KiCad (for hardware design)

## Project Structure

```
├── README.md
└── step-motor-kontrol-sistemi/
    ├── include/
    │   └── README
    ├── lib/
    │   └── README
    ├── platformio.ini
    ├── readme.txt
    └── resimler/
        ├── IMG_20240515_142348.jpg
        ├── IMG_20240515_142356.jpg
        ├── IMG_20240515_142402.jpg
        ├── IMG_20240515_142434.jpg
        ├── IMG_20240515_142437.jpg
        ├── IMG_20240515_142443.jpg
        ├── IMG_20240515_142459.jpg
        ├── IMG_20240515_142505.jpg
        ├── IMG_20240515_142507.jpg
    └── src/
        └── main.cpp
```

## Prerequisites & Dependencies

*   **Arduino IDE or PlatformIO:** Recommended to use PlatformIO IDE for building and uploading the code.
*   **ESP32 Board Support:** Ensure you have the ESP32 board support installed in your Arduino IDE or PlatformIO.
*   **DWIN HMI Library:** This project likely depends on a specific library for communicating with the DWIN HMI display. Please check the code (especially `main.cpp`) and ensure you have the necessary library installed. (Specific library name to be filled after code review)
*   **Stepper Motor Driver Library:** A library for controlling stepper motors based on the driver being used. (Specific library name to be filled after code review)
*   **KiCad:** (Optional) If you want to modify the hardware design.

## Installation & Setup Instructions

1.  **Clone the Repository:**

    ```bash
    git clone https://github.com/abdulkadirEras/step-motor-kontrol-sistemi.git
    cd step-motor-kontrol-sistemi
    ```

2.  **Install Dependencies:**

    *   **Using PlatformIO:** Open the project in PlatformIO IDE and it should automatically install the dependencies specified in the `platformio.ini` file.

    *   **Using Arduino IDE:** Open the project in Arduino IDE, and install the required libraries (DWIN HMI Library, Stepper Driver Library) using the Library Manager (`Sketch > Include Library > Manage Libraries`).

3.  **Configure `platformio.ini` (if using PlatformIO):**

    *   Ensure the `platformio.ini` file is configured correctly for your ESP32 board and upload speed.  For example:
        ```ini
        [env:esp32dev]
        platform = espressif32
        board = esp32dev
        framework = arduino
        upload_speed = 921600
        lib_deps =
           # Add any necessary libraries here, for example:
           # adafruit/Adafruit GFX Library @ ^1.11.0
        ```
    *   Add missing libraries if prompted by the compiler.

4.  **Upload the Code:**

    *   **Using PlatformIO:** Connect your ESP32 board to your computer and click the "Upload" button in PlatformIO IDE.
    *   **Using Arduino IDE:** Select the correct board and port in the Arduino IDE (`Tools > Board` and `Tools > Port`), then click the "Upload" button.

5.  **Connect the Hardware:**

    *   Connect the DWIN HMI display to the ESP32 according to the project's schematic (details will be required based on the schematic, such as which GPIO pins are connected for Serial communication).
    *   Connect the stepper motor drivers to the ESP32 according to the project's schematic (details will be required, such as which GPIO pins are connected to enable, direction, and step pins on the stepper drivers).
    *   Connect the stepper motors to the stepper motor drivers.
    *   Connect the power supply to the ESP32 and stepper motor drivers.

## Usage Examples & API Documentation

Detailed API documentation is not available at this time. However, the `main.cpp` file contains the core logic for controlling the stepper motors and communicating with the DWIN HMI display.  Further examination of the code is needed to fully understand the API for specific libraries that are used.

**Example Snippet (Illustrative - Actual code may vary):**

```c++
// Example of setting the direction and speed of a stepper motor
// (Requires actual library details to be added)

// stepperMotor1.setDirection(CLOCKWISE);
// stepperMotor1.setSpeed(500); // steps per second
// stepperMotor1.step(200);     // move 200 steps
```

## Configuration Options

The project likely has configurable settings within the code. Some possible configuration options may include:

*   **Stepper Motor Pin Assignments:** The GPIO pins used to control the stepper motor drivers can be configured in the code.
*   **DWIN HMI Communication Settings:** The baud rate and other communication parameters for the DWIN HMI display.
*   **Motor Speed Limits:** The maximum and minimum speeds for the stepper motors.
*   **Calibration Parameters:** Calibration values to account for differences between motors.

Refer to the `main.cpp` file and other relevant code files for specific configuration options.

## Contributing Guidelines

We welcome contributions to this project! To contribute, please follow these steps:

1.  Fork the repository.
2.  Create a new branch for your feature or bug fix.
3.  Make your changes and commit them with clear, descriptive commit messages.
4.  Submit a pull request.

## License Information

License information is not currently specified. All rights reserved.
Consider adding a license such as MIT or Apache 2.0.

## Acknowledgments

*   DWIN for the HMI display.
*   Epressif for the ESP32 microcontroller.
*   The open-source community for the libraries used in this project.
