cmake_minimum_required(VERSION 3.1)

find_package(ECM 5.19.0  NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
include(KDEInstallDirs)

find_package(Qt5Gui REQUIRED)

include_directories(/usr/include/FLIF)
 
add_library(flif MODULE src/FlifPlugin.cpp)
target_link_libraries(flif Qt5::Gui)

# TODO: use target_compile_features instead
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

install(TARGETS flif DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/imageformats/)
install(FILES configuration/qimageioplugins/x-flif.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR}/qimageioplugins/)
install(FILES configuration/qimageioplugins/x-flif.desktop DESTINATION ${SERVICES_INSTALL_DIR}/qimageioplugins/)