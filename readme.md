# LeapCar

This is the Arduino side implementation for our Leap Motion Controlled Armed Car Project... That's a long name...

- `src/LeapCar.cpp`: the main file for the Arduino side implementation
- `platformio.ini`: setting file for PlatformIO, defines board type and stuff...

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
  - Go to `platformio.ini` and change the `upload_port` to your desired port (Maybe try uploading through Bluetooth? I don't know)
  - Then, just click the **right arrow** under your **Visual Studio Code** to upload the binary to your board

### Bluetooth Port

**Windows**:

- Control Panel -> Search for **Bluetooth** -> Click **Change Bluetooth Settings** -> Click **COM Ports** -> Find your BT04 Port

## Important

For \*\*\*\*'s sake, the `Servo` library doesn't work with `analogWrite` since they both uses the arduino timer. What a giant mess...

Now we can temporarily fix that by some freaking hacking modifications.

You'll need to locate the file `ServoTimers.h`

Ctrl+Click on `Servo.h` to locate its source

![image-20210514120319217](readme.assets/image-20210514120319217.png)

Most likely in some `Documents/Arduino/libraries` directory.

Then, find something similar to this and ctrl-click `ServoTimer.h`

![image-20210514120429995](readme.assets/image-20210514120429995.png)

You only need to modify the first `ServoTimers.h` since only that uses our architecture.

I'm using Visual Studio Code so it highlights the defined and undefined stuff...

In `ServoTimers.h` change this `_useTimer1` to `_useTimer3` and update `_timer1` to `_timer3`

![image-20210514120626750](readme.assets/image-20210514120626750.png)

And you're done.
