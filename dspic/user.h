/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

/* TODO Application specific user parameters used in user.c may go here */

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

/* TODO User level functions prototypes (i.e. InitApp) go here */


void InitApp(void);         /* I/O and Peripheral Initialization */
void InitIO(void);
void MapPins(void);
void InitTimer(void);
void InitSPI1(void);



void SampleOnce(void);
void loadShadowBuff(void);
void enter_sampling_state(void);
void clear_state(void);