cmake_minimum_required(VERSION 3.10)
project(nim)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(nim
        clientMain.cpp
        connectsock.cpp
        getBroadcastAdress.cpp
        getIPAdressInfo.cpp
        getServers.cpp
        Nim.cpp
        Nim.h
        passivesock.cpp
        playNIM.cpp
        serverMain.cpp
        timestamp.cpp
        UDP_recv.cpp
        UDP_send.cpp
        UI.cpp
        ui.h
        wait.cpp)
