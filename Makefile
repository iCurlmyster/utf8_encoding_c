CC=gcc
CFLAGS=-c -Wall
TARGET=utf8

DEBUG_INFO=

OBJECTS=main.o utf8.o

define call_exec
	@echo Running program
	./$(TARGET)
	@echo Running gprof
	gprof -c $(TARGET) gmon.out > analysis.out
	@echo Human readable gprof in analysis.out
endef

define profiling_info
	$(eval CFLAGS += -g -pg)
	$(eval DEBUG_INFO += -pg)
	@echo Setting profiling flags
endef

all: $(TARGET)

prof: prof_cflags $(TARGET)
	$(call call_exec)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(DEBUG_INFO)

main.o: main.c
	$(CC) $(CFLAGS) main.c

utf8.o: utf8.c
	$(CC) $(CFLAGS) utf8.c

prof_cflags:
	$(call profiling_info)

clean:
	rm *.o $(TARGET)