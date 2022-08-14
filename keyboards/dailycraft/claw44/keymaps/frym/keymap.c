#include QMK_KEYBOARD_H
#include "keymap_japanese.h"
#include "secrets.h"

#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

#define _QWERTY 0
#define _NAGINATA 1
#define _LOWER 2
#define _RAISE 3
#define _ADJUST 4

#include "naginata.h"
NGKEYS naginata_keys;





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
    TDD_G_TAB = 0,
    TDD_H_ESC,
    TDD_G_NOF,
    TDD_H_NON,
    TDD_F_MHEN,
    TDD_J_HENK,
    TDD_Z_UNDO,
    TDD_X_CUT,
    TDD_C_COPY,
    TDD_V_PASTE,
    TDD_QUES_EXLM,
    TDD_QUES_CIRC,
    TDD_EXLM_TILD,
    TDD_QUOT_DQUO,
    TDD_LBRC_LCBR,
    TDD_RBRC_RCBR,
    TDD_LPRN_LBRC,
    TDD_RPRN_RBRC,
    TDD_CIRC_TILD,
    TDD_CIRC_GRV,
    TDD_COLN_SCLN,
    TDD_YEN_DLR,
    TDD_PERC_GRV,
    TDD_AT_HASH,
    TDD_HASH_PERC,
    TDD_AT_GRV,
    TDD_AT_TILD,
    TDD_MINS_UNDS,
    TDD_TAB_COM,
    TDD_SFT_DOT,
    TDD_ESC_ENT,
    TDD_YEN_UNDS,
};

qk_tap_dance_action_t tap_dance_actions[] = {
    [TDD_G_TAB]     = ACTION_TAP_DANCE_DOUBLE(KC_G,    KC_TAB),
    [TDD_G_NOF]     = ACTION_TAP_DANCE_DOUBLE(KC_G,    NG_OFF),
    [TDD_F_MHEN]    = ACTION_TAP_DANCE_DOUBLE(KC_F,    JP_MHEN),
    [TDD_J_HENK]    = ACTION_TAP_DANCE_DOUBLE(KC_J,    JP_HENK),
    [TDD_H_ESC]     = ACTION_TAP_DANCE_DOUBLE(KC_H,    KC_ESC),
    [TDD_H_NON]     = ACTION_TAP_DANCE_DOUBLE(KC_H,    NG_ON),
    [TDD_Z_UNDO]    = ACTION_TAP_DANCE_DOUBLE(KC_Z,    LCTL(KC_Z)),
    [TDD_X_CUT]     = ACTION_TAP_DANCE_DOUBLE(KC_X,    LCTL(KC_X)),
    [TDD_C_COPY]    = ACTION_TAP_DANCE_DOUBLE(KC_C,    LCTL(KC_C)),
    [TDD_V_PASTE]   = ACTION_TAP_DANCE_DOUBLE(KC_V,    LCTL(KC_V)),
    [TDD_QUES_EXLM] = ACTION_TAP_DANCE_DOUBLE(JP_QUES, JP_EXLM),
    [TDD_QUES_CIRC] = ACTION_TAP_DANCE_DOUBLE(JP_QUES, JP_CIRC),
    [TDD_EXLM_TILD] = ACTION_TAP_DANCE_DOUBLE(JP_EXLM, JP_TILD),
    [TDD_QUOT_DQUO] = ACTION_TAP_DANCE_DOUBLE(JP_QUOT, JP_DQUO),
    [TDD_LBRC_LCBR] = ACTION_TAP_DANCE_DOUBLE(JP_LBRC, JP_LCBR),
    [TDD_RBRC_RCBR] = ACTION_TAP_DANCE_DOUBLE(JP_RBRC, JP_RCBR),
    [TDD_LPRN_LBRC] = ACTION_TAP_DANCE_DOUBLE(JP_LPRN, JP_LBRC),
    [TDD_RPRN_RBRC] = ACTION_TAP_DANCE_DOUBLE(JP_RPRN, JP_RBRC),
    [TDD_CIRC_TILD] = ACTION_TAP_DANCE_DOUBLE(JP_CIRC, JP_TILD),
    [TDD_CIRC_GRV]  = ACTION_TAP_DANCE_DOUBLE(JP_CIRC, JP_GRV),
    [TDD_COLN_SCLN] = ACTION_TAP_DANCE_DOUBLE(JP_COLN, JP_SCLN),
    [TDD_YEN_DLR]   = ACTION_TAP_DANCE_DOUBLE(JP_YEN,  JP_DLR),
    [TDD_PERC_GRV]  = ACTION_TAP_DANCE_DOUBLE(JP_PERC, JP_GRV),
    [TDD_MINS_UNDS] = ACTION_TAP_DANCE_DOUBLE(JP_MINS, JP_UNDS),
    [TDD_AT_HASH]   = ACTION_TAP_DANCE_DOUBLE(JP_AT,   JP_HASH),
    [TDD_AT_GRV]    = ACTION_TAP_DANCE_DOUBLE(JP_AT,   JP_GRV),
    [TDD_AT_TILD]   = ACTION_TAP_DANCE_DOUBLE(JP_AT,   JP_TILD),
    [TDD_HASH_PERC] = ACTION_TAP_DANCE_DOUBLE(JP_HASH, JP_PERC),
    [TDD_TAB_COM]   = ACTION_TAP_DANCE_DOUBLE(KC_TAB,  KC_COMM),
    [TDD_SFT_DOT]   = ACTION_TAP_DANCE_DOUBLE(KC_LSFT, KC_DOT),
    [TDD_ESC_ENT]   = ACTION_TAP_DANCE_DOUBLE(KC_ESC,  KC_ENT),
    [TDD_YEN_UNDS]  = ACTION_TAP_DANCE_DOUBLE(JP_YEN,  JP_UNDS),
};



#define TD_G_TAB TD(TDD_G_TAB)
#define TD_F_MH  TD(TDD_F_MHEN)
#define TD_J_HE  TD(TDD_J_HENK)
#define TD_H_ESC TD(TDD_H_ESC)
#define TD_Z_UND TD(TDD_Z_UNDO)
#define TD_X_CUT TD(TDD_X_CUT)
#define TD_C_CPY TD(TDD_C_COPY)
#define TD_V_PST TD(TDD_V_PASTE)
#define TD_QU_EX TD(TDD_QUES_EXLM)
#define TD_QU_CI TD(TDD_QUES_CIRC)
#define TD_EX_TL TD(TDD_EXLM_TILD)
#define TD_QUOT  TD(TDD_QUOT_DQUO)
#define TD_LBRC  TD(TDD_LBRC_LCBR)
#define TD_RBRC  TD(TDD_RBRC_RCBR)
#define TD_LPRN  TD(TDD_LPRN_LBRC)
#define TD_RPRN  TD(TDD_RPRN_RBRC)
#define TD_CI_TL TD(TDD_CIRC_TILD)
#define TD_CI_GR TD(TDD_CIRC_GRV)
#define TD_COLN  TD(TDD_COLN_SCLN)
#define TD_YE_DL TD(TDD_YEN_DLR)
#define TD_PE_GR TD(TDD_PERC_GRV)
#define TD_AT_HS TD(TDD_AT_HASH)
#define TD_HS_PE TD(TDD_HASH_PERC)
#define TD_AT_GR TD(TDD_AT_GRV)
#define TD_AT_TL TD(TDD_AT_TILD)
#define TD_MI_UD TD(TDD_MINS_UNDS)
#define TD_TBCM  TD(TDD_TAB_COM)
#define TD_SFDT  TD(TDD_SFT_DOT)
#define TD_ESEN  TD(TDD_ESC_ENT)





/* enum macro_keycodes {
  KC_SAMPLEMACRO,
};
 */
#define KC_ KC_TRNS
#define KC_RST RESET

#define CT_TAB  CTL_T(KC_TAB)
#define SF_SPACE  LSFT_T(KC_SPACE)
#define SF_ENTER  LSFT_T(KC_ENTER)
#define SF_ESC  SFT_T(KC_ESC)
#define CT_BS   CTL_T(KC_BSPC)
#define SF_DEL  SFT_T(KC_DEL)
#define CT_DEL  CTL_T(KC_DEL)
#define CT_ALEN CTL_T(LALT(KC_ENT)) // shift / Alt+Enter
#define KC_ALEN LALT(KC_ENT)
#define LGUI_TAB LGUI_T(KC_TAB)
#define LALT_ESC LALT_T(KC_ESC)
#define LGUI_BS  LGUI_T(KC_BSPC)
#define LALT_DEL LALT_T(KC_DEL)

#define SF_A  LSFT_T(KC_A) // shift / A
#define SF_G  LSFT_T(KC_G) // shift / G
#define SF_H  LSFT_T(KC_H) // shift / H
#define SF_MINS LSFT_T(JP_MINS)

#define SF_MHEN LSFT_T(JP_MHEN)
#define SF_HENK LSFT_T(JP_HENK)

#define CTL_1 LSFT_T(LCTL(KC_1))
#define SCOPY LGUI(S(KC_S))

#define TH_F_NOF MT(NG_OFF, KC_F)
#define TH_J_NON MT(NG_ON,  KC_J)


const uint16_t PROGMEM combo_lprn[]  = {KC_F, KC_G, COMBO_END};
const uint16_t PROGMEM combo_rprn[]  = {KC_H, KC_J, COMBO_END};
const uint16_t PROGMEM combo_lbrc[]  = {KC_R, KC_T, COMBO_END};
const uint16_t PROGMEM combo_rbrc[]  = {KC_Y, KC_U, COMBO_END};
const uint16_t PROGMEM combo_lcbr[]  = {KC_V, KC_B, COMBO_END};
const uint16_t PROGMEM combo_rcbr[]  = {KC_N, KC_M, COMBO_END};
const uint16_t PROGMEM combo_at[]    = {KC_J, KC_K, COMBO_END};
combo_t key_combos[COMBO_COUNT] = {
    COMBO(combo_lprn, JP_LPRN),
    COMBO(combo_rprn, JP_RPRN),
    COMBO(combo_lbrc, JP_LBRC),
    COMBO(combo_rbrc, JP_RBRC),
    COMBO(combo_lcbr, JP_LCBR),
    COMBO(combo_rcbr, JP_RCBR),
    COMBO(combo_at , JP_AT), // keycodes with modifiers are possible too!
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_QWERTY] = LAYOUT( \
  //,--------+---------+---------+---------+---------+---------.   ,---------+---------+--------+---------+--------+--------.
     KC_TAB,  KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,         KC_Y,     KC_U,     KC_I,    KC_O,     KC_P,    TD_COLN,
  //|--------+---------+---------+---------+---------+---------|   |---------+---------+--------+---------+--------+--------|
     KC_LSFT, KC_A,     KC_S    , KC_D,     KC_F,     KC_G,         KC_H,     KC_J,     KC_K,    KC_L,     JP_MINS, KC_RSFT,
  //|--------+---------+---------+---------+---------+---------|   |---------+---------+--------+---------+--------+--------|
     KC_LALT, KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,         KC_N,     KC_M,     KC_COMM, KC_DOT,   JP_SLSH, JP_BSLS,
  //`--------+---------+---------+---------+---------+---------/   \---------+---------+--------+---------+--------+--------'
                       LGUI_TAB,  LOWER,    SF_SPACE, CT_BS,        CT_DEL,   SF_ENTER, RAISE,   LALT_ESC
  //                  `----------+---------+---------+---------'   `---------+---------+--------+---------'
  ),

  [_NAGINATA] = LAYOUT( \
  // 左右反転
  //,--------+--------+---------+--------+---------+--------.   ,--------+---------+--------+---------+--------+--------.
     _______,  NG_P   , NG_O    , NG_I   , NG_U    , NG_Y   ,     NG_T   , NG_R    , NG_E   , NG_W    , NG_Q   , _______,
  //|--------+--------+---------+--------+---------+--------|   |--------+---------+--------+---------+--------+--------|
     _______,  NG_SCLN, NG_L    , NG_K   , NG_J    , NG_H   ,     NG_G   , NG_F    , NG_D   , NG_S    , NG_A,    _______,
  //|--------+--------+---------+--------+---------+--------|   |--------+---------+--------+---------+--------+--------|
     _______,  NG_SLSH, NG_DOT  , NG_COMM, NG_M    , NG_N   ,     NG_B   , NG_V    , NG_C   , NG_X  ,  NG_Z,     _______,
  //`--------+--------+--------+--------+----------+--------/   \---------+---------+--------+---------+--------+--------'
                        _______,  _______, NG_SHFT,  _______,     _______, NG_SHFT2, _______, _______
  //                 `----------+--------+---------,+--------'   `--------+---------+--------+---------'
  ),

  [_LOWER] = LAYOUT( \
  //,--------+--------+--------+--------+--------+--------.   ,--------+--------+--------+--------+--------+--------.
     _______, JP_PLUS, JP_ASTR, JP_SLSH, JP_MINS, JP_EQL,      JP_AMPR, JP_PIPE,TD_LBRC, TD_RBRC, TD_CI_GR,_______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______,  KC_1   , KC_2   , KC_3   , KC_4   , KC_5   ,    TD_AT_TL,TD_COLN, TD_LPRN, TD_RPRN, TD_YE_DL, JP_UNDS,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     NG_OFF,  KC_6   , KC_7   , KC_8   , KC_9   , KC_0   ,     TD_QUOT, TD_HS_PE,JP_LABK, JP_RABK, TD_QU_EX,_______,
  //`--------+--------+--------+--------+--------+--------/   \--------+--------+--------+--------+--------+--------'
                       LGUI_TAB,  LOWER, SF_SPACE, CT_BS,      CT_DEL,  SF_ENTER, RAISE, LALT_ESC
  //                  `--------+--------+--------+--------'   `--------+--------+--------+--------'
  ),

  [_RAISE] = LAYOUT( \
  //,--------+--------+--------+--------+--------+--------.   ,--------+--------+--------+--------+--------+--------.
     _______, _______, _______, _______, KC_F11,  KC_F12,      KC_APP,  KC_BSPC, KC_DEL,  SCOPY,   _______, _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, CTL_1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,       KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_RSFT, _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,      KC_HOME, KC_PGDN, KC_PGUP, KC_END,  KC_RALT,  NG_ON,
  //`--------+--------+--------+--------+--------+--------/   \--------+--------+--------+--------+--------+--------'
                       LGUI_TAB,  LOWER, SF_SPACE, CT_BS,      CT_DEL,   SF_ENTER, RAISE, LALT_ESC
  //                  `--------+--------+--------+--------'   `--------+--------+--------+--------'
  ),

  [_ADJUST] = LAYOUT( \
  //,--------+--------+--------+--------+--------+--------.   ,--------+--------+--------+--------+--------+--------.
     _______, RESET,   KC_INS,  _______, MAIL_AD, LDAP_ID,     PASS_1,  KC_VOLD, KC_VOLU, PASS_2,  RESET,   _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, KC_F1,   _______, KC_BTN2, KC_BTN1, KC_BTN3,     KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R, _______, _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, NG_OFF, _______, _______, _______,  JP_KANA,     KC_WH_L, KC_WH_D, KC_WH_U, KC_WH_R, NG_ON,   _______,
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
  //uint16_t ngonkeys[]  = {KC_J, KC_K};
  //uint16_t ngoffkeys[] = {KC_D, KC_F};
  set_naginata(_NAGINATA);
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
        if (record->event.pressed) {
            shift_pressed = true;
        } else {
            shift_pressed = false;
        }
        break;
    case QWERTY:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case LOWER:
      if (record->event.pressed) {
        lower_pressed = true;
        lower_pressed_time = record->event.time;
        layer_on(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);

        //一瞬だけ押したとき
        if (lower_pressed && (TIMER_DIFF_16(record->event.time, lower_pressed_time) < TAPPING_TERM)) {
          uint8_t code_for_tap = JP_MHEN; //keycode == LOWER ? KC_SPC : JP_HENK;
          register_code(code_for_tap);
          unregister_code(code_for_tap);
        }

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

        //一瞬押すなら変換（_SはBS）
        if (raise_pressed && (TIMER_DIFF_16(record->event.time, raise_pressed_time) < TAPPING_TERM)) {
          uint8_t code_for_tap = JP_HENK; //keycode == RAISE ? KC_ENT : JP_MHEN;
          register_code(code_for_tap);
          unregister_code(code_for_tap);
        }

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
