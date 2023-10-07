#!/bin/bash

export LD_LIBRARY_PATH=$(pwd)

unavailable_dependencies_amount="$(ldd Watercolor | grep "not found" -c)" 

if [[ $unavailable_dependencies_amount =~ "0" ]]; then
    echo "All dependencies are available."
else
    echo "Some dependencies need to be installed"

    sudo apt update && sudo apt install libgtk-3-common freeglut3 libvorbisfile3 libopenal1 libsm6 libpcre2-32-0 -y
fi

./Watercolor
