#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    while (1) {

        printf("bound to %s, waiting connections\n", addrstr);
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);
        char key_word[BUFSZ];
        printf("Type a key-word:\n");
        scanf("%s", key_word);

        char sMessage_one[2*BUFSZ];
        memset(sMessage_one, 0, 2*BUFSZ);
        int messageType = 1;
        sprintf(sMessage_one,"%d|%lu|",messageType, strlen(key_word));

        // Sending Message 1
        size_t count = send(csock, sMessage_one, strlen(sMessage_one)+1, 0);
        if (count != strlen(sMessage_one)+1) {
          logexit("send");
        }

        // Continuous guessing

        size_t total_guessed = 0;
        char letters_guessed[26];
        memset(letters_guessed,0,26);

        while (1) {
          //Receiving letter guess
          char rMessage_guess[2*BUFSZ];
          memset(rMessage_guess,0,2*BUFSZ);
          count = recv(csock, rMessage_guess, 2*BUFSZ - 1, 0);
          printf("[log] Message guess --- %s\n", rMessage_guess);
          size_t hit_positions[strlen(key_word)];
          size_t hits = 0;
          for (size_t i = 0; i < strlen(key_word); i++) {
            if (rMessage_guess[0] == key_word[i]) {
              hit_positions[hits] = i;
              hits++;
            }
          }
          for (size_t i = 0; i <= strlen(letters_guessed); i++) { // Search if guess has been made
            if (letters_guessed[i] == rMessage_guess[0]) {
              printf("[log] already guessed\n");
              break;
            }
            else if (i==strlen(letters_guessed)) {
              letters_guessed[i]=rMessage_guess[0];
              total_guessed += hits;
              break;
            }
          }
          if (total_guessed == strlen(key_word)) { // exit if all letters found
            break;
          }

          // Composing message type 3
          char sMessage_three[BUFSZ*(2+hits)];
          sprintf(sMessage_three,"3|%lu|",hits);
          for (size_t i = 0; i < hits; i++) {
            sprintf(&sMessage_three[strlen(sMessage_three)],"%lu|",hit_positions[i]);
          }
          printf("[DEBUG] Sending message --- %s\n",sMessage_three );
          count = send(csock,sMessage_three,strlen(sMessage_three)+1,0);
          if (count != strlen(sMessage_three)+1) {
            logexit("send");
          }
        }

        // Sending Message 4
        char finished[2]={'4','|'};
        printf("Game finished\n");
        count = send(csock,finished,2,0);

        // sprintf(buf, "remote endpoint: %.1000s\n", caddrstr);
        // count = send(csock, buf, strlen(buf) + 1, 0);
        // if (count != strlen(buf) + 1) {
        //     logexit("send");
        // }
        close(csock);
    }

    exit(EXIT_SUCCESS);
}
