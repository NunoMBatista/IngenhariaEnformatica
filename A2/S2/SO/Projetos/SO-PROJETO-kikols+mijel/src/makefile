all: 5g_auth_platform backoffice_user mobile_user
	> log.txt	# Clear log file



5g_auth_platform: 5g_auth_platform.o message_struct.o queue_struct.o
	gcc -Wall -Wextra 5g_auth_platform.o message_struct.o -pthread -o 5g_auth_platform

backoffice_user: backoffice_user.o message_struct.o
	gcc -Wall -Wextra backoffice_user.o message_struct.o -pthread -o backoffice_user

mobile_user: mobile_user.o message_struct.o
	gcc -Wall -Wextra mobile_user.o message_struct.o -pthread -o mobile_user



5g_auth_platform.o: 5g_auth_platform.c
	gcc -Wall -Wextra -c 5g_auth_platform.c

backoffice_user.o: backoffice_user.c
	gcc -Wall -Wextra -c backoffice_user.c

mobile_user.o: mobile_user.c
	gcc -Wall -Wextra -c mobile_user.c

message_struct.o: message_struct.c
	gcc -Wall -Wextra -c message_struct.c

queue_struct.o: queue_struct.c queue_struct.h
	gcc -Wall -Wextra -c queue_struct.c


clean:
	rm -f *.o
	rm -f 5g_auth_platform backoffice_user mobile_user
	> log.txt	# Clear log file
