#pragma once

#include <stdlib.h>

#include <arpa/inet.h>

void logexit(const char *msg);

void decodeMessage(char message[], size_t size,
									 size_t *decoded_message);

int addrparse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize);

int server_sockaddr_init(const char *proto, const char *portstr,
                         struct sockaddr_storage *storage);
