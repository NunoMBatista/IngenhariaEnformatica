CC = gcc
CFLAGS = -I. -pthread -g -Wall -Wextra
DEPS = global.h system_functions.h queue.h auth_engine.h
OBJ = mobile_user.o backoffice_user.o system_initialization.o structures_creation.o arm_threads.o auth_engine.o general_functions.o clean_up.o system_manager.o queue.o auth_engine.o monitor_engine.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: mobile_user backoffice_user 5g_auth_platform

mobile_user: mobile_user.o queue.o
	$(CC) -o $@ $^ $(CFLAGS)

backoffice_user: backoffice_user.o queue.o
	$(CC) -o $@ $^ $(CFLAGS)

5g_auth_platform: system_manager.o system_initialization.o structures_creation.o arm_threads.o auth_engine.o general_functions.o clean_up.o queue.o auth_engine.o monitor_engine.o
	$(CC) -o $@ $^ $(CFLAGS)

.INTERMEDIATE: $(OBJ)

.PHONY: clean

clean:
	rm -f *.o mobile_user backoffice_user 5g_auth_platform