/* Routines defined in abcmusic.s */
extern void silence(void);
extern char abcLock;
extern void abc_update(void);
extern void abcmusic(unsigned char channel, char *tune);
extern void abcinstrument(
    unsigned char decay,
    unsigned char hold,
    unsigned char attack,
    unsigned char volume,
    unsigned char tempo,
    unsigned char flags,
    unsigned char backup,
    unsigned char lobackup,
    unsigned char shift,
    unsigned char ch);
extern char *abc_score_ptr0[];
#pragma zpsym("abc_score_ptr0");
extern char *abc_score_ptr1[];
#pragma zpsym("abc_score_ptr1");
extern char *abc_score_ptr2[];
#pragma zpsym("abc_score_ptr2");
extern char *abc_score_ptr3[];
#pragma zpsym("abc_score_ptr3");
extern char abc_music_ptr[4];

