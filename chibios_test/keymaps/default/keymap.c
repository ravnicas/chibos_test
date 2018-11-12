/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "chibios_test.h"
#include "action_layer.h"

enum layers {
	QWERTY,
	NUM,
	PROG,
	NAV,
    FUNC,
};

enum {
    TD_ESC = 0,
};


qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_ESC] = ACTION_TAP_DANCE_DOUBLE(KC_GRV, KC_ESC),
};


const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[QWERTY] = {
	{TD(TD_ESC),    KC_1,		KC_2, 	  KC_3, 		KC_4, 		KC_5, 		KC_ESC,		          KC_DELETE,	KC_6,		KC_7,		KC_8,		KC_9,		KC_0,		KC_MINUS},
    {KC_TAB,        KC_Q, 		KC_W, 	  KC_E, 		KC_R, 		KC_T, 		KC_END,		          KC_HOME, 	    KC_Y,		KC_U,		KC_I,		KC_O,		KC_P,		KC_BSPACE},
    {KC_CAPS,       KC_A, 		KC_S, 	  KC_D, 		KC_F, 		KC_G, 		KC_PGUP,	          KC_END,		KC_H,		KC_J,		KC_K,		KC_L,		KC_O,		KC_ENTER},
    {KC_LSHIFT,     KC_Z,		KC_X, 	  KC_C, 		KC_V, 		KC_B, 		LT(FUNC, KC_PGDOWN),  KC_INSERT,	KC_N,		KC_M,		KC_COMMA,	KC_DOT,		KC_SLASH,	KC_RSHIFT},
    {KC_LCTRL,      KC_LWIN,	KC_LALT,  _______, 		KC_BSPACE, 	KC_SPACE, 	TO(NUM),	          TO(PROG), 	KC_ENTER,	MO(NAV),	OSL(NAV),	_______,	KC_RWIN,	KC_RCTRL}},

[NUM] = {
	{_______,	_______,	_______,	_______,	_______,	_______, 	_______,	_______,	_______,	_______,	KC_KP_SLASH,	 KC_KP_ASTERISK,	KC_KP_MINUS,   _______},
    {_______, 	_______, 	_______,	_______,	_______,	_______,	_______,	_______, 	_______,	KC_KP_7,	KC_KP_8,	     KC_KP_9,	        KC_KP_PLUS,    _______},
    {_______, 	_______, 	_______,	_______,	_______,	_______, 	_______,	_______,	_______,	KC_KP_4,	KC_KP_5,	     KC_KP_6,	        KC_KP_DOT,	   _______},
    {_______,	_______,	_______,	_______,	_______,	_______, 	_______,	_______,	KC_KP_0,	KC_KP_1,	KC_KP_2,	     KC_KP_3,	        KC_KP_ENTER,   _______},
    {_______,	_______,	_______,	_______, 	_______, 	_______, 	TO(QWERTY),	_______,	_______,	_______,	_______,	     _______,	        _______,	   _______}},

[PROG] = {
	{_______,	_______,	_______,	_______,				_______,				_______, 	_______,	_______,	_______,			_______,			_______,	_______,	_______,	_______},
    {_______, 	_______, 	_______,	KC_LEFT_PAREN,			KC_RIGHT_PAREN,			_______,	_______,	_______, 	KC_AUDIO_VOL_UP,	KC_AUDIO_VOL_DOWN,	_______,	_______,	_______,	_______},
    {_______, 	_______, 	_______,	KC_LBRACKET,			KC_RBRACKET,			_______, 	_______,	_______,	_______,			_______,			_______,	_______,	_______,	_______},
    {_______,	_______,	_______,	KC_LEFT_CURLY_BRACE,	KC_RIGHT_CURLY_BRACE,	_______, 	_______,	_______,	_______,			_______,			_______,	_______,	_______,	_______},
    {_______,	_______,	_______,	_______, 				_______, 				_______, 	_______,	TO(QWERTY),	_______,			_______,			_______,	_______,	_______,	_______}},

[NAV] = {
	{_______,	_______,	_______,	_______,	_______,	_______, 	_______,	_______,	_______,	_______,	_______,	_______,	_______,	_______},
    {_______, 	_______, 	_______,	_______,	_______,	_______,	_______,	_______, 	_______,	_______,	KC_UP,	    _______,	_______,	_______},
    {_______, 	_______, 	_______,	_______,	_______,	_______, 	_______,	_______,	_______,	KC_LEFT,	KC_DOWN,	KC_RIGHT,	_______,	_______},
    {_______,	_______,	_______,	_______,	_______,	_______, 	_______,	_______,	_______,	_______,	_______,	_______,	_______,	_______},
    {_______,	_______,	_______,	_______, 	_______, 	_______, 	_______,	_______,	_______,	_______,	_______,	_______,	_______,	_______}},

[FUNC] = {
    {_______,   _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______},
    {_______,   KC_F1,      KC_F2,      KC_F3,      KC_F4,      _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______},
    {_______,   KC_F5,      KC_F6,      KC_F7,      KC_F8,      _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______},
    {_______,   KC_F9,      KC_F10,     KC_F11,     KC_F12,     _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______},
    {_______,   _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______}}

};

const uint16_t fn_actions[] = {
};

uint32_t layer_state_set_user(uint32_t state){
    switch(biton32(state)) {
        case QWERTY:
            printf("qwerty \n\r");
            break;
        case NUM:
            printf("num \n\r");
            break;
        case PROG:
            printf("prog \n\r");
            break;
        case NAV:
            printf("nav \n\r");
            break;
        case FUNC:
            printf("func \n\r");
            break;
        default:
            break;
    }

    return state;
}