#include <mmo/telnet.h>

#include <stdint.h>

#include <mmo/log.h>
#include <mmo/arr/char.h>
#include <mmo/arr/telopt.h>
#include <mmo/net.h>

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
#define MMO_ECHO 1
#define MMO_SGA  3  /* Supress-go-ahead. */
#define MMO_NAWS 31 /* Negotiate-about-window-size. */

static uint8_t mmo_cmd_allow[256] = {
    [MMO_WILL] = MMO_DO,
    [MMO_DO]   = MMO_WILL,
    [MMO_WONT] = MMO_DONT,
    [MMO_DONT] = MMO_WONT,
};

static uint8_t mmo_cmd_deny[256] = {
    [MMO_WILL] = MMO_DONT,
    [MMO_DO]   = MMO_WONT,
    [MMO_WONT] = MMO_DO,
    [MMO_DONT] = MMO_WILL,
};

void mmo_telnet_negotiate_options(mmo_client_t *client, mmo_server_t *server) {
    /* Disable echo. */
    mmo_telopt_t echo = {false, MMO_WILL, MMO_ECHO};
    /* Disable half-duplex communication (suppress-go-ahead). */
    mmo_telopt_t sga = {false, MMO_WILL, MMO_SGA};
    /* Enable negotiate-about-window-size. */
    mmo_telopt_t naws = {false, MMO_DO, MMO_NAWS};

    mmo_telopt_arr_append(&client->telnet.opts, &echo);
    mmo_telopt_arr_append(&client->telnet.opts, &sga);
    mmo_telopt_arr_append(&client->telnet.opts, &naws);

    /* Send options to client. */
    MMO_FOREACH(client->telnet.opts, opt) {
        uint8_t request[3] = {MMO_IAC, opt->cmd, opt->opt};

        mmo_server_send(
            server, client->handle,
            &(mmo_char_arr_t){.elems = (char *)request, .num_elems = 3});
    }
}

static bool mmo_find_telopt(mmo_telopt_t *opt, void *ctx) {
    return opt->opt == *(uint8_t *)ctx;
}

static void mmo_telnet_handle_command(uint8_t opt, mmo_client_t *client,
                                      mmo_server_t *server) {
    mmo_telopt_t *telopt =
        mmo_telopt_arr_find(&client->telnet.opts, mmo_find_telopt, &opt);

    /* Unsupported telopt. */
    if (!telopt) {
        uint8_t response[3] = {MMO_IAC, mmo_cmd_deny[client->telnet.cmd], opt};

        mmo_server_send(
            server, client->handle,
            &(mmo_char_arr_t){.elems = (char *)response, .num_elems = 3});

        return;
    }

    /* If client denies option, disconnect client. */
    if (telopt->cmd != mmo_cmd_allow[client->telnet.cmd]) {
        char *msg = "Client denied one of the following telnet "
                    "options: ECHO, SGA, NAWS.\n";

        mmo_server_send(
            server, client->handle,
            &(mmo_char_arr_t){.elems = msg, .num_elems = strlen(msg)});

        mmo_server_remove_client(server, client->handle);

        return;
    }

    /* Otherwise client has agreed upon option. */
    telopt->done = true;
}

static void mmo_handle_subnegotiation(mmo_client_t *client,
                                      mmo_server_t *server) {
    (void)server;

    if (client->telnet.sb_buf.num_elems == 0) {
        mmo_log_fmt(MMO_LOG_WARNING, "Received empty telnet subnegotiation.");
        return;
    }

    switch (client->telnet.sb_opt) {
        case MMO_NAWS: {
            if (client->telnet.sb_buf.num_elems != 4) {
                return;
            }

            uint8_t *buf = (uint8_t *)client->telnet.sb_buf.elems;

            client->terminal_width  = buf[0] << 8 | buf[1];
            client->terminal_height = buf[2] << 8 | buf[3];

            break;
        }
    }
}

void mmo_telnet_parse(mmo_char_span_t bytes, mmo_client_t *client,
                      mmo_server_t *server) {
    for (size_t i = 0; i < bytes.num_elems; i += 1) {
        uint8_t byte = (uint8_t)bytes.elems[i];

        switch (client->telnet.state) {
            case MMO_TELNET_STATE_DATA: {
                if (byte == MMO_IAC) {
                    client->telnet.state = MMO_TELNET_STATE_IAC;
                } else {
                    mmo_char_arr_append(&client->in, (char *)&byte);
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
                        break;

                    case MMO_SB:
                        client->telnet.state = MMO_TELNET_STATE_SB;
                        break;

                    case MMO_SE:
                        client->telnet.state = MMO_TELNET_STATE_DATA;
                        break;

                    /* Escaped IAC (0xffff). */
                    case MMO_IAC:
                        client->telnet.state = MMO_TELNET_STATE_DATA;
                        mmo_char_arr_append(&client->in, (char *)&byte);
                        break;

                    default:
                        client->telnet.state = MMO_TELNET_STATE_DATA;
                        break;
                }
                break;
            }

            case MMO_TELNET_STATE_COMMAND: {
                mmo_telnet_handle_command(byte, client, server);
                client->telnet.state = MMO_TELNET_STATE_DATA;
                break;
            }

            case MMO_TELNET_STATE_SB: {
                client->telnet.state  = MMO_TELNET_STATE_SB_DATA;
                client->telnet.sb_opt = byte;
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
                switch (byte) {
                    case MMO_SE: {
                        mmo_handle_subnegotiation(client, server);

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
