#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <netinet/in.h>

typedef struct proxy_args {
    uint16_t victim_port;
    uint16_t attacker_port;
    char* address;
    uint16_t address_port;
} proxy_args;

typedef struct proxy {
    int listen_fd;
    struct sockaddr_in listen_addr;
    struct sockaddr_in listen_client;

    int server_fd;
    struct sockaddr_in luanti_server;

    int attacker_fd;
    struct sockaddr_in attacker_addr;
    struct sockaddr_in attacker_client;
} proxy;

bool detect_auth_ok(uint8_t* bytes, size_t length);
void wait_and_auth(proxy* prox);
void proxy_init(proxy* prox, proxy_args* args);
void proxy_run_hijack(proxy* prox);
