
# variables
SRC_DIR 	= ./src/
INC_DIR 	= ./include/
OBJ_DIR 	= ./obj/
EXEC    	= ./Gestion_Tv_Fitness

CXX=/home/yannick/raspi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++
MOC=/home/yannick/raspi/qt5/bin/moc

INCLUDEDIR += ./include/
INCLUDEDIR += /home/yannick/raspi/qt5pi/include
INCLUDEDIR += /home/yannick/raspi/qt5pi/include/QtNetwork
INCLUDEDIR += /home/yannick/raspi/qt5pi/include/QtCore
INCLUDEDIR += /home/yannick/raspi/qt5pi/include/QtGui
INCLUDEDIR += /home/yannick/raspi/qt5pi/include/QtXml

LIBRARYDIR += /home/yannick/raspi/qt5pi/lib
LIBRARYDIR += /home/yannick/raspi/sysroot/opt/vc/lib
LIBRARYDIR += /home/yannick/raspi/sysroot/usr/lib/arm-linux-gnueabihf
LIBRARY += Qt5Xml Qt5Gui Qt5Network Qt5Core
XLINK_LIBDIR += /home/yannick/raspi/sysroot/opt/vc/lib
XLINK_LIBDIR += /home/yannick/raspi/sysroot/usr/lib/arm-linux-gnueabihf
XLINK_LIBDIR += /home/yannick/raspi/sysroot/lib/arm-linux-gnueabih

INCDIR  = $(patsubst %,-I%,$(INCLUDEDIR))
LIBDIR  = $(patsubst %,-L%,$(LIBRARYDIR))
LIB    = $(patsubst %,-l%,$(LIBRARY))
XLINKDIR = $(patsubst %,-Xlinker -rpath-link=%,$(XLINK_LIBDIR))
 
# les programmes et leurs options
DEFINES = 

CXXFLAGS 	= -march=armv8-a -mtune=cortex-a53 -mfpu=crypto-neon-fp-armv8 -mfloat-abi=hard --sysroot=/home/yannick/raspi/sysroot -O2 -std=gnu++0x -fno-exceptions -Wall -W -D_REENTRANT -fPIC $(DEFINES) $(INCDIR)
LDFLAGS		= -mfloat-abi=hard --sysroot=/home/yannick/raspi/sysroot -lGLESv2 -lpthread $(LIBDIR) $(LIB) $(XLINKDIR)

SOURCES 	= $(wildcard $(SRC_DIR)*.cpp)
INCLUDES 	= $(wildcard $(INC_DIR)*.h)
OBJECTS 	= $(patsubst $(SRC_DIR)%.cpp, $(OBJ_DIR)%.o, $(SOURCES))
MOCS 			= $(patsubst $(INC_DIR)%.h, $(OBJ_DIR)%.moc.o, $(shell grep -l Q_OBJECT $(INCLUDES)))

all: $(EXEC)

$(EXEC): $(OBJECTS) $(MOCS)
	@echo "====== édition des liens ====== "
	$(CXX) $(LDFLAGS) $^ -o $@
	@echo "~~~~~~ Programme $@ Compiller avec success ~~~~~~"

$(OBJ_DIR)%.moc.cpp: $(INC_DIR)%.h
	@echo "------ compilation du moc $*.moc.cpp ------"
	$(MOC) $< -o $@

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@echo "------ compilation de $*.cpp ------"
	$(CXX) -c $< -o $@ $(CXXFLAGS)

copy: $(EXEC)
	cp $(EXEC) $(MOUNT_DIR)/home/pi/

mount_rpi:
	#sudo mkdir $(MOUNT_DIR)
	echo raspberry | sudo sshfs pi@192.168.1.25:/ $(MOUNT_DIR)/ -o transform_symlinks -o allow_other -o password_stdin

umount_rpi:
	sudo umount $(MOUNT_DIR)

create_links:
	sudo ln -s $(MOUNT_DIR)/usr/lib/arm-linux-gnueabihf/ /usr/lib/arm-linux-gnueabihf
	sudo ln -s $(MOUNT_DIR)/lib/arm-linux-gnueabihf/ /lib/arm-linux-gnueabihf

clean:
	-rm $(OBJECTS) $(MOCS) $(EXEC)

