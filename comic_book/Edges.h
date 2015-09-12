#include	"gz.h"
#include    "disp.h"
#include    "rend.h"


void EdgeDec(FILE* outfile, char* framebuffer, GzDisplay* display, GzDisplay* tempDisplay);
void ve(int rownum, int size, int *vl, GzDisplay *display);
void he(int colnum, int size, int *hl, GzDisplay *display);
