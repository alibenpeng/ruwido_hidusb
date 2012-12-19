// Ruwido Merlin IR Keycodes

#define MERLIN_MOD_CONTROL_LEFT     0x08AB
#define MERLIN_MOD_SHIFT_LEFT       0x08AC
#define MERLIN_MOD_ALT_LEFT         0x0A2D
#define MERLIN_MOD_ALT_RIGHT        0x0A39

#define MERLIN_KEY_A                0x0AB3
#define MERLIN_KEY_B                0x0AB4
#define MERLIN_KEY_C                0x0AB9
#define MERLIN_KEY_D                0x0AE8
#define MERLIN_KEY_E                0x0ACB
#define MERLIN_KEY_F                0x0ACC
#define MERLIN_KEY_G                0x02B5
#define MERLIN_KEY_H                0x0AD8
#define MERLIN_KEY_I                0x0AE3
#define MERLIN_KEY_J                0x0AE4
#define MERLIN_KEY_K                0x0AE9
#define MERLIN_KEY_L                0x0BA8
#define MERLIN_KEY_M                0x0B2B
#define MERLIN_KEY_N                0x0B2C
#define MERLIN_KEY_O                0x02CD
#define MERLIN_KEY_P                0x0B38
#define MERLIN_KEY_Q                0x02D3
#define MERLIN_KEY_R                0x02D4
#define MERLIN_KEY_S                0x02D9
#define MERLIN_KEY_T                0x0B68
#define MERLIN_KEY_U                0x0B8B
#define MERLIN_KEY_V                0x0B8C
#define MERLIN_KEY_W                0x02E5
#define MERLIN_KEY_X                0x0B98
#define MERLIN_KEY_Y                0x0BA3
#define MERLIN_KEY_Z                0x0BA4
#define MERLIN_KEY_1                0x0BA9
#define MERLIN_KEY_2                0x0EA8
#define MERLIN_KEY_3                0x0CAB
#define MERLIN_KEY_4                0x0CAC
#define MERLIN_KEY_5                0x032D
#define MERLIN_KEY_6                0x0CB8
#define MERLIN_KEY_7                0x0333
#define MERLIN_KEY_8                0x0334
#define MERLIN_KEY_9                0x0339
#define MERLIN_KEY_0                0x0CE8

#define MERLIN_KEY_TAB              0x0358
#define MERLIN_KEY_COMMA            0x0399
#define MERLIN_KEY_PERIOD           0x0E68
#define MERLIN_KEY_SLASH            0x0E8B
#define MERLIN_KEY_SEMICOLON        0x0E38
#define MERLIN_KEY_QUOTE            0x0393
#define MERLIN_KEY_EQUALS           0x0369
#define MERLIN_KEY_BACKSPACE        0x0CD5
#define MERLIN_KEY_ENTER            0x034B
#define MERLIN_KEY_SPACE            0x0363

#define MERLIN_KEY_POWER            0x034C
#define MERLIN_KEY_P_               0x05A3
#define MERLIN_KEY_M_               0x05A4
#define MERLIN_KEY_S_               0x0598
#define MERLIN_KEY_TXT              0x0E98
#define MERLIN_KEY_EXIT             0x0D54
#define MERLIN_KEY_RED              0x06A8
#define MERLIN_KEY_GREEN            0x08AB
#define MERLIN_KEY_VT               0x0D53
#define MERLIN_KEY_YELLOW           0x08AC
#define MERLIN_KEY_BLUE             0x062D
#define MERLIN_KEY_REWIND           0x0EA4
#define MERLIN_KEY_FASTFORWARD      0x0AA8
#define MERLIN_KEY_PLAYPAUSE        0x0EA9
#define MERLIN_KEY_FM               0x0568
#define MERLIN_KEY_STOP             0x02AB
#define MERLIN_KEY_HELP             0x0D59
#define MERLIN_KEY_REC              0x0633
#define MERLIN_KEY_UP               0x0D4D
#define MERLIN_KEY_DOWN             0x052C
#define MERLIN_KEY_LEFT             0x052B
#define MERLIN_KEY_RIGHT            0x03A8
#define MERLIN_KEY_OK               0x058B
#define MERLIN_KEY_VOLPLUS          0x02AC
#define MERLIN_KEY_VOLMINUS         0x04AD
#define MERLIN_KEY_CHPLUS           0x02B8
#define MERLIN_KEY_CHMINUS          0x04B3

// Alice remote IR Keycodes

#define ALICE_KEY_POWER             0x034c
#define ALICE_KEY_1                 0x0ba9
#define ALICE_KEY_2                 0x0ea8
#define ALICE_KEY_3                 0x0cab
#define ALICE_KEY_4                 0x0cac
#define ALICE_KEY_5                 0x032d
#define ALICE_KEY_6                 0x0cb8
#define ALICE_KEY_7                 0x0333
#define ALICE_KEY_8                 0x0334
#define ALICE_KEY_9                 0x0339
#define ALICE_KEY_0                 0x0ce8
#define ALICE_KEY_TV                0x03a5
#define ALICE_KEY_LIST              0x0ba8
#define ALICE_KEY_INFO              0x0ae3
#define ALICE_KEY_EPG               0x0acb
#define ALICE_KEY_ESC               0x0b98
#define ALICE_KEY_MOVIE             0x0b38
#define ALICE_KEY_UP                0x0d4d
#define ALICE_KEY_DOWN              0x052c
#define ALICE_KEY_LEFT              0x052b
#define ALICE_KEY_RIGHT             0x03a8
#define ALICE_KEY_OK                0x034b
#define ALICE_KEY_RED               0x02d4
#define ALICE_KEY_GREEN             0x02b5
#define ALICE_KEY_YELLOW            0x0ba4
#define ALICE_KEY_BLUE              0x0ab4
#define ALICE_KEY_VOLPLUS           0x05a3
#define ALICE_KEY_VOLMINUS          0x05a9
#define ALICE_KEY_CHPLUS            0x0568
#define ALICE_KEY_CHMINUS           0x0d59
#define ALICE_KEY_ALICE             0x0b68
#define ALICE_KEY_PLAYPAUSE         0x0e98
#define ALICE_KEY_LAST              0x0ea3
#define ALICE_KEY_REWIND            0x0ea4
#define ALICE_KEY_FASTFORWARD       0x0ea9
#define ALICE_KEY_NEXT              0x0aa8
#define ALICE_KEY_MUTE              0x0634
#define ALICE_KEY_STOP              0x02ab
#define ALICE_KEY_REC               0x02ac
#define ALICE_KEY_TXT               0x04ad

/* Keyboard usage values, see usb.org's HID-usage-tables document, chapter
 * 10 Keyboard/Keypad Page for more codes.
 */
#define MOD_CONTROL_LEFT    (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_ALT_RIGHT       (1<<6)

#define KEY_A                4
#define KEY_B                5
#define KEY_C                6
#define KEY_D                7
#define KEY_E                8
#define KEY_F                9
#define KEY_G                10
#define KEY_H                11
#define KEY_I                12
#define KEY_J                13
#define KEY_K                14
#define KEY_L                15
#define KEY_M                16
#define KEY_N                17
#define KEY_O                18
#define KEY_P                19
#define KEY_Q                20
#define KEY_R                21
#define KEY_S                22
#define KEY_T                23
#define KEY_U                24
#define KEY_V                25
#define KEY_W                26
#define KEY_X                27
#define KEY_Y                28
#define KEY_Z                29
#define KEY_1                30
#define KEY_2                31
#define KEY_3                32
#define KEY_4                33
#define KEY_5                34
#define KEY_6                35
#define KEY_7                36
#define KEY_8                37
#define KEY_9                38
#define KEY_0                39

#define KEY_TAB              43
#define KEY_COMMA            54
#define KEY_PERIOD           55
#define KEY_SLASH            56
#define KEY_SEMICOLON        51
#define KEY_QUOTE            52
#define KEY_EQUALS           46
#define KEY_BACKSPACE        42
#define KEY_ENTER            40
#define KEY_SPACE            44

#define KEY_EXIT             41 // escape
#define KEY_RED              47 // [
#define KEY_GREEN            0  // control
#define KEY_VT               45 // -
#define KEY_YELLOW           0  // shift
#define KEY_BLUE             48 // ]

#define KEY_UP               82
#define KEY_DOWN             81
#define KEY_LEFT             80
#define KEY_RIGHT            79
#define KEY_OK               88

#define REMOTE_KEY_POWER            (0x02<<8)
#define REMOTE_KEY_P_               0
#define REMOTE_KEY_M_               (0x01<<8) // mute
#define REMOTE_KEY_S_               0
#define REMOTE_KEY_TXT              0
#define REMOTE_KEY_EXIT             (0x08<<8)
#define REMOTE_KEY_RED              47 // [
#define REMOTE_KEY_GREEN            0  // control
#define REMOTE_KEY_VT               45 // -
#define REMOTE_KEY_YELLOW           0  // shift
#define REMOTE_KEY_BLUE             48 // ]
#define REMOTE_KEY_REWIND           (0x0f<<8)
#define REMOTE_KEY_FASTFORWARD      (0x0e<<8)
#define REMOTE_KEY_PLAYPAUSE        (0x0c<<8)
#define REMOTE_KEY_FM               0
#define REMOTE_KEY_STOP             (0x12<<8)
#define REMOTE_KEY_HELP             (0x0b<<8)
#define REMOTE_KEY_REC              (0x0d<<8)
#define REMOTE_KEY_UP               (0x04<<8)
#define REMOTE_KEY_DOWN             (0x05<<8)
#define REMOTE_KEY_LEFT             (0x06<<8)
#define REMOTE_KEY_RIGHT            (0x07<<8)
#define REMOTE_KEY_OK               (0x03<<8)
#define REMOTE_KEY_VOLPLUS          (1<<6)
#define REMOTE_KEY_VOLMINUS         (1<<7)
#define REMOTE_KEY_CHPLUS           (1<<4)
#define REMOTE_KEY_CHMINUS          (1<<5)

#define REMOTE_KEY_TV               (0x09<<8)
#define REMOTE_KEY_LIST             (0x13<<8)
#define REMOTE_KEY_INFO             (0x0b<<8)
#define REMOTE_KEY_EPG              0
#define REMOTE_KEY_ESC              (0x08<<8)
#define REMOTE_KEY_MOVIE            (0x0a<<8)
#define REMOTE_KEY_ALICE            0
#define REMOTE_KEY_LAST             (0x10<<8)
#define REMOTE_KEY_NEXT             (0x11<<8)
#define REMOTE_KEY_MUTE             (1<<8)

#define REMOTE_KEY_0                0x01
#define REMOTE_KEY_1                0x02
#define REMOTE_KEY_2                0x03
#define REMOTE_KEY_3                0x04
#define REMOTE_KEY_4                0x05
#define REMOTE_KEY_5                0x06
#define REMOTE_KEY_6                0x07
#define REMOTE_KEY_7                0x08
#define REMOTE_KEY_8                0x09
#define REMOTE_KEY_9                0x0a

// Keymapping array (could/should this be in progmem?)

#define NUM_MODS    4
#define NUM_MERLIN_KEYS    57
#define NUM_MERLIN_REMOTE_KEYS    16
#define NUM_ALICE_KEYS    40

struct keymap_t {
	uint16_t ruwido;
	uint8_t  usb;
};

struct remote_keymap_t {
	uint16_t ruwido;
	uint16_t  usb;
};

struct keymap_t modmap[NUM_MODS] = {
		{ MERLIN_MOD_CONTROL_LEFT,     MOD_CONTROL_LEFT     },
		{ MERLIN_MOD_SHIFT_LEFT,       MOD_SHIFT_LEFT       },
		{ MERLIN_MOD_ALT_LEFT,         MOD_ALT_LEFT         },
		{ MERLIN_MOD_ALT_RIGHT,        MOD_ALT_RIGHT        },
};

struct keymap_t merlinKeymap[NUM_MERLIN_KEYS] = {
		{ MERLIN_KEY_A,                KEY_A                },
		{ MERLIN_KEY_B,                KEY_B                },
		{ MERLIN_KEY_C,                KEY_C                },
		{ MERLIN_KEY_D,                KEY_D                },
		{ MERLIN_KEY_E,                KEY_E                },
		{ MERLIN_KEY_F,                KEY_F                },
		{ MERLIN_KEY_G,                KEY_G                },
		{ MERLIN_KEY_H,                KEY_H                },
		{ MERLIN_KEY_I,                KEY_I                },
		{ MERLIN_KEY_J,                KEY_J                },
		{ MERLIN_KEY_K,                KEY_K                },
		{ MERLIN_KEY_L,                KEY_L                },
		{ MERLIN_KEY_M,                KEY_M                },
		{ MERLIN_KEY_N,                KEY_N                },
		{ MERLIN_KEY_O,                KEY_O                },
		{ MERLIN_KEY_P,                KEY_P                },
		{ MERLIN_KEY_Q,                KEY_Q                },
		{ MERLIN_KEY_R,                KEY_R                },
		{ MERLIN_KEY_S,                KEY_S                },
		{ MERLIN_KEY_T,                KEY_T                },
		{ MERLIN_KEY_U,                KEY_U                },
		{ MERLIN_KEY_V,                KEY_V                },
		{ MERLIN_KEY_W,                KEY_W                },
		{ MERLIN_KEY_X,                KEY_X                },
		{ MERLIN_KEY_Y,                KEY_Y                },
		{ MERLIN_KEY_Z,                KEY_Z                },
		{ MERLIN_KEY_1,                KEY_1                },
		{ MERLIN_KEY_2,                KEY_2                },
		{ MERLIN_KEY_3,                KEY_3                },
		{ MERLIN_KEY_4,                KEY_4                },
		{ MERLIN_KEY_5,                KEY_5                },
		{ MERLIN_KEY_6,                KEY_6                },
		{ MERLIN_KEY_7,                KEY_7                },
		{ MERLIN_KEY_8,                KEY_8                },
		{ MERLIN_KEY_9,                KEY_9                },
		{ MERLIN_KEY_0,                KEY_0                },
		{ MERLIN_KEY_TAB,              KEY_TAB              },
		{ MERLIN_KEY_COMMA,            KEY_COMMA            },
		{ MERLIN_KEY_PERIOD,           KEY_PERIOD           },
		{ MERLIN_KEY_SLASH,            KEY_SLASH            },
		{ MERLIN_KEY_SEMICOLON,        KEY_SEMICOLON        },
		{ MERLIN_KEY_QUOTE,            KEY_QUOTE            },
		{ MERLIN_KEY_EQUALS,           KEY_EQUALS           },
		{ MERLIN_KEY_BACKSPACE,        KEY_BACKSPACE        },
		{ MERLIN_KEY_ENTER,            KEY_ENTER            },
		{ MERLIN_KEY_SPACE,            KEY_SPACE            },
		{ MERLIN_KEY_RED,              KEY_RED              },
		{ MERLIN_KEY_GREEN,            KEY_GREEN            },
		{ MERLIN_KEY_VT,               KEY_VT               },
		{ MERLIN_KEY_YELLOW,           KEY_YELLOW           },
		{ MERLIN_KEY_BLUE,             KEY_BLUE             },
		{ MERLIN_KEY_UP,               KEY_UP               },
		{ MERLIN_KEY_DOWN,             KEY_DOWN             },
		{ MERLIN_KEY_LEFT,             KEY_LEFT             },
		{ MERLIN_KEY_RIGHT,            KEY_RIGHT            },
		{ MERLIN_KEY_OK,               KEY_OK               },
		{ MERLIN_KEY_EXIT,             KEY_EXIT             },
};

struct remote_keymap_t merlinRemoteKeymap[NUM_MERLIN_REMOTE_KEYS] = {
		{ MERLIN_KEY_VOLPLUS,          REMOTE_KEY_VOLPLUS          },
		{ MERLIN_KEY_VOLMINUS,         REMOTE_KEY_VOLMINUS         },
		{ MERLIN_KEY_CHPLUS,           REMOTE_KEY_CHPLUS           },
		{ MERLIN_KEY_CHMINUS,          REMOTE_KEY_CHMINUS          },
		{ MERLIN_KEY_POWER,            REMOTE_KEY_POWER            },
		{ MERLIN_KEY_P_,               REMOTE_KEY_P_               },
		{ MERLIN_KEY_M_,               REMOTE_KEY_M_               },
		{ MERLIN_KEY_S_,               REMOTE_KEY_S_               },
		{ MERLIN_KEY_TXT,              REMOTE_KEY_TXT              },
		{ MERLIN_KEY_REWIND,           REMOTE_KEY_REWIND           },
		{ MERLIN_KEY_FASTFORWARD,      REMOTE_KEY_FASTFORWARD      },
		{ MERLIN_KEY_PLAYPAUSE,        REMOTE_KEY_PLAYPAUSE        },
		{ MERLIN_KEY_FM,               REMOTE_KEY_FM               },
		{ MERLIN_KEY_STOP,             REMOTE_KEY_STOP             },
		{ MERLIN_KEY_HELP,             REMOTE_KEY_HELP             },
		{ MERLIN_KEY_REC,              REMOTE_KEY_REC              },
}; 

struct remote_keymap_t aliceKeymap[NUM_ALICE_KEYS] = {
		{ ALICE_KEY_POWER,             REMOTE_KEY_POWER             },
		{ ALICE_KEY_1,                 REMOTE_KEY_1                 },
		{ ALICE_KEY_2,                 REMOTE_KEY_2                 },
		{ ALICE_KEY_3,                 REMOTE_KEY_3                 },
		{ ALICE_KEY_4,                 REMOTE_KEY_4                 },
		{ ALICE_KEY_5,                 REMOTE_KEY_5                 },
		{ ALICE_KEY_6,                 REMOTE_KEY_6                 },
		{ ALICE_KEY_7,                 REMOTE_KEY_7                 },
		{ ALICE_KEY_8,                 REMOTE_KEY_8                 },
		{ ALICE_KEY_9,                 REMOTE_KEY_9                 },
		{ ALICE_KEY_0,                 REMOTE_KEY_0                 },
		{ ALICE_KEY_TV,                REMOTE_KEY_TV                },
		{ ALICE_KEY_LIST,              REMOTE_KEY_LIST              },
		{ ALICE_KEY_INFO,              REMOTE_KEY_INFO              },
		{ ALICE_KEY_EPG,               REMOTE_KEY_EPG               },
		{ ALICE_KEY_ESC,               REMOTE_KEY_ESC               },
		{ ALICE_KEY_MOVIE,             REMOTE_KEY_MOVIE             },
		{ ALICE_KEY_UP,                REMOTE_KEY_UP                },
		{ ALICE_KEY_DOWN,              REMOTE_KEY_DOWN              },
		{ ALICE_KEY_LEFT,              REMOTE_KEY_LEFT              },
		{ ALICE_KEY_RIGHT,             REMOTE_KEY_RIGHT             },
		{ ALICE_KEY_OK,                REMOTE_KEY_OK                },
		{ ALICE_KEY_RED,               REMOTE_KEY_RED               },
		{ ALICE_KEY_GREEN,             REMOTE_KEY_GREEN             },
		{ ALICE_KEY_YELLOW,            REMOTE_KEY_YELLOW            },
		{ ALICE_KEY_BLUE,              REMOTE_KEY_BLUE              },
		{ ALICE_KEY_VOLPLUS,           REMOTE_KEY_VOLPLUS           },
		{ ALICE_KEY_VOLMINUS,          REMOTE_KEY_VOLMINUS          },
		{ ALICE_KEY_CHPLUS,            REMOTE_KEY_CHPLUS            },
		{ ALICE_KEY_CHMINUS,           REMOTE_KEY_CHMINUS           },
		{ ALICE_KEY_ALICE,             REMOTE_KEY_ALICE             },
		{ ALICE_KEY_PLAYPAUSE,         REMOTE_KEY_PLAYPAUSE         },
		{ ALICE_KEY_LAST,              REMOTE_KEY_LAST              },
		{ ALICE_KEY_REWIND,            REMOTE_KEY_REWIND            },
		{ ALICE_KEY_FASTFORWARD,       REMOTE_KEY_FASTFORWARD       },
		{ ALICE_KEY_NEXT,              REMOTE_KEY_NEXT              },
		{ ALICE_KEY_MUTE,              REMOTE_KEY_MUTE              },
		{ ALICE_KEY_STOP,              REMOTE_KEY_STOP              },
		{ ALICE_KEY_REC,               REMOTE_KEY_REC               },
		{ ALICE_KEY_TXT,               REMOTE_KEY_TXT               },
};

