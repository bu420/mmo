#include <mmo/game.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include <mmo/arr/char.h>
#include <mmo/net.h>
#include <mmo/arr/client.h>
#include <mmo/arr/player.h>
#include <mmo/arr/char_arr.h>
#include <mmo/input.h>

void mmo_player_new(mmo_player_t *player, mmo_server_t *server,
                    mmo_client_handle_t handle) {
    (void)server;

    assert(player);
    assert(server);

    player->handle = handle;
    player->state  = MMO_PLAYER_STATE_LOGIN;

    player->sent_greeting = false;

    mmo_char_arr_new(&player->name);
}

void mmo_player_free(mmo_player_t *player) { mmo_char_arr_free(&player->name); }

static void mmo_prompt(mmo_player_t *player, mmo_server_t *server) {
    char *prompt = ">";

    mmo_server_send(
        server, player->handle,
        (mmo_char_span_t){.elems = prompt, .num_elems = strlen(prompt)});
}

typedef enum mmo_print_type_e {
    MMO_PRINT_AFTER_COMMAND,
    MMO_PRINT_INTERRUPT
} mmo_print_type_t;

static void mmo_print_fmt(mmo_player_t *player, mmo_server_t *server,
                          mmo_print_type_t action, const char *fmt, ...) {
    mmo_char_span_t out = {.elems = "\r\n", .num_elems = 2};

    if (action == MMO_PRINT_INTERRUPT) {
        mmo_server_send(server, player->handle, out);
    }

    char buf[1024];

    va_list args;
    va_start(args, fmt);

    size_t len = (size_t)vsnprintf(buf, sizeof buf, fmt, args);

    va_end(args);

    out.elems     = buf;
    out.num_elems = len;
    mmo_server_send(server, player->handle, out);

    out.elems     = "\r\n";
    out.num_elems = 2;
    mmo_server_send(server, player->handle, out);

    mmo_prompt(player, server);
}

void mmo_player_update(mmo_player_t *player, mmo_game_t *game,
                       mmo_server_t *server, mmo_char_arr_t *in) {
    (void)server;

    assert(player);
    assert(server);
    assert(in);

    switch (player->state) {
        case MMO_PLAYER_STATE_LOGIN: {
            if (!player->sent_greeting) {
                player->sent_greeting = true;

                static char greeting[] = {
#embed "greeting.ans"
                };

                mmo_server_send(
                    server, player->handle,
                    (mmo_char_span_t){.elems     = greeting,
                                      .num_elems = sizeof greeting});

                mmo_prompt(player, server);
            }

            mmo_char_arr_t line;
            mmo_char_arr_new(&line);

            if (mmo_get_line(&line, in)) {
                if (line.num_elems == 0) {
                    mmo_prompt(player, server);
                }

                else {
                    MMO_FOREACH(game->players, other) {
                        if (other->handle != player->handle) {
                            mmo_print_fmt(other, server, MMO_PRINT_INTERRUPT,
                                          "Someone said %.*s",
                                          (int)line.num_elems, line.elems);
                        }
                    }

                    mmo_print_fmt(player, server, MMO_PRINT_AFTER_COMMAND,
                                  "You said %.*s", (int)line.num_elems,
                                  line.elems);
                }
            }

            mmo_char_arr_free(&line);

            break;
        }

        case MMO_PLAYER_STATE_FISHING: {
            break;
        }
    }
}

void mmo_game_new(mmo_game_t *game) { mmo_player_arr_new(&game->players); }

void mmo_game_free(mmo_game_t *game) { mmo_player_arr_free(&game->players); }

static bool mmo_find_player(mmo_player_t *player, void *ctx) {
    return player->handle == *(mmo_client_handle_t *)ctx;
}

static bool mmo_find_client(mmo_client_t *client, void *ctx) {
    return client->handle == *(mmo_client_handle_t *)ctx;
}

void mmo_game_update(mmo_game_t *game, mmo_server_t *server) {
    MMO_FOREACH(server->clients, client) {
        if (client->state == MMO_CLIENT_STATE_NEW) {
            mmo_player_t new_player;
            mmo_player_new(&new_player, server, client->handle);

            mmo_player_arr_append(&game->players, &new_player);
        }

        else if (client->state == MMO_CLIENT_STATE_TO_BE_REMOVED) {
            mmo_player_t *player = mmo_player_arr_find(
                &game->players, mmo_find_player, &client->handle);
            assert(player);

            mmo_player_free(player);
            mmo_player_arr_remove_from_ptr(&game->players, player);
        }
    }

    /* Update players. */
    MMO_FOREACH(game->players, player) {
        mmo_client_t *client = mmo_client_arr_find(
            &server->clients, mmo_find_client, &player->handle);
        assert(client);

        mmo_player_update(player, game, server, &client->in);
    }
}
