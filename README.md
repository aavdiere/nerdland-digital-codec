# STM Template
Template project tailored with use with STM32 microcontroller and Visual Studio Code

## Prerequisites

You need to have the following installed and properly setup. Ensure that they are available in your path.

- [GNU ARM Embedded tools](https://developer.arm.com/downloads/-/gnu-rm)
- [GNU Make](https://www.gnu.org/software/make/)

### When using vscode

You'll want to install some extensions to make development smoother

- `C/C++`
- `Cortex-Debug`
- `CMake Tools`

## Repo setup

```bash
# Initialise the submodules (libopencm3)
git submodule init
git submodule update

# Build libopencm3
cd libopencm3
make -j8
cd ..

# Build the main application firmware
make
```

## Debuggers

### J-Link

The YouTube series makes use of J-Link debugger hardware and JLinkGDBServer for integrating with VS Code's debug functionality.

You can use the "JLink: Debug Application" and "JLink: Attach to active" VSCode tasks to debug your built binaries.

### ST-Link

ST-Link is alternative debugger that you can use if you don't have a JLink debugger.

It's recommended that you install the [ST-Link drivers](https://www.st.com/en/development-tools/stsw-link009.html).

You'll also need to install the [open source ST-Link debugging tools](https://github.com/stlink-org/stlink). The primary application you'll need from that tool-set is *stutil*. Verify that *stutil* is available in your path before attempting to use the VSCode ST-Link debugging tasks.

Once your drivers and debugging tools are installed, you can use the "ST-Link: Debug Application" and "ST-Link: Attach active" VSCode tasks to debug your firmware over ST-Link.
