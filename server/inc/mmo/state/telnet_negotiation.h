#ifndef MMO_STATE_TELNET_NEGOTIATION_H
#define MMO_STATE_TELNET_NEGOTIATION_H

#include <mmo/state/state.h>

typedef struct mmo_telnet_negotiation_state_s {
  /* Disable echo. */
  bool do_echo;
  /* Disable half-duplex communication. */
  bool do_suppress_go_ahead;
  /* Enable character-at-a-time input. */
  bool dont_linemode;
} mmo_telnet_negotiation_state_t;

void mmo_telnet_negotiation_state_new(mmo_telnet_negotiation_state_t *state);
void mmo_telnet_negotiation_state_free(void *ctx);
void mmo_telnet_negotiation_state_update(
    void *ctx, mmo_client_handle_t handle, mmo_server_t *server,
    mmo_char_arr_span_t *client_keyboard_inputs);
void mmo_telnet_negotiation_state_render(void *ctx,
                                         mmo_screen_buf_t *screen_buf);

#endif
