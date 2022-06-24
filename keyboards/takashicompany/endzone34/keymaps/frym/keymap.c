/* Copyright 2021 takashicompany
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "keymap_japanese.h"
#include "secrets.h"


#include "naginata.h"
NGKEYS naginata_keys;

#define _QWERTY 0
#define _NAGINATA 1
#define _LOWER 2
#define _RAISE 3
#define _ADJUST 4

enum custom_keycodes {
  QWERTY = NG_SAFE_RANGE,
  ADJUST,
  LDAP_ID,
  MAIL_AD,
  PASS_1,
  PASS_2,
};



//Tap Dance:キーの二度押し
//TAP_DANCE_ENABLE = yes

enum {
  TDD_G_TAB = 0,
  TDD_H_ESC,
  TDD_F_MHEN,
  TDD_J_HENK,
  TDD_Z_UNDO,
  TDD_X_CUT,
  TDD_C_COPY,
  TDD_V_PASTE,
  TDD_SLSH_UNDS,
  TDD_QUES_EXLM,
  TDD_AMPR_PIPE,
  TDD_QUOT_DQUO,
  TDD_LBRC_LCBR,
  TDD_RBRC_RCBR,
  TDD_CIRC_TILD,
  TDD_COLN_SCLN,
  TDD_YEN_DLR,
  TDD_HASH_PERC,
  TDD_AT_GRV,
};

qk_tap_dance_action_t tap_dance_actions[] = {
    [TDD_G_TAB]     = ACTION_TAP_DANCE_DOUBLE(KC_G,    KC_TAB),
    [TDD_F_MHEN]    = ACTION_TAP_DANCE_DOUBLE(KC_F,    JP_MHEN),
    [TDD_J_HENK]    = ACTION_TAP_DANCE_DOUBLE(KC_J,    JP_HENK),
    [TDD_H_ESC]     = ACTION_TAP_DANCE_DOUBLE(KC_H,    KC_ESC),
    [TDD_Z_UNDO]    = ACTION_TAP_DANCE_DOUBLE(KC_Z,    LCTL(KC_Z)),
    [TDD_X_CUT]     = ACTION_TAP_DANCE_DOUBLE(KC_X,    LCTL(KC_X)),
    [TDD_C_COPY]    = ACTION_TAP_DANCE_DOUBLE(KC_C,    LCTL(KC_C)),
    [TDD_V_PASTE]   = ACTION_TAP_DANCE_DOUBLE(KC_V,    LCTL(KC_V)),
    [TDD_SLSH_UNDS] = ACTION_TAP_DANCE_DOUBLE(JP_SLSH, JP_UNDS),
    [TDD_QUES_EXLM] = ACTION_TAP_DANCE_DOUBLE(JP_QUES, JP_EXLM),
    [TDD_AMPR_PIPE] = ACTION_TAP_DANCE_DOUBLE(JP_AMPR, JP_PIPE),
    [TDD_QUOT_DQUO] = ACTION_TAP_DANCE_DOUBLE(JP_QUOT, JP_DQUO),
    [TDD_LBRC_LCBR] = ACTION_TAP_DANCE_DOUBLE(JP_LBRC, JP_LCBR),
    [TDD_RBRC_RCBR] = ACTION_TAP_DANCE_DOUBLE(JP_RBRC, JP_RCBR),
    [TDD_CIRC_TILD] = ACTION_TAP_DANCE_DOUBLE(JP_CIRC, JP_TILD),
    [TDD_COLN_SCLN] = ACTION_TAP_DANCE_DOUBLE(JP_COLN, JP_SCLN),
    [TDD_YEN_DLR]   = ACTION_TAP_DANCE_DOUBLE(JP_YEN,  JP_DLR),
    [TDD_HASH_PERC] = ACTION_TAP_DANCE_DOUBLE(JP_HASH, JP_PERC),
    [TDD_AT_GRV]    = ACTION_TAP_DANCE_DOUBLE(JP_AT,   JP_GRV),
};

#define ADJUST       MO(_ADJUST)
#define TD_G_TAB     TD(TDD_G_TAB)
#define TD_F_MH      TD(TDD_F_MHEN)
#define TD_J_HE      TD(TDD_J_HENK)
#define TD_H_ESC     TD(TDD_H_ESC)
#define TD_Z_UND     TD(TDD_Z_UNDO)
#define TD_X_CUT     TD(TDD_X_CUT)
#define TD_C_CPY     TD(TDD_C_COPY)
#define TD_V_PST     TD(TDD_V_PASTE)
#define TD_SL_UN     TD(TDD_SLSH_UNDS)
#define TD_QU_EX     TD(TDD_QUES_EXLM)
#define TD_AM_PI     TD(TDD_AMPR_PIPE)
#define TD_QUOT      TD(TDD_QUOT_DQUO)
#define TD_LBRC      TD(TDD_LBRC_LCBR)
#define TD_RBRC      TD(TDD_RBRC_RCBR)
#define TD_CI_TL     TD(TDD_CIRC_TILD)
#define TD_COLN      TD(TDD_COLN_SCLN)
#define TD_YE_DL     TD(TDD_YEN_DLR)
#define TD_HS_PE     TD(TDD_HASH_PERC)
#define TD_AT_GR     TD(TDD_AT_GRV)

#define CT_SPC    CTL_T(KC_SPC)
#define SF_ENT    SFT_T(KC_ENT)
#define CT_BS     CTL_T(KC_BSPC)
#define SF_DEL    SFT_T(KC_DEL)
#define CT_NOFF   CTL_T(NG_OFF)
#define SF_NON    SFT_T(NG_ON)
#define CT_TAB    CTL_T(KC_TAB)
#define SF_ESC    SFT_T(KC_ESC)
#define SF_MINS   SFT_T(JP_MINS)
#define ALT_SLSH  LALT_T(JP_SLSH)
#define SF_A      SFT_T(KC_A)
#define TH_B_GUI  LGUI_T(KC_B)
#define TH_N_ALT  LALT_T(KC_N)
#define LOWER_SPC LT(_LOWER,KC_SPC)
#define RAISE_ENT LT(_RAISE,KC_ENT)
#define LOWER_OFF LT(_LOWER,NG_OFF)
#define RAISE_ON  LT(_RAISE,NG_ON)

#define CTL_1 LCTL(KC_1)
#define SCOPY LGUI(S(KC_S))

#define CAD  LCTL(LALT(KC_DEL))
#define ALT_TAB  LALT(KC_TAB)



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

   [_QWERTY] = LAYOUT( \
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_Q,     KC_W,     KC_E,      KC_R,     KC_T,                               KC_Y,     KC_U,      KC_I,     KC_O,     KC_P,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           SF_A,     KC_S,     KC_D,      TD_F_MH,  TD_G_TAB,                           TD_H_ESC, TD_J_HE,   KC_K,     KC_L,     SF_MINS,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           TD_Z_UND, TD_X_CUT, TD_C_CPY, TD_V_PST,  TH_B_GUI,                           TH_N_ALT, KC_M,      KC_COMM,  KC_DOT,   ALT_SLSH,
        //,---------+---------+----------+---------+---------+---------+.   ,----------+---------+----------+---------+---------+---------.
                                                    LOWER_SPC, CT_BS,          SF_DEL,  RAISE_ENT
        //                                         ,---------+---------+.   ,----------+----------.
        //
    ),

   [_NAGINATA] = LAYOUT( \
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           NG_P,     NG_O,     NG_I,      NG_U,     NG_Y,                               NG_T,     NG_R,      NG_E,     NG_W,     NG_Q,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           NG_SCLN,  NG_L,     NG_K,      NG_J,     NG_H,                               NG_G,     NG_F,      NG_D,     NG_S,     NG_A,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           NG_SLSH,  NG_DOT,   NG_COMM,   NG_M,     NG_N,                               NG_B,     NG_V,      NG_C,     NG_X,     NG_Z,
        //,---------+---------+----------+---------+---------+---------+.   ,----------+---------+----------+---------+---------+---------.
                                                    _______,  NG_SHFT,        NG_SHFT2, _______
        //                                         ,---------+---------+.   ,----------+----------.
    ),

   [_LOWER] =  LAYOUT( \
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           JP_ASTR,  JP_PLUS,  JP_SLSH,   JP_MINS,  JP_EQL,                             JP_AMPR,  JP_PIPE,   TD_LBRC,  TD_RBRC,  TD_CI_TL,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_1,     KC_2,     KC_3,      KC_4,     KC_5,                               TD_AT_GR, TD_COLN,   JP_LPRN,  JP_RPRN,  TD_YE_DL,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_6,     KC_7,     KC_8,      KC_9,     KC_0,                               TD_QUOT,  TD_HS_PE,  JP_LABK,  JP_RABK,  TD_QU_EX,
        //,---------+---------+----------+---------+---------+---------+.    ,---------+---------+----------+---------+---------+---------.
                                                    _______,   _______,        _______, ADJUST
        //                                         ,---------+---------+.    ,---------+----------.
    ),

   [_RAISE] = LAYOUT(
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           CTL_1,    _______,  _______,   KC_F11,   KC_F12,                             KC_APP,    KC_BSPC,   KC_DEL,   SCOPY,    CAD,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_LSFT,  KC_F2,    KC_F3,     KC_F4,    KC_F5,                              KC_LEFT,  KC_DOWN,   KC_UP ,   KC_RGHT,  KC_RSFT,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_F6,    KC_F7,    KC_F8,     KC_F9,    KC_F10,                             KC_HOME,  KC_PGDN,   KC_PGUP,  KC_END,   KC_RALT,
        //,---------+---------+----------+---------+---------+---------+.    ,---------+---------+----------+---------+---------+---------.
                                                    ADJUST,   _______,          _______,  _______
        //                                         ,---------+---------+.    ,---------+----------.
    ),

   [_ADJUST] = LAYOUT(
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           JP_KANA,  KC_INS,   _______,   MAIL_AD,  LDAP_ID,                            PASS_1,   PASS_2,    _______,  _______,  RESET,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_F1,    _______,  KC_BTN2,   KC_BTN1,  KC_BTN3,                            KC_MS_L,  KC_MS_D,   KC_MS_U,  KC_MS_R,  _______,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           _______,  _______,  _______,   _______,  JP_KANA,                            KC_WH_L,  KC_WH_D,   KC_WH_U,  KC_WH_R,  _______,
        //,---------+---------+----------+---------+---------+---------+.    ,---------+---------+----------+---------+---------+---------.
                                                    _______,  _______,        _______,  _______
        //                                         ,---------+---------+.    ,---------+----------.
    ),

};


void matrix_init_user(void) {
//void keyboard_post_init_user(void) {
  // 薙刀式
  uint16_t ngonkeys[] = {KC_H, KC_J};
  uint16_t ngoffkeys[] = {KC_F, KC_G};
  set_naginata(_NAGINATA, ngonkeys, ngoffkeys);
  // 薙刀式
}

//Shift+特定のキーの押下判定用
static bool shift_pressed = false;
static bool exceptional_shift_pressed = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {


   switch (keycode) {
      case SF_DEL:
      case SF_A:
      case SF_MINS:
      case KC_LSFT:
      case KC_RSFT:
         if (record->event.pressed) {
            shift_pressed = true;
         } else {
            shift_pressed = false;
         }
         break;
      case NG_ON:
        if(record->event.pressed) {
            naginata_on();
        }
        break;
      case NG_OFF:
        if(record->event.pressed) {
            naginata_off();
        }
        break;
      case LDAP_ID:
        if(record->event.pressed) {
            SEND_STRING(SS_TAP(X_INT5) MYLDAP);
        }
        break;
      case MAIL_AD:
         if(record->event.pressed) {
               SEND_STRING(SS_TAP(X_INT5) MYMAILAD SS_TAP(X_LBRC) MYMAILDOMAIN);
         }
         break;
      case PASS_1:
         if(record->event.pressed) {
               SEND_STRING(SS_TAP(X_INT5) MYPASSWORD1);
         }
         break;
      case PASS_2:
         if(record->event.pressed) {
               SEND_STRING(SS_TAP(X_INT5) MYPASSWORD2);
         }
         break;
      default:
        if (shift_pressed || exceptional_shift_pressed){
            switch(keycode){
                //Shift+/ は _ にする
                case JP_SLSH:
                case ALT_SLSH:
                  if(record->event.pressed) {
                     register_code(KC_LSFT);
                     register_code(JP_BSLS);
                     exceptional_shift_pressed = true;
                  } else {
                     unregister_code(JP_BSLS);
                     unregister_code(KC_LSFT);
                     if (shift_pressed) {
                        register_code(KC_LSFT);
                     }
                     exceptional_shift_pressed = false;
                  }
                  return false;
                  break;
            }
        }

  }
  if (!process_naginata(keycode, record)) {
    return false;
  }
  return true;

}

