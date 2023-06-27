FROM ubuntu

RUN apt update && apt install g++ cmake make p7zip-full wget freeglut3-dev libxxf86vm-dev libalut-dev libopenal-dev libvorbis-dev libgtk-3-dev -y

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
RUN ../configure --enable-propgrid --with-gtk=3 --with-libjpeg=builtin && make -j3
ENV WX_WIDGETS=/Libs/wxWidgets-3.2.2.1

# Preparing libs finished

RUN mkdir /Project
WORKDIR /Project
COPY . .
RUN mkdir /Project/build
WORKDIR /Project/build
RUN cmake .. && make