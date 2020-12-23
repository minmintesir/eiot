INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS +=         \
            $$PWD/cscanapresults.h  \
            $$PWD/cwlanconfigfile.h \
            $$PWD/cwlanconfigfile.h \
            $$PWD/cscanapresults.h \
            $$PWD/eiot_sysdep_api.h \
            $$PWD/iniparse.h \
            $$PWD/macian_json.h \
            $$PWD/macian_sha256.h \
            $$PWD/macian_string.h \
            $$PWD/publicdefine.h \
            $$PWD/watchdog.h


HEADERS += $$PWD/common.h $$PWD/log.h

SOURCES +=  \
            $$PWD/cwlanconfigfile.cpp \
            $$PWD/cscanapresults.cpp \
            $$PWD/iniparse.cpp \
            $$PWD/macian_json.c \
            $$PWD/macian_sha256.c \
            $$PWD/macian_string.c \
            $$PWD/publicdefine.cpp \
            $$PWD/watchdog.cpp

SOURCES += $$PWD/common.cpp $$PWD/log.cpp
