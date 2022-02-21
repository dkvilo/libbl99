CC 							:= cc -std=c99 -w
FRAMEWORK 			:= .
SANDBOX_SOURCE 	:= app_layer
BUILD_DIR 			:= build
BIN 						:= app

GLFW_LIB 				:= librarys/libglfw.3.4.dylib

FINAL_CFLAGS 		:= -I./$(FRAMEWORK) ./$(GLFW_LIB) -framework OpenGL -framework Cocoa -framework CoreVideo -framework CoreFoundation -framework IOKit -Wno-deprecated

prepare:
	mkdir build

clean_bin:
	rm $(BUILD_DIR)/$(BIN)

clean:
	rm -rf $(BUILD_DIR)

build:
	make prepare && $(CC) $(SANDBOX_SOURCE)/main.c $(FINAL_CFLAGS) -O1 -o $(BUILD_DIR)/$(BIN)

.PHONY:
	build clean prepare