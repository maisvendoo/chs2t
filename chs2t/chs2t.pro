TEMPLATE = lib

QT -= gui
QT += xml

TARGET = chs2t

DESTDIR = ../../modules/$$join(TARGET,,,)

DESTDIR = ../../modules/$$join(TARGET,,,)

LIBS += -L$$(RRS_DEV_ROOT)/bin -lCfgReader
LIBS += -L$$(RRS_DEV_ROOT)/bin -lphysics
LIBS += -L$$(RRS_DEV_ROOT)/bin -lvehicle
LIBS += -L$$(RRS_DEV_ROOT)/bin -ldevice
LIBS += -L$$(RRS_DEV_ROOT)/bin -lfilesystem
LIBS += -L$$(RRS_DEV_ROOT)/bin -lJournal


INCLUDEPATH += ./include
INCLUDEPATH += ../chs2t-equipment/include
INCLUDEPATH += $$(RRS_DEV_ROOT)/sdk/include

HEADERS += $$files(./include/*.h)
HEADERS += $$files(../chs2t-equipment/include/*.h)
SOURCES += $$files(./src/*.cpp)
SOURCES += $$files(../chs2t-equipment/src/*.cpp)
