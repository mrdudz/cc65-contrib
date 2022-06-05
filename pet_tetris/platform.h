/*****************************************************************************
* TETRIS for Commodore PET (using cc65 compiler)
* (c) Tim Howe, March 2010
*
*****************************************************************************/

/* platform.h */


/* PET screen RAM */
#define SCREEN          0x8000
#define SCREEN_WIDTH    40
#define SCREEN_HEIGHT   25

/* PET screen char codes */
#define BLACK           32
#define GREY            102
#define WHITE           160
#define WHITE1          215
#define WHITE2          214
#define FULL_LINE       42
#define LINE_BOT        100
#define LINE_TOP        99
#define LINE_LEFT       101
#define LINE_RIGHT      103
#define BORDER_TOP      67
#define BORDER_BOTTOM   70
#define BORDER_LEFT     66
#define BORDER_RIGHT    72
#define BORDER_TL       85
#define BORDER_TR       73
#define BORDER_BL       74
#define BORDER_BR       75

/* PET keyboard matrix codes */
#define NO_KEY          0xFF
#define KEY_LEFT        42      // 4 key
#define KEY_RIGHT       41      // 6 key
#define KEY_DOWN        18      // 2 key
#define KEY_ROT_L       32      // Z key
#define KEY_ROT_R       24      // X key
#define KEY_DROP        6       // Space key
#define KEY_QUIT        64      // Q key
#define KEY_ENTER       27      // Return

/* PET zero-page locations (ROM version specific) */
#define KEYB_MATRIX         0x97
#define KEYB_BUF_NUM_CHARS  0x9E    // # chars in buffer 0 to 9
#define CURSOR_FLAG         0xA7    // 0=on, 1=off

/* Other platform specifics */
#define HIGHSCORE_MEM       0x033C  // Cass #2 buffer is less volatile than main RAM
#define HIGHSCORE_GUID_MEM  0x033A  // Position of GUID
#define HIGHSCORE_GUID      0x1234  // GUID value

/* Derived & implementation specific constants */
#define BOARD_POS_X     ((SCREEN_WIDTH - BOARD_X) / 2)
#define BOARD_POS_Y     2
#define GAME_SCR_POS    (SCREEN_WIDTH*BOARD_POS_Y + BOARD_POS_X)
#define GAMEOVER_X      17
#define GAMEOVER_Y      9
#define SCORE_X         2
#define SCORE_Y         4
#define HIGHSC_X        2
#define HIGHSC_Y        8
#define LEVEL_X         (BOARD_POS_X + 7)
#define LEVEL_Y         23
#define NEXT_X          31
#define NEXT_Y          4
#define KEYS_X          28
#define KEYS_Y          8


/* Times in units of 1/60 sec (jiffies) */
#define INITIAL_DROP_DELAY      40  // Start with 0.66 second drop rate
#define KEY_REP_HOLD_TIME       20  // For key auto-repeat
#define KEY_REP_TIME            3   //      "
#define FULL_ROWS_DISP_TIME     40
#define END_GAME_PAUSE          120


/* Prototypes for functions in platform.c */
void configure_screen(void);
void print_score(UINT16 sc);
void print_level(UINT8 lev);
void print_game_over(void);
void print_highscores(HIGHSCORE *high_scores);
#ifndef TETRIS_8K
    void print_splash_screen(void);
#endif
void init_high_scores(void);
UINT8 read_keyboard(void);
void flush_keyboard_buffer(void);
void jiffy_wait(UINT16 delay);
#ifdef DAC_TETRIS
    void reset_user_port(void);
#endif


/* Prototypes for functions in tetris_funcs.asm */
void blit_board(unsigned char *board, unsigned char *screenpos);
void blit_tetromino_trans(unsigned char *cells, unsigned char *screenpos);
void blit_tetromino_opaque(unsigned char *cells, unsigned char *screenpos);
void wait_vsync(void);
void rotate_left4(unsigned char *src_cells, unsigned char *dest_cells);
void rotate_right4(unsigned char *src_cells, unsigned char *dest_cells);
void rotate_left3(unsigned char *src_cells, unsigned char *dest_cells);
void rotate_right3(unsigned char *src_cells, unsigned char *dest_cells);
int get_time(void);
void reset_time(void);
void print_uint16_5(UINT16 n);
void print_uint8_2(UINT8 n);
void print_string(char *str);
#ifdef DAC_TETRIS
    void play_sample(unsigned char speed, const unsigned char *sample,
        unsigned int length);
#endif

/* EOF */

