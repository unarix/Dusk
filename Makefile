## Dusk - Night Shift for Haiku

NAME = Dusk
TYPE = APP
APP_MIME_SIG = application/x-vnd.Dusk
SRCS = src/main.cpp \
       src/DuskApp.cpp \
       src/DuskWindow.cpp \
       src/DuskView.cpp \
       src/GammaEngine.cpp \
       src/TrayView.cpp

RDEFS = res/Dusk.rdef
LIBS = be tracker localestub $(STDCPPLIBS)
LIBPATHS =
SYSTEM_INCLUDE_PATHS =
LOCAL_INCLUDE_PATHS = src

OPTIMIZE := FULL
CFLAGS = -Wall
CXXFLAGS = -Wall -std=c++17

OBJ_DIR := objects

include $(BUILDHOME)/etc/makefile-engine
