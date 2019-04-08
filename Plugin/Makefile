#make file for agi sim
CC = g++

PLUGIN_NAME = GLICSensor

OUTPUT_DIR = output

INCLUDE_DIRS = -Isrc -I/opt/ANVEL/src/AnvelCore -I/opt/ANVEL/deps/ode/include -I/opt/ANVEL/deps/

DEFINES = -DANVEL_SAMPLE_PLUGIN_EXPORT -D_LINUX -DANVEL_CONFIG_DEBUG

CCFLAGS = -std=gnu++1y -Wall -g

CC_SRCS=$(shell find . -name '*.cpp')

CC_OBJS=$(patsubst %.cpp,$(OUTPUT_DIR)/%.o,$(CC_SRCS))

all: $(OUTPUT_DIR)/$(PLUGIN_NAME)

$(OUTPUT_DIR)/$(PLUGIN_NAME): $(CC_OBJS)
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) $(DEFINES) $(CC_OBJS) -shared -o $@.so

$(OUTPUT_DIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CC) $(CCFLAGS) $(INCLUDE_DIRS) $(DEFINES) -MMD -MF $@.d -c -fPIC $< -o $@
	

rebuild: clean all

clean:
	rm -rf output

.PHONY: all clean