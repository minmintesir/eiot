INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS +=  $$PWD/enum.h \
            $$PWD/eiot_state_api.h \
            $$PWD/typedef.h \
            $$PWD/version.h\
            $$PWD/imx6ull.h \
            $$PWD/globaldata.h

SOURCES +=  \
            $$PWD/eiot_state_api.c \
            $$PWD/imx6ull.c \
            $$PWD/globaldata.cpp
