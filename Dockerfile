FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install g++ cmake make python3 p7zip-full wget freeglut3-dev libxxf86vm-dev libalut-dev libopenal-dev libvorbis-dev libgtk-3-dev bzip2 zip git -y

# Preparing libs
RUN mkdir /Libs

WORKDIR /Libs
RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.7z && 7z x ./boost_1_82_0.7z && rm ./boost_1_82_0.7z
WORKDIR /Libs/boost_1_82_0
RUN ./bootstrap.sh && ./b2 link=static variant=release cxxflags=-fPIC cflags=-fPIC
ENV BOOST_ROOT=/Libs/boost_1_82_0

WORKDIR /Libs
RUN wget https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.2.1/wxWidgets-3.2.2.1.tar.bz2 && tar xf ./wxWidgets-3.2.2.1.tar.bz2 && rm ./wxWidgets-3.2.2.1.tar.bz2
RUN mkdir /Libs/wxWidgets-3.2.2.1/gtk-build
WORKDIR /Libs/wxWidgets-3.2.2.1/gtk-build
RUN ../configure --enable-propgrid --with-gtk=3 --with-libjpeg=builtin --enable-unicode && make -j$(python3 -c 'import multiprocessing; print(multiprocessing.cpu_count())')
ENV WX_WIDGETS=/Libs/wxWidgets-3.2.2.1

# Preparing libs finished

RUN mkdir /Project
WORKDIR /Project
COPY . .
RUN mkdir /Project/build
WORKDIR /Project/build
RUN cmake .. && make -j$(python3 -c 'import multiprocessing; print(multiprocessing.cpu_count())')

RUN mkdir /release

# Copy prepared builds
RUN cp ./Watercolor/WelcomeWindow/WelcomeWindow /release/Watercolor
RUN cp ./TLauncher/TLauncher /release/TLauncher
RUN cp -r ../Watercolor/MainWindow/Resources /release/Resources

# and dependencies
RUN ldd /release/Watercolor | grep libwx | awk 'NF == 4 { system("cp " $3 " /release") }'
RUN ldd /release/Watercolor | grep libIrrlicht | awk 'NF == 4 { system("cp " $3 " /release") }'
RUN ldd /release/Watercolor | grep libtiff.so | awk 'NF == 4 { system("cp " $3 " /release") }'
RUN ldd /release/Watercolor | grep libjpeg.so | awk 'NF == 4 { system("cp " $3 " /release") }'

# Prepare game API
RUN mkdir -p /release/Development/include && \
    cp ../Irrlicht/include/* /release/Development/include && \
    cp ../TGameApi/* /release/Development/include && \
    cp ../TEngineFramework/TNC/Sources/WebSocket/Client/IWebSocketClient.h /release/Development/include && \
    cp ../TEngineFramework/TNC/Sources/WebSocket/Client/TWebSocketClientDecorator.h /release/Development/include && \
    cp ../TEngineFramework/TNC/Sources/WebSocket/Server/IWebSocketServer.h /release/Development/include && \
    cp ../TEngineFramework/TNC/Sources/WebSocket/Server/TWebSocketServerDecorator.h /release/Development/include
RUN mkdir -p /release/Development/Release && \
    cp ./TEngineFramework/TNC/libTNC.a /release/Development/Release

# Add plugins
RUN mkdir -p /release/Plugins/V1

# Add run script
RUN cp /Project/run_watercolor.sh /release/run_watercolor.sh 

RUN cd /release && zip -r /Watercolor.zip *