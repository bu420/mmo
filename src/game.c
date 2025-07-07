#include <mmo/game.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <mmo/arr/char.h>
#include <mmo/net.h>
#include <mmo/arr/client.h>
#include <mmo/arr/player.h>
#include <mmo/render.h>
#include <mmo/arr/char_arr.h>
#include <mmo/input.h>

void mmo_player_new(mmo_player_t *player, mmo_server_t *server,
                    mmo_client_handle_t handle) {
    (void)server;

    assert(player);
    assert(server);

    player->handle = handle;
    player->state  = MMO_PLAYER_STATE_LOGIN;

    mmo_screen_buf_new(&player->screen_buf);
    mmo_char_arr_new(&player->name);

    /* Hide cursor. */
    mmo_server_send(
        server, handle,
        &(mmo_char_arr_t){.elems     = MMO_ANSI_HIDE_CURSOR,
                          .num_elems = strlen(MMO_ANSI_HIDE_CURSOR)});
}

void mmo_player_free(mmo_player_t *player) {
    mmo_char_arr_free(&player->name);

    mmo_screen_buf_free(&player->screen_buf);
}

void mmo_player_update(mmo_player_t *player, mmo_server_t *server,
                       mmo_char_arr_t *in) {
    (void)server;

    assert(player);
    assert(server);
    assert(in);

    switch (player->state) {
        case MMO_PLAYER_STATE_LOGIN: {
            if (in->num_elems > 0) {
                bool hit_enter_and_valid_username;
                mmo_parse_username(&player->name, &hit_enter_and_valid_username,
                                   in);

                if (hit_enter_and_valid_username) {
                    player->state = MMO_PLAYER_STATE_FISHING;
                    player->screen_buf.should_clear = true;
                }
            }

            break;
        }

        case MMO_PLAYER_STATE_FISHING: {
            break;
        }
    }
}

void mmo_player_render(mmo_player_t *player) {
    switch (player->state) {
        case MMO_PLAYER_STATE_LOGIN: {
            static char splash[] = {
#embed "../res/login.ans"
                , '\0'};

            mmo_cell_buf_t cell_buf;
            mmo_cell_buf_parse_string(
                &cell_buf, (mmo_char_span_t){.elems     = splash,
                                             .num_elems = strlen(splash)});

            for (int x = 0; x < cell_buf.cols; x += 1) {
                for (int y = 0; y < cell_buf.rows; y += 1) {
                    mmo_screen_buf_set(
                        &player->screen_buf, x, y,
                        &cell_buf.cells.elems[y * cell_buf.cols + x]);
                }
            }

            mmo_cell_buf_free(&cell_buf);

            mmo_cell_t cell;
            cell.codepoint[0] = ' ';
            cell.codepoint[1] = '\0';
            cell.fg.is_set    = false;
            cell.bg.is_set    = false;

            for (int i = 0; i < MMO_USERNAME_MAX; i += 1) {
                mmo_screen_buf_set(&player->screen_buf, 26 + i, 11, &cell);
            }

            for (int i = 0; i < (int)player->name.num_elems; i += 1) {
                cell.codepoint[0] = player->name.elems[i];
                mmo_screen_buf_set(&player->screen_buf, 26 + i, 11, &cell);
            }

            break;
        }

        case MMO_PLAYER_STATE_FISHING: {
            static char art[] = {
#embed "../res/fishing.ans"
                , '\0'};

            mmo_cell_buf_t cell_buf;
            mmo_cell_buf_parse_string(
                &cell_buf, (mmo_char_span_t){.elems     = art,
                                             .num_elems = strlen(art)});

            for (int x = 0; x < cell_buf.cols; x += 1) {
                for (int y = 0; y < cell_buf.rows; y += 1) {
                    mmo_screen_buf_set(
                        &player->screen_buf, x, y,
                        &cell_buf.cells.elems[y * cell_buf.cols + x]);
                }
            }

            mmo_cell_buf_free(&cell_buf);

            break;
        }
    }
}

void mmo_game_new(mmo_game_t *game) { mmo_player_arr_new(&game->players); }

void mmo_game_free(mmo_game_t *game) { mmo_player_arr_free(&game->players); }

static bool mmo_find_player(mmo_player_t *player, void *ctx) {
    return player->handle == *(mmo_client_handle_t *)ctx;
}

static void mmo_handle_new_and_old_clients(mmo_game_t *game,
                                           mmo_server_t *server) {
    /* Create new players. */
    MMO_FOREACH(server->events.new_clients, handle) {
        mmo_player_t new_player;
        mmo_player_new(&new_player, server, *handle);

        mmo_player_arr_append(&game->players, &new_player);
    }

    /* Remove players. */
    for (size_t i = 0; i < server->events.removed_clients.num_elems; i += 1) {
        mmo_player_t *player =
            mmo_player_arr_find(&game->players, mmo_find_player,
                                &server->events.removed_clients.elems[i]);

        assert(player);

        mmo_player_free(player);

        if (player) {
            mmo_player_arr_remove_from_ptr(&game->players, player);
        }
    }
}

static bool mmo_find_client(mmo_client_t *client, void *ctx) {
    return client->handle == *(mmo_client_handle_t *)ctx;
}

void mmo_game_update(mmo_game_t *game, mmo_server_t *server) {
    mmo_handle_new_and_old_clients(game, server);

    /* Update and render player states. */
    MMO_FOREACH(game->players, player) {
        mmo_client_t *client = mmo_client_arr_find(
            &server->clients, mmo_find_client, &player->handle);
        assert(client);

        mmo_player_update(player, server, &client->in);

        /* Check if the client terminal is too small. */
        if (client->terminal_width < MMO_COLS ||
            client->terminal_height < MMO_ROWS) {
            char *msg = MMO_ANSI_CLEAR_SCREEN
                MMO_ANSI_MOVE_CURSOR_TO_START MMO_ANSI_RESET
                "Resize your terminal to atleast 80 x 24.";

            mmo_server_send(
                server, client->handle,
                &(mmo_char_arr_t){.elems = msg, .num_elems = strlen(msg)});

            MMO_FOREACH(player->screen_buf.cells_modified_flags, modified) {
                *modified = true;
            }

            player->screen_buf.should_clear = true;

            continue;
        }

        mmo_player_render(player);

        /* Convert player screen buffer to byte array and send to client. */

        mmo_char_arr_t bytes;
        mmo_char_arr_new(&bytes);

        mmo_screen_buf_to_str(&player->screen_buf, client->terminal_width,
                              client->terminal_height, &bytes);
        memset(player->screen_buf.cells_modified_flags.elems, false,
               player->screen_buf.cells_modified_flags.num_elems);

        mmo_server_send(server, player->handle, &bytes);
    }
}
