cmake_minimum_required(VERSION 3.16)

project(ScoobyCartagena LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

include_directories(logica interfaz)

file(GLOB_RECURSE FUENTES_LOGICA logica/*.cpp)
file(GLOB_RECURSE FUENTES_INTERFAZ interfaz/*.cpp)

qt_add_resources(RECURSOS_COMPILADOS recursos_estaticos/recursos.qrc)

add_executable(ScoobyCartagena
    main.cpp
    ${FUENTES_LOGICA}
    ${FUENTES_INTERFAZ}
    ${RECURSOS_COMPILADOS}
)

target_link_libraries(ScoobyCartagena PRIVATE Qt6::Widgets Qt6::Multimedia)

include(GNUInstallDirs)
install(TARGETS ScoobyCartagena
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
