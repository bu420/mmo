#include <mmo/packet.h>

static int32_t mmo_read_int32(const mmo_char_arr_t *bytes) {
    assert(bytes);
    assert(bytes->num_elems >= sizeof(int32_t));

    return (int32_t)ntohl(*(uint32_t *)bytes->elems);
}

static bool mmo_has_received_complete_packet(const mmo_char_arr_t *bytes) {
    assert(bytes);

    if (bytes->num_elems < sizeof(int32_t)) {
        return false;
    }

    int32_t packet_id = mmo_read_int32(bytes->elems);
    
    switch (packet_id) {
        /* Handshake. */
        case 0: mmo_handle_packet_handshake(&server->clients[i].in); break;

        /* Text. */
        case 1: mmo_handle_packet_text(&server->clients[i].in); break;

        /* New terminal size. */
        case 2: mmo_handle_packet_terminal_size(&server->clients[i].in); break;
    }
}

static int mmo_handle_packet_handshake(mmo_char_arr_t *bytes, mmo_client_arr_t *clients, size_t client_idx) {
    assert(bytes);
    assert(client);

    if (bytes->num_elems < 3 * sizeof(int32_t)) {
        return -1;
    }

    /* Read client version. */

    int32_t version = mmo_read_int32(bytes->elems);

    if (version != MMO_ALLOWED_CLIENT_VERSION) {
        const char response[256];
        snprintf(response, 256,
                 "Connection refused. Client version must be %d. Your client is on version %d.\r\n",
                 MMO_ALLOWED_CLIENT_VERSION, version);

        send(socket, response, strlen(response), 0);

        close(socket);

        return -1;
    }

    /* Read client terminal width. */
    mmo_read_int32(arr->elems + sizeof(int32_t));

    /* Read client terminal height. */
    mmo_read_int32(arr->elems + 2 * sizeof(int32_t));
}

static void mmo_handle_packet_text(const mmo_char_arr_t *arr) {}

static void mmo_handle_packet_terminal_size(const mmo_char_arr_t *arr) {}