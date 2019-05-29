# anvel-lidar-plugin
This repository contains a very basic ANVEL plugin that transfers LIDAR simulation data over UDP socket for external applications to consume.  It also contains a simple example of how to consume the produced data.

# Releases
Check the tags for release versions.  Any tag with a name of "REL-x-y-z" is a stable release meant for external use.

# Building
Both the plugin and the example have a makefile which should allow for building the executable code.  To build, simply launch a terminal in either the Plugin or ExampleParser directories and type the command `make`.  If you do not have make installed type `sudo apt-get install build-essential` to install it.  The example consumer requires the standard c++ math library for linking.  The plugin requires the ANVEL headers to link against.  The locations of these are assumed to be in the default location but can be changed my modifying the makefile in the Plugin directory.

# Using the Plugin
Once the plugin has been built, it will produce the output in `.../anvel-lidar-plugin/Plugin/output/GLICSensor.so`.  This can then be loaded into ANVEL by using `File->Load Plugin` and navigating to the GLICSensor.so and clicking OK in ANVEL.  This will add a sensor in the sensors section of ANVEL called `GLIC Sensor` which can be added to the world like any other sensor.  Be sure to add the 360 Lidar sensor to the world before adding the GLIC Sensor as it must exist when the GLIC sensor is created so that it can be found correctly.
