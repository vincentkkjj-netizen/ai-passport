#include "jianshan_cards.h"

const jianshan_card_t jianshan_cards[] = {
    {
        .title = "原文 · 1930",
        .body = "星星之火，\n可以燎原。",
        .source = "毛泽东《星星之火，可以燎原》",
    },
    {
        .title = "143 天",
        .body = "1943 年，凯利·约翰逊和团队，\n用 143 天设计并造出了\nXP-80 喷气式战斗机原型。",
        .source = "据洛克希德·马丁项目史改写",
    },
    {
        .title = "重返苹果",
        .body = "30 岁，乔布斯被苹果解雇。\n后来，他创办了 NeXT。\n再后来，苹果收购了 NeXT，\n他也因此回到了苹果。",
        .source = "据乔布斯 2005 年斯坦福演讲改写",
    },
};

const size_t jianshan_card_count =
    sizeof(jianshan_cards) / sizeof(jianshan_cards[0]);

void jianshan_state_init(jianshan_state_t *state, size_t card_count,
                         size_t initial_card)
{
    state->card_count = card_count;
    state->history[0] = card_count == 0 ? 0 : initial_card % card_count;
    state->history_len = 1;
    state->history_pos = 0;
}

size_t jianshan_state_draw(jianshan_state_t *state, uint32_t entropy)
{
    size_t current = jianshan_state_current(state);
    size_t next = current;

    if (state->card_count > 1) {
        next = entropy % (state->card_count - 1);
        if (next >= current) next++;
    }

    // 抽新卡时丢弃“向下”方向的旧分支，行为与浏览器历史一致。
    state->history_len = state->history_pos + 1;
    if (state->history_len == JIANSHAN_HISTORY_CAPACITY) {
        for (size_t i = 1; i < state->history_len; i++) {
            state->history[i - 1] = state->history[i];
        }
        state->history_len--;
        state->history_pos--;
    }
    state->history[state->history_len++] = next;
    state->history_pos = state->history_len - 1;
    return next;
}

bool jianshan_state_older(jianshan_state_t *state)
{
    if (state->history_pos == 0) return false;
    state->history_pos--;
    return true;
}

bool jianshan_state_newer(jianshan_state_t *state)
{
    if (state->history_pos + 1 >= state->history_len) return false;
    state->history_pos++;
    return true;
}

size_t jianshan_state_current(const jianshan_state_t *state)
{
    return state->history[state->history_pos];
}
