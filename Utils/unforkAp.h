#define UNFORKAPP "/home/ap/pub/unforkap.ini"
#define AMT "/home/ap/pub/amt"
#ifdef	__cplusplus
extern "C" {
#endif

void vdNotLoadUnusedApps(void);
char CheckUnloadApp( char * apname );
    
#ifdef	__cplusplus
}
#endif