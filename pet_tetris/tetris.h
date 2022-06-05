/*****************************************************************************
* TETRIS for Commodore PET (using cc65 compiler)
* (c) Tim Howe, March 2010
*
*****************************************************************************/


/* Define the dimensions of the game board */
#define BOARD_X             10
#define BOARD_Y             20

/* Tetromino definitions (used as indexes: must be between 0 - [TETROMINO_TYPES - 1])
 * http://tetris.wikia.com/wiki/Tetromino */
#define TETROMINO_I         0
#define TETROMINO_O         1
#define TETROMINO_T         2
#define TETROMINO_S         3
#define TETROMINO_Z         4
#define TETROMINO_J         5
#define TETROMINO_L         6

#define TETROMINO_TYPES     7
#define TETROMINO_SIZE      4

/* Constants used for score generation (divided by 10) */
#define SC_1LINE            4
#define SC_2LINE            10
#define SC_3LINE            30
#define SC_4LINE            120

#define ROWS_FOR_LEVEL_UP   10
#define NUM_HIGH_SCORES     7

/* Struct used to hold all information about a tetromino */
typedef struct
{
    UINT8   cells[TETROMINO_SIZE][TETROMINO_SIZE];
    INT8    x;          // Tet x position
    INT8    y;          // Tet y position
    UINT8   size;       // Tet size
    UINT16  array_ofst; // y scaled by BOARD_X (code opt)

} TETROMINO;

/* Struct for a high score entry */
typedef struct
{
    UINT16  score;
    char    name[6];
} HIGHSCORE;


/* Prototypes for functions in tetris.c */
void start_new_game(void);
void create_new_tet(TETROMINO *tet);
void process_key(UINT8 key);
BOOL move_tet_down(void);
BOOL test_collision(TETROMINO *tet);
void tet_landed(TETROMINO *tet);
void update_screen(void);
void update_stats(UINT8 num_full);
void check_for_high_score(void);


/* EOF */

