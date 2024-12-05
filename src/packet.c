#include <mmo/packet.h>

#include <stdint.h>
#include <stdio.h>

#include <mmo/net.h>

static int32_t mmo_read_int32(const mmo_char_arr_t *bytes, size_t offset) {
    assert(bytes);
    assert(bytes->num_elems >= sizeof(int32_t));

    return (int32_t)ntohl(*(uint32_t *)(bytes->elems + offset));
}

static int32_t mmo_read_next_int32(mmo_char_arr_t *bytes) {
    assert(bytes);
    assert(bytes->num_elems >= sizeof(int32_t));

    int32_t result = mmo_read_int32(bytes, 0);

    /* Pop first 32-bit integer. */
    for (size_t i = 0; i < sizeof(int32_t); i += 1) {
        mmo_char_arr_remove(bytes, 0);
    }

    return result;
}

bool mmo_has_received_complete_packet(const mmo_char_arr_t *bytes) {
    assert(bytes);

    if (bytes->num_elems < sizeof(int32_t)) {
        return false;
    }

    int32_t packet_id = mmo_read_int32(bytes, 0);

    switch (packet_id) {
        /* Handshake. 3 x 32-bit integers. */
        case 0:
            if (bytes->num_elems < 4 * sizeof(int32_t)) {
                return false;
            }

        /* Text. 1 x 32-bit integer plus N bytes of text. */
        case 1: {
            if (bytes->num_elems < 2 * sizeof(int32_t)) {
                return false;
            }

            int32_t num_bytes = mmo_read_int32(bytes, sizeof(int32_t));

            if (bytes->num_elems < 2 * sizeof(int32_t) + (size_t)num_bytes) {
                return false;
            }
        }

        /* Terminal size. 2 x 32-bit integers. */
        case 2:
            if (bytes->num_elems < 3 * sizeof(int32_t)) {
                return false;
            }
    }

    return true;
}

int mmo_handle_packet(mmo_char_arr_t *bytes, mmo_client_t *client) {
    assert(bytes);

    int32_t packet_id = mmo_read_next_int32(bytes);

    switch (packet_id) {
        /* Handshake. 3 x 32-bit integers. */
        case 0: {
            int32_t client_version         = mmo_read_next_int32(bytes);
            int32_t client_terminal_width  = mmo_read_next_int32(bytes);
            int32_t client_terminal_height = mmo_read_next_int32(bytes);

            if (client_version != MMO_ALLOWED_CLIENT_VERSION) {
                return -1;
            }

            if (client_terminal_width > MMO_MAX_TERMINAL_WIDTH ||
                client_terminal_height > MMO_MAX_TERMINAL_HEIGHT) {
                return -1;
            }

            client->has_shook_hands = true;
            client->terminal_width  = client_terminal_width;
            client->terminal_height = client_terminal_height;

            break;
        }

        /* Text. 1 x 32-bit integer plus N bytes of text. */
        case 1: {
            int32_t num_bytes_text = mmo_read_next_int32(bytes);

            for (int i = 0; i < num_bytes_text; i += 1) {
                printf("Received: %c\n", bytes->elems[0]);

                /* Pop text from array. */
                mmo_char_arr_remove(bytes, 0);
            }

            break;
        }

        /* Terminal size. 2 x 32-bit integers. */
        case 2: {
            client->terminal_width  = mmo_read_next_int32(bytes);
            client->terminal_height = mmo_read_next_int32(bytes);

            if (client->terminal_width > MMO_MAX_TERMINAL_WIDTH ||
                client->terminal_height > MMO_MAX_TERMINAL_HEIGHT) {
                return -1;
            }
            
            break;
        }
    }

    return 0;
}