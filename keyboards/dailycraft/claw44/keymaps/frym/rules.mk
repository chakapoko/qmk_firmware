BACKLIGHT_ENABLE = no
RGBLIGHT_ENABLE = no
AUDIO_ENABLE = no

EXTRAKEY_ENABLE = yes
COMBO_ENABLE = no

# MOUSEKEY_ENABLE = yes
TAP_DANCE_ENABLE = no

#薙刀式
UNICODE_ENABLE = yes
SRC +=    honjin.c
# SRC +=    naginata_frym.c
# SRC +=    naginata_v14.c
EXTRAFLAGS += -flto
