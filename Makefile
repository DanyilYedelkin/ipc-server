all: zadanie proc_p1 proc_p2 proc_t proc_d proc_serv2

zadanie: zadanie.c
	gcc -std=gnu11 -Wall -Wextra zadanie.c -o zadanie

proc_p1: proc_p1.c
	gcc -std=gnu11 -Wall -Wextra proc_p1.c -o proc_p1

proc_p2: proc_p2.c
	gcc -std=gnu11 -Wall -Wextra proc_p2.c -o proc_p2

proc_t: proc_t.c
	gcc -std=gnu11 -Wall -Wextra proc_t.c -o proc_t

proc_d: proc_d.c
	gcc -std=gnu11 -Wall -Wextra proc_d.c -o proc_d

proc_serv2: proc_serv2.c
	gcc -std=gnu11 -Wall -Wextra proc_serv2.c -o proc_serv2
