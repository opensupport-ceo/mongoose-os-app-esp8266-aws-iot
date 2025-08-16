# mongoose-os-app-esp8266-aws-iot

## Project Overview

This project implements a firmware based on the Mongoose OS for the ESP8266 microcontroller, specifically targeting the NodeMCU DevKit v1.0 development board. It demonstrates connectivity to AWS IoT Core using MQTT, enabling remote control of GPIOs, reporting sensor data (if enabled), and handling button presses.

*   **Developer:** Jaehong Park
*   **Email:** jaehong1972@gmail.com
*   **Target Hardware:** NodeMCU DevKit v1.0 (ESP8266)
*   **Framework:** Mongoose OS

## Features

This firmware provides the following functionalities:

*   **AWS IoT Core Connectivity:** Securely connects to AWS IoT Core via MQTT for cloud communication.
*   **GPIO Control:** Remotely control GPIO pins (e.g., turn LEDs on/off) by sending MQTT messages.
*   **Button Press Reporting:** Reports button press events (e.g., Flash button) to AWS IoT Core via MQTT.
*   **Sensor Integration (Conditional):**
    *   **DHT Sensor Support:** If `USE_DHT` is enabled in `src/topfeature.h`, it can read temperature and humidity data from a DHT11/DHT22 sensor and report it to AWS IoT Core.
*   **I2C Communication (Conditional):** If `USE_I2C` is enabled, it supports reading from and writing to I2C devices via MQTT commands.
*   **Basic Web Interface:** A simple web page served by the device, accessible via its IP address.
*   **Device Monitoring:** Logs device uptime, RAM usage, and other system information.

## Folder Structure and File Descriptions

This project is organized into the following directories and files:

```
/home/jhp/PRJ/opensupport-ceo/mongoose-os-app-esp8266-aws-iot/
├───.gitignore
├───LICENSE
├───mos.yml
├───README.md
├───.git/
├───doc/
│   └───NODEMCU_DEVKIT_V1.0.PDF
├───fs/
│   ├───index.html
│   └───init.js
└───src/
    ├───hwiodef.h
    ├───main.c
    └───topfeature.h
```

*   `.gitignore`: Specifies intentionally untracked files that Git should ignore.
*   `LICENSE`: Contains the licensing information for this project (Apache License, Version 2.0).
*   `mos.yml`: The main Mongoose OS application configuration file. It defines project metadata, source directories, filesystem content, build variables, and external libraries (e.g., AWS, MQTT, Wi-Fi, DHT). It also includes custom configuration schema for MQTT topics and device settings.
*   `README.md`: This file, providing an overview and documentation for the project.
*   `.git/`: The hidden directory used by Git to store all the information about your repository, including all commits and remote repository addresses.
*   `doc/`: Contains project-related documentation.
    *   `NODEMCU_DEVKIT_V1.0.PDF`: A PDF document providing specifications or details about the NodeMCU DevKit v1.0 board.
*   `fs/`: This directory contains files that will be uploaded to the device's filesystem. These are typically web assets or JavaScript files for device-side scripting.
    *   `index.html`: A very basic HTML file that serves as the device's web interface. It currently displays "Welcome to the gcamp project".
    *   `init.js`: A JavaScript file executed by Mongoose OS on device startup. It loads various Mongoose OS APIs (config, MQTT, sys, timer) and contains commented-out code for a basic AWS test that publishes device metrics.
*   `src/`: Contains the C source code for the Mongoose OS application.
    *   `hwiodef.h`: Hardware I/O definition header file. It defines GPIO pin assignments for LEDs (ESP8266 built-in, NodeMCU built-in) and buttons, and error codes for I2C operations.
    *   `main.c`: The core C application logic. It handles MQTT communication (subscribe/publish), GPIO control, button interrupt handling, network event callbacks, and sensor data reporting (if enabled). It integrates with AWS IoT Core.
    *   `topfeature.h`: A header file for defining top-level features using preprocessor macros. This allows for conditional compilation of various functionalities like DHT sensor support, network callbacks, MQTT port usage, and a JavaScript AWS test mode.

## Existing README.md Content

The original `README.md` file contained only the project title:

```markdown
# mongoose-os-app-esp8266-aws-iot
```

This has been expanded upon to provide a comprehensive overview of the project, its features, and detailed descriptions of its file structure and components.

## Getting Started

To build and run this firmware on your NodeMCU DevKit v1.0, follow these steps:

### Prerequisites

*   **Mongoose OS Development Environment:** Install the Mongoose OS `mos` tool and its dependencies. Refer to the official Mongoose OS documentation for installation instructions: [https://mongoose-os.com/docs/mongoose-os/quickstart/setup.md](https://mongoose-os.com/docs/mongoose-os/quickstart/setup.md)
*   **NodeMCU DevKit v1.0:** Ensure you have the development board and a USB cable for flashing.
*   **AWS Account (Optional):** If you plan to connect to AWS IoT Core, you will need an AWS account and appropriate permissions.

### Building and Flashing

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-repo/mongoose-os-app-esp8266-aws-iot.git
    cd mongoose-os-app-esp8266-aws-iot
    ```
    *(Note: Replace `your-repo` with the actual repository URL if this project is hosted on GitHub.)*

2.  **Build the firmware:**
    ```bash
    mos build --platform esp8266
    ```

3.  **Flash the firmware to your device:**
    ```bash
    mos flash
    ```
    Ensure your NodeMCU board is connected to your computer via USB.

### Configuration

After flashing, you need to configure Wi-Fi and MQTT settings. You can do this using the `mos` tool.

1.  **Connect to Wi-Fi:**
    ```bash
    mos wifi <YOUR_SSID> <YOUR_PASSWORD>
    ```

2.  **Configure MQTT (for AWS IoT Core):**
    The `mos.yml` file defines default MQTT settings. For AWS IoT Core, you'll typically need to configure certificates and specific endpoints. Refer to the Mongoose OS AWS IoT documentation for detailed setup: [https://mongoose-os.com/docs/mongoose-os/cloud/aws.md](https://mongoose-os.com/docs/mongoose-os/cloud/aws.md)

    You can set MQTT server, publish, and subscribe topics:
    ```bash
    mos config-set mqtt.server="YOUR_AWS_IOT_ENDPOINT:8883" mqtt.pub="/your/publish/topic" mqtt.sub="/your/subscribe/topic"
    ```
    *(Replace placeholders with your actual AWS IoT endpoint and topics.)*

    Example MQTT configuration from `mos.yml`:
    ```yaml
    config_schema:
      - ["mqtt.pub", "s", "/response", {title: "Publish topic"}]
      - ["mqtt.sub", "s", "/request", {title: "Subscribe topic"}]
      - ["mqtt.server", "broker.mqttdashboard.com:1883"] # Example public broker
      - ["device.id", "esp8266"]
      - ["device.password", "test"]
      - ["i2c.enable", true]
    ```

## Usage

Once configured and connected, the device will:

*   **Publish Data:**
    *   System uptime and RAM usage (if `USE_LOG_TIMER` or `USE_NET_CB` is enabled).
    *   DHT sensor readings (temperature, humidity) if `USE_DHT` and `USE_REPORT_TEMP` are enabled.
    *   Button press events.
*   **Subscribe to Commands:**
    *   Listen for commands on the configured MQTT subscribe topic (e.g., `/request`).
    *   **GPIO Control Example:** To toggle a GPIO pin (e.g., built-in LED on pin 2), publish a JSON message to the subscribe topic:
        ```json
        {
            "gpio": {
                "pin": 2,
                "state": 0
            }
        }
        ```
        *(`pin`: GPIO number, `state`: 0 for LOW, 1 for HIGH)*
    *   **Button Handler Example:** To set up a button handler on a specific pin (e.g., Flash button on pin 0), publish:
        ```json
        {
            "button": {
                "pin": 0
            }
        }
        ```
    *   **I2C Read Example (if enabled):**
        ```json
        {
            "i2c_read": {
                "addr": 100,
                "len": 4
            }
        }
        ```
    *   **I2C Write Example (if enabled):**
        ```json
        {
            "i2c_write": {
                "data": "01020304"
            }
        }
        ```

## Troubleshooting

*   **Serial Output:** Use `mos console` to view the device's serial output for debugging information.
*   **Wi-Fi Connectivity:** Ensure your Wi-Fi credentials are correct and the device is within range.
*   **MQTT Connection:** Verify your MQTT server address, port, and topic configurations. Check AWS IoT Core logs if connecting to AWS.
*   **Firmware Size:** If you encounter issues with flashing or device stability, check the `build_vars` in `mos.yml` related to `MGOS_ROOT_FS_SIZE` and `APP_SLOT_SIZE`. Adjusting these might be necessary for larger firmwares.

## Contributing

Contributions are welcome! Please feel free to fork the repository, make your changes, and submit a pull request.

## License

This project is licensed under the Apache License, Version 2.0. See the `LICENSE` file for details.