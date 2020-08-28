#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void usage(int argc, char **argv) {
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

#define BUFSZ 1024

int main(int argc, char **argv) {
	if (argc < 3) {
		usage(argc, argv);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);

	printf("connected to %s\n", addrstr);

	char rec_message[2*BUFSZ];
	memset(rec_message, 0, 2*BUFSZ);

	size_t message_contents[2];

  // Receiving Message 1
	printf("[log] Receiving Message 1\n");
	size_t count = recv(s, rec_message, 2*BUFSZ - 1, 0);
	decodeMessage(rec_message,count,message_contents);
	size_t key_size = message_contents[1];

	// Guessing letters

	char rec_occurrences[BUFSZ*(key_size + 2)];
	size_t occurrences[key_size+2];

	// Creating key word from the ones found from the client
	char key_assemble[key_size];
	for (size_t i = 0; i < key_size; i++) {
		key_assemble[i] = '_';
	}

	while (1) {
		size_t letters_found=0;
		for (size_t i = 0; i < key_size; i++) {
			if (key_assemble[i]!='_') {
				letters_found++;
			}
		}
		printf("The key-word has %lu undiscovered letters:\n",(key_size-letters_found));
		printf("%s\n", key_assemble);
		printf("type in a letter to guess:\n");
		// guess = getchar();
		// fgets (guess, 2, stdin);
		char guess_str[BUFSZ];
		memset(guess_str,0,BUFSZ);
		char guess;
		scanf("%s", guess_str);
		guess = guess_str[0];
		count = send(s,&guess,1,0);
		if (count != 1) {
			logexit("send");
		}
		count = recv(s,rec_occurrences,BUFSZ*(key_size + 2),0);
		decodeMessage(rec_occurrences,count,occurrences);
		printf("Printing occurrences: ");
		for (size_t i = 0; i < sizeof(occurrences)/sizeof(size_t); i++) {
			printf("%lu ", occurrences[i]);
		}
		printf("\n");
		if (occurrences[1]>0) {
			printf("Letter found!\n");
		}
		else{
			printf("Letter not found!\n");
		}
		if (occurrences[0] == 4) {	// If message type is completed word

			for (size_t i = 0; i < key_size; i++) {
				if (key_assemble[i]=='_') {
					key_assemble[i]=guess;
				}
			}
			printf("Congratulations, the key is: %s\n",key_assemble);
			logexit("EXIT_SUCCESS");	// EXIT GAME
		}

		// Assembling the letters found in a string using key_assemble
		for (size_t i = 0; i < key_size; i++) {
			for (size_t n = 0; n < occurrences[1]; n++) { // Iterating over number of hits
				if (occurrences[n+2]==i) {
					key_assemble[i]=guess; // If found, adds to key string
				}
			}
		}
	}
	close(s);
	//
	// printf("received %u bytes\n", total);
	// puts(rec_message);

	exit(EXIT_SUCCESS);
}
