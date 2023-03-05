TARGET := ../bin/main

CC := gcc

CPP := g++ -std=c++11

CSRCS :=$(wildcard *.c)

CPPSRCS :=$(wildcard *.cpp)

O_PATH = ../obj

OBJS := $(patsubst %.c, $(O_PATH)/%.o, $(CSRCS))

OBJS += $(patsubst %.cpp, $(O_PATH)/%.o, $(CPPSRCS))

INC := -I ./ -I ../inc

LIBS := -l pthread

$(TARGET):$(OBJS)
	$(CC) $^ $(LIBS) -o $@

$(O_PATH)/%.o:%.c 
	$(CC) -c $< $(INC) -o $@

$(O_PATH)%.o:%.cpp 
	$(CPP) -c $< $(INC) -o $@

clean:
	rm -rf  $(OBJS)
	rm -rf  $(TARGET)