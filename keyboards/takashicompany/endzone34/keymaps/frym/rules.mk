EXTRAKEY_ENABLE = no        # Audio control and System control
BACKLIGHT_ENABLE = no
RGBLIGHT_ENABLE = no
AUDIO_ENABLE = no
COMBO_ENABLE = no

#NKRO_ENABLE = yes            # USB Nkey Rollover
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
OLED_ENABLE = no
MOUSEKEY_ENABLE = yes
TAP_DANCE_ENABLE = yes


UNICODE_ENABLE = yes
# SRC += naginata_v14.c
SRC +=    naginata_frym.c

EXTRAFLAGS += -flto