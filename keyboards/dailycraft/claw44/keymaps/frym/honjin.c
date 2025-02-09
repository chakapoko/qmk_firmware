/* Copyright 2018-2019 eswai <@eswai>
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
#include "honjin.h"

#if !defined(__AVR__)
  #include <string.h>
  // #define memcpy_P(des, src, len) memcpy(des, src, len)
#endif

#define CANCEL_TIME 200 //shift1,shift2のキャンセル時間
#define NGBUFFER 10 // キー入力バッファのサイズ

#ifdef NAGINATA_TATEGAKI
  #define NGUP X_UP
  #define NGDN X_DOWN
  #define NGLT X_LEFT
  #define NGRT X_RIGHT
  #define NGKUP KC_UP
  #define NGKDN KC_DOWN
  #define NGKLT KC_LEFT
  #define NGKRT KC_RIGHT
#endif
#ifdef NAGINATA_YOKOGAKI
  #define NGUP X_LEFT
  #define NGDN X_RIGHT
  #define NGLT X_DOWN
  #define NGRT X_UP
  #define NGKUP KC_LEFT
  #define NGKDN KC_RIGHT
  #define NGKLT KC_DOWN
  #define NGKRT KC_UP
#endif

//#define NAGINATA_EDIT_WIN

static uint8_t ng_chrcount = 0; // 文字キー入力のカウンタ
static bool is_naginata = false; // 薙刀式がオンかオフか
static uint8_t naginata_layer = 0; // NG_*を配置しているレイヤー番号
static uint32_t keycomb = 0UL; // 同時押しの状態を示す。32bitの各ビットがキーに対応する。
static uint16_t ngon_keys[2]; // 薙刀式をオンにするキー(通常HJ)
static uint16_t ngoff_keys[2]; // 薙刀式をオフにするキー(通常FG)

// 31キーを32bitの各ビットに割り当てる
#define B_Q    (1UL<<0)
#define B_W    (1UL<<1)
#define B_E    (1UL<<2)
#define B_R    (1UL<<3)
#define B_T    (1UL<<4)

#define B_Y    (1UL<<5)
#define B_U    (1UL<<6)
#define B_I    (1UL<<7)
#define B_O    (1UL<<8)
#define B_P    (1UL<<9)

#define B_A    (1UL<<10)
#define B_S    (1UL<<11)
#define B_D    (1UL<<12)
#define B_F    (1UL<<13)
#define B_G    (1UL<<14)

#define B_H    (1UL<<15)
#define B_J    (1UL<<16)
#define B_K    (1UL<<17)
#define B_L    (1UL<<18)
#define B_SCLN (1UL<<19)

#define B_Z    (1UL<<20)
#define B_X    (1UL<<21)
#define B_C    (1UL<<22)
#define B_V    (1UL<<23)
#define B_B    (1UL<<24)

#define B_N    (1UL<<25)
#define B_M    (1UL<<26)
#define B_COMM (1UL<<27)
#define B_DOT  (1UL<<28)
#define B_SLSH (1UL<<29)

#define B_SHFT (1UL<<30)
#define B_SHFT2 (1UL<<31)


// 文字入力バッファ
static uint16_t ninputs[NGBUFFER];

// キーコードとキービットの対応
// メモリ削減のため配列はNG_Qを0にしている
const uint32_t ng_key[] = {
  [NG_Q    - NG_Q] = B_Q,
  [NG_W    - NG_Q] = B_W,
  [NG_E    - NG_Q] = B_E,
  [NG_R    - NG_Q] = B_R,
  [NG_T    - NG_Q] = B_T,

  [NG_Y    - NG_Q] = B_Y,
  [NG_U    - NG_Q] = B_U,
  [NG_I    - NG_Q] = B_I,
  [NG_O    - NG_Q] = B_O,
  [NG_P    - NG_Q] = B_P,

  [NG_A    - NG_Q] = B_A,
  [NG_S    - NG_Q] = B_S,
  [NG_D    - NG_Q] = B_D,
  [NG_F    - NG_Q] = B_F,
  [NG_G    - NG_Q] = B_G,

  [NG_H    - NG_Q] = B_H,
  [NG_J    - NG_Q] = B_J,
  [NG_K    - NG_Q] = B_K,
  [NG_L    - NG_Q] = B_L,
  [NG_SCLN - NG_Q] = B_SCLN,

  [NG_Z    - NG_Q] = B_Z,
  [NG_X    - NG_Q] = B_X,
  [NG_C    - NG_Q] = B_C,
  [NG_V    - NG_Q] = B_V,
  [NG_B    - NG_Q] = B_B,

  [NG_N    - NG_Q] = B_N,
  [NG_M    - NG_Q] = B_M,
  [NG_COMM - NG_Q] = B_COMM,
  [NG_DOT  - NG_Q] = B_DOT,
  [NG_SLSH - NG_Q] = B_SLSH,

  [NG_SHFT - NG_Q] = B_SHFT,
  [NG_SHFT2 - NG_Q] = B_SHFT2,
};

// カナ変換テーブル
typedef struct {
  uint32_t key;
  char kana[7];
} naginata_keymap;

// ロング
typedef struct {
  uint32_t key;
  char kana[26];
} naginata_keymap_long;

// UNICODE
typedef struct {
  uint32_t key;
  char uc[10];
} naginata_keymap_unicode;

// IME変換する文字列
typedef struct {
  uint32_t key;
  char kana[25];
} naginata_keymap_ime;

//キーマップ　自動で書き換える
const PROGMEM naginata_keymap ngmap[] = {
  {.key = B_H, .kana = "a"}, //あ
  {.key = B_J, .kana = "i"}, //い
  {.key = B_K, .kana = "u"}, //う
  {.key = B_N|B_SHFT, .kana = "e"}, //え
  {.key = B_M|B_SHFT, .kana = "o"}, //お
  {.key = B_F, .kana = "ka"}, //か
  {.key = B_B, .kana = "ki"}, //き
  {.key = B_G, .kana = "ku"}, //く
  {.key = B_T, .kana = "ke"}, //け
  {.key = B_R, .kana = "ko"}, //こ
  {.key = B_S|B_SHFT, .kana = "sa"}, //さ
  {.key = B_V, .kana = "si"}, //し
  {.key = B_U, .kana = "su"}, //す
  {.key = B_Z|B_SHFT, .kana = "se"}, //せ
  {.key = B_A|B_SHFT, .kana = "so"}, //そ
  {.key = B_COMM, .kana = "ta"}, //た
  {.key = B_X, .kana = "ti"}, //ち
  {.key = B_M, .kana = "tu"}, //つ
  {.key = B_C, .kana = "te"}, //て
  {.key = B_E, .kana = "to"}, //と
  {.key = B_N, .kana = "na"}, //な
  {.key = B_F|B_SHFT, .kana = "ni"}, //に
  {.key = B_Y|B_SHFT, .kana = "nu"}, //ぬ
  {.key = B_W|B_SHFT, .kana = "ne"}, //ね
  {.key = B_V|B_SHFT, .kana = "no"}, //の
  {.key = B_D|B_SHFT, .kana = "ha"}, //は
  {.key = B_W, .kana = "hi"}, //ひ
  {.key = B_I|B_SHFT, .kana = "hu"}, //ふ
  {.key = B_Y, .kana = "he"}, //へ
  {.key = B_Q|B_SHFT, .kana = "ho"}, //ほ
  {.key = B_G|B_SHFT, .kana = "ma"}, //ま
  {.key = B_R|B_SHFT, .kana = "mi"}, //み
  {.key = B_COMM|B_SHFT, .kana = "mu"}, //む
  {.key = B_X|B_SHFT, .kana = "me"}, //め
  {.key = B_C|B_SHFT, .kana = "mo"}, //も
  {.key = B_U|B_SHFT, .kana = "ya"}, //や
  {.key = B_H|B_SHFT, .kana = "yu"}, //ゆ
  {.key = B_J|B_SHFT, .kana = "yo"}, //よ
  {.key = B_Z, .kana = "ra"}, //ら
  {.key = B_E|B_SHFT, .kana = "ri"}, //り
  {.key = B_I, .kana = "ru"}, //る
  {.key = B_S, .kana = "re"}, //れ
  {.key = B_Q, .kana = "ro"}, //ろ
  {.key = B_K|B_SHFT, .kana = "wa"}, //わ
  {.key = B_D, .kana = "nn"}, //ん
  {.key = B_A, .kana = "-"}, //ー
  {.key = B_F|B_SHFT2, .kana = "ga"}, //が
  {.key = B_B|B_SHFT2, .kana = "gi"}, //ぎ
  {.key = B_G|B_SHFT2, .kana = "gu"}, //ぐ
  {.key = B_T|B_SHFT2, .kana = "ge"}, //げ
  {.key = B_R|B_SHFT2, .kana = "go"}, //ご
  {.key = B_S|B_SHFT2, .kana = "za"}, //ざ
  {.key = B_V|B_SHFT2, .kana = "zi"}, //じ
  {.key = B_U|B_SHFT2, .kana = "zu"}, //ず
  {.key = B_Z|B_SHFT2, .kana = "ze"}, //ぜ
  {.key = B_A|B_SHFT2, .kana = "zo"}, //ぞ
  {.key = B_COMM|B_SHFT2, .kana = "da"}, //だ
  {.key = B_X|B_SHFT2, .kana = "di"}, //ぢ
  {.key = B_M|B_SHFT2, .kana = "du"}, //づ
  {.key = B_C|B_SHFT2, .kana = "de"}, //で
  {.key = B_E|B_SHFT2, .kana = "do"}, //ど
  {.key = B_D|B_SHFT2, .kana = "ba"}, //ば
  {.key = B_W|B_SHFT2, .kana = "bi"}, //び
  {.key = B_I|B_SHFT2, .kana = "bu"}, //ぶ
  {.key = B_Y|B_SHFT2, .kana = "be"}, //べ
  {.key = B_Q|B_SHFT2, .kana = "bo"}, //ぼ
  {.key = B_K|B_SHFT2, .kana = "vu"}, //ゔ
  {.key = B_D|B_K, .kana = "pa"}, //ぱ
  {.key = B_W|B_K, .kana = "pi"}, //ぴ
  {.key = B_I|B_D, .kana = "pu"}, //ぷ
  {.key = B_Y|B_D, .kana = "pe"}, //ぺ
  {.key = B_Q|B_K, .kana = "po"}, //ぽ
  {.key = B_U|B_T|B_SHFT, .kana = "lya"}, //ゃ
  {.key = B_H|B_T|B_SHFT, .kana = "lyu"}, //ゅ
  {.key = B_J|B_T|B_SHFT, .kana = "lyo"}, //ょ
  {.key = B_H|B_T, .kana = "la"}, //ぁ
  {.key = B_J|B_T, .kana = "li"}, //ぃ
  {.key = B_K|B_T, .kana = "lu"}, //ぅ
  {.key = B_N|B_T|B_SHFT, .kana = "le"}, //ぇ
  {.key = B_M|B_T|B_SHFT, .kana = "lo"}, //ぉ
  {.key = B_K|B_T|B_SHFT, .kana = "lwa"}, //ゎ
  {.key = B_B|B_SHFT, .kana = "ltu"}, //っ
  {.key = B_V|B_U, .kana = "sya"}, //しゃ
  {.key = B_V|B_H, .kana = "syu"}, //しゅ
  {.key = B_V|B_J, .kana = "syo"}, //しょ
  {.key = B_V|B_U|B_SHFT2, .kana = "zya"}, //じゃ
  {.key = B_V|B_H|B_SHFT2, .kana = "zyu"}, //じゅ
  {.key = B_V|B_J|B_SHFT2, .kana = "zyo"}, //じょ
  {.key = B_B|B_U, .kana = "kya"}, //きゃ
  {.key = B_B|B_H, .kana = "kyu"}, //きゅ
  {.key = B_B|B_J, .kana = "kyo"}, //きょ
  {.key = B_B|B_U|B_SHFT2, .kana = "gya"}, //ぎゃ
  {.key = B_B|B_H|B_SHFT2, .kana = "gyu"}, //ぎゅ
  {.key = B_B|B_J|B_SHFT2, .kana = "gyo"}, //ぎょ
  {.key = B_X|B_U, .kana = "tya"}, //ちゃ
  {.key = B_X|B_H, .kana = "tyu"}, //ちゅ
  {.key = B_X|B_J, .kana = "tyo"}, //ちょ
  {.key = B_X|B_U|B_SHFT2, .kana = "dya"}, //ぢゃ
  {.key = B_X|B_H|B_SHFT2, .kana = "dyu"}, //ぢゅ
  {.key = B_X|B_J|B_SHFT2, .kana = "dyo"}, //ぢょ
  {.key = B_F|B_U, .kana = "nya"}, //にゃ
  {.key = B_F|B_H, .kana = "nyu"}, //にゅ
  {.key = B_F|B_J, .kana = "nyo"}, //にょ
  {.key = B_W|B_U, .kana = "hya"}, //ひゃ
  {.key = B_W|B_H, .kana = "hyu"}, //ひゅ
  {.key = B_W|B_J, .kana = "hyo"}, //ひょ
  {.key = B_W|B_U|B_SHFT2, .kana = "bya"}, //びゃ
  {.key = B_W|B_H|B_SHFT2, .kana = "byu"}, //びゅ
  {.key = B_W|B_J|B_SHFT2, .kana = "byo"}, //びょ
  {.key = B_W|B_U|B_K, .kana = "pya"}, //ぴゃ
  {.key = B_W|B_H|B_K, .kana = "pyu"}, //ぴゅ
  {.key = B_W|B_J|B_K, .kana = "pyo"}, //ぴょ
  {.key = B_R|B_U, .kana = "mya"}, //みゃ
  {.key = B_R|B_H, .kana = "myu"}, //みゅ
  {.key = B_R|B_J, .kana = "myo"}, //みょ
  {.key = B_E|B_U, .kana = "rya"}, //りゃ
  {.key = B_E|B_H, .kana = "ryu"}, //りゅ
  {.key = B_E|B_J, .kana = "ryo"}, //りょ
  {.key = B_C|B_J|B_K, .kana = "thi"}, //てぃ
  {.key = B_C|B_H, .kana = "thu"}, //てゅ
  {.key = B_C|B_J|B_SHFT2, .kana = "dhi"}, //でぃ
  {.key = B_C|B_H|B_SHFT2, .kana = "dhu"}, //でゅ
  {.key = B_E|B_K, .kana = "toxu"}, //とぅ
  {.key = B_E|B_K|B_SHFT2, .kana = "doxu"}, //どぅ
  {.key = B_V|B_N|B_K, .kana = "sye"}, //しぇ
  {.key = B_X|B_N, .kana = "tye"}, //ちぇ
  {.key = B_V|B_N|B_SHFT2, .kana = "zye"}, //じぇ
  {.key = B_X|B_N|B_SHFT2, .kana = "dye"}, //ぢぇ
  {.key = B_I|B_H, .kana = "fa"}, //ふぁ
  {.key = B_I|B_J, .kana = "fi"}, //ふぃ
  {.key = B_I|B_N, .kana = "fe"}, //ふぇ
  {.key = B_I|B_M, .kana = "fo"}, //ふぉ
//  {.key = B_I|B_H, .kana = "fyu"}, //ふゅ
  {.key = B_J|B_N|B_D, .kana = "ixe"}, //いぇ
  {.key = B_K|B_J|B_D, .kana = "wi"}, //うぃ
  {.key = B_K|B_N|B_D, .kana = "we"}, //うぇ
  {.key = B_K|B_M|B_D, .kana = "uxo"}, //うぉ
  {.key = B_K|B_H|B_SHFT2, .kana = "va"}, //ゔぁ
  {.key = B_K|B_J|B_SHFT2, .kana = "vi"}, //ゔぃ
  {.key = B_K|B_N|B_SHFT2, .kana = "ve"}, //ゔぇ
  {.key = B_K|B_M|B_SHFT2, .kana = "vo"}, //ゔぉ
//  {.key = B_K|B_H|B_SHFT2, .kana = "vuxyu"}, //ゔゅ
  {.key = B_G|B_H|B_K, .kana = "kuxa"}, //くぁ
  {.key = B_G|B_J|B_K, .kana = "kuxi"}, //くぃ
  {.key = B_G|B_N|B_K, .kana = "kuxe"}, //くぇ
  {.key = B_G|B_M|B_K, .kana = "kuxo"}, //くぉ
  {.key = B_G|B_K, .kana = "kuxwa"}, //くゎ
  {.key = B_G|B_H|B_SHFT2, .kana = "guxa"}, //ぐぁ
  {.key = B_G|B_J|B_SHFT2, .kana = "guxi"}, //ぐぃ
  {.key = B_G|B_N|B_SHFT2, .kana = "guxe"}, //ぐぇ
  {.key = B_G|B_M|B_SHFT2, .kana = "guxo"}, //ぐぉ
  {.key = B_G|B_K|B_SHFT2, .kana = "guxwa"}, //ぐゎ
  {.key = B_M|B_H|B_D, .kana = "tsa"}, //つぁ
  {.key = B_M|B_J|B_D, .kana = "tsi"}, //つぃ
  {.key = B_M|B_N|B_D, .kana = "tse"}, //つぇ
  {.key = B_M|B_D, .kana = "tso"}, //つぉ
// 追加
//ここまで自動書き換え対象

  {.key = B_F|B_G, .kana = "/"}, //・
  {.key = B_V|B_B, .kana = ","}, //、
  {.key = B_M|B_COMM, .kana = "."}, //。
  // {.key = B_SHFT            , .kana = " "},
  // {.key = B_SHFT2            , .kana = SS_TAP(X_ENTER)},

};

const PROGMEM naginata_keymap_long ngmapl[] = {
  {.key = B_N|B_M, .kana = "tenntenn"}, //…

  // {.key = B_T        , .kana = SS_TAP(X_LEFT)}, //SS_LSFT(SS_TAP(NGLT))
  // {.key = B_Y        , .kana = SS_TAP(X_RIGHT)}, //SS_LSFT(SS_TAP(NGRT))
  // {.key = B_T          , .kana = SS_LSFT(SS_TAP(X_LEFT))}, //SS_LSFT(SS_TAP(NGLT))
  // {.key = B_Y          , .kana = SS_LSFT(SS_TAP(X_RIGHT))}, //SS_LSFT(SS_TAP(NGRT))
  //{.key = B_P        , .kana = "tenn"}, //SS_LSFT(SS_TAP(NGRT))

  {.key = B_D|B_G  , .kana = SS_LSFT(SS_TAP(X_8))SS_LSFT(SS_TAP(X_9))SS_TAP(X_LEFT)}, //（）
  {.key = B_C|B_B  , .kana = "]\\"SS_TAP(X_LEFT)}, //「」

  {.key = B_D|B_F, .kana = "wo"}, //を


/*
  {.key = B_H|B_J, .kana = "ltu"}, //っ
  {.key = B_H|B_I, .kana = "sa"}, //さ
  {.key = B_H|B_I|B_D, .kana = "za"}, //ざ
  {.key = B_H|B_K, .kana = "tu"}, //つ
  {.key = B_H|B_K|B_D, .kana = "du"}, //づ
  {.key = B_H|B_COMM, .kana = "ra"}, //ら
  {.key = B_U|B_K, .kana = "se"}, //せ
  {.key = B_U|B_K|B_D, .kana = "ze"}, //ぜ
  {.key = B_J|B_K, .kana = "ku"}, //く
  {.key = B_J|B_K|B_D, .kana = "gu"}, //ぐ
  {.key = B_M|B_K, .kana = "yo"}, //よ
  {.key = B_N|B_K, .kana = "na"}, //な

  {.key = B_E|B_G, .kana = "ti"}, //ち
  {.key = B_D|B_G, .kana = "ma"}, //ま
  {.key = B_C|B_G, .kana = "wa"}, //わ
  {.key = B_F|B_G, .kana = "mi"}, //み
  {.key = B_D|B_R, .kana = "nu"}, //ぬ
  {.key = B_D|B_F, .kana = "wo"}, //を
  {.key = B_D|B_V, .kana = "ho"}, //ほ
  {.key = B_D|B_V|B_K, .kana = "bo"}, //ぼ
  {.key = B_D|B_V|B_COMM, .kana = "po"}, //ぽ
  {.key = B_D|B_B, .kana = "me"}, //め
*/




  // enter
  //{.key = B_V|B_M           , .kana = SS_TAP(X_ENTER)},
  // enter+シフト(連続シフト)
  //{.key = B_SHFT|B_V|B_M    , .kana = SS_LSFT(SS_TAP(X_TAB))},


#if defined(NAGINATA_EDIT_WIN) || defined(NAGINATA_EDIT_LINUX)
// 編集モード Win
  {.key = B_J|B_K|B_Q       , .kana = SS_LCTL(SS_TAP(X_END))}, // ^{End}
//{.key = B_J|B_K|B_W       , .uc   = "｜"}, // ｜{改行}
  {.key = B_J|B_K|B_E       , .kana = "dexi"}, // /*ディ*/
  {.key = B_J|B_K|B_R       , .kana = SS_LCTL("s")}, // ^s
//{.key = B_J|B_K|B_T       , .uc   = "・"}, // ・
//{.key = B_J|B_K|B_A       , .uc   = "……"}, // ……{改行}
//{.key = B_J|B_K|B_S       , .uc   = "《"}, // 《{改行}
  {.key = B_J|B_K|B_D       , .kana = "?"SS_TAP(X_ENTER)}, // ？{改行}
//{.key = B_J|B_K|B_F       , .uc   = "「"}, // 「{改行}
//{.key = B_J|B_K|B_G       , .uc   = "("}, // ({改行}
//{.key = B_J|B_K|B_Z       , .uc   = "││"}, // ││{改行}
//{.key = B_J|B_K|B_X       , .uc   = "》"}, // 》{改行}
  {.key = B_J|B_K|B_C       , .kana = "!"SS_TAP(X_ENTER)}, // ！{改行}
//{.key = B_J|B_K|B_V       , .uc   = "」"}, // 」{改行}
//{.key = B_J|B_K|B_B       , .uc   = ")"}, // ){改行}
  {.key = B_D|B_F|B_Y       , .kana = SS_TAP(X_HOME)}, // {Home}
  {.key = B_D|B_F|B_U       , .kana = SS_LSFT(SS_TAP(X_END))SS_TAP(X_BSPACE)}, // +{End}{BS}
  {.key = B_D|B_F|B_I       , .kana = SS_TAP(X_INT4)}, // {vk1Csc079}
  {.key = B_D|B_F|B_O       , .kana = SS_TAP(X_DELETE)}, // {Del}
  {.key = B_D|B_F|B_P       , .kana = SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)}, // {Esc 3}
  {.key = B_D|B_F|B_H       , .kana = SS_TAP(X_ENTER)SS_TAP(X_END)}, // {Enter}{End}
  {.key = B_D|B_F|B_J       , .kana = SS_TAP(NGUP)}, // {↑}
  {.key = B_D|B_F|B_K       , .kana = SS_LSFT(SS_TAP(NGUP))}, // +{↑}
  {.key = B_D|B_F|B_L       , .kana = SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)}, // {↑ 5}
  {.key = B_D|B_F|B_SCLN       , .kana = SS_LCTL("i")}, // ^i
  {.key = B_D|B_F|B_N       , .kana = SS_TAP(X_END)}, // {End}
  {.key = B_D|B_F|B_M       , .kana = SS_TAP(NGDN)}, // {↓}
  {.key = B_D|B_F|B_COMM       , .kana = SS_LSFT(SS_TAP(NGDN))}, // +{↓}
  {.key = B_D|B_F|B_DOT       , .kana = SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)}, // {↓ 5}
  {.key = B_D|B_F|B_SLSH       , .kana = SS_LCTL("u")}, // ^u
//{.key = B_M|B_COMM|B_Q       , .uc   = "／"}, // ／{改行}
//{.key = B_M|B_COMM|B_W       , .macro , // ｜{改行}{End}《》{改行}{↑}
  {.key = B_M|B_COMM|B_E       , .kana = SS_TAP(X_HOME)SS_TAP(X_ENTER)SS_TAP(X_SPACE)SS_TAP(X_SPACE)SS_TAP(X_SPACE)SS_TAP(X_END)}, // {Home}{改行}{Space 3}{End}
  {.key = B_M|B_COMM|B_R       , .kana = SS_TAP(X_HOME)SS_TAP(X_ENTER)SS_TAP(X_SPACE)SS_TAP(X_END)}, // {Home}{改行}{Space 1}{End}
// {.key = B_M|B_COMM|B_T       , .uc   = "〇"}, // 〇{改行}
//{.key = B_M|B_COMM|B_A       , .uc   = "【"}, // 【{改行}
//{.key = B_M|B_COMM|B_S       , .uc   = "〈"}, // 〈{改行}
//{.key = B_M|B_COMM|B_D       , .uc   = "『"}, // 『{改行}
//{.key = B_M|B_COMM|B_F       , .macro , // 」{改行 2}「{改行}
  {.key = B_M|B_COMM|B_G       , .kana = SS_TAP(X_SPACE)SS_TAP(X_SPACE)SS_TAP(X_SPACE)}, // {Space 3}
//{.key = B_M|B_COMM|B_Z       , .uc   = "】"}, // 】{改行}
//{.key = B_M|B_COMM|B_X       , .uc   = "〉"}, // 〉{改行}
//{.key = B_M|B_COMM|B_C       , .uc   = "』"}, // 』{改行}
//{.key = B_M|B_COMM|B_V       , .macro , // 」{改行 2}{Space}
//{.key = B_M|B_COMM|B_B       , .macro , // 　　　×　　　×　　　×{改行 2}
  {.key = B_C|B_V|B_Y       , .kana = SS_LSFT(SS_TAP(X_HOME))}, // +{Home}
  {.key = B_C|B_V|B_U       , .kana = SS_LCTL("x")}, // ^x
  {.key = B_C|B_V|B_I       , .kana = SS_LCTL("v")}, // ^v
  {.key = B_C|B_V|B_O       , .kana = SS_LCTL("y")}, // ^y
  {.key = B_C|B_V|B_P       , .kana = SS_LCTL("z")}, // ^z
  {.key = B_C|B_V|B_H       , .kana = SS_LCTL("c")}, // ^c
  {.key = B_C|B_V|B_J       , .kana = SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)}, // {→ 5}
  {.key = B_C|B_V|B_K       , .kana = SS_LSFT(SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT))}, // +{→ 5}
  {.key = B_C|B_V|B_L       , .kana = SS_LCTL(SS_TAP(X_PGUP))}, // ^{PgUp}
  {.key = B_C|B_V|B_SCLN       , .kana = SS_LCTL(SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP))}, // ^{PgUp 5}
  {.key = B_C|B_V|B_N       , .kana = SS_LSFT(SS_TAP(X_END))}, // +{End}
  {.key = B_C|B_V|B_M       , .kana = SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)}, // {← 5}
  {.key = B_C|B_V|B_COMM       , .kana = SS_LSFT(SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT))}, // +{← 5}
  {.key = B_C|B_V|B_DOT       , .kana = SS_LCTL(SS_TAP(X_PGDOWN))}, // ^{PgDn}
  {.key = B_C|B_V|B_SLSH       , .kana = SS_LCTL(SS_TAP(X_PGDOWN)SS_TAP(X_PGDOWN)SS_TAP(X_PGDOWN)SS_TAP(X_PGDOWN)SS_TAP(X_PGDOWN))}, // ^{PgDn 5}
#endif
#ifdef NAGINATA_EDIT_MAC
// 編集モード Mac
  {.key = B_J|B_K|B_Q       , .kana = SS_LCMD(SS_LCTL("e"))}, // ^{End}
//{.key = B_J|B_K|B_W       , .kana = "nagitatesenn"}, // ｜{改行}
  {.key = B_J|B_K|B_E       , .kana = "dexi"}, // /*ディ*/
  {.key = B_J|B_K|B_R       , .kana = SS_LCMD("s")}, // ^s
//{.key = B_J|B_K|B_T       , .kana = "nagichuutenn"}, // ・
//{.key = B_J|B_K|B_A       , .kana = "nagitentenn"}, // ……{改行}
//{.key = B_J|B_K|B_S       , .kana = "nagikakkohi5"}, // 《{改行}
  {.key = B_J|B_K|B_D       , .kana = "?"SS_TAP(X_ENTER)}, // ？{改行}
//{.key = B_J|B_K|B_F       , .kana = "nagikakkohi3"}, // 「{改行}
//{.key = B_J|B_K|B_G       , .kana = "nagikakkohi6"}, // ({改行}
//{.key = B_J|B_K|B_Z       , .kana = "nagitatesenn2"}, // ││{改行}
//{.key = B_J|B_K|B_X       , .kana = "nagikakkomi5"}, // 》{改行}
  {.key = B_J|B_K|B_C       , .kana = "!"SS_TAP(X_ENTER)}, // ！{改行}
//{.key = B_J|B_K|B_V       , .kana = "nagikakkomi3"}, // 」{改行}
//{.key = B_J|B_K|B_B       , .kana = "nagikakkomi6"}, // ){改行}
  {.key = B_D|B_F|B_Y       , .kana = SS_LCTL("a")}, // {Home}
  {.key = B_D|B_F|B_U       , .kana = SS_LSFT(SS_LCTL("e"))SS_TAP(X_BSPACE)}, // +{End}{BS}
  {.key = B_D|B_F|B_I       , .kana = SS_TAP(X_INT4)}, // {vk1Csc079}
  {.key = B_D|B_F|B_O       , .kana = SS_TAP(X_DELETE)}, // {Del}
  {.key = B_D|B_F|B_P       , .kana = SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)SS_TAP(X_ESCAPE)}, // {Esc 3}
  {.key = B_D|B_F|B_H       , .kana = SS_TAP(X_ENTER)SS_TAP(X_END)}, // {Enter}{End}
  {.key = B_D|B_F|B_J       , .kana = SS_TAP(NGUP)}, // {↑}
  {.key = B_D|B_F|B_K       , .kana = SS_LSFT(SS_TAP(NGUP))}, // +{↑}
  {.key = B_D|B_F|B_L       , .kana = SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)SS_TAP(NGUP)}, // {↑ 5}
  {.key = B_D|B_F|B_SCLN       , .kana = SS_LCMD("i")}, // ^i
  {.key = B_D|B_F|B_N       , .kana = SS_LCTL("e")}, // {End}
  {.key = B_D|B_F|B_M       , .kana = SS_TAP(NGDN)}, // {↓}
  {.key = B_D|B_F|B_COMM       , .kana = SS_LSFT(SS_TAP(NGDN))}, // +{↓}
  {.key = B_D|B_F|B_DOT       , .kana = SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)SS_TAP(NGDN)}, // {↓ 5}
  {.key = B_D|B_F|B_SLSH       , .kana = SS_LCTL("j")}, // ^u
//{.key = B_M|B_COMM|B_Q       , .kana = "naginaname"}, // ／{改行}
//{.key = B_M|B_COMM|B_W       , .macro , // ｜{改行}{End}《》{改行}{↑}
  {.key = B_M|B_COMM|B_E       , .kana = SS_LCTL("a")SS_TAP(X_ENTER)SS_TAP(X_SPACE)SS_TAP(X_SPACE)SS_TAP(X_SPACE)SS_LCTL("e")}, // {Home}{改行}{Space 3}{End}
  {.key = B_M|B_COMM|B_R       , .kana = SS_LCTL("a")SS_TAP(X_ENTER)SS_TAP(X_SPACE)SS_LCTL("e")}, // {Home}{改行}{Space 1}{End}
//{.key = B_M|B_COMM|B_T       , .kana = "nagimaru"}, // 〇{改行}
//{.key = B_M|B_COMM|B_A       , .kana = "nagikakkohi1"}, // 【{改行}
//{.key = B_M|B_COMM|B_S       , .kana = "nagikakkohi2"}, // 〈{改行}
//{.key = B_M|B_COMM|B_D       , .kana = "nagikakkohi4"}, // 『{改行}
//{.key = B_M|B_COMM|B_F       , .macro , // 」{改行 2}「{改行}
  {.key = B_M|B_COMM|B_G       , .kana = SS_TAP(X_SPACE)SS_TAP(X_SPACE)SS_TAP(X_SPACE)}, // {Space 3}
//{.key = B_M|B_COMM|B_Z       , .kana = "nagikakkomi1"}, // 】{改行}
//{.key = B_M|B_COMM|B_X       , .kana = "nagikakkomi2"}, // 〉{改行}
//{.key = B_M|B_COMM|B_C       , .kana = "nagikakkomi4"}, // 』{改行}
//{.key = B_M|B_COMM|B_V       , .macro , // 」{改行 2}{Space}
//{.key = B_M|B_COMM|B_B       , .macro , // 　　　×　　　×　　　×{改行 2}
  {.key = B_C|B_V|B_Y       , .kana = SS_LSFT(SS_LCTL(SS_TAP(NGUP)))}, // +{Home}
  {.key = B_C|B_V|B_U       , .kana = SS_LCMD("x")}, // ^x
  {.key = B_C|B_V|B_I       , .kana = SS_LCMD("v")}, // ^v
  {.key = B_C|B_V|B_O       , .kana = SS_LCMD("y")}, // ^y
  {.key = B_C|B_V|B_P       , .kana = SS_LCMD("z")}, // ^z
  {.key = B_C|B_V|B_H       , .kana = SS_LCMD("c")}, // ^c
  {.key = B_C|B_V|B_J       , .kana = SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)}, // {→ 5}
  {.key = B_C|B_V|B_K       , .kana = SS_LSFT(SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT)SS_TAP(NGRT))}, // +{→ 5}
  {.key = B_C|B_V|B_L       , .kana = SS_TAP(X_PGUP)}, // ^{PgUp}
  {.key = B_C|B_V|B_SCLN       , .kana = SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)SS_TAP(X_PGUP)}, // ^{PgUp 5}
  {.key = B_C|B_V|B_N       , .kana = SS_LSFT(SS_LCTL("e"))}, // +{End}
  {.key = B_C|B_V|B_M       , .kana = SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)}, // {← 5}
  {.key = B_C|B_V|B_COMM       , .kana = SS_LSFT(SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT)SS_TAP(NGLT))}, // +{← 5}
  {.key = B_C|B_V|B_DOT       , .kana = SS_TAP(X_PGDOWN)}, // ^{PgDn}
  {.key = B_C|B_V|B_SLSH       , .kana = SS_TAP(X_PGDOWN)SS_TAP(X_PGDOWN)SS_TAP(X_PGDOWN)SS_TAP(X_PGDOWN)SS_TAP(X_PGDOWN)}, // ^{PgDn 5}
#endif
};

const PROGMEM naginata_keymap_unicode ngmapu[] = {
#if defined(NAGINATA_EDIT_WIN) || defined(NAGINATA_EDIT_LINUX)
  {.key = B_J|B_K|B_W       , .uc   = "｜"}, // ｜{改行}
  {.key = B_J|B_K|B_T       , .uc   = "・"}, // ・
  {.key = B_J|B_K|B_A       , .uc   = "……"}, // ……{改行}
  {.key = B_J|B_K|B_S       , .uc   = "《"}, // 《{改行}
  {.key = B_J|B_K|B_F       , .uc   = "「"}, // 「{改行}
  {.key = B_J|B_K|B_G       , .uc   = "("}, // ({改行}
  {.key = B_J|B_K|B_Z       , .uc   = "││"}, // ││{改行}
  {.key = B_J|B_K|B_X       , .uc   = "》"}, // 》{改行}
  {.key = B_J|B_K|B_V       , .uc   = "」"}, // 」{改行}
  {.key = B_J|B_K|B_B       , .uc   = ")"}, // ){改行}
  {.key = B_M|B_COMM|B_Q       , .uc   = "／"}, // ／{改行}
  {.key = B_M|B_COMM|B_T       , .uc   = "〇"}, // 〇{改行}
  {.key = B_M|B_COMM|B_A       , .uc   = "【"}, // 【{改行}
  {.key = B_M|B_COMM|B_S       , .uc   = "〈"}, // 〈{改行}
  {.key = B_M|B_COMM|B_D       , .uc   = "『"}, // 『{改行}
  {.key = B_M|B_COMM|B_Z       , .uc   = "】"}, // 】{改行}
  {.key = B_M|B_COMM|B_X       , .uc   = "〉"}, // 〉{改行}
  {.key = B_M|B_COMM|B_C       , .uc   = "』"}, // 』{改行}

#endif
};

const PROGMEM naginata_keymap_ime ngmapi[] = {
#ifdef NAGINATA_EDIT_MAC
  {.key = B_J|B_K|B_W       , .kana = "nagitatesenn"}, // ｜{改行}
  {.key = B_J|B_K|B_T       , .kana = "nagichuutenn"}, // ・
  {.key = B_J|B_K|B_A       , .kana = "nagitentenn"}, // ……{改行}
  {.key = B_J|B_K|B_S       , .kana = "nagikakkohi5"}, // 《{改行}
  {.key = B_J|B_K|B_F       , .kana = "nagikakkohi3"}, // 「{改行}
  {.key = B_J|B_K|B_G       , .kana = "nagikakkohi6"}, // ({改行}
  {.key = B_J|B_K|B_Z       , .kana = "nagitatesenn2"}, // ││{改行}
  {.key = B_J|B_K|B_X       , .kana = "nagikakkomi5"}, // 》{改行}
  {.key = B_J|B_K|B_V       , .kana = "nagikakkomi3"}, // 」{改行}
  {.key = B_J|B_K|B_B       , .kana = "nagikakkomi6"}, // ){改行}
  {.key = B_M|B_COMM|B_Q       , .kana = "naginaname"}, // ／{改行}
  {.key = B_M|B_COMM|B_T       , .kana = "nagimaru"}, // 〇{改行}
  {.key = B_M|B_COMM|B_A       , .kana = "nagikakkohi1"}, // 【{改行}
  {.key = B_M|B_COMM|B_S       , .kana = "nagikakkohi2"}, // 〈{改行}
  {.key = B_M|B_COMM|B_D       , .kana = "nagikakkohi4"}, // 『{改行}
  {.key = B_M|B_COMM|B_Z       , .kana = "nagikakkomi1"}, // 】{改行}
  {.key = B_M|B_COMM|B_X       , .kana = "nagikakkomi2"}, // 〉{改行}
  {.key = B_M|B_COMM|B_C       , .kana = "nagikakkomi4"}, // 』{改行}
#endif
};

// 薙刀式のレイヤー、オンオフするキー
// void set_naginata(uint8_t layer) {
//     naginata_layer = layer;
// }
void set_naginata(uint8_t layer, uint16_t *onk, uint16_t *offk) {
 naginata_layer = layer;
 ngon_keys[0] = *onk;
 ngon_keys[1] = *(onk+1);
 ngoff_keys[0] = *offk;
 ngoff_keys[1] = *(offk+1);
}

// 薙刀式をオン
void naginata_on(void) {
  is_naginata = true;
  keycomb = 0UL;
  naginata_clear();
  layer_on(naginata_layer);

  //tap_code(KC_LANG1); // Mac
  tap_code(KC_HENK); // Win
}

// 薙刀式をオフ
void naginata_off(void) {
  is_naginata = false;
  keycomb = 0UL;
  naginata_clear();
  layer_off(naginata_layer);

  //  tap_code(KC_LANG2); // Mac
  tap_code(KC_MHEN); // Win
}

// 薙刀式のon/off状態を返す
bool naginata_state(void) {
  return is_naginata;
}

// OSのかな/英数モードをキーボードに合わせる
void makesure_mode(void) {
  if (is_naginata) {
    tap_code(KC_LANG1); // Mac
    tap_code(KC_HENK); // Win
  } else {
    tap_code(KC_LANG2); // Mac
    tap_code(KC_MHEN); // Win
  }
}

// バッファをクリアする
void naginata_clear(void) {
  for (int i = 0; i < NGBUFFER; i++) {
    ninputs[i] = 0;
  }
  ng_chrcount = 0;
}

// バッファから先頭n文字を削除する
void compress_buffer(int n) {
  if (ng_chrcount == 0) return;
  for (int j = 0; j < NGBUFFER; j++) {
    if (j + n < NGBUFFER) {
      ninputs[j] = ninputs[j + n];
    } else {
      ninputs[j] = 0;
    }
  }
  ng_chrcount -= n;
}

#ifdef MAC_LIVE_CONVERSION
static bool is_live_conv = true;
#else
static bool is_live_conv = false;
#endif

void mac_live_conversion_toggle() {
  is_live_conv = !is_live_conv;
}

void mac_live_conversion_on() {
  is_live_conv = true;
}

void mac_live_conversion_off() {
  is_live_conv = false;
}

void mac_send_string(const char *str) {
  send_string(str);
  if (!is_live_conv) tap_code(KC_SPC);
  tap_code(KC_ENT);
}

void ng_send_unicode_string(const char *str) {
  #if defined(NAGINATA_EDIT_LINUX)
  tap_code(KC_MHEN); // Win
  send_unicode_string(str);
  tap_code(KC_HENK); // Win
  #endif
  #if defined(NAGINATA_EDIT_WIN) || defined(NAGINATA_EDIT_MAC)
  send_unicode_string(str);
  tap_code(KC_ENT);
  #endif
}

// modifierが押されたら薙刀式レイヤーをオフしてベースレイヤーに戻す
// get_mods()がうまく動かない
static int n_modifier = 0;

bool process_modifier(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_LCTL:
    case KC_RCTL:
    case KC_LSHIFT:
    case KC_LALT:
    case KC_LGUI:
    case KC_RSHIFT:
    case KC_RALT:
    case KC_RGUI:
    case LCTL_T(0x01) ... LCTL_T(0xFF):
    case LSFT_T(0x01) ... LSFT_T(0xFF):
    case LALT_T(0x01) ... LALT_T(0xFF):
    case LGUI_T(0x01) ... LGUI_T(0xFF):
    case RCTL_T(0x01) ... RCTL_T(0xFF):
    case RSFT_T(0x01) ... RSFT_T(0xFF):
    case RALT_T(0x01) ... RALT_T(0xFF):
    case RGUI_T(0x01) ... RGUI_T(0xFF):
      if (record->event.pressed) {
        n_modifier++;
        layer_off(naginata_layer);
      } else {
        n_modifier--;
        if (n_modifier == 0) {
          layer_on(naginata_layer);
        }
      }
      return true;
      break;
  }
  return false;
}

static uint16_t fghj_buf = 0;

// 薙刀式の起動処理(COMBOを使わない)
bool enable_naginata(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {

    if (fghj_buf == 0 && (keycode == ngon_keys[0] || keycode == ngon_keys[1] ||
        keycode == ngoff_keys[0] || keycode == ngoff_keys[1])) {
      fghj_buf = keycode;
      return false;
    } else if (fghj_buf > 0) {
      if ((keycode == ngon_keys[0] && fghj_buf == ngon_keys[1]) ||
          (keycode == ngon_keys[1] && fghj_buf == ngon_keys[0])) {
        naginata_on();
        fghj_buf = 0;
        return false;
      } else if ((keycode == ngoff_keys[0] && fghj_buf == ngoff_keys[1]) ||
                 (keycode == ngoff_keys[1] && fghj_buf == ngoff_keys[0])) {
        naginata_off();
        fghj_buf = 0;
        return false;
      } else {
        tap_code(fghj_buf);
        fghj_buf = 0;
        return true;
      }
    }
  } else {
    if (fghj_buf > 0) {
      tap_code(fghj_buf);
      fghj_buf = 0;
      return true;
    }
  }
  fghj_buf = 0;
  return true;
}


//s1,s2のタップ判定用
static bool s1_only = false;
static uint16_t s1_only_time = 0;
static bool s2_only = false;
static uint16_t s2_only_time = 0;

// 薙刀式の入力処理
bool process_naginata(uint16_t keycode, keyrecord_t *record) {
  if (!is_naginata)
    return enable_naginata(keycode, record);


  if (process_modifier(keycode, record))
    return true;

  if (record->event.pressed) {
    switch (keycode) {
      case NG_SHFT ... NG_SHFT2:
        if (keycode == NG_SHFT)  {
          s1_only = true;
          s1_only_time = record->event.time;
        }
        if (keycode == NG_SHFT2) {
          s2_only = true;
          s2_only_time = record->event.time;
        }
#ifndef NAGINATA_KOUCHI_SHIFT
        if (ng_chrcount >= 1) {
          naginata_type();
          keycomb = 0UL;
        }
        ninputs[ng_chrcount] = keycode; // キー入力をバッファに貯める
        ng_chrcount++;
        keycomb |= ng_key[keycode - NG_Q]; // キーの重ね合わせ
        return false;
        break;
#endif
      case NG_Q ... NG_SLSH:
        // s1_only_time = record->event.time;
        // s2_only_time = record->event.time;
        
        ninputs[ng_chrcount] = keycode; // キー入力をバッファに貯める
        ng_chrcount++;
        keycomb |= ng_key[keycode - NG_Q]; // キーの重ね合わせ
        // バッファが一杯になったら処理を開始
        if (ng_chrcount >= NGBUFFER) {
          naginata_type();
        }
        return false;
        break;
    }
  } else { // key release
    switch (keycode) {
      case NG_Q ... NG_SLSH:
        // s1_only_time = record->event.time;
        // s2_only_time = record->event.time;
        keycomb &= ~ng_key[keycode - NG_Q]; // キーの重ね合わせ
        if (ng_chrcount > 0) {
          naginata_type();
        }
        return false;
        break;
      case NG_SHFT ... NG_SHFT2:
        if (shift_only()) {
          s1_only = (keycode == NG_SHFT  && record->event.time - s1_only_time > CANCEL_TIME) ? true : false;
          s2_only = (keycode == NG_SHFT2 && record->event.time - s2_only_time > CANCEL_TIME) ? true : false;
        }
        
        keycomb &= ~ng_key[keycode - NG_Q]; // キーの重ね合わせ
        if (ng_chrcount > 0) {
          naginata_type();
        }
        s1_only = false;
        s2_only = false;
        return false;
        break;
    }
    // どれかキーを離したら処理を開始する
  }
  return true;
}

//
bool shift_only(void){
  if (ng_chrcount == 1) return true;
  if (ng_chrcount == 2 && ninputs[0] == NG_SHFT  && ninputs[1] == NG_SHFT2) return true;
  if (ng_chrcount == 2 && ninputs[0] == NG_SHFT2 && ninputs[1] == NG_SHFT)  return true;
  return false;
}

// キー入力を文字に変換して出力する
void naginata_type(void) {
  // バッファの最初からnt文字目までを検索キーにする。
  // 一致する組み合わせがなければntを減らして=最後の1文字を除いて再度検索する。
  int nt = ng_chrcount;

  while (nt > 0) {
    if (naginata_lookup(nt, true)) return; // 連続シフト有効で探す
    if (naginata_lookup(nt, false)) return; // 連続シフト無効で探す
    nt--; // 最後の1キーを除いて、もう一度仮名テーブルを検索する
  }
  compress_buffer(1);
}


// バッファの頭からnt文字の範囲を検索キーにしてテーブル検索し、文字に変換して出力する
// 検索に成功したらtrue、失敗したらfalseを返す
bool naginata_lookup(int nt, bool shifted) {
  naginata_keymap bngmap; // PROGMEM buffer
  naginata_keymap_long bngmapl; // PROGMEM buffer
  naginata_keymap_unicode bngmapu; // PROGMEM buffer
  naginata_keymap_ime bngmapi; // PROGMEM buffer

  // keycomb_bufはバッファ内のキーの組み合わせ、keycombはリリースしたキーを含んでいない
  uint32_t keycomb_buf = 0UL;

  // バッファ内のキーを組み合わせる
  for (int i = 0; i < nt; i++) {
    keycomb_buf |= ng_key[ninputs[i] - NG_Q];
  }

  // NG_SHFT2はスペースの代わりにエンターを入力する
  if (keycomb_buf == B_SHFT && ninputs[0] == NG_SHFT) {
    if (!s1_only) tap_code(KC_SPACE);
    compress_buffer(nt);
    return true;
  }
  if (keycomb_buf == B_SHFT2 && ninputs[0] == NG_SHFT2) {
    if (!s2_only) tap_code(KC_ENT);
    // if (!s2_only) tap_code(KC_BSPC);
    compress_buffer(nt);
    return true;
  }




  if (shifted) {
    // 濁音・半濁音を連続する
    if ((keycomb & B_SHFT) == B_SHFT) keycomb_buf |= B_SHFT;
    if ((keycomb & B_SHFT2) == B_SHFT2) keycomb_buf |= B_SHFT2;

    // 編集モードを連続する
    //if ((keycomb & (B_D | B_F))    == (B_D | B_F))    keycomb_buf |= (B_D | B_F);
    // if ((keycomb & (B_C | B_V))    == (B_C | B_V))    keycomb_buf |= (B_C | B_V);
    //if ((keycomb & (B_J | B_K))    == (B_J | B_K))    keycomb_buf |= (B_J | B_K);
    // if ((keycomb & (B_M | B_COMM)) == (B_M | B_COMM)) keycomb_buf |= (B_M | B_COMM);

    // シフトを連続する
    if ((keycomb & B_J) == B_J) keycomb_buf |= B_J;
    if ((keycomb & B_K) == B_K) keycomb_buf |= B_K;
  }

  switch (keycomb_buf) {
     // send_stringできないキー、長すぎるマクロはここで定義
    // case B_J|B_K:
    //   naginata_on();
    //   compress_buffer(nt);
    //   return true;
    //   break;
    // case B_D|B_F:
    //   naginata_off();
    //   compress_buffer(nt);
    //   return true;
    //   break;

#if defined(NAGINATA_EDIT_WIN) || defined(NAGINATA_EDIT_LINUX)
    case B_M|B_COMM|B_B: //　　　×　　　×　　　×{改行 2}
      ng_send_unicode_string("　　　×　　　×　　　×");
      tap_code(KC_ENT);
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_W: // {改行}{End}《》{改行}{↑}
      tap_code(KC_ENT);
      tap_code(KC_END);
      ng_send_unicode_string("《》");
      tap_code(NGKUP);
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_F: // 」{改行 2}「{改行}
      ng_send_unicode_string("」");
      tap_code(KC_ENT);
      ng_send_unicode_string("「");
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_V: // 」{改行 2}{Space}
      ng_send_unicode_string("」");
      tap_code(KC_ENT);
      tap_code(KC_SPC);
      compress_buffer(nt);
      return true;
      break;
#endif
#ifdef NAGINATA_EDIT_MAC
    case B_M|B_COMM|B_B: //　　　×　　　×　　　×{改行 2}
      mac_send_string("nagibatu");
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_W: // {改行}{End}《》{改行}{↑}
      tap_code(KC_ENT);
      tap_code(KC_END);
      mac_send_string("nagikakkohi5");
      mac_send_string("nagikakkomi5");
      tap_code(NGKUP);
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_F: // 」{改行 2}「{改行}
      mac_send_string("nagikakkomi3");
      tap_code(KC_ENT);
      mac_send_string("nagikakkohi3");
      compress_buffer(nt);
      return true;
      break;
    case B_M|B_COMM|B_V: // 」{改行 2}{Space}
      mac_send_string("nagikakkomi3");
      tap_code(KC_ENT);
      tap_code(KC_SPC);
      compress_buffer(nt);
      return true;
      break;
#endif
    default:
      // キーから仮名に変換して出力する

      // 通常の仮名
      for (int i = 0; i < sizeof ngmap / sizeof bngmap; i++) {
        memcpy_P(&bngmap, &ngmap[i], sizeof(bngmap));
        if (keycomb_buf == bngmap.key) {
          send_string(bngmap.kana);
          compress_buffer(nt);
          return true;
        }
      }
      // 仮名ロング
      for (int i = 0; i < sizeof ngmapl / sizeof bngmapl; i++) {
        memcpy_P(&bngmapl, &ngmapl[i], sizeof(bngmapl));
        if (keycomb_buf == bngmapl.key) {
          send_string(bngmapl.kana);
          compress_buffer(nt);
          return true;
        }
      }
      // UNICODE文字
      for (int i = 0; i < sizeof ngmapu / sizeof bngmapu; i++) {
        memcpy_P(&bngmapu, &ngmapu[i], sizeof(bngmapu));
        if (keycomb_buf == bngmapu.key) {
          ng_send_unicode_string(bngmapu.uc);
          // send_unicode_hex_string(bngmapu.kana);
          // tap_code(KC_ENT);
          compress_buffer(nt);
          return true;
        }
      }
      // IME変換する文字
      for (int i = 0; i < sizeof ngmapi / sizeof bngmapi; i++) {
        memcpy_P(&bngmapi, &ngmapi[i], sizeof(bngmapi));
        if (keycomb_buf == bngmapi.key) {
          #if defined(NAGINATA_EDIT_WIN) || defined(NAGINATA_EDIT_LINUX)
          send_string(bngmapi.kana);
          #endif
          #ifdef NAGINATA_EDIT_MAC
          mac_send_string(bngmapi.kana);
          #endif
          compress_buffer(nt);
          return true;
        }
      }
  }
  return false;
}
