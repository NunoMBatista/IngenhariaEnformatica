all: 5g_auth_platform backoffice_user mobile_user



5g_auth_platform: 5g_auth_platform.o
	gcc -Wall -Wextra 5g_auth_platform.o -pthread -o 5g_auth_platform

backoffice_user: backoffice_user.o
	gcc -Wall -Wextra backoffice_user.o -o backoffice_user

mobile_user: mobile_user.o
	gcc -Wall -Wextra mobile_user.o -o mobile_user



5g_auth_platform.o: 5g_auth_platform.c
	gcc -Wall -Wextra -c 5g_auth_platform.c

backoffice_user.o: backoffice_user.c
	gcc -Wall -Wextra -c backoffice_user.c

mobile_user.o: mobile_user.c
	gcc -Wall -Wextra -c mobile_user.c


clear:
	rm -f *.o
	rm -f 5g_auth_platform backoffice_user mobile_user
	> log.txt
