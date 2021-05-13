# LeapCar

This is the Arduino side implementation for our Leap Motion Controlled Armed Car Project... That's a long name...

## Quick Start

- Make sure you have **Visual Studio Code** installed.
- Then go to **extension**, search for **PlatformIO**, install that.
  - This will take some time, please patiently wait
  - After installation, **PlatformIO Home** tab will be opened automatically after the restart (you'll know what I'm talking about)
- Click **Open Project** and select this project folder
  - **PlatformIO** will automatically create a **Visual Studio Code Workspace** (supposedly...)
- Navigate to **PlatformIO Home** again, click **Library** to install the libraries required by this project (I'm not sure whether **PlatformIO** will automatically install them for you so you should check this before building your project)
- Click on **Platforms** tab of **PlatformIO Home**, then install **Atmel AVR** (honestly I don't know what this is...)
- Then click the **check mark** under your **Visual Studio Code** to build the project, if all went well you're receive no errors.
- Now you should be ready to **upload** to compiled binary to your Arduino.
  - Go to `platformio.ini` and change the `upload_port` to your desired port (Maybe try uploading through bluetooth? I don't know)
  - Then, just click the **right arrow** under your **Visual Studio Code** to upload the binary to your board

### Bluetooth Port

**Windows**:

- Control Panel -> Search for **Bluetooth** -> Click **Change Bluetooth Settings** -> Click **COM Ports** -> Find your BT04 Port
