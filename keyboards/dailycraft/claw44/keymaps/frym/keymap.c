#include QMK_KEYBOARD_H
#include "keymap_japanese.h"
#include "secrets.h"
#include "naginata.h"

#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

  #include "ssd1306.h"

#define _QWERTY 0
#define _NAGINATA 1
#define _LOWER 2
#define _RAISE 3
#define _ADJUST 4



extern uint8_t is_master;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.

enum custom_keycodes {
    QWERTY = NG_SAFE_RANGE,
    LOWER,
    RAISE,
    ADJUST,
    LDAP_ID,
    MAIL_AD,
    PASS_1,
    PASS_2,
    NG_ON,
    NG_OFF
};


//Tap Dance:キーの二度押し
//TAP_DANCE_ENABLE = yes

enum {
    TDD_DQUO_QUOT = 0,
    TDD_LBRC_RBRC,
    TDD_LCBR_RCBR,
    TDD_LPRN_RPRN,
    TDD_CIRC_GRV,
    TDD_COLN_SCLN,
    TDD_YEN_DLR,
    TDD_PERC_HASH,
    TDD_AT_TILD,
    TDD_MINS_UNDS,
};

qk_tap_dance_action_t tap_dance_actions[] = {
    [TDD_DQUO_QUOT] = ACTION_TAP_DANCE_DOUBLE(JP_DQUO, JP_QUOT),
    [TDD_LBRC_RBRC] = ACTION_TAP_DANCE_DOUBLE(JP_LBRC, JP_RBRC),
    [TDD_LCBR_RCBR] = ACTION_TAP_DANCE_DOUBLE(JP_LCBR, JP_RCBR),
    [TDD_LPRN_RPRN] = ACTION_TAP_DANCE_DOUBLE(JP_LPRN, JP_RPRN),
    [TDD_CIRC_GRV]  = ACTION_TAP_DANCE_DOUBLE(JP_CIRC, JP_GRV),
    [TDD_COLN_SCLN] = ACTION_TAP_DANCE_DOUBLE(JP_COLN, JP_SCLN),
    [TDD_YEN_DLR]   = ACTION_TAP_DANCE_DOUBLE(JP_YEN,  JP_DLR),
    [TDD_MINS_UNDS] = ACTION_TAP_DANCE_DOUBLE(JP_MINS, JP_UNDS),
    [TDD_AT_TILD]   = ACTION_TAP_DANCE_DOUBLE(JP_AT,   JP_TILD),
    [TDD_PERC_HASH] = ACTION_TAP_DANCE_DOUBLE(JP_HASH, JP_PERC),
};



#define TD_DQUO  TD(TDD_DQUO_QUOT)
#define TD_BRC   TD(TDD_LBRC_RBRC)
#define TD_CBR   TD(TDD_LCBR_RCBR)
#define TD_PRN   TD(TDD_LPRN_RPRN)
#define TD_CI_GR TD(TDD_CIRC_GRV)
#define TD_COLN  TD(TDD_COLN_SCLN)
#define TD_YE_DL TD(TDD_YEN_DLR)
#define TD_PE_HS TD(TDD_PERC_HASH)
#define TD_AT_TL TD(TDD_AT_TILD)
#define TD_MI_UD TD(TDD_MINS_UNDS)





/* enum macro_keycodes {
  KC_SAMPLEMACRO,
};
 */
#define KC_ KC_TRNS
#define KC_RST RESET

#define CT_TAB  CTL_T(KC_TAB)
#define SF_SPACE  LSFT_T(KC_SPACE)
#define SF_ENTER  LSFT_T(KC_ENTER)
#define CT_SPACE  CTL_T(KC_SPACE)
#define CT_ENTER  CTL_T(KC_ENTER)
#define SF_ESC  SFT_T(KC_ESC)
#define CT_BS   CTL_T(KC_BSPC)
#define SF_BS  SFT_T(KC_BSPC)
#define SF_DEL  SFT_T(KC_DEL)
#define CT_DEL  CTL_T(KC_DEL)
#define CT_ALEN CTL_T(LALT(KC_ENT)) // shift / Alt+Enter
#define KC_ALEN LALT(KC_ENT)
#define LGUI_TAB LGUI_T(KC_TAB)
#define LALT_ESC LALT_T(KC_ESC)
#define LGUI_BS  LGUI_T(KC_BSPC)
#define LALT_DEL LALT_T(KC_DEL)

#define SF_LEFT  S(KC_LEFT)
#define SF_RGHT  S(KC_RIGHT)
#define SF_UP    S(KC_UP)
#define SF_DOWN  S(KC_DOWN)
#define SF_HOME  S(KC_HOME)
#define SF_PGDN  S(KC_PGDN)
#define SF_PGUP  S(KC_PGUP)
#define SF_END   S(KC_END)


#define SF_A  LSFT_T(KC_A) // shift / A
#define SF_G  LSFT_T(KC_G) // shift / G
#define SF_H  LSFT_T(KC_H) // shift / H
#define SF_MINS LSFT_T(JP_MINS)

#define SF_MHEN LSFT_T(JP_MHEN)
#define SF_HENK LSFT_T(JP_HENK)

#define CTL_1 LSFT_T(LCTL(KC_1))
#define SCOPY LGUI(S(KC_S))

//#define TH_F_NOF MT(NG_OFF, KC_F)
//#define TH_J_NON MT(NG_ON,  KC_J)

/*
enum combo_events {
  PRN,
  BRC,
  CBR,
  QUOT,
  DQUOT,
  MENTION,
  COMBO_LENGTH
};

uint16_t COMBO_LEN = COMBO_LENGTH;


const uint16_t PROGMEM combo_prn[]   = {KC_K, KC_L, COMBO_END};
const uint16_t PROGMEM combo_brc[]   = {KC_I, KC_O, COMBO_END};
const uint16_t PROGMEM combo_cbr[]   = {KC_COMM, KC_DOT, COMBO_END};
const uint16_t PROGMEM combo_at[]    = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM combo_quot[]  = {KC_U, KC_I, COMBO_END};
const uint16_t PROGMEM combo_dquot[] = {KC_M, KC_COMM, COMBO_END};

combo_t key_combos[] = {
    [PRN]     = COMBO_ACTION(combo_prn),
    [BRC]     = COMBO_ACTION(combo_brc),
    [CBR]     = COMBO_ACTION(combo_cbr),
    [QUOT]    = COMBO_ACTION(combo_quot),
    [DQUOT]   = COMBO_ACTION(combo_dquot),
    [MENTION] = COMBO(combo_at , JP_AT), 
};


 void process_combo_event(uint16_t combo_index, bool pressed) {
  switch (combo_index) {
    case PRN:
      if (pressed) {
        tap_code16(JP_LPRN);
        tap_code16(JP_RPRN);
        tap_code16(KC_LEFT);
      }
      break;
    case BRC:
      if (pressed) {
        tap_code16(JP_LBRC);
        tap_code16(JP_RBRC);
        tap_code16(KC_LEFT);
      }
      break;
    case CBR:
      if (pressed) {
        tap_code16(JP_LCBR);
        tap_code16(JP_RCBR);
        tap_code16(KC_LEFT);
      }
      break;
    case QUOT:
      if (pressed) {
        tap_code16(JP_QUOT);
        tap_code16(JP_QUOT);
        tap_code16(KC_LEFT);
      }
      break;
    case DQUOT:
      if (pressed) {
        tap_code16(JP_DQUO);
        tap_code16(JP_DQUO);
        tap_code16(KC_LEFT);
      }
      break;
  }
}

 */


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_QWERTY] = LAYOUT( \
  //,--------+---------+---------+---------+---------+---------.   ,---------+---------+--------+---------+--------+--------.
     _______,  KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,         KC_Y,     KC_U,     KC_I,    KC_O,     KC_P,   _______,
  //|--------+---------+---------+---------+---------+---------|   |---------+---------+--------+---------+--------+--------|
     KC_Q,     KC_A,     KC_S    , KC_D,     KC_F,     KC_G,         KC_H,     KC_J,     KC_K,    KC_L,     JP_MINS, KC_P, 
  //|--------+---------+---------+---------+---------+---------|   |---------+---------+--------+---------+--------+--------|
     _______,  KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,         KC_N,     KC_M,     KC_COMM, KC_DOT,   JP_SLSH, _______,
  //`--------+---------+---------+---------+---------+---------/   \---------+---------+--------+---------+--------+--------'
                      // LGUI_TAB,  LOWER,     KC_SPC,   CT_BS,        SF_DEL,   KC_ENTER, RAISE,   LALT_ESC
                       LGUI_TAB,  LOWER,     SF_SPACE, CT_BS,        CT_DEL,   SF_ENTER, RAISE,   LALT_ESC
   //                  `----------+---------+---------+---------'   `---------+---------+--------+---------'
  ),

[_NAGINATA] = LAYOUT( \
  // 
  //,--------+--------+---------+--------+---------+--------.   ,--------+---------+--------+---------+--------+--------.
     _______,  NG_Q   , NG_W    , NG_E   , NG_R    , NG_T   ,     NG_Y   , NG_U    , NG_I   , NG_O    , NG_P   , _______,
  //|--------+--------+---------+--------+---------+--------|   |--------+---------+--------+---------+--------+--------|
     NG_Q,     NG_A   , NG_S    , NG_D   , NG_F    , NG_G   ,     NG_H   , NG_J    , NG_K   , NG_L    , NG_SCLN,  NG_P,
  //|--------+--------+---------+--------+---------+--------|   |--------+---------+--------+---------+--------+--------|
     _______,  NG_Z,    NG_X    , NG_C   , NG_V    , NG_B   ,     NG_N   , NG_M    , NG_COMM, NG_DOT  , NG_SLSH, _______,
  //`--------+--------+--------+--------+----------+--------/   \---------+---------+--------+---------+--------+--------'
                        _______,  _______, _______,  _______,     _______, _______,  _______, _______
                        //  _______,  _______, NG_SHFT, _______,  _______, NG_SHFT2, _______, _______
  //                 `----------+--------+---------,+--------'   `--------+---------+--------+---------'
  ),

  [_LOWER] = LAYOUT( \
  //,--------+--------+--------+--------+--------+--------.   ,--------+--------+--------+--------+--------+--------.
     _______, JP_SLSH, JP_MINS, JP_ASTR, JP_PLUS, JP_EQL,      JP_PIPE, JP_AMPR, TD_BRC,  TD_CBR,  TD_CI_GR, _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     JP_SLSH,  KC_1   , KC_2   , KC_3   , KC_4   , KC_5   ,    TD_COLN, TD_AT_TL,TD_PRN,  TD_PE_HS, JP_EXLM, TD_CI_GR,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     NG_OFF,   KC_6   , KC_7   , KC_8   , KC_9   , KC_0   ,     TD_YE_DL,TD_DQUO, JP_LABK, JP_RABK, JP_QUES, _______,
  //`--------+--------+--------+--------+--------+--------/   \--------+--------+--------+--------+--------+--------'
                        _______,  _______, _______,  _______,     _______, _______,  _______, _______
  //                  `--------+--------+--------+--------'   `--------+--------+--------+--------'
  ),

  [_RAISE] = LAYOUT( \
  //,--------+--------+--------+--------+--------+--------.   ,--------+--------+--------+--------+--------+--------.
     _______, _______, _______, _______, KC_F11,  KC_F12,      KC_APP,  KC_BSPC, KC_DEL,  SCOPY,   _______, _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, CTL_1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,       KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_RSFT, _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,      KC_HOME, KC_PGDN, KC_PGUP, KC_END,  JP_KANA, NG_ON,
  //`--------+--------+--------+--------+--------+--------/   \--------+--------+--------+--------+--------+--------'
                        _______, _______, _______,_______,     _______, _______,  _______, _______
  //                  `--------+--------+--------+--------'   `--------+--------+--------+--------'
  ),

  [_ADJUST] = LAYOUT( \
  //,--------+--------+--------+--------+--------+--------.   ,--------+--------+--------+--------+--------+--------.
     _______, RESET,   KC_INS,  _______, LDAP_ID, MAIL_AD,     PASS_1,  PASS_2,  _______, _______, RESET,   _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, _______, _______, KC_VOLD, KC_VOLU, _______,     SF_LEFT, SF_DOWN, SF_UP,   SF_RGHT, _______, _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, _______, _______, _______, _______, _______,     SF_HOME, SF_PGDN, SF_PGUP, SF_END,  _______, _______,
  //`--------+--------+--------+--------+--------+--------/   \--------+--------+--------+--------+--------+--------'
                       _______, _______, _______, _______,     _______, _______, _______, _______
  //                  `--------+--------+--------+--------'   `--------+--------+--------+--------'
  ),

};

int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}


void matrix_init_user(void) {
  //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
  #ifdef SSD1306OLED
    iota_gfx_init(!has_usb());   // turns on the display
  #endif

  // 薙刀式
  uint16_t ngonkeys[]  = {KC_H, KC_J};
  uint16_t ngoffkeys[] = {KC_F, KC_G};
  set_naginata(_NAGINATA, ngonkeys, ngoffkeys);
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo(void);
void set_keylog(uint16_t keycode, keyrecord_t *record);
const char *read_keylog(void);
const char *read_keylogs(void);

// const char *read_mode_icon(bool swap);
// const char *read_host_led_state(void);
// void set_timelog(void);
// const char *read_timelog(void);

void matrix_scan_user(void) {
  iota_gfx_task();
}

void matrix_render_user(struct CharacterMatrix *matrix) {
  if (is_master) {
    // If you want to change the display of OLED, you need to change here
    matrix_write_ln(matrix, read_layer_state());
    matrix_write_ln(matrix, read_keylog());
    matrix_write_ln(matrix, read_keylogs());
    //matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
    //matrix_write_ln(matrix, read_host_led_state());
    //matrix_write_ln(matrix, read_timelog());
  } else {
    matrix_write(matrix, read_logo());
  }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;
  matrix_clear(&matrix);
  matrix_render_user(&matrix);
  matrix_update(&display, &matrix);
}
#endif//SSD1306OLEDv



/* uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD_Q_TAB:
        case TD_G_MH:
        case TD_H_HE:
        case TD_W_ESC:
        case TD_Z_UND:
        case TD_X_CUT:
        case TD_C_CPY:
        case TD_V_PST:
            return 150;
        default:
            return TAPPING_TERM;
    }
}
 */
// Lower, Raiseのtap判定用
static bool lower_pressed = false;
static uint16_t lower_pressed_time = 0;
static bool raise_pressed = false;
static uint16_t raise_pressed_time = 0;

//Shift+特定のキーの押下判定用
static bool shift_pressed = false;
static bool exceptional_shift_pressed = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
#ifdef SSD1306OLED
    set_keylog(keycode, record);
#endif
    // set_timelog();
  }

  switch (keycode) {
    case KC_LSFT:
    case KC_RSFT:
    case SF_SPACE:
    case SF_ENTER:
    case SF_DEL:
        if (record->event.pressed) {
            shift_pressed = true;
        } else {
            shift_pressed = false;
        }
        break;
    /* case QWERTY:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case EUKTN:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_EUKTN);
      }
      return false;
      break; */
    case LOWER:
      if (record->event.pressed) {
        lower_pressed = true;
        lower_pressed_time = record->event.time;
        layer_on(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);

/*         //一瞬だけ押したとき:IMEオフ
        if (lower_pressed && (TIMER_DIFF_16(record->event.time, lower_pressed_time) < TAPPING_TERM)) {
          uint8_t code_for_tap = JP_MHEN;
          register_code(code_for_tap);
          unregister_code(code_for_tap);
          naginata_off();
        }
*/
      }
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        raise_pressed = true;
        raise_pressed_time = record->event.time;
        layer_on(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);

        //一瞬押すなら変換
/*         if (raise_pressed && (TIMER_DIFF_16(record->event.time, raise_pressed_time) < TAPPING_TERM)) {
          uint8_t code_for_tap = JP_HENK; //keycode == RAISE ? KC_ENT : JP_MHEN;
          register_code(code_for_tap);
          unregister_code(code_for_tap);
          naginata_on();
        } */

      }
      return false;
      break;
    case ADJUST:
      if (record->event.pressed) {
        layer_on(_ADJUST);
      } else {
        layer_off(_ADJUST);
      }
      return false;
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
    case NG_ON:
        //if(record->event.pressed) {
            naginata_on();
        //}
        break;
    case NG_OFF:
        //if(record->event.pressed) {
            naginata_off();
        //}
        break;
    default:
        if (shift_pressed || exceptional_shift_pressed){
            switch(keycode){
                case TD_MI_UD:
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
