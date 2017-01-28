TARGET  = $$qtLibraryTarget(flif)
TEMPLATE = lib
CONFIG += plugin
LIBS += -lflif

CONFIG += c++14

unix{ #TODO: Find out why FLIF do not find its own files
	INCLUDEPATH += /usr/include/FLIF/
}

SOURCES = src/FlifPlugin.cpp
HEADERS = src/FlifPlugin.hpp src/Flif++.hpp
OTHER_FILES += extensions.json
