#include <assert.h>
#include "jianshan_cards.h"

int main(void)
{
    jianshan_state_t state;
    jianshan_state_init(&state, 3, 1);
    assert(jianshan_state_current(&state) == 1);
    assert(!jianshan_state_older(&state));
    assert(!jianshan_state_newer(&state));

    assert(jianshan_state_draw(&state, 0) == 0);
    assert(jianshan_state_draw(&state, 1) == 2);
    assert(jianshan_state_older(&state));
    assert(jianshan_state_current(&state) == 0);
    assert(jianshan_state_newer(&state));
    assert(jianshan_state_current(&state) == 2);

    assert(jianshan_state_older(&state));
    assert(jianshan_state_draw(&state, 1) == 2);
    assert(state.history_len == 3);
    assert(!jianshan_state_newer(&state));

    for (unsigned i = 0; i < 32; i++) {
        size_t before = jianshan_state_current(&state);
        size_t after = jianshan_state_draw(&state, i);
        assert(after < 3);
        assert(after != before);
        assert(state.history_len <= JIANSHAN_HISTORY_CAPACITY);
    }
    return 0;
}
