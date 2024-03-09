/*****************************************************************************
* TETRIS for Commodore PET (using cc65 compiler and ca65 assembler)
* (c) Tim Howe, March 2010
* V1.3 12.3.2010 (Source tidy up for release Apr 2010)
*
* This currently runs on a PET 3016 or 3032.
* I am not sure why this does not run on a 4016 or 4032.
* According to VICE V1.22, the 4000 series appears to switch to lower
* case for no known reason, plus there are gaps between the text lines and
* keyboard matrix differences. Perhaps one day I'll look into these.
*
* This Tetris implementation has been influenced by some of the concepts
* found in Laurens Rodriguez Oscanoa's 'STC', although this source has been
* written from scratch to keep binary output from cc65 to a minimum.
*
* V1.2  Added sticky high score table in cass #2 buffer 033A-037A
* V1.3  Enabled cc65 optimisation (-O).
*       Changed vsync to wait for every even vsync.
*****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "types.h"
#include "tetris.h"
#include "platform.h"

#ifdef DAC_TETRIS
    /* Include the sound sample data */
    #include "pop.h"
    #include "bell.h"
    #include "doh.h"
    //#include "boring.h"
#endif


/* Define the tetromino patterns */
const UINT8 ref_tet[TETROMINO_TYPES][TETROMINO_SIZE][TETROMINO_SIZE] =
{
    {   /* TETROMINO_I */
        { 0,        WHITE2,     0,      0 },
        { 0,        WHITE2,     0,      0 },
        { 0,        WHITE2,     0,      0 },
        { 0,        WHITE2,     0,      0 }
    },
    {   /* TETROMINO_O */
        { GREY,     GREY,       0,      0 },
        { GREY,     GREY,       0,      0 },
        { 0,        0,          0,      0 },
        { 0,        0,          0,      0 }
    },
    {   /* TETROMINO_T */
        { 0,        GREY,       0,      0 },
        { GREY,     GREY,       0,      0 },
        { 0,        GREY,       0,      0 },
        { 0,        0,          0,      0 }
    },
    {   /* TETROMINO_S */
        { 0,        WHITE,      0,      0 },
        { WHITE,    WHITE,      0,      0 },
        { WHITE,    0,          0,      0 },
        { 0,        0,          0,      0 }
    },
    {   /* TETROMINO_Z */
        { WHITE1,   0,          0,      0 },
        { WHITE1,   WHITE1,     0,      0 },
        { 0,        WHITE1,     0,      0 },
        { 0,        0,          0,      0 }
    },
    {   /* TETROMINO_J */
        { WHITE,    WHITE,      0,      0 },
        { 0,        WHITE,      0,      0 },
        { 0,        WHITE,      0,      0 },
        { 0,        0,          0,      0 }
    },
    {   /* TETROMINO_L */
        { 0,        WHITE1,     0,      0 },
        { 0,        WHITE1,     0,      0 },
        { WHITE1,   WHITE1,     0,      0 },
        { 0,        0,          0,      0 }
    }
};

/* Define the size of each tetronino */
const UINT8 ref_tet_size[TETROMINO_TYPES] = { 4, 2, 3, 3, 3, 3, 3 };

/* Global variables */
/* (Global scope is used here to reduce the amount of parameters
   that need to be passed into functions and hence the overall size of
   the code.) */
TETROMINO falling_tet;      /* Struct holding the falling tetronino */
TETROMINO next_tet;         /* Struct to hold the next tetronino */
TETROMINO temp_tet;         /* Temp tetronino space for rotation */
UINT8 board[BOARD_Y][BOARD_X];      /* Main game board */
UINT8 composite[BOARD_Y][BOARD_X];  /* Board and falling tet combined */
UINT16 base_score[TETROMINO_SIZE+1];    /* Scores that increase with level */
HIGHSCORE *high_scores = (HIGHSCORE *)HIGHSCORE_MEM;
UINT8 drop_period;          /* Current period between drops */
UINT8 level;                /* Current game level */
UINT16 score;               /* Corrent score */
UINT16 total_lines;         /* Total lines cleared this game */
UINT16 time;                /* Temp storage of time */
UINT16 next_drop;           /* Abs time at which tet will next drop */
BOOL end_game;              /* Flag used to stay in game loop */


extern void blit_board(unsigned char *board, unsigned char *screenpos);
extern void blit_tetromino_trans(unsigned char *cells, unsigned char *arraypos);
extern void blit_tetromino_opaque(unsigned char *cells, unsigned char *screenpos);

extern void wait_vsync(void);

extern void rotate_left4(unsigned char *src_cells, unsigned char *dest_cells);
extern void rotate_right4(unsigned char *src_cells, unsigned char *dest_cells);

extern void rotate_left3(unsigned char *src_cells, unsigned char *dest_cells);
extern void rotate_right3(unsigned char *src_cells, unsigned char *dest_cells);

extern int get_time(void);
extern void reset_time(void);

extern void print_uint16_5(UINT16 n);
extern void print_uint8_2(UINT8 n);
extern void print_string(char *str);

extern void __fastcall__ play_sample(unsigned char speed, const unsigned char *sample, unsigned int length);

/*****************************************************************************
* Main program entry point
*****************************************************************************/
void main(void)
{
    BOOL quit_game = FALSE;
    UINT8 key;
    UINT8 prev_key = NO_KEY;    // Used to detect key hold for auto-repeat
    UINT16 key_rep_timer;       //  "

    srand(get_time());  // Seed the random source with the current time
    init_high_scores();
    while (read_keyboard() != NO_KEY);  // Wait till no keys are pressed

#ifndef TETRIS_8K
    print_splash_screen();
    while (read_keyboard() == NO_KEY);  // Wait till a key is pressed
#endif

    configure_screen();
    print_highscores(high_scores);


    while (!quit_game)
    {
        start_new_game();

        while (!end_game)
        {
            //*((UINT8 *)SCREEN) ^= 128; // Show game loop activity

            key = read_keyboard();
            time = get_time();

            if (key != NO_KEY) // A key is pressed
            {
                if (key != prev_key) // Different key
                {
                    key_rep_timer = time + KEY_REP_HOLD_TIME;
                    prev_key = key;
                    process_key(key);
                    update_screen();
                }
                else // Same key
                {
                    if (time >= key_rep_timer) // Key held
                    {
                        key_rep_timer = time + KEY_REP_TIME;
                        process_key(key);
                        update_screen();
                    }
                }

                if (key == KEY_QUIT) end_game = TRUE;
            }
            else // No key is pressed
            {
                prev_key = key;
            }


            if (time >= next_drop)
            {
                move_tet_down();
                if (!end_game) update_screen();

                next_drop = get_time() + drop_period;
            }
        } // end while (!end_game)

        print_game_over();
        jiffy_wait(END_GAME_PAUSE);

        check_for_high_score();

        while (read_keyboard() != NO_KEY);

        /* Wait for new game or quit */
        while(1)
        {
            key = read_keyboard();
            if (key == KEY_QUIT) quit_game = TRUE;
            if (key != NO_KEY) break;
        }

    } // end while (!quit_game)

    /* Exit game */
    clrscr();
#ifndef TETRIS_8K
    cputs("splitter.\n\n");
#endif
    flush_keyboard_buffer();

#ifdef DAC_TETRIS
    //play_sample(0x11, boring, BORING_LENGTH);
    reset_user_port();
#endif

}



/*****************************************************************************
* Initialise variables ready for a new game
*****************************************************************************/
void start_new_game(void)
{
    score = 0;
    total_lines = 0;
    level = 0;
    base_score[0] = 0; // Not used
    base_score[1] = SC_1LINE;
    base_score[2] = SC_2LINE;
    base_score[3] = SC_3LINE;
    base_score[4] = SC_4LINE;
    print_score(score);
    print_level(level);
    create_new_tet(&falling_tet);
    create_new_tet(&next_tet);
    drop_period = INITIAL_DROP_DELAY;
    memset(board, BLACK, BOARD_X*BOARD_Y);
    update_screen();
    blit_tetromino_opaque((UINT8 *)(next_tet.cells),
        (UINT8 *)(SCREEN + NEXT_X + NEXT_Y*SCREEN_WIDTH));
    reset_time(); // This is done to avoid a 16bit time-rollover during a game
    next_drop = get_time() + drop_period;
    end_game = FALSE;
}



/*****************************************************************************
* Generate a new random tetromino
*****************************************************************************/
void create_new_tet(TETROMINO *tet)
{
    UINT8 type = rand() % TETROMINO_TYPES;
    memcpy(&tet->cells, &ref_tet[type], TETROMINO_SIZE*TETROMINO_SIZE);
    tet->size = ref_tet_size[type];
    tet->x =  (BOARD_X - ref_tet_size[type]) / 2;
    tet->y = 0;
    tet->array_ofst = (UINT16)composite + tet->x;
}



/*****************************************************************************
* Perform the appropriate action for each key
* Assumed globals: falling_tet, next_tet, drop_period, next_drop
*   score, base_score[]
*****************************************************************************/
void process_key(UINT8 key)
{
    switch (key)
    {
        case KEY_LEFT:
            --falling_tet.x; // Try new position
            if (! test_collision(&falling_tet))
            --falling_tet.array_ofst; // Perform rest of move operation
            else ++falling_tet.x; // Revert back to orig position
        break;

        case KEY_RIGHT:
            ++falling_tet.x; // Try new position
            if (! test_collision(&falling_tet))
            ++falling_tet.array_ofst; // Perform rest of move operation
            else --falling_tet.x; // Revert back to orig position
        break;

        case KEY_ROT_L:
            memcpy((UINT8 *)&temp_tet, (UINT8 *)&falling_tet, sizeof(TETROMINO)); // Take a copy
            /* Rotate the copied tet and store back into the original array */
            if (falling_tet.size == 4) rotate_left4((UINT8 *)temp_tet.cells, (UINT8 *)falling_tet.cells);
            if (falling_tet.size == 3) rotate_left3((UINT8 *)temp_tet.cells, (UINT8 *)falling_tet.cells);
            if (test_collision(&falling_tet))
            memcpy((UINT8 *)&falling_tet, (UINT8 *)&temp_tet, sizeof(TETROMINO)); // Abort rotate
        break;

        case KEY_ROT_R:
            memcpy((UINT8 *)&temp_tet, (UINT8 *)&falling_tet, sizeof(TETROMINO)); // Take a copy
            /* Rotate the copied tet and store back into the original array */
            if (falling_tet.size == 4) rotate_right4((UINT8 *)temp_tet.cells, (UINT8 *)falling_tet.cells);
            if (falling_tet.size == 3) rotate_right3((UINT8 *)temp_tet.cells, (UINT8 *)falling_tet.cells);
            if (test_collision(&falling_tet))
            memcpy((UINT8 *)&falling_tet, (UINT8 *)&temp_tet, sizeof(TETROMINO)); // Abort rotate
        break;

        case KEY_DOWN:
            move_tet_down();
            next_drop = get_time() + drop_period; // Down key restarts the drop timer
        break;

        case KEY_DROP:
            wait_vsync();

            /* Repeat-drop until a new tetromino is generated */
            while(!move_tet_down())
            {
                if (!end_game) update_screen();
            }
            next_drop = get_time() + drop_period;  // A drop restarts the drop timer

            /* Give extra points (= half the 2-row score for the current level) */
            score += (base_score[2] >> 1);
            print_score(score);
        break;

        default:
        break;
    }
}



/*****************************************************************************
* Move the falling tetromino down one line. If a collision would result then
* move the tetromino into the board, update the score & launch a new one.
* Returns TRUE if the tetromino lands and a new one is generated.
* Assumed globals: falling_tet, next_tet
*****************************************************************************/
BOOL move_tet_down(void)
{
    BOOL new_tet = FALSE;

    ++falling_tet.y; // Tentative move down
    if (test_collision(&falling_tet))
    {
        --falling_tet.y; // Collision, so undo move

        /* Weld tetromino into the board & check for completed lines */
        /* Note this func takes a long time if full rows are displayed/deleted */
        tet_landed(&falling_tet);

        /* Select the next tetromino */
        memcpy((UINT8 *)&falling_tet, (UINT8 *)&next_tet, sizeof(TETROMINO));

        /* Create a new next tetromino */
        create_new_tet(&next_tet);
        blit_tetromino_opaque((UINT8 *)(next_tet.cells),
            (UINT8 *)(SCREEN + NEXT_X + NEXT_Y*SCREEN_WIDTH));
        new_tet = TRUE;

        /* If the tetromino immediately results in a collision then the
           game is over */
        if (test_collision(&falling_tet))
        {
            end_game = TRUE;
#ifdef DAC_TETRIS
            play_sample(0x11, doh, DOH_LENGTH);
#endif
        }
    }
    else // No collision
    {
        falling_tet.array_ofst += BOARD_X; // Complete the move down
    }

    return new_tet;
}



/*****************************************************************************
* Test whether the supplied tetromino intersects with eiter the side walls,
*   the bottom wall or any blocks in the board map.
* Returns TRUE if a collision is detected.
* Assumed globals: board[]
*****************************************************************************/
BOOL test_collision(TETROMINO *tet)
{
    INT8 i, j;

    for (i=0; i<tet->size; ++i)
    {
        for (j=0; j<tet->size; ++j)
        {
            if (tet->cells[j][i] != 0)
            {
                /* Check collision with left, right or bottom borders of the map */
                if ((tet->x + i < 0) || (tet->x + i >= BOARD_X)
                        || (tet->y + j >= BOARD_Y))
                {
                    return TRUE;
                }

                /* Check collision with existing cells in the map */
                if (board[j + tet->y][i + tet->x] != BLACK)
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}



/*****************************************************************************
* Copy the supplied tetromino into the board and test for completed rows.
* If rows are filled, mark them for deletion, then delete them
*   and update the stats.
* Assumed globals: board[]
*****************************************************************************/
void tet_landed(TETROMINO *tet)
{
    INT8 i, j;
    UINT8 num_full = 0;
    INT8 full_rows[TETROMINO_SIZE] = { -1, -1, -1, -1 };
    INT8 line;


    /* Copy the tetromnio into the board */
    for (i=0; i<tet->size; ++i)
    {
        for (j=0; j<tet->size; ++j)
        {
            if (tet->cells[j][i] != 0)
                board[j + tet->y][i + tet->x] = tet->cells[j][i];
        }
    }

    /* Test for full rows */
    for (i=0; i<tet->size; ++i)
    {
        line = i + tet->y;
        if (line >= BOARD_Y) break;

        full_rows[i] = line; // Record y pos
        for (j=0; j<BOARD_X; ++j)
        {
            if (board[line][j] == BLACK)
            {
                full_rows[i] = -1; // Not a full row
                break;
            }
        }
        if (full_rows[i] >= 0)
        {
            num_full++;

            /* Mark the line */
            for (j=0; j<BOARD_X; ++j)
            {
                board[line][j] = FULL_LINE;
            }
        }
    }


    if (num_full > 0)
    {
        /* Display board with rows marked for deletion */
        blit_board((UINT8 *)board, (UINT8 *)(SCREEN + GAME_SCR_POS));

#ifdef DAC_TETRIS
        /* Play a sound for each row cleared */
        for (i=0; i<num_full-1; ++i)
        {
            play_sample(0x03, pop, SHORT_POP_LENGTH);
        }
        play_sample(0x03, pop, POP_LENGTH);
        jiffy_wait(20);
#else
        jiffy_wait(FULL_ROWS_DISP_TIME);
#endif

        /* Destroy full rows */
        /* TODO - this could be more efficient for multiple rows */
        for (i=0; i<TETROMINO_SIZE; ++i)
        {
            if (full_rows[i] >= 0)
            {
                memmove(&board[1][0], board,  full_rows[i]*BOARD_X);
                // memcpy didn't work here due to overlapping regions.
            }
        }

        /* Update the score */
        update_stats(num_full);
    }
}



/*****************************************************************************
* Re-paint the board and the falling tetromino
* Assumed globals: falling_tet, board[], composite[]
*****************************************************************************/
void update_screen(void)
{
    memcpy(composite, board, BOARD_X*BOARD_Y);
    blit_tetromino_trans((UINT8 *)(falling_tet.cells), (UINT8 *)falling_tet.array_ofst);
    wait_vsync();
    blit_board((UINT8 *)composite, (UINT8 *)(SCREEN + GAME_SCR_POS));
}



/*****************************************************************************
* Update the score and increase the game level when enough rows
*   have been cleared. (This also shortens the drop period.)
* Assumed globals: score, base_score[], total_lines, level, drop_period
*****************************************************************************/
void update_stats(UINT8 num_full)
{
    /* Update the score */
    score += base_score[num_full];
    print_score(score);

    total_lines += num_full;

    if (total_lines >= ROWS_FOR_LEVEL_UP)
    {
        /* Increase level */
#ifdef DAC_TETRIS
        play_sample(0x0F, bell, SHORT_BELL_LENGTH);
        play_sample(0x0F, bell, BELL_LENGTH);
#endif

        total_lines -= ROWS_FOR_LEVEL_UP;

        ++level;
        print_level(level);

        /* Update the values used for scoring */
        base_score[1] += SC_1LINE;
        base_score[2] += SC_2LINE;
        base_score[3] += SC_3LINE;
        base_score[4] += SC_4LINE;

        /* Increase the game speed */
        drop_period -= (drop_period >> 3); // *= 0.875

        /* The above exponential suffers from severe quantisation.
           I evaluated a version that worked with all values scaled up
           by 256 and this gave a much cleaner exponential decay.
           However, I went back to the version above as it is adequately
           exponential and the 'limit cycle' that occurs when drop_period
           reaches 7 enforces a cap on the min drop_period for free. */
    }
}



/*****************************************************************************
* If the game score is > one in the HS table then shift the lower scores
*   down, insert the current score and fetch the player's 3 initials.
* Assumed globals: score, high_scores[]
*****************************************************************************/
void check_for_high_score(void)
{
    UINT8 i, j;
    BOOL new_high_score = FALSE;

    for (i=0; i<NUM_HIGH_SCORES; ++i)
    {
        if (score > high_scores[i].score)
        {
            /* Move old scores down */
            if (i < NUM_HIGH_SCORES-1)
            {
                memmove(&high_scores[i+1], &high_scores[i],
                    (NUM_HIGH_SCORES-1-i)*sizeof(HIGHSCORE));
            }
            /* Place score into high score table */
            high_scores[i].score = score;
            memset(high_scores[i].name, '?', 3);
            print_highscores(high_scores);
            new_high_score = TRUE;
            break;
        }
    }

    if (new_high_score)
    {
        flush_keyboard_buffer();
        gotoxy(HIGHSC_X+7, HIGHSC_Y+2+(i<<1));
        cursor(1); // cc65 conio

        /* Get new high score name */
        for (j=0; j<3; ++j)
        {
            high_scores[i].name[j] = cgetc();
            /* Filter out unacceptable keypresses */
            if (high_scores[i].name[j] < '0' ||
                high_scores[i].name[j] > 'z')
            {
                --j; // Redo this one
            }
            else
            {
                cputc(high_scores[i].name[j]);
            }
        }

        print_highscores(high_scores);
    }
}


/* EOF */
