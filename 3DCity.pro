
CONFIG += c++11

TARGET = Result


LIBS= 	\
        -L/usr/local/lib64 -lOpenThreads\
        -L/usr/local/lib64 -losgAnimation\
        -L/usr/local/lib64 -losgDB\
        -L/usr/local/lib64 -losgEarthAnnotation \
        -L/usr/local/lib64 -losgEarthFeatures \
        -L/usr/local/lib64 -losgEarthQt \
        -L/usr/local/lib64 -losgEarth \
        -L/usr/local/lib64 -losgEarthSymbology \
        -L/usr/local/lib64 -losgEarthUtil \
        -L/usr/local/lib64 -losgFX \
        -L/usr/local/lib64 -losgGA \
        -L/usr/local/lib64 -losgManipulator \
        -L/usr/local/lib64 -losgParticle \
        -L/usr/local/lib64 -losgPresentation \
        -L/usr/local/lib64 -losgQt \
        -L/usr/local/lib64 -losgShadow \
        -L/usr/local/lib64 -losgSim \
        -L/usr/local/lib64 -losg \
        -L/usr/local/lib64 -losgTerrain \
        -L/usr/local/lib64 -losgText \
        -L/usr/local/lib64 -losgUtil \
        -L/usr/local/lib64 -losgViewer \
        -L/usr/local/lib64 -losgVolume \
        -L/usr/local/lib64 -losgWidget

SOURCES += \
    world.cc \
    Xc3DCity.cc

HEADERS += \
    Xc3DCity.h
