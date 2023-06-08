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
  TDD_T_TAB = 0,
  TDD_Y_ESC,
//   TDD_Z_UNDO,
//   TDD_X_CUT,
//   TDD_C_COPY,
//   TDD_V_PASTE,
  TDD_SLSH_UNDS,
  TDD_MINS_UNDS,
  TDD_QUES_EXLM,
  TDD_AMPR_PIPE,
  TDD_QUOT_DQUO,
  TDD_LBRC_LCBR,
  TDD_RBRC_RCBR,
  TDD_CIRC_GRV,
  TDD_COLN_SCLN,
  TDD_YEN_DLR,
  TDD_HASH_PERC,
  TDD_AT_TILD,
};

qk_tap_dance_action_t tap_dance_actions[] = {
    [TDD_T_TAB]     = ACTION_TAP_DANCE_DOUBLE(KC_T,    KC_TAB),
    [TDD_Y_ESC]     = ACTION_TAP_DANCE_DOUBLE(KC_Y,    KC_ESC),
   //  [TDD_Z_UNDO]    = ACTION_TAP_DANCE_DOUBLE(KC_Z,    LCTL(KC_Z)),
   //  [TDD_X_CUT]     = ACTION_TAP_DANCE_DOUBLE(KC_X,    LCTL(KC_X)),
   //  [TDD_C_COPY]    = ACTION_TAP_DANCE_DOUBLE(KC_C,    LCTL(KC_C)),
   //  [TDD_V_PASTE]   = ACTION_TAP_DANCE_DOUBLE(KC_V,    LCTL(KC_V)),
    [TDD_SLSH_UNDS] = ACTION_TAP_DANCE_DOUBLE(JP_SLSH, JP_UNDS),
    [TDD_MINS_UNDS] = ACTION_TAP_DANCE_DOUBLE(JP_MINS, JP_UNDS),
    [TDD_QUES_EXLM] = ACTION_TAP_DANCE_DOUBLE(JP_QUES, JP_EXLM),
    [TDD_AMPR_PIPE] = ACTION_TAP_DANCE_DOUBLE(JP_AMPR, JP_PIPE),
    [TDD_QUOT_DQUO] = ACTION_TAP_DANCE_DOUBLE(JP_QUOT, JP_DQUO),
    [TDD_LBRC_LCBR] = ACTION_TAP_DANCE_DOUBLE(JP_LBRC, JP_LCBR),
    [TDD_RBRC_RCBR] = ACTION_TAP_DANCE_DOUBLE(JP_RBRC, JP_RCBR),
    [TDD_CIRC_GRV]  = ACTION_TAP_DANCE_DOUBLE(JP_CIRC, JP_GRV),
    [TDD_COLN_SCLN] = ACTION_TAP_DANCE_DOUBLE(JP_COLN, JP_SCLN),
    [TDD_YEN_DLR]   = ACTION_TAP_DANCE_DOUBLE(JP_YEN,  JP_DLR),
    [TDD_HASH_PERC] = ACTION_TAP_DANCE_DOUBLE(JP_HASH, JP_PERC),
    [TDD_AT_TILD]   = ACTION_TAP_DANCE_DOUBLE(JP_AT,   JP_TILD),
};

#define ADJUST       MO(_ADJUST)
#define TD_T_TAB     TD(TDD_T_TAB)
#define TD_Y_ESC     TD(TDD_Y_ESC)
// #define TD_Z_UND     TD(TDD_Z_UNDO)
// #define TD_X_CUT     TD(TDD_X_CUT)
// #define TD_C_CPY     TD(TDD_C_COPY)
// #define TD_V_PST     TD(TDD_V_PASTE)
#define TD_SL_UN     TD(TDD_SLSH_UNDS)
#define TD_MI_UN     TD(TDD_MINS_UNDS)
#define TD_QU_EX     TD(TDD_QUES_EXLM)
#define TD_AM_PI     TD(TDD_AMPR_PIPE)
#define TD_QUOT      TD(TDD_QUOT_DQUO)
#define TD_LBRC      TD(TDD_LBRC_LCBR)
#define TD_RBRC      TD(TDD_RBRC_RCBR)
#define TD_CI_GR     TD(TDD_CIRC_GRV)
#define TD_COLN      TD(TDD_COLN_SCLN)
#define TD_YE_DL     TD(TDD_YEN_DLR)
#define TD_HS_PE     TD(TDD_HASH_PERC)
#define TD_AT_TL     TD(TDD_AT_TILD)

#define CT_SPC    CTL_T(KC_SPC)
#define CT_ENT    CTL_T(KC_ENT)
#define SF_ENT    SFT_T(KC_ENT)
#define SF_SPC    SFT_T(KC_SPC)
#define SF_SSPC   SFT_T(S(KC_SPC))
#define CT_BS     CTL_T(KC_BSPC)
#define SF_DEL    SFT_T(KC_DEL)
#define SF_BS     SFT_T(KC_BSPC)
#define CT_DEL    CTL_T(KC_DEL)
#define CT_NOFF   CTL_T(NG_OFF)
#define SF_NON    SFT_T(NG_ON)
#define CT_TAB    CTL_T(KC_TAB)
#define SF_ESC    SFT_T(KC_ESC)
#define SF_MINS   SFT_T(JP_MINS)
#define ALT_SLSH  LALT_T(JP_SLSH)
#define SF_A      SFT_T(KC_A)
#define TH_B_GUI  LGUI_T(KC_B)
#define TH_N_ALT  LALT_T(KC_N)
#define LOWER LT(_LOWER,KC_SPC)
#define RAISE LT(_RAISE,KC_ENT)
// #define LOWER LT(_LOWER,JP_HENK)
// #define RAISE LT(_RAISE,JP_MHEN)se

#define CTL_1 LCTL(KC_1)
#define SCOPY LGUI(S(KC_S))

#define CAD  LCTL(LALT(KC_DEL))
#define ALT_TAB  LALT(KC_TAB)



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

   [_QWERTY] = LAYOUT( \
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_Q,     KC_W,     KC_E,      KC_R,     TD_T_TAB,                           TD_Y_ESC, KC_U,      KC_I,     KC_O,     KC_P,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_A,     KC_S,     KC_D,      KC_F,     KC_G,                               KC_H,     KC_J,      KC_K,     KC_L,     JP_MINS,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_Z,     KC_X,     KC_C,      KC_V,     TH_B_GUI,                           TH_N_ALT, KC_M,      KC_COMM,  KC_DOT,   JP_SLSH,
        //,---------+---------+----------+---------+---------+---------+.   ,----------+---------+----------+---------+---------+---------.
                                                    LOWER,    SF_BS,         CT_DEL,   RAISE
        //                                         ,---------+---------+.   ,----------+----------.
        //
    ),

   [_NAGINATA] = LAYOUT( \
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           NG_Q,     NG_W,     NG_E,      NG_R,     NG_T,                               NG_Y,     NG_U,      NG_I,     NG_O,     NG_P,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           NG_A,     NG_S,     NG_D,      NG_F,     NG_G,                               NG_H,     NG_J,      NG_K,     NG_L,     NG_SCLN,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           NG_Z,     NG_X,     NG_C,      NG_V,     NG_B,                               NG_N,     NG_M,      NG_COMM,  NG_DOT,   NG_SLSH,
        //,---------+---------+----------+---------+---------+---------+.   ,----------+---------+----------+---------+---------+---------.
                                                     _______, NG_SHFT,       NG_SHFT2,  _______
        //                                         ,---------+---------+.   ,----------+----------.
    ),

   [_LOWER] =  LAYOUT( \
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           JP_ASTR,  JP_PLUS,  TD_SL_UN,   JP_MINS,  JP_EQL,                            JP_PIPE,  JP_AMPR,   TD_LBRC,  TD_RBRC,  TD_CI_GR,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_1,     KC_2,     KC_3,      KC_4,     KC_5,                               TD_COLN,  TD_AT_TL,  JP_LPRN,  JP_RPRN,  TD_YE_DL,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_6,     KC_7,     KC_8,      KC_9,     KC_0,                               TD_HS_PE, TD_QUOT,   JP_LABK,  JP_RABK,  TD_QU_EX,
        //,---------+---------+----------+---------+---------+---------+.    ,---------+---------+----------+---------+---------+---------.
                                                    _______,   _______,       _______,  ADJUST
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
                                                     ADJUST,  SF_SSPC,         _______,  _______
        //                                         ,---------+---------+.    ,---------+----------.
    ),

   [_ADJUST] = LAYOUT(
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           JP_KANA,  KC_INS,   _______,   MAIL_AD,  LDAP_ID,                            PASS_1,   PASS_2,    _______,  _______,  RESET,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           KC_F1,    _______,  KC_BTN2,   KC_BTN1,  KC_BTN3,                            KC_MS_L,  KC_MS_D,   KC_MS_U,  KC_MS_R,  _______,
        //,---------+---------+----------+---------+---------+.                        ,---------+----------+---------+---------+---------.
           NG_OFF,  _______,  _______,    S(KC_SPC),  JP_KANA,                            KC_WH_L,  KC_WH_D,   KC_WH_U,  KC_WH_R,  NG_ON,
        //,---------+---------+----------+---------+---------+---------+.    ,---------+---------+----------+---------+---------+---------.
                                                    _______,  _______,        _______,  _______
        //                                         ,---------+---------+.    ,---------+----------.
    ),

};


void matrix_init_user(void) {
//void keyboard_post_init_user(void) {
  // 薙刀式
  uint16_t ngonkeys[] = {KC_J, KC_K};
  uint16_t ngoffkeys[] = {KC_D, KC_F};
  set_naginata(_NAGINATA, ngonkeys, ngoffkeys);
   // set_naginata(_NAGINATA);
  // 薙刀式
}

//Shift+特定のキーの押下判定用
static bool shift_pressed = false;
static bool exceptional_shift_pressed = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {


   switch (keycode) {
      case KC_LSFT:
      case KC_RSFT:
      case SF_SPC:
      case NG_SHFT:
         if (record->event.pressed) {
            shift_pressed = true;
         } else {
            shift_pressed = false;
         }
         break;
      case NG_ON:
      case JP_HENK:
        if(record->event.pressed) {
            naginata_on();
        }
        break;
      case NG_OFF:
      case JP_MHEN:
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
               case JP_SLSH:
               case JP_MINS:
                  if(record->event.pressed) {
                     register_code(KC_LSFT);
                     register_code(JP_BSLS);
                     exceptional_shift_pressed = true;
                  } else {
                     unregister_code(JP_BSLS);
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

