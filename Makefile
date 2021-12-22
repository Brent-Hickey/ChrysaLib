SRC_DIR := ./src
OBJ_DIR := ./obj
APP_DIR := $(SRC_DIR)/apps
APP_OBJ_DIR := $(OBJ_DIR)/apps
LIB_DIR := $(SRC_DIR)/lib
LIB_OBJ_DIR := $(OBJ_DIR)/lib

APP_FILES := $(shell find $(APP_DIR) -name "*.cpp")
APP_DIRS := $(shell find $(APP_DIR) -type d)
LIB_FILES := $(shell find $(LIB_DIR) -name "*.cpp")
LIB_DIRS := $(shell find $(LIB_DIR) -type d)

APP_OBJ_FILES := $(patsubst $(APP_DIR)/%.cpp,$(APP_OBJ_DIR)/%.o,$(APP_FILES))
APP_OBJ_DIRS := $(patsubst $(APP_DIR)/%,$(APP_OBJ_DIR)/%,$(APP_DIRS))
LIB_OBJ_FILES := $(patsubst $(LIB_DIR)/%.cpp,$(LIB_OBJ_DIR)/%.o,$(LIB_FILES))
LIB_OBJ_DIRS := $(patsubst $(LIB_DIR)/%,$(LIB_OBJ_DIR)/%,$(LIB_DIRS))

OBJ_DIRS_CREATE := $(shell mkdir -p $(APP_OBJ_DIRS) $(LIB_OBJ_DIRS))
OS := $(shell uname)

CPPFLAGS := -std=c++14 -D ASIO_STANDALONE
CXXFLAGS += -MMD

all:	CXXFLAGS += -O3
all:	hub files gui
debug:	CXXFLAGS += -DDEBUG -g -O0
debug:	hub files gui

hub:	$(LIB_OBJ_FILES) $(APP_OBJ_DIR)/hub/hub.o
ifeq ($(OS),Darwin)
	c++ -o $@ $^ \
		-F/Library/Frameworks \
		-framework CoreFoundation \
		-framework IOKit \
		/usr/local/Cellar/libusb/1.0.24/lib/libusb-1.0.a
endif
ifeq ($(OS),Linux)
	c++ -o $@ $^ \
		-pthread \
		-L/usr/local/lib -lusb-1.0
endif

gui:	$(LIB_OBJ_FILES) $(APP_OBJ_DIR)/gui/gui.o
ifeq ($(OS),Darwin)
	c++ -o $@ $^ \
		-F/Library/Frameworks \
		-framework CoreFoundation \
		-framework IOKit \
		/usr/local/Cellar/libusb/1.0.24/lib/libusb-1.0.a
endif
ifeq ($(OS),Linux)
	c++ -o $@ $^ \
		-pthread \
		-L/usr/local/lib -lusb-1.0
endif

files:	$(LIB_OBJ_FILES) $(APP_OBJ_DIR)/files/files.o
ifeq ($(OS),Darwin)
	c++ -o $@ $^ \
		-F/Library/Frameworks \
		-framework CoreFoundation \
		-framework IOKit \
		/usr/local/Cellar/libusb/1.0.24/lib/libusb-1.0.a
endif
ifeq ($(OS),Linux)
	c++ -o $@ $^ \
		-pthread \
		-L/usr/local/lib -lusb-1.0
endif

$(LIB_OBJ_DIR)/%.o : $(LIB_DIR)/%.cpp
ifeq ($(OS),Darwin)
	c++ $(CPPFLAGS) $(CXXFLAGS) -c \
		-I/usr/local/include/libusb-1.0/ \
		-o $@ $<
endif
ifeq ($(OS),Linux)
	c++ $(CPPFLAGS) $(CXXFLAGS) -c \
		-I/usr/local/include/libusb-1.0/ \
		-o $@ $<
endif

$(APP_OBJ_DIR)/%.o : $(APP_DIR)/%.cpp
ifeq ($(OS),Darwin)
	c++ $(CPPFLAGS) $(CXXFLAGS) -c \
		-I/usr/local/include/libusb-1.0/ \
		-o $@ $<
endif
ifeq ($(OS),Linux)
	c++ $(CPPFLAGS) $(CXXFLAGS) -c \
		-I/usr/local/include/libusb-1.0/ \
		-o $@ $<
endif

clean:
	rm -f hub files gui $(shell find . -name "*.o") $(shell find . -name "*.d")
	rm -rf $(OBJ_DIR)/

-include $(LIB_OBJ_FILES:.o=.d)
-include $(APP_OBJ_FILES:.o=.d)
