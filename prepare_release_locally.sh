#!/bin/bash

# Save the current directory
current_dir="$(pwd)"

# Install necessary packages
apt update && apt install -y g++ cmake make python3 p7zip-full wget freeglut3-dev libxxf86vm-dev libalut-dev libopenal-dev libvorbis-dev libgtk-3-dev

# Set up Boost
mkdir /Libs
cd /Libs
wget https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.7z
7z x ./boost_1_82_0.7z
rm ./boost_1_82_0.7z
cd /Libs/boost_1_82_0
./bootstrap.sh
./b2 link=static variant=release cxxflags=-fPIC cflags=-fPIC
export BOOST_ROOT=/Libs/boost_1_82_0

# Set up wxWidgets
cd /Libs
wget https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.2.1/wxWidgets-3.2.2.1.tar.bz2
tar xf ./wxWidgets-3.2.2.1.tar.bz2
rm ./wxWidgets-3.2.2.1.tar.bz2
mkdir /Libs/wxWidgets-3.2.2.1/gtk-build
cd /Libs/wxWidgets-3.2.2.1/gtk-build
../configure --enable-propgrid --with-gtk=3 --with-libjpeg=builtin --enable-unicode
make -j$(python3 -c 'import multiprocessing; print(multiprocessing.cpu_count())')
export WX_WIDGETS=/Libs/wxWidgets-3.2.2.1

# Build your project
mkdir /Project
cd /Project
cp -r /path/to/your/source/files/* .
mkdir /Project/build
cd /Project/build
cmake ..
make -j$(python3 -c 'import multiprocessing; print(multiprocessing.cpu_count())')

# Create release directory
mkdir /release

# Copy necessary files to release directory
cp ./Watercolor/WelcomeWindow/WelcomeWindow /release/Watercolor
cp ./TLauncher/TLauncher /release/TLauncher
cp -r ../Watercolor/MainWindow/Resources /release/Resources

# Copy required shared libraries
ldd /release/Watercolor | grep libwx | awk 'NF == 4 { system("cp " $3 " /release") }'
ldd /release/Watercolor | grep libIrrlicht | awk 'NF == 4 { system("cp " $3 " /release") }'
ldd /release/Watercolor | grep libtiff.so | awk 'NF == 4 { system("cp " $3 " /release") }'
ldd /release/Watercolor | grep libjpeg.so | awk 'NF == 4 { system("cp " $3 " /release") }'

# Copy development headers
mkdir -p /release/Development/include
cp ../Irrlicht/include/* /release/Development/include
cp ../TGameApi/* /release/Development/include

# Create Plugins directory
mkdir -p /release/Plugins/V1

# Copy your run script
cp /Project/run_watercolor.sh /release/run_watercolor.sh

7z a /Watercolor.7z /release/*

# Restore the original directory
cd "$current_dir"