# Build / Porting instructions

## Note

Blockgame targets mainly POSIX/UNIX-like operating systems. Blockgame
may not work on other operating systems without additional patches and
hacks.

We assume you're familiar with Qt 5, C++ development, Minecraft
innerworkings along with the operating system you're targeting.

## Prerequisites

To be able to launch and play Minecraft, you'll require the following:

- Minecraft <= 1.12.2
  - [LWJGL 2] and its native libraries (OpenAL, OpenGL)
  - Java 8

- Minecraft > 1.12.2
  - [LWJGL 3] and its native libraries (OpenAL, OpenGL, TinyFD, STB)
  - Java 11 or above

Other than that, Blockgame requires Qt 5 and CMake.

## LWJGL 2 / LWJGL 3 porting

This guide does not cover how to port LWJGL 2 / LWJGL 3 for your
operating system / architecture. You need to do your own research.

However, if LWJGL porting is required, you will need to have folders
containing the following files:

- A folder for LWJGL 2 libraries (i.e `/usr/lwjgl/`):
  - `lwjgl.jar`: Java bindings for LWJGL 2 native library.
  - `lwjgl_util.jar`: Java utility library for LWJGL 2.
  - `jinput.jar`: Java library to access input devices.
  - `liblwjgl`: The LWJGL 2 native library.

- A folder for LWJGL 3 libraries (i.e `/usr/lwjgl3`):
  - `lwjgl.jar`: LWJGL Library.
  - `lwjgl-glfw.jar`: Java bindings for GLFW.
  - `lwjgl-openal.jar`: Java bindings for OpenAL.
  - `lwjgl-openal-natives-<system>.jar`: Native libraries for OpenAL.
  - `lwjgl-opengl.jar`: Java bindings for OpenGL.
  - `lwjgl-opengl-natives-<system>.jar`: Native libraries for OpenGL.
  - `lwjgl-tinyfd.jar`: Java bindings for TinyFD.
  - `lwjgl-tinyfd-natives-<system>.jar`: Native libraries for TinyFD.
  - `lwjgl-stb.jar`: Java bindings for STB.
  - `lwjgl-stb-natives-<system>.jar`: Native libraries for STB.

If LWJGL 3 porting is needed, make sure the appropiate suffix for your
operating system is added to `launcher/minecraft/launch/PatchLibraries.cpp`
and an `OpSys` enum declared for your operating system (see commit
`8b34505`).

Once those libraries are ported and installed on the system, you're
ready to build and run Blockgame.

## System Info patching

If your operating system is not supported by MultiMC or Blockgame, you
need to patch `libraries/systeminfo/src/sys_unix.cpp`.

Specifically, the `Sys::getSystemRam()` function which is used to
retrieve the amount of system memory available.

If this is not applied, Minecraft will not launch as it will not have
the necessary amount of memory allocated.

## Java installation

In order for Blockgame to find installations of Java for your
operating system, please patch `launcher/java/JavaUtils.cpp` (see
commit `b3bd535`).

## Microsoft / Xbox accounts

To support Microsoft / Xbox accounts, a Microsoft Azure Active
Directory App client token is required.

See
https://docs.microsoft.com/en-us/azure/active-directory/develop/quickstart-register-app
for a guide on how to make one.

Once made, please make sure "Live SDK support" and "Allow public
client flows" are both enabled (can be found under "Authentication").

## Build

```console
$ # Assuming you're currrently inside the project folder.
$ mkdir build
$ cd build
$ cmake \
	# Only required to support Microsoft/Xbox accounts.
	-DBGL_MSA_CLIENT_ID=xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
	# Only required if LWJGL 2 does not officially support your operating system / architecture
	-DBGL_SYSTEM_LWJGL2_PATH=/usr/local/share/lwjgl
	# Only required if LWJGL 3 does not officially support your operating system / architecture
	-DBGL_SYSTEM_LWJGL3_PATH=/usr/local/share/lwjgl3
	..
$ make
# make install
```

[LWJGL 2]: https://github.com/LWJGL/lwjgl
[LWJGL 3]: https://github.com/LWJGL/lwjgl3
