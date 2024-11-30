#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>

#define ADDRESS "mmo.severinsen.se"
#define PORT "2000"
#define VERSION 1

struct termios orig_term_settings;

void restore_terminal() {
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_settings);
}

void sigint_callback(int) {
    restore_terminal();
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

int send_int(int sock, int value) {
    uint32_t net_value = htonl((uint32_t)value);

    if (send(sock, &net_value, 4, 0) == -1) {
        return -1;
    }

    return 0;
} 

int send_handshake(int sock, int width, int height) {
    /* Send packet ID. */
    if (send_int(sock, 0) == -1) {
        return -1;
    }

    /* Send client version. */
    if (send_int(sock, VERSION) == -1) {
        return -1;
    }

    /* Send terminal size. */

    if (send_int(sock, width) == -1) {
        return -1;
    }

    if (send_int(sock, height) == -1) {
        return -1;
    }

    return 0;
}

int send_text(int sock, const char *text, int len) {
    /* Send packet ID. */
    if (send_int(sock, 1) == -1) {
        return -1;
    }

    /* Send text length. */
    if (send_int(sock, len) == -1) {
        return -1;
    }

    /* Send text. */
    if (send(sock, text, (size_t)len, 0) == -1) {
        return -1;
    }

    return 0;
}

int send_term_size(int sock, int width, int height) {
    /* Send packet ID. */
    if (send_int(sock, 2) == -1) {
        return -1;
    }

    /* Send terminal size. */

    if (send_int(sock, width) == -1) {
        return -1;
    }

    if (send_int(sock, height) == -1) {
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

    /* Make user input non-blocking. */
    if (set_nonblocking(fileno(stdin)) == -1) {
        return -1;
    }

    /* Disable buffering on the standard output stream.
       Otherwise it won't flush until a newline is printed. */
    if (setvbuf(stdout, NULL, _IONBF, 0) != 0) {
        return -1;
    }

    /* Restore terminal on exit or SIGINT (Ctrl + C). */

    if (atexit(restore_terminal) == -1) {
        return -1;
    }

    if (signal(SIGINT, sigint_callback) == SIG_ERR) {
        return -1;
    }

    /* Create socket. */

    struct addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *addr;

    if (getaddrinfo(ADDRESS, PORT, &hints, &addr) == -1) {
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

    /* Send handshake packet (client version and terminal size) to server. */

    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, &size);

    int width = size.ws_col;
    int height = size.ws_row;

    if (send_handshake(sock, width, height) == -1) {
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
        if (poll(fds, 2, 500) == -1) {
            return -1;
        }

        /* Check if socket has received data. */
        if (fds[0].revents & POLLIN) {
            /* Read received data. */
            char bytes[2048];
            int num_bytes = (int)recv(sock, bytes, sizeof(bytes), 0);

            if (num_bytes == 0) {
                printf("Connection closed.\n");
                return 0;
            }
            else if (num_bytes == -1) {
                return -1;
            }

            /* Print data. */
            printf("%.*s", num_bytes, bytes);
        }

        /* Check if user has entered input. */
        if (fds[1].revents & POLLIN) {
            /* Read input from standard input stream. */
            char bytes[256];
            int num_bytes = (int)read(fileno(stdin), bytes, 256);
            
            if (num_bytes == -1) {
                return -1;
            }

            /* Send user input to server. */
            if (send_text(sock, bytes, num_bytes) == -1) {
                return -1;
            }
        }

        /* Check if terminal size has changed. */

        struct winsize size;
        ioctl(fileno(stdout), TIOCGWINSZ, &size);

        if (size.ws_col != width || size.ws_row != height) {
            width = size.ws_col;
            height = size.ws_row;

            /* Send new terminal size to server. */
            if (send_term_size(sock, width, height) == -1) {
                return -1;
            }
        }
    }
}