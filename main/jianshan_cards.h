#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define JIANSHAN_HISTORY_CAPACITY 16

typedef struct {
    const char *title;
    const char *body;
    const char *source;
    const char *stamp;
    bool quote;
} jianshan_card_t;

typedef struct {
    size_t card_count;
    size_t history[JIANSHAN_HISTORY_CAPACITY];
    size_t history_len;
    size_t history_pos;
} jianshan_state_t;

extern const jianshan_card_t jianshan_cards[];
extern const size_t jianshan_card_count;

void jianshan_state_init(jianshan_state_t *state, size_t card_count,
                         size_t initial_card);
size_t jianshan_state_draw(jianshan_state_t *state, uint32_t entropy);
bool jianshan_state_older(jianshan_state_t *state);
bool jianshan_state_newer(jianshan_state_t *state);
size_t jianshan_state_current(const jianshan_state_t *state);
