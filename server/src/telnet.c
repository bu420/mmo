#include "mmo/arr/char.h"
#include "mmo/arr/telopt.h"
#include "mmo/net.h"
#include <__stddef_unreachable.h>
#include <mmo/telnet.h>

#include <mmo/log.h>
#include <stdint.h>

/* Telnet interpret-as-command byte. */
#define MMO_IAC 0xff

/* Telnet commands. */
#define MMO_WILL 0xfb
#define MMO_WONT 0xfc
#define MMO_DO   0xfd
#define MMO_DONT 0xfe
#define MMO_SB   0xfa
#define MMO_SE   0xf0

/* Telnet options. */
#define MMO_ECHO     1
#define MMO_SGA      3  /* Supress-go-ahead. */
#define MMO_NAWS     31 /* Negotiate-about-window-size. */
#define MMO_LINEMODE 34

void mmo_telnet_negotiate_options(mmo_client_t *client, mmo_server_t *server) {
    /* Disable echo. */
    mmo_telopt_t echo = {false, MMO_WILL, MMO_ECHO};
    /* Disable half-duplex communication (suppress-go-ahead). */
    mmo_telopt_t sga = {false, MMO_WILL, MMO_SGA};
    /* Enable character-at-a-time input. */
    mmo_telopt_t linemode = {false, MMO_DONT, MMO_LINEMODE};

    mmo_telopt_arr_append(&client->telnet.opts, &echo);
    mmo_telopt_arr_append(&client->telnet.opts, &sga);
    mmo_telopt_arr_append(&client->telnet.opts, &linemode);

    /* Send options to client. */
    MMO_FOREACH(client->telnet.opts, opt) {
        uint8_t request[3] = {MMO_IAC, opt->cmd, opt->opt};

        mmo_server_send(
            server, client->handle,
            &(mmo_char_arr_t){.elems = (char *)request, .num_elems = 3});
    }
}

static bool mmo_find_telopt(const mmo_telopt_t *opt, void *ctx) {
    return opt->opt == *(uint8_t *)ctx;
}

static void mmo_telnet_handle_command(uint8_t opt, mmo_client_t *client,
                                      mmo_server_t *server) {
    mmo_log_fmt(MMO_LOG_DEBUG, "Telnet command: 0x%02x, option: 0x%02x",
                client->telnet.cmd, opt);

    mmo_telopt_t *telopt =
        mmo_telopt_arr_find(&client->telnet.opts, mmo_find_telopt, &opt);

    /* Unsupported telopt. */
    if (!telopt) {
        /* The ^3 bit magic results in the command that denies the given
           command. */
        uint8_t response[3] = {MMO_IAC, client->telnet.cmd ^ 3, opt};

        mmo_server_send(
            server, client->handle,
            &(mmo_char_arr_t){.elems = (char *)response, .num_elems = 3});

        return;
    }

    /* If client denies option, disconnect client.
       The ^6 bit magic gives us the valid response to a given
       command. */
    if (telopt->cmd != (client->telnet.cmd ^ 6)) {
        char *msg = "Client denied one of the following telnet "
                    "options: ECHO, SGA, LINEMODE.\n";

        mmo_server_send(
            server, client->handle,
            &(mmo_char_arr_t){.elems = msg, .num_elems = strlen(msg)});

        mmo_server_remove_client(server, client->handle);

        return;
    }

    /* Otherwise client has agreed upon option. */
    telopt->done = true;
}

void mmo_telnet_parse(mmo_char_span_t bytes, mmo_client_t *client,
                      mmo_server_t *server) {
    for (size_t i = 0; i < bytes.num_elems; i += 1) {
        uint8_t byte = (uint8_t)bytes.elems[i];

        switch (client->telnet.state) {
            case MMO_TELNET_STATE_DATA: {
                if (byte == MMO_IAC) {
                    client->telnet.state = MMO_TELNET_STATE_IAC;
                    mmo_log_fmt(MMO_LOG_DEBUG, "Received IAC.");
                } else {
                    mmo_char_arr_append(&client->in, (char *)&byte);
                    mmo_log_fmt(MMO_LOG_DEBUG, "Received normal data.");
                }
                break;
            }

            case MMO_TELNET_STATE_IAC: {
                switch (byte) {
                    case MMO_WILL:
                    case MMO_WONT:
                    case MMO_DO:
                    case MMO_DONT:
                        client->telnet.state = MMO_TELNET_STATE_COMMAND;
                        client->telnet.cmd   = byte;
                        mmo_log_fmt(MMO_LOG_DEBUG,
                                    "Received WILL/WONT/DO/DONT command.");
                        break;

                    case MMO_SB:
                        client->telnet.state = MMO_TELNET_STATE_SB;
                        mmo_log_fmt(MMO_LOG_DEBUG, "Received subnegotiation.");
                        break;

                    case MMO_SE:
                        client->telnet.state = MMO_TELNET_STATE_DATA;
                        mmo_log_fmt(MMO_LOG_DEBUG,
                                    "Received end of subnegotiation.");
                        break;

                    /* Escaped IAC (0xffff). */
                    case MMO_IAC:
                        client->telnet.state = MMO_TELNET_STATE_DATA;
                        mmo_char_arr_append(&client->in, (char *)&byte);
                        mmo_log_fmt(MMO_LOG_DEBUG,
                                    "Nvm, it was an escaped 0xff.");
                        break;

                    default:
                        client->telnet.state = MMO_TELNET_STATE_DATA;
                        mmo_log_fmt(MMO_LOG_DEBUG, "Received unknown command.");
                        break;
                }
                break;
            }

            case MMO_TELNET_STATE_COMMAND: {
                mmo_telnet_handle_command(byte, client, server);
                break;
            }

            case MMO_TELNET_STATE_SB: {
                client->telnet.state = MMO_TELNET_STATE_SB_DATA;
                break;
            }

            case MMO_TELNET_STATE_SB_DATA: {
                if (byte == MMO_IAC) {
                    client->telnet.state = MMO_TELNET_STATE_SB_IAC;
                } else {
                    mmo_char_arr_append(&client->telnet.sb_buf, (char *)&byte);
                }
                break;
            }

            case MMO_TELNET_STATE_SB_IAC: {
                if (byte == MMO_SE) {
                    // TODO: finish sb...
                    mmo_char_arr_clear(&client->telnet.sb_buf);
                }

                switch (byte) {
                    case MMO_SE: {
                        mmo_log_fmt(MMO_LOG_DEBUG,
                                    "Telnet subnegotiation received: %.*s",
                                    (int)client->telnet.sb_buf.num_elems,
                                    client->telnet.sb_buf.elems);

                        mmo_char_arr_clear(&client->telnet.sb_buf);

                        client->telnet.state = MMO_TELNET_STATE_DATA;
                        break;
                    }

                    /* Escaped IAC (0xffff). */
                    case MMO_IAC: {
                        mmo_char_arr_append(&client->telnet.sb_buf,
                                            (char *)&byte);
                        client->telnet.state = MMO_TELNET_STATE_SB_DATA;
                        break;
                    }

                    default: {
                        client->telnet.state = MMO_TELNET_STATE_DATA;
                        break;
                    }
                }

                break;
            }
        }
    }
}
