#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct termios orig_term_settings;

void reset_terminal() {
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_settings);
}

void sigint_callback(int) {
    reset_terminal();
}

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    
    if (flags == -1) {
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }

    return 0;
}

int main() {
    /* Put terminal in non-canonical mode and disable echo. */

    if (tcgetattr(fileno(stdin), &orig_term_settings) == -1) {
        return -1;
    }

    struct termios term_settigs = orig_term_settings;

    term_settigs.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    term_settigs.c_cc[VTIME] = 0;
    term_settigs.c_cc[VMIN] = 1;

    if (tcsetattr(fileno(stdin), TCSAFLUSH, &term_settigs) == -1) {
        return -1;
    }

    /* Reset terminal on exit. */
    if (atexit(reset_terminal) == -1) {
        return -1;
    }

    /* Reset terminal on SIGINT (Ctrl + C). */
    if (signal(SIGINT, sigint_callback) == SIG_ERR) {
        return -1;
    }

    /* Make stdin non-blocking so that read() doesn't block. */
    if (set_nonblocking(fileno(stdin)) == -1) {
        return -1;
    }

    /* Create socket. */

    struct addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *addr;

    if (getaddrinfo("mmo.severinsen.se", "2000", &hints, &addr) == -1) {
        return -1;
    }

    int sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if (sock == -1) {
        return -1;
    }

    /* Connect to server. */

    if (connect(sock, addr->ai_addr, addr->ai_addrlen) == -1) {
        printf("Failed to connect to server.\n");
        return -1;
    }

    /* Make socket non-blocking. */

    if (set_nonblocking(sock) == -1) {
        return -1;
    }

    /* Setup poll structure to poll on both socket and
       the standard input stream. */

    struct pollfd fds[2];

    fds[0].fd = sock;
    fds[0].events = POLLIN;

    fds[1].fd = fileno(stdin);
    fds[1].events = POLLIN;

    /* Main loop. */
    while (true) {    
        /* Poll for socket events. */
        if (poll(fds, 2, 100) == -1) {
            return -1;
        }

        /* Check if socket has received data. */
        if (fds[0].revents & POLLIN) {
            /* Read received data. */
            char bytes[512];
            int num_bytes = (int)recv(sock, bytes, sizeof(bytes), 0);

            if (num_bytes == 0) {
                printf("Connection closed.\n");
                return 0;
            }
            else if (num_bytes == -1) {
                printf("Error.\n");
                return -1;
            }

            /* Print data. */
            printf("%.*s", num_bytes, bytes);
        }

        /* Check if user has entered input. */
        if (fds[1].revents & POLLIN) {
            char bytes[64];
            int num_bytes = (int)read(fileno(stdin), bytes, 64);
            
            if (num_bytes == -1) {
                return -1;
            }

            printf("Num bytes: %d, bytes: %.*s\n", num_bytes, num_bytes, bytes);
        }
    }
}