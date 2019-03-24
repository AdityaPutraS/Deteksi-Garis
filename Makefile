CC = g++

TARGET = a.o

INCLUDE_DIRS = -I include

CXXFLAGS += -O2 -g --std=c++11 -Wall $(INCLUDE_DIRS)
LFLAGS += -lncurses `pkg-config --libs opencv`

OBJS = src/goalFinder.o \
		src/driver.o \
		src/minIni/minIni.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LFLAGS) $(CXXFLAGS)

all: $(TARGET)

clean:
	rm -f $(OBJS) 