# Purdue Robomasters Control Template

This repository is a template for Purdue Robomasters robot projects.

## Repository Initialization Guide
First, use the template by clicking the green "Use this template" button in the top right. Then click the "New Repository" button and initalize your own repository. Then on your computer, run these commands:
```bash
git clone <url-to-your-repo>
cd <folder-you-cloned>
git submodule update --init --recursive
```

## Environment Setup

### Install Tools

**Installed your favorite text editor** 

- **VSCode**: Download VSCode from [here](https://code.visualstudio.com/download).
- **Neovim**: Install Neovim from [here](https://github.com/neovim/neovim#install-from-package)

**Package Manager**

- **Windows**: Install [MSYS2](https://www.msys2.org/).
- **Mac**: Install [homebrew](https://docs.brew.sh/Installation). 
- **Linux**: Use the one that comes with your system (eg. `apt, dnf/yum, pacman`) 

**Arm GNU Tools and OpenOCD** 

- **Windows**: OpenOCD, arm-none-eabi-gcc, and gdb-multiarch by running these commands in MSYS2 terminal. The default installation path is `C:\msys64`, run `C:\msys64\msys2.exe`.

```powershell
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-openocd
pacman -S mingw-w64-x86_64-arm-none-eabi-gcc
pacman -S mingw-w64-x86_64-gdb-multiarch
```

> Refer to the [Common Issues](#common-issues) section for local pointer to OpenOCD and GNU toolchain.

- **MacOS**: Run these commands in your terminal.

```zsh
brew install gcc-arm-embedded
brew install openocd
```

- **Linux**: Run these commands in your terminal.

```bash
sudo apt update
sudo apt upgrade
sudo apt install openocd
sudo apt install gcc-arm-none-eabi gdb-arm-none-eabi
```

**Check installation and environment variables**:

- **Windows**: Add `C:\msys64\mingw64\bin` to `PATH`.
- **MacOS/Linux**: Check tool path installation using `which` (eg. `which openocd`). Remember the path for `arm-non-eabi-gdb`, as this will be used to configure the debug extension.

### Set Up VSCode

**Install VSCode extensions**:

- Install the VSCode extension [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) to enable ARM microcontroller debugging.
- Edit the extension setting .json file.

**Modify Extension settings**: Add GDB path by opening your VSCode `settings.json` in VSCode and add the following to the end of the file. The default installation paths are shown, but you should use the path you found by using `which`.

- **Windows**: `"cortex-debug.gdbPath": "c:/msys64/mingw64/bin/gdb-multiarch.exe"`.
- **MacOS**: `"cortex-debug.gdbPath": "/opt/homebrew/bin/arm-none-eabi-gdb"`.
- **Linux**: `"cortex-debug.gdbPath": "/usr/bin/gdb-multiarch"`.

**[Optional] VSCode IntelliSense Configuration**: Adding this to `c_cpp_properties.json` will link the standard library header files (eg. `stdint.h`, `stdlib.h`, `math.h`).

```
"C_Cpp.default.compilerPath": "C:/msys64/mingw64/bin/arm-none-eabi-gcc.exe"
``` 

### Set Up Neovim

**Install Extensions**:
- Use lazy.nvim, mason, lspconfig, luasnip, and nvim-cmp to install clangd 
- Add conform for formatting
- Configure with compile_commands.json, .clangd, and .clang-format file 

Example config [here](https://github.com/ironic1234/embedded-workflow)

**Debugging**:

Coming soon

## Usage

### Tasks

VSCode tasks are used to do things like build, clean, or flash the project. Task configurations are located in `tasks.json`

- Open the Command Palette in VSCode using [Ctrl/Cmd+Shift+P].
- Then, select **Tasks: Run Tasks** and pick the appropriate task.

> You can use the shortcut [Ctrl/Cmd+Shift+B] to run the default build task.

In the terminal:

- clean - delete the build folder: `make clean`
- build - build the executable: `make`, you can add the -j flag and then the number of cores you have on your cpu to do a multithreaded build
- flash - flash the executable on to the board: `make download`

### Debugging

A debug session can be used to test and diagnose issues. Debug configurations are located in `launch.json`

- Navigate to **Run and Debug** in VSCode or use [Ctrl/Cmd+Shift+D].
- Select the appropriate launch configuration, depending on if you are using ST-LINK or CMSIS-DAP debugger.
- Click on the green play button or press [F5] to start a debug session.

**GDB** 

Coming soon

### Motor Config Example

```C
Motor_Config_t yaw_motor_config = {
        // Comm Config
        .can_bus = 1, // set can bus currently using
        .speed_controller_id = 3,
        .offset = 3690,

        // Motor Reversal Config (if motor is installed in
        // opposite direction, change to MOTOR_REVERSAL_REVERSED)
        .motor_reversal = MOTOR_REVERSAL_NORMAL,

        //external sensor config
        .use_external_feedback = 1,
        .external_feedback_dir = 1, // 1 if the feedback matches with task space direction, 0 otherwise
        .external_angle_feedback_ptr = &g_imu.rad.yaw, // assign the pointer to the external angle feedback
        .external_velocity_feedback_ptr = &(g_imu.bmi088_raw.gyro[2]), // assign the poitner to the external velocity feedback

        // Controller Config
        .control_mode = POSITION_CONTROL, // Control Mode, see control mode for detail
        .angle_pid =
            {
                .kp = 20000.0f,
                .kd = 1000000.0f,
                .output_limit = GM6020_MAX_CURRENT,
            },
        .velocity_pid =
            {
                .kp = 500.0f,
                .output_limit = GM6020_MAX_CURRENT,
            },
    };
```

## Common Issues

### 1. Windows fails to initializing CMSIS-DAP debugger.

Solution: Go to device manager and uninstall the usb device (probably having some error message in the list). Unplug and plug in the debugger again.

### 2. Tools (OpenOCD, make tools) not found

```
Failed to launch OpenOCD GDB Server:...
```

or

```
mingw32-make: The term 'mingw32-make' is not recognized as a name of a cmdlet, function, script file, or executable program.
Check the spelling of the name, or if a path was included, verify that the path is correct and try again.
```

**Solution 1:**
Add openocd.exe to system environmental variable. If you followed the installation instruction in this README file, then OpenOCD should be install at default location `C:\msys64\mingw64\bin\openocd.exe`, for windows user. Add `C:\msys64\mingw64\bin` to system executable path.

**Solution 2:**
If you don't want to mess with the system path, you could also add local openocd path in `.vscode/launch.json`. Add attribute `serverpath` by adding `"serverpath": "C:\\msys64\\mingw64\\bin\\openocd.exe"` in configuration.

> restarting the terminal is likely needed for new environment variables to take effect.
