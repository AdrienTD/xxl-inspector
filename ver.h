#if XXLVER == 1
#define _XXLNAME "Asterix XXL 1"
#elif XXLVER == 2
#define _XXLNAME "Asterix XXL 2"
#elif XXLVER == 4
#define _XXLNAME "Asterix Olympic Games"
#endif
#if REMASTER
#define XXLNAME _XXLNAME " Remaster"
#else
#define XXLNAME _XXLNAME
#endif
