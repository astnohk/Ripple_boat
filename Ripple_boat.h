#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>


#define DEF_PI 3.141592653589793238462643383279
#define WINDOW_X 600
#define WINDOW_Y 600
#define MAPDATSIZE 200
#define TERRAINDATSIZE 52
#define SPLASH_MAX 128




struct SWITCH
{
	int Rain;
	int Storm;
	int Write;
	int Damper;
	int Sin;
	int View;
	int Boat;
	int Gravity;
	int Foundering;
	int Track;
	int Splash;
	int Quake;
};




// Functions
void memctrl(int opt);
void drawdot();

void rotterrain(int x,int y);
void drawterrain();

void moveboat();
void calcbank();
void drawboat();

void rain_cloud(double coeff);
void quaker();

