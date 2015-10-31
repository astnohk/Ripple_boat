#include "Ripple_boat.h"




Display *d;
Window w;
Pixmap pix;
GC gc,gcw,gcb,gcc[9],gcEne[7],gcTer;
Colormap cmap;
XColor col,exact;
XEvent event,noev;

struct IXY{int x;int y;};
struct XY{double x;double y;};
struct XYZ{double x;double y;double z;};
struct SPLASH_LIST{
	double x; double y; double z;
	double Vx; double Vy; double Vz;
	struct SPLASH_LIST *prev;
	struct SPLASH_LIST *next;
} Splash_List_Start,*Splash_Pt;

struct DRAW_LIST{
	int x; int y; int z;
	struct DRAW_LIST *next;
} Start_Draw_List;

const int Rand_Half = RAND_MAX/2,Window_X = WINDOW_X,Window_Y = WINDOW_Y,Window_Xh = WINDOW_X/2,Window_Yh = WINDOW_Y/2;
const int MapDatSize = MAPDATSIZE,MapSize = MAPDATSIZE-2,MapSizeh = (MAPDATSIZE-2)/2,TerrainDatSize = TERRAINDATSIZE,TerrainSize = TERRAINDATSIZE-2,coeffTerrainScale = (MAPDATSIZE-2)/(TERRAINDATSIZE-2),Def_Rain_Wait = 25,Def_Quake_Time = 350;
const double Def_M = 1.0,Def_K = 0.05,Def_Strength = 6.0,Def_Damp = 0.97,Def_G = 0.5,Pi = DEF_PI,Pi2 = 2*DEF_PI;
int WMap[MAPDATSIZE][MAPDATSIZE];
double Map[MAPDATSIZE][MAPDATSIZE],Vel[MAPDATSIZE][MAPDATSIZE],Terrain[TERRAINDATSIZE][TERRAINDATSIZE];
struct XY DispTerrain[TERRAINDATSIZE][TERRAINDATSIZE];
int not,key,Rain_Wait,Changed_Rain_Wait,Quake_Time;
double M,K,F,G,Strength,Damp,Scale,Average_KE,Average_PE,Sin_Time,Sin_Coefficient,Threshold_Rain_Wait;
char met[128],off[2][4] = {"off","on "},Viewmode[3][15] = {"Normal","Velocity","Kinetic Energy"};
struct timeval T;
struct IXY Bef_Axis,First_Terrain;
struct XY Axis,ACos,ASin,Shift_raw,Shift,Another_Pointer;

const double Def_MassBoat = 1.0;
const int SurfaceFrame[6][6] = {{2,3,4,5,6,-1},{1,6,10,-1},{1,2,9,-1},{0,5,8,10,-1},{0,3,9,7,-1},{4,7,8,-1}},Frame[11][2] = {{0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,2},{0,4},{0,5},{1,3},{1,6}};
const struct XYZ Boat[7] = {{0,-10,0},{0,4,0},{0,10,5},{5,4,5},{5,-10,5},{-5,-10,5},{-5,4,5}},NormalVect[6] = {{0,0,1},{-6,5,-6},{6,5,-6},{-1,0,-1},{1,0,-1},{0,-1,0}};
struct XYZ xyz_velBoat;
struct XY posBoat,tmposBoat,bankCos,bankSin,bankRad;
int accelBoat;
double MassBoat,vectBoat,vectCos,vectSin,velBoat,height,bef_height,Foundering_height;

SWITCH Switch;




int main(){
	int i,k,itp1,itp2;
	double dtp1,dtp2,dtp3,dtp4;
	struct IXY srchTerrain;
	d = XOpenDisplay(0);
	w = XCreateSimpleWindow(d,RootWindow(d,0),0,0,Window_X,Window_Y,0,BlackPixel(d,0),WhitePixel(d,0));
	XSelectInput(d,w,ExposureMask|KeyPressMask|ButtonPressMask|ButtonMotionMask);
	XStoreName(d,w,"Ripple Boat7");
	XMapWindow(d,w);
	cmap = DefaultColormap(d,0);
	if(!(XAllocNamedColor(d,cmap,"Blue",&col,&exact))) exit(1);
	XSetForeground(d,gc = XCreateGC(d,w,0,0),col.pixel);
	XSetBackground(d,gc,WhitePixel(d,0));

	if(!(XAllocNamedColor(d,cmap,"grey16",&col,&exact))) exit(1);
	XSetForeground(d,gcc[0] = XCreateGC(d,w,0,0),col.pixel);
	if(!(XAllocNamedColor(d,cmap,"NavyBlue",&col,&exact))) exit(1);
	XSetForeground(d,gcc[1] = XCreateGC(d,w,0,0),col.pixel);
	if(!(XAllocNamedColor(d,cmap,"RoyalBlue4",&col,&exact))) exit(1);
	XSetForeground(d,gcc[2] = XCreateGC(d,w,0,0),col.pixel);
	if(!(XAllocNamedColor(d,cmap,"RoyalBlue3",&col,&exact))) exit(1);
	XSetForeground(d,gcc[3] = XCreateGC(d,w,0,0),col.pixel);
	if(!(XAllocNamedColor(d,cmap,"RoyalBlue",&col,&exact))) exit(1);
	XSetForeground(d,gcc[4] = XCreateGC(d,w,0,0),col.pixel);
	if(!(XAllocNamedColor(d,cmap,"Cyan",&col,&exact))) exit(1);
	XSetForeground(d,gcc[5] = XCreateGC(d,w,0,0),col.pixel);
	if(!(XAllocNamedColor(d,cmap,"turquoise",&col,&exact))) exit(1);
	XSetForeground(d,gcc[6] = XCreateGC(d,w,0,0),col.pixel);
	if(!(XAllocNamedColor(d,cmap,"SkyBlue",&col,&exact))) exit(1);
	XSetForeground(d,gcc[7] = XCreateGC(d,w,0,0),col.pixel);
	XSetForeground(d,gcc[8] = XCreateGC(d,w,0,0),WhitePixel(d,0));

	for(i = 0;i<7;i++) gcEne[i] = XCreateGC(d,w,0,0);
	if(!(XAllocNamedColor(d,cmap,"DarkGreen",&col,&exact))) exit(1);
	XSetForeground(d,gcEne[0],col.pixel);
	if(!(XAllocNamedColor(d,cmap,"Green",&col,&exact))) exit(1);
	XSetForeground(d,gcEne[1],col.pixel);
	if(!(XAllocNamedColor(d,cmap,"chartreuse",&col,&exact))) exit(1);
	XSetForeground(d,gcEne[2],col.pixel);
	if(!(XAllocNamedColor(d,cmap,"Yellow",&col,&exact))) exit(1);
	XSetForeground(d,gcEne[3],col.pixel);
	if(!(XAllocNamedColor(d,cmap,"Orange",&col,&exact))) exit(1);
	XSetForeground(d,gcEne[4],col.pixel);
	if(!(XAllocNamedColor(d,cmap,"OrangeRed",&col,&exact))) exit(1);
	XSetForeground(d,gcEne[5],col.pixel);
	if(!(XAllocNamedColor(d,cmap,"Red",&col,&exact))) exit(1);
	XSetForeground(d,gcEne[6],col.pixel);

	if(!(XAllocNamedColor(d,cmap,"grey25",&col,&exact))) exit(1);
	XSetForeground(d,gcTer = XCreateGC(d,w,0,0),col.pixel);

	XSetForeground(d,gcw = XCreateGC(d,w,0,0),WhitePixel(d,0));
	XSetForeground(d,gcb = XCreateGC(d,w,0,0),BlackPixel(d,0));
	pix = XCreatePixmap(d,w,Window_X,Window_Y,DefaultDepth(d,0));
	XMaskEvent(d,ExposureMask,&noev);

	All_Reset:
	srand(time(0));
	M = Def_M;
	K = Def_K;
	G = Def_G;
	Axis.x = Axis.y = .0;
	ACos.x = ACos.y = 1.0;
	ASin.x = ASin.y = .0;
	Scale = 1.12;
	Shift_raw.x = Shift_raw.y = Shift.x = Shift.y = 0;
	Strength = Def_Strength;
	Damp = Def_Damp;
	Changed_Rain_Wait = Threshold_Rain_Wait = Def_Rain_Wait;
	Sin_Coefficient = 2.0;
	Another_Pointer.x = Another_Pointer.y = MapSize/2-5;
	accelBoat = 0;
	vectBoat = 0;
	velBoat = 0;
	MassBoat = Def_MassBoat;
	posBoat.x = posBoat.y = MapSize/2;
	vectCos = bankCos.x = bankCos.y = 1.0;
	vectSin = bankSin.x = bankSin.y = 0;
	W_Reset:
	for(i = 0;i< = MapSize;i++)
		for(k = 0;k< = MapSize;k++)
			WMap[i][k] = Map[i][k] = Vel[i][k] = .0;
	for(i = 0;i<TerrainDatSize;i++){
		for(k = 0;k<TerrainDatSize;k++){
			Terrain[i][k] = -5.0;
			rotterrain(0,0);
		}
	}
	xyz_velBoat.x = xyz_velBoat.y = xyz_velBoat.z = .0;

	while(1){
		not = 0;
		T.tv_usec = 1E6/60.0;
		while(XPending(d)){
			XNextEvent(d,&event);
			if((key = XLookupKeysym(&event.xkey,0))= = XK_Escape) exit(0);
			switch(event.type){
				case ButtonPress:
					if(event.xbutton.button= = Button4){
						Scale* = 1.12;
						Shift_raw.x* = 1.12; Shift_raw.y* = 1.12;
						Shift.x* = 1.12; Shift.y* = 1.12;
						rotterrain(0,0);
						break;
					}
					else if(event.xbutton.button= = Button5){
						Scale/ = 1.12;
						Shift_raw.x/ = 1.12; Shift_raw.y/ = 1.12;
						Shift.x/ = 1.12; Shift.y/ = 1.12;
						rotterrain(0,0);
						break;
					}
					if(event.xbutton.button= = Button2 || event.xbutton.button= = Button3){
						Bef_Axis.x = event.xbutton.x;
						Bef_Axis.y = event.xbutton.y;
					}else if(Switch.Write){
						dtp4 = 1E8;
						srchTerrain.x = srchTerrain.y = 0;
						for(i = 1;i< = TerrainSize;i++){
							for(k = 1;k< = TerrainSize;k++){
								dtp1 = event.xbutton.x-DispTerrain[i][k].x;
								dtp2 = event.xbutton.y-DispTerrain[i][k].y;
								if((dtp3 = dtp1*dtp1+dtp2*dtp2)<dtp4){
									dtp4 = dtp3;
									srchTerrain.x = i;
									srchTerrain.y = k;
								}
							}
						}
						if(srchTerrain.x<1 || srchTerrain.x>TerrainSize || srchTerrain.y<0 || srchTerrain.y>TerrainSize) break;
						Terrain[srchTerrain.x][srchTerrain.y]+ = event.xbutton.button= = Button1?0.2:-0.2;
						rotterrain(srchTerrain.x,srchTerrain.y);
						First_Terrain.x = srchTerrain.x;
						First_Terrain.y = srchTerrain.y;
					}else{
						itp1 = (event.xbutton.x-50)/2-MapSizeh;
						itp2 = (event.xbutton.y-50)/2-MapSizeh;
						dtp2 = itp2/ACos.y;
						dtp1 = (itp1*ACos.x-dtp2*ASin.x)/Scale+MapSizeh;
						itp2 = (dtp2*ACos.x+itp1*ASin.x)/Scale+MapSizeh;
						itp1 = dtp1;
						if(itp1<1 || itp1>MapSize) break;
						if(itp2<1 || itp2>MapSize) break;
						Map[itp1][itp2]+ = Strength;
					}
					break;
				case MotionNotify:
					if(event.xbutton.state&Button3Mask){
						Axis.x+ = (event.xbutton.x-Bef_Axis.x)/100.0;
						Axis.y- = (event.xbutton.y-Bef_Axis.y)/100.0;
						if(fabs(Axis.x)>Pi) Axis.x+ = Axis.x>0?-Pi2:Pi2;
						if(fabs(Axis.y)>Pi) Axis.y+ = Axis.y>0?-Pi2:Pi2;
						ASin.x = sin(Axis.x);
						ASin.y = sin(Axis.y);
						ACos.x = cos(Axis.x);
						ACos.y = cos(Axis.y);
						Shift.x = Shift_raw.x*ACos.x+Shift_raw.y*ASin.x;
						Shift.y = (Shift_raw.y*ACos.x-Shift_raw.x*ASin.x)*ACos.y;
						rotterrain(0,0);
						Bef_Axis.x = event.xbutton.x;
						Bef_Axis.y = event.xbutton.y;
					}else if(event.xbutton.state&Button2Mask){
						Shift.x+ = event.xbutton.x-Bef_Axis.x;
						Shift.y+ = event.xbutton.y-Bef_Axis.y;
						Shift_raw.x = Shift.x*ACos.x-Shift.y/ACos.y*ASin.x;
						Shift_raw.y = Shift.y/ACos.y*ACos.x+Shift.x*ASin.x;
						Bef_Axis.x = event.xbutton.x;
						Bef_Axis.y = event.xbutton.y;
					}else if(Switch.Write){
						dtp4 = 1E8;
						itp1 = First_Terrain.x+2< = TerrainSize?First_Terrain.x+2:TerrainSize;
						itp2 = First_Terrain.y+2< = TerrainSize?First_Terrain.y+2:TerrainSize;
						for(i = First_Terrain.x-2>0?First_Terrain.x-2:1;i< = itp1;i++){
							for(k = First_Terrain.y-2>0?First_Terrain.y-2:1;k< = itp2;k++){
								dtp1 = event.xbutton.x-DispTerrain[i][k].x;
								dtp2 = event.xbutton.y-DispTerrain[i][k].y;
								if((dtp3 = dtp1*dtp1+dtp2*dtp2)<dtp4){
									dtp4 = dtp3;
									srchTerrain.x = i;
									srchTerrain.y = k;
								}
							}
						}
						if(srchTerrain.x<1 || srchTerrain.x>TerrainSize || srchTerrain.y<0 || srchTerrain.y>TerrainSize) break;
						Terrain[srchTerrain.x][srchTerrain.y]+ = event.xbutton.state&Button1Mask?0.2:-0.2;
						rotterrain(srchTerrain.x,srchTerrain.y);
						if(fabs(Axis.y)<Pi/6.0){
							First_Terrain.x = srchTerrain.x;
							First_Terrain.y = srchTerrain.y;
						}
					}else{
						itp1 = (event.xbutton.x-50)/2-MapSizeh;
						itp2 = (event.xbutton.y-50)/2-MapSizeh;
						dtp2 = itp2/ACos.y;
						dtp1 = (itp1*ACos.x-dtp2*ASin.x)/Scale+MapSizeh;
						itp2 = (dtp2*ACos.x+itp1*ASin.x)/Scale+MapSizeh;
						itp1 = dtp1;
						if(itp1<1 || itp1>MapSize) break;
						if(itp2<1 || itp2>MapSize) break;
						Map[itp1][itp2]+ = Strength;
					}
					break;
				case KeyPress:
					switch(key){
						case XK_e:
							while(1){
								XDrawImageString(d,w,gcw,270,180,"EDIT MODE",9);
								sprintf(met,"[e]nd   [s]trength   [r]ain Wait  [b]oat Weight  [g]ravity");
								XDrawImageString(d,w,gcw,130,260,met,strlen(met));
								XNextEvent(d,&event);
								if(event.type! = KeyPress) continue;
								if((key = XLookupKeysym(&event.xkey,0))= = XK_Escape) exit(0);
								else if(key= = XK_e) break;
								switch(key){
									case XK_s:
										XCopyArea(d,pix,w,gc,0,0,Window_X,Window_Y,0,0);
										XDrawImageString(d,w,gcw,260,180,"EDIT Strength",13);
										XDrawImageString(d,w,gcw,170,210,"[Up/Down] Change the Value  [d]efault  [e]nd",44);
										while(1){
											sprintf(met,"Strength  %4.0f",Strength);
											XDrawImageString(d,w,gcw,260,280,met,strlen(met));
											XNextEvent(d,&event);
											if(event.type! = KeyPress) continue;
											if((key = XLookupKeysym(&event.xkey,0))= = XK_Escape) exit(0);
											else if(key= = XK_e) break;
											switch(key){
												case XK_d: Strength = Def_Strength; break;
												case XK_Up: Strength++; break;
												case XK_Down: Strength--; break;
											}
											not = (Strength>1000 && (Strength = 1000));
											not = (Strength<-1000 && (Strength = -1000));
										}
										break;
									case XK_r:
										XCopyArea(d,pix,w,gc,0,0,Window_X,Window_Y,0,0);
										XDrawImageString(d,w,gcw,260,180,"EDIT Rain Wait",14);
										XDrawImageString(d,w,gcw,170,210,"[Up/Down] Change the Value  [d]efault  [e]nd",44);
										while(1){
											sprintf(met,"Rain Wait  %.3f",Threshold_Rain_Wait);
											XDrawImageString(d,w,gcw,250,280,met,strlen(met));
											XNextEvent(d,&event);
											if(event.type! = KeyPress) continue;
											if((key = XLookupKeysym(&event.xkey,0))= = XK_Escape) exit(0);
											else if(key= = XK_e) break;
											switch(key){
												case XK_d: Changed_Rain_Wait = Threshold_Rain_Wait = Def_Rain_Wait; break;
												case XK_Up: Changed_Rain_Wait = Threshold_Rain_Wait++; break;
												case XK_Down: Changed_Rain_Wait = Threshold_Rain_Wait--; break;
											}
											not = (Threshold_Rain_Wait>100 && (Changed_Rain_Wait = Threshold_Rain_Wait = 100));
											not = (Threshold_Rain_Wait<1 && (Changed_Rain_Wait = Threshold_Rain_Wait = 1));
										}
										break;
									case XK_b:
										XCopyArea(d,pix,w,gc,0,0,Window_X,Window_Y,0,0);
										XDrawImageString(d,w,gcw,250,180,"EDIT Boat Weight",16);
										XDrawImageString(d,w,gcw,170,210,"[Up/Down] Change the Value  [d]efault  [e]nd",44);
										while (1){
											sprintf(met,"Boat Mass  %.3f",MassBoat);
											XDrawImageString(d,w,gcw,250,280,met,strlen(met));
											XNextEvent(d,&event);
											if(event.type! = KeyPress) continue;
											if((key = XLookupKeysym(&event.xkey,0))= = XK_Escape) exit(0);
											else if(key= = XK_e) break;
											switch(key){
												case XK_d: MassBoat = Def_MassBoat; break;
												case XK_Up: MassBoat+ = (MassBoat<5.0)*0.25; break;
												case XK_Down: MassBoat- = (MassBoat>.0)*0.25; break;
											}
										}
										break;
									case XK_g:
										XCopyArea(d,pix,w,gc,0,0,Window_X,Window_Y,0,0);
										XDrawImageString(d,w,gcw,230,180,"EDIT Gravity Coefficient",24);
										XDrawImageString(d,w,gcw,170,210,"[Up/Down] Change the Value  [d]efault  [e]nd",44);
										while(1){
											sprintf(met,"Gravity Coeff  %.3f",G);
											XDrawImageString(d,w,gcw,240,280,met,strlen(met));
											XNextEvent(d,&event);
											if(event.type! = KeyPress) continue;
											if((key = XLookupKeysym(&event.xkey,0))= = XK_Escape) exit(0);
											else if(key= = XK_e) break;
											switch(key){
												case XK_d: G = Def_G; break;
												case XK_Up: G+ = (G<0.98)*0.01; break;
												case XK_Down: G- = (G>0.01)*0.01; break;
											}
										}
										break;
								}
								XCopyArea(d,pix,w,gc,0,0,Window_X,Window_Y,0,0);
							}
							break;
						case XK_F2: goto All_Reset;
						case XK_F3: goto W_Reset;
						case XK_r:
							Switch.Rain = !Switch.Rain;
							if(!Switch.Rain) Threshold_Rain_Wait = Changed_Rain_Wait;
							break;
						case XK_c: Switch.Storm = !Switch.Storm; break;
						case XK_w: Switch.Write = !Switch.Write; break;
						case XK_d: Switch.Damper = !Switch.Damper; break;
						case XK_s:
							Switch.Sin = !Switch.Sin;
							if(!Switch.Sin) Sin_Time = .0;
							break;
						case XK_v: Switch.View = !Switch.View; break;
						case XK_b: Switch.Boat = !Switch.Boat; break;
						case XK_g: Switch.Gravity = !Switch.Gravity; break;
						case XK_f: Switch.Foundering = !Switch.Foundering; break;
						case XK_t: Switch.Track = !Switch.Track; break;
						case XK_q: Switch.Quake = !Switch.Quake; break;
						case XK_Up:
							if(Switch.Boat)
								accelBoat+ = accelBoat<5;
							else{
								Another_Pointer.x+ = ASin.x*2.0;
								Another_Pointer.y- = ACos.x*2.0;
								goto Another_Pointer_Adjust;
							}
							break;
						case XK_Down:
							if(Switch.Boat)
								accelBoat- = accelBoat>-2;
							else{
								Another_Pointer.x- = ASin.x*2.0;
								Another_Pointer.y+ = ACos.x*2.0;
								goto Another_Pointer_Adjust;
							}
							break;
						case XK_Left:
							if(Switch.Boat){
								vectBoat+ = 0.075;
								goto vectBoat_Calc;
							}else{
								Another_Pointer.x- = ACos.x*2.0;
								Another_Pointer.y- = ASin.x*2.0;
								goto Another_Pointer_Adjust;
							}
						case XK_Right:
							if(Switch.Boat){
								vectBoat- = 0.075;
								vectBoat_Calc:
								vectCos = cos(vectBoat);
								vectSin = sin(vectBoat);
							}else{
								Another_Pointer.x+ = ACos.x*2.0;
								Another_Pointer.y+ = ASin.x*2.0;
								Another_Pointer_Adjust:
								if(Another_Pointer.x<1.0) Another_Pointer.x = MapSize;
								else if(Another_Pointer.x> = MapDatSize) Another_Pointer.x = 1.0;
								if(Another_Pointer.y<1.0) Another_Pointer.y = MapSize;
								else if(Another_Pointer.y> = MapDatSize) Another_Pointer.y = 1.0;
							}
							break;
					}
			}
		}

		if(Switch.Rain){
			if(!(Rain_Wait = (Rain_Wait+1)*(Rain_Wait<Threshold_Rain_Wait))){
				Threshold_Rain_Wait* = 0.96;
				rain_cloud(3.0);
			}
		}
		if(Switch.Storm)
			for(i = 0;i<4;i++) rain_cloud(7.0);

		if(Switch.Sin){
			Sin_Time+ = 0.024544*Sin_Coefficient;
			Map[(int)Another_Pointer.x][(int)Another_Pointer.y] = Strength*sin(Sin_Time)*2.0;
		}

		Average_PE = Average_KE = .0;
		#pragma omp parallel for private(k, F, dtp1,dtp2) reduction(+:Average_PE) reduction(+:Average_KE)
		for(i = 1;i< = MapSize;i++){
			k = 0;
			while(++k< = MapSize){
				if(WMap[i][k]){
					Map[i][k] = Vel[i][k] = .0;
					continue;
				}
				dtp1 = Map[i][k-1]-Map[i][k];
				F = dtp1;
				dtp2 = dtp1*dtp1;
				dtp1 = Map[i][k+1]-Map[i][k];
				F+ = dtp1;
				dtp2+ = dtp1*dtp1;
				dtp1 = Map[i-1][k]-Map[i][k];
				F+ = dtp1;
				dtp2+ = dtp1*dtp1;
				dtp1 = Map[i+1][k]-Map[i][k];
				F+ = dtp1;
				dtp2+ = dtp1*dtp1;
				Average_PE+ = K*dtp2/2.0;
				Vel[i][k]+ = K*F/M;
				not = (Switch.Damper && (Vel[i][k]* = Damp));
				Average_KE+ = M*Vel[i][k]*Vel[i][k]/2.0;
			}
		}
		Average_KE/ = 62500.0;
		Average_PE/ = 62500.0;
		itp1 = posBoat.x-3.0*sin(vectBoat);
		itp2 = posBoat.y-3.0*cos(vectBoat);

		#pragma omp parallel for private(k)
		for(i = 1;i< = MapSize;i++){
			k = 0;
			while(++k< = MapSize){
				Map[i][k]+ = Vel[i][k];
				if(Switch.Boat && i= = itp1 && k= = itp2){
					Map[i][k]- = MassBoat*(height+Foundering_height<Map[i][k]);
					if(Switch.Foundering) Map[i][k]+ = rand()/(1.0+RAND_MAX)*4.0-2.0;
				}
			}
		}

		if(Switch.Quake){
			if(Quake_Time-->0) quaker();
			else Switch.Quake = 0;
		}else if(Quake_Time<Def_Quake_Time) Quake_Time = Def_Quake_Time;

		moveboat();

		XFillRectangle(d,pix,gcb,0,0,Window_X,Window_Y);
		drawterrain();
		if(!Switch.Boat) drawdot();
		else{
			calcbank();
			if(ACos.y<0){
				drawboat();
				drawdot();
			}else{
				drawdot();
				drawboat();
			}
		}
		sprintf(met,"View \"%s\" Boat_V(% .2f kt, Z % .2f kt) Scale %.2f rot(% .2f % .2f)",Viewmode[Switch.View],velBoat,xyz_velBoat.z,Scale,Axis.x,Axis.y);
		XDrawString(d,pix,gcw,160,12,met,strlen(met));
		sprintf(met,"Quake %s",off[Switch.Quake]);
		XDrawString(d,pix,gcw,3,568,met,strlen(met));
		sprintf(met,"Kinetic %f  Potential %f  Boat %s  Gravity %s  Founder %s  Tracking %s",Average_KE,Average_PE,off[Switch.Boat],off[Switch.Gravity],off[Switch.Foundering],off[Switch.Track]);
		XDrawString(d,pix,gcw,3,582,met,strlen(met));
		sprintf(met,"Write %s  Damp %s  Sin %s  Rain %s  Cataract %s  View %d  Thres_Rain %f  Rain_Wait %d",off[Switch.Write],off[Switch.Damper],off[Switch.Sin],off[Switch.Rain],off[Switch.Storm],Switch.View,Threshold_Rain_Wait,Rain_Wait);
		XDrawString(d,pix,gcw,3,596,met,strlen(met));
		XCopyArea(d,pix,w,gc,0,0,Window_X,Window_Y,0,0);
		select(0,0,0,0,&T);
		XMaskEvent(d,ExposureMask,&noev);
	}
	return 0;
}

void memctrl(int opt){
	static int Max = TERRAINDATSIZE*TERRAINDATSIZE;
	int i;
	struct DRAW_LIST *bef = &Start_Draw_List,*pt;
	if(!opt){
		for(i = 0;i<Max;i++){
			if((pt = calloc(1,sizeof(struct DRAW_LIST)))= = 0){
				printf("MEMORY ALLOCATE ERROR\n");
				exit(1);
			}
			bef->next = pt;
			pt->next = 0;
			bef = pt;
		}
	}else{
		pt = Start_Draw_List.next;
		while (1) {
			if(pt->next){
				bef = pt;
				pt = pt->next;
				free(bef);
			}else{
				free(pt);
				break;
			}
		}
	}
}

void drawdot(){
	int i,k,itp1;
	double dtp1,dtp2;
	struct XY disp;

	for(i = 1;i< = MapSize;i++){
		for(k = 1;k< = MapSize;k++){
			dtp1 = i*2-MapSize-1;
			dtp2 = k*2-MapSize-1;
			disp.x = (dtp1*ACos.x+dtp2*ASin.x-Switch.Track*tmposBoat.x)*Scale+Window_Xh+Shift.x;
			disp.y = dtp2*ACos.x-dtp1*ASin.x;
			disp.y = (disp.y*ACos.y-Map[i][k]*ASin.y-Switch.Track*tmposBoat.y)*Scale+Window_Yh+Shift.y;
			dtp1 = 1.0+0.2*Scale;
			if(!Switch.View){
				itp1 = Map[i][k]/2;
				XFillRectangle(d,pix,gcc[itp1>-3?itp1<3?itp1+4:8:0],disp.x,disp.y,dtp1,dtp1);
			}else{
				itp1 = M*fabs(Vel[i][k])*20.0;
				if(itp1>6) itp1 = 6;
				XFillRectangle(d,pix,gcEne[itp1],disp.x,disp.y,dtp1,dtp1);
			}
		}
	}
	for(i = 0;i<MapSize;i++){
		for(k = 0;k<MapSize;k++){
			if(WMap[i][k]){
				dtp1 = i*2-MapSize;
				dtp2 = k*2-MapSize;
				disp.x = (dtp1*ACos.x+dtp2*ASin.x)*Scale+Window_Xh+Shift.x;
				disp.y = dtp2*ACos.x-dtp1*ASin.x;
				disp.y = disp.y*ACos.y*Scale+Window_Yh+Shift.y;
				XFillRectangle(d,pix,gcw,disp.x,disp.y,1.5*Scale,1.5*Scale);
			}
		}
	}

	dtp1 = Another_Pointer.x*2-MapDatSize;
	dtp2 = Another_Pointer.y*2-MapDatSize;
	disp.x = (dtp1*ACos.x+dtp2*ASin.x-Switch.Track*tmposBoat.x)*Scale+300+Shift.x;
	disp.y = dtp2*ACos.x-dtp1*ASin.x;
	disp.y = (disp.y*ACos.y-Switch.Track*tmposBoat.y)*Scale+300+Shift.y;
	XDrawArc(d,pix,gcw,disp.x,disp.y,2+2.0*Scale,(2+2.0*Scale)*fabs(ACos.y),0,23000);
}

void rotterrain(int x,int y){
	int i,k;
	double dx,dy,tmpx,tmpy;
	if(x>0 && x<TerrainDatSize-1 && y>0 && y<TerrainDatSize){
		dx = x*2-TerrainDatSize+1;
		dy = y*2-TerrainDatSize+1;
		tmpx = dx*ACos.x+dy*ASin.x;
		tmpy = dy*ACos.x-dx*ASin.x;
		tmpy = tmpy*ACos.y-Terrain[x][y]*ASin.y;
		DispTerrain[x][y].x = tmpx*coeffTerrainScale*Scale+Window_Xh;
		DispTerrain[x][y].y = tmpy*coeffTerrainScale*Scale+Window_Yh;
	}else{
		dx = -TerrainDatSize+1;
		for(i = 0;i<TerrainDatSize;i++){
			dy = -TerrainDatSize+1;
			for(k = 0;k<TerrainDatSize;k++){
				tmpx = dx*ACos.x+dy*ASin.x;
				tmpy = dy*ACos.x-dx*ASin.x;
				tmpy = tmpy*ACos.y-Terrain[i][k]*ASin.y;
				DispTerrain[i][k].x = tmpx*coeffTerrainScale*Scale+Window_Xh;
				DispTerrain[i][k].y = tmpy*coeffTerrainScale*Scale+Window_Yh;
				dy+ = 2;
			}
			dx+ = 2;
		}
	}
}

void drawterrain(){
	int i,k;
	for(i = 0;i<TerrainDatSize;i++){
		for(k = 0;k<TerrainDatSize;k++){
			if(i+1<TerrainDatSize) XDrawLine(d,pix,gcTer,DispTerrain[i][k].x+Shift.x,DispTerrain[i][k].y+Shift.y,DispTerrain[i+1][k].x+Shift.x,DispTerrain[i+1][k].y+Shift.y);
			if(k+1<TerrainDatSize) XDrawLine(d,pix,gcTer,DispTerrain[i][k].x+Shift.x,DispTerrain[i][k].y+Shift.y,DispTerrain[i][k+1].x+Shift.x,DispTerrain[i][k+1].y+Shift.y);
		}
	}
}

void moveboat(){
	int i,itp1,itp2;
	double dtp1,dtp2,dtp3;
	struct XY Cos,Sin;

	if(accelBoat! = 0){
		velBoat+ = accelBoat*0.0078125;
		if(velBoat>1.25) velBoat = 1.25;
		else if(velBoat<-0.5) velBoat = -0.5;
	}

	if(Switch.Gravity){
		dtp1 = dtp2 = dtp3 = .0;
		itp2 = posBoat.y;
		if(itp2<1) itp2 = 1;
		if(itp2>MapSize) itp2 = MapSize;
		for(i = -1;i>-5;i--){
			itp1 = posBoat.x+i;
			if(itp1>0 && itp1< = MapSize) dtp3 = Map[itp1][itp2];
			dtp1+ = dtp3;
		}
		dtp3 = .0;
		for(i = 1;i<5;i++){
			itp1 = posBoat.x+i;
			if(itp1>0 && itp1< = MapSize) dtp3 = Map[itp1][itp2];
			dtp2+ = dtp3;
		}
		dtp1 = atan((dtp1-dtp2)/16.0);
		Cos.x = cos(dtp1);
		Sin.x = sin(dtp1);

		dtp1 = dtp2 = dtp3 = .0;
		itp1 = posBoat.x;
		if(itp1<1) itp1 = 1;
		else if(itp1>MapSize) itp1 = MapSize;
		for(i = -1;i>-5;i--){
			itp2 = posBoat.y+i;
			if(itp2>0 && itp2< = MapSize) dtp3 = Map[itp1][itp2];
			dtp1+ = dtp3;
		}
		dtp3 = .0;
		for(i = 1;i<5;i++){
			itp2 = posBoat.y+i;
			if(itp2>0 && itp2< = MapSize) dtp3 = Map[itp1][itp2];
			dtp2+ = dtp3;
		}
		dtp1 = atan((dtp1-dtp2)/16.0);
		Cos.y = cos(dtp1);
		Sin.y = sin(dtp1);

		xyz_velBoat.x+ = G*Sin.x*Cos.x/100.0;
		if(fabs(xyz_velBoat.x)>1.0) xyz_velBoat.x = xyz_velBoat.x>0?1.0:-1.0;
		xyz_velBoat.y+ = G*Sin.y*Cos.y/100.0;
		if(fabs(xyz_velBoat.y)>1.0) xyz_velBoat.y = xyz_velBoat.y>0?1.0:-1.0;
	}else
		xyz_velBoat.x = xyz_velBoat.y = .0;

	posBoat.x+ = sin(vectBoat)*velBoat+xyz_velBoat.x*Switch.Gravity;
	posBoat.y+ = cos(vectBoat)*velBoat+xyz_velBoat.y*Switch.Gravity;
	velBoat- = velBoat>0?0.0015625:-0.0015625;
	if(posBoat.x<1.0) posBoat.x = 1.0;
	else if(posBoat.x>MapSize) posBoat.x = MapSize;
	if(posBoat.y<1.0) posBoat.y = 1.0;
	else if(posBoat.y>MapSize) posBoat.y = MapSize;

	tmposBoat.x = ((posBoat.x-MapSizeh)*ACos.x+(posBoat.y-MapSizeh)*ASin.x)*2.0;
	tmposBoat.y = ((posBoat.y-MapSizeh)*ACos.x-(posBoat.x-MapSizeh)*ASin.x)*2.0*ACos.y;
}

void calcbank(){
	int i,k,itp1,itp2;
	double dtp1,dtp2,dtp3,Map_height;

	for(dtp1 = dtp2 = 0,i = -3;i<4;i++){
		for(k = -3;k<4;k++){
			itp1 = posBoat.x+i;
			itp2 = posBoat.y+k;
			if(itp1>0 && itp1< = MapSize && itp2>0 && itp2< = MapSize){
				dtp1+ = Map[itp1][itp2];
				dtp2++;
			}
		}
	}
	Map_height = dtp1 = dtp1/dtp2;
	dtp2 = dtp1-0.75-bef_height;
	xyz_velBoat.z+ = dtp2;
	xyz_velBoat.z+ = xyz_velBoat.z*xyz_velBoat.z*(xyz_velBoat.z>0?-0.1:0.1);
	bef_height = height = bef_height+xyz_velBoat.z;

	itp1 = posBoat.x/MapSize*TerrainSize;
	itp2 = posBoat.y/MapSize*TerrainSize;
	if(itp1<1) itp1 = 1;
	else if(itp1> = TerrainSize) itp1 = TerrainSize-1;
	if(itp2<1) itp2 = 1;
	else if(itp2> = TerrainSize) itp2 = TerrainSize-1;
	dtp1 = (Terrain[itp1][itp2]+Terrain[itp1+1][itp2]+Terrain[itp1][itp2+1]+Terrain[itp1+1][itp2+1])/4.0*coeffTerrainScale;
	dtp2 = height+Foundering_height;
	if(Switch.Foundering && dtp2>dtp1) Foundering_height- = 0.25;
	else if(!Switch.Foundering && Foundering_height<0)
		if((Foundering_height+ = 0.4)> = 0) Switch.Splash = 1;

	if(height+Foundering_height<Map_height){
		dtp1 = dtp2 = dtp3 = .0;
		itp2 = posBoat.y;
		if(itp2<1) itp2 = 1;
		if(itp2>MapSize) itp2 = MapSize;
		for(i = -1;i>-5;i--){
			itp1 = posBoat.x+i;
			if(itp1>0 && itp1< = MapSize) dtp3 = Map[itp1][itp2];
			dtp1+ = dtp3;
		}
		dtp3 = .0;
		for(i = 1;i<5;i++){
			itp1 = posBoat.x+i;
			if(itp1>0 && itp1< = MapSize) dtp3 = Map[itp1][itp2];
			dtp2+ = dtp3;
		}
		dtp1 = atan((dtp1-dtp2)/32.0);
		if(fabs(dtp2 = dtp1-bankRad.x)<0.03125) bankRad.x = dtp1;
		else bankRad.x+ = dtp2>0?0.03125:-0.03125;
		bankCos.x = cos(bankRad.x);
		bankSin.x = sin(bankRad.x);

		dtp1 = dtp2 = dtp3 = .0;
		itp1 = posBoat.x;
		if(itp1<1) itp1 = 1;
		else if(itp1>MapSize) itp1 = MapSize;
		for(i = -1;i>-5;i--){
			itp2 = posBoat.y+i;
			if(itp2>0 && itp2< = MapSize) dtp3 = Map[itp1][itp2];
			dtp1+ = dtp3;
		}
		dtp3 = .0;
		for(i = 1;i<5;i++){
			itp2 = posBoat.y+i;
			if(itp2>0 && itp2< = MapSize) dtp3 = Map[itp1][itp2];
			dtp2+ = dtp3;
		}
		dtp1 = atan((dtp1-dtp2)/32.0);
		if(fabs(dtp2 = dtp1-bankRad.y)<0.03125) bankRad.y = dtp1;
		else bankRad.y+ = dtp2>0?0.03125:-0.03125;
		bankCos.y = cos(bankRad.y);
		bankSin.y = sin(bankRad.y);
	}
}

void drawboat(){
	int i,k,itp1,itp2,drawlist[11],accelBar[8][2] = {{4,14},{7,11},{7,11},{7,11},{7,11},{4,14},{7,11},{4,14}};
	double dtp1,dtp2;
	static struct XYZ bankBoat[7],bankVect[6],tmpBoat[7],tmpVect[6];
	struct XY disp;
	struct SPLASH_LIST *splash_del = 0;

	for(i = 0;i<7;i++){
		tmpBoat[i].x = Boat[i].x*vectCos+Boat[i].y*vectSin;
		tmpBoat[i].y = Boat[i].y*vectCos-Boat[i].x*vectSin;
		tmpBoat[i].z = Boat[i].z-2.0;
		bankBoat[i].x = tmpBoat[i].x*bankCos.x+tmpBoat[i].z*bankSin.x;
		bankBoat[i].z = tmpBoat[i].z*bankCos.x-tmpBoat[i].x*bankSin.x;
		bankBoat[i].y = tmpBoat[i].y*bankCos.y+bankBoat[i].z*bankSin.y;
		bankBoat[i].z = bankBoat[i].z*bankCos.y-tmpBoat[i].y*bankSin.y;
		bankBoat[i].z+ = height+Foundering_height;
	}
	for(i = 0;i<6;i++){
		tmpVect[i].x = NormalVect[i].x*vectCos+NormalVect[i].y*vectSin;
		tmpVect[i].y = NormalVect[i].y*vectCos-NormalVect[i].x*vectSin;
		bankVect[i].x = tmpVect[i].x*bankCos.x+NormalVect[i].z*bankSin.x;
		bankVect[i].z = NormalVect[i].z*bankCos.x-tmpVect[i].x*bankSin.x;
		bankVect[i].y = tmpVect[i].y*bankCos.y+bankVect[i].z*bankSin.y;
		bankVect[i].z = bankVect[i].z*bankCos.y-tmpVect[i].y*bankSin.y;
	}

	for(i = 0;i<7;i++){
		tmpBoat[i].x = bankBoat[i].x*ACos.x+bankBoat[i].y*ASin.x;
		tmpBoat[i].y = bankBoat[i].y*ACos.x-bankBoat[i].x*ASin.x;
		tmpBoat[i].z = bankBoat[i].z*ACos.y+tmpBoat[i].y*ASin.y;
		tmpBoat[i].y = tmpBoat[i].y*ACos.y-bankBoat[i].z*ASin.y;
	}
	for(i = 0;i<6;i++){
		tmpVect[i].y = bankVect[i].y*ACos.x-bankVect[i].x*ASin.x;
		tmpVect[i].z = bankVect[i].z*ACos.y+tmpVect[i].y*ASin.y;
	}

	for(i = 0;i<11;i++) drawlist[i] = 0;
	for(i = 0;i<6;i++){
		if(tmpVect[i].z>0){
			k = 0;
			while(SurfaceFrame[i][k]>-1) drawlist[SurfaceFrame[i][k++]] = 1;
		}
	}
	for(i = 0;i<11;i++){
		if(drawlist[i]){
			itp1 = Frame[i][0];
			itp2 = Frame[i][1];
			XDrawLine(d,pix,gcw,Window_Xh+Shift.x+(!Switch.Track*tmposBoat.x+tmpBoat[itp1].x)*Scale,Window_Yh+Shift.y+(!Switch.Track*tmposBoat.y+tmpBoat[itp1].y)*Scale,Window_Xh+Shift.x+(!Switch.Track*tmposBoat.x+tmpBoat[itp2].x)*Scale,Window_Yh+Shift.y+(!Switch.Track*tmposBoat.y+tmpBoat[itp2].y)*Scale);
		}
	}

	if(Switch.Splash){
		Switch.Splash = 0;
		Splash_Pt = &Splash_List_Start;
		while(Splash_Pt->next) Splash_Pt = Splash_Pt->next;
		for(i = 0;i<SPLASH_MAX;i++){
			if(!(Splash_Pt->next = calloc(1,sizeof(struct SPLASH_LIST)))){
				printf("MEMORY ALLOCATE ERROR\n");
				exit(1);
			}
			Splash_Pt->next->prev = Splash_Pt;
			Splash_Pt = Splash_Pt->next;
			Splash_Pt->x = posBoat.x+rand()/(1.0*RAND_MAX)*2.0-1.0;
			Splash_Pt->y = posBoat.y+rand()/(1.0*RAND_MAX)*2.0-1.0;
			Splash_Pt->z = height;
			Splash_Pt->Vz = 1.0+rand()/(1.0+RAND_MAX)*4.0;
			Splash_Pt->Vx = rand()/(1.0+RAND_MAX)*1.0-0.5;
			Splash_Pt->Vy = rand()/(1.0+RAND_MAX)*1.0-0.5;
			Splash_Pt->next = 0;
		}
	}
	if(Splash_List_Start.next){
		Splash_Pt = Splash_List_Start.next;
		while(Splash_Pt){
			Splash_Pt->Vz- = 9.8/60.0;
			Splash_Pt->z+ = Splash_Pt->Vz;
			itp1 = Splash_Pt->x+ = Splash_Pt->Vx;
			itp2 = Splash_Pt->y+ = Splash_Pt->Vy;
			if(itp1<0 || itp1>MapSize || itp2<0 || itp2>MapSize){
				if(Splash_Pt->z<0)
					splash_del = Splash_Pt;
			}else{
				if(Splash_Pt->z<Map[itp1][itp2]){
					splash_del = Splash_Pt;
					Map[itp1][itp2]- = 1.0;
				}
			}
			if(!splash_del){
				dtp1 = (Splash_Pt->x-MapSizeh)*2.0;
				dtp2 = (Splash_Pt->y-MapSizeh)*2.0;
				disp.x = (dtp1*ACos.x+dtp2*ASin.x-Switch.Track*tmposBoat.x)*Scale+Window_Xh+Shift.x;
				disp.y = dtp2*ACos.x-dtp1*ASin.x;
				disp.y = (disp.y*ACos.y-Splash_Pt->z*ASin.y-Switch.Track*tmposBoat.y)*Scale+Window_Yh+Shift.y;
				dtp1 = 1.0+0.2*Scale;
				if(!Switch.View){
					itp1 = Splash_Pt->z/2.0;
					XFillRectangle(d,pix,gcc[itp1>-3?itp1<3?itp1+4:8:0],disp.x,disp.y,dtp1,dtp1);
				}else{
					itp1 = M*sqrt(Splash_Pt->Vx*Splash_Pt->Vx+Splash_Pt->Vy*Splash_Pt->Vy+Splash_Pt->Vz*Splash_Pt->Vz)*20.0;
					if(itp1>6) itp1 = 6;
					XFillRectangle(d,pix,gcEne[itp1],disp.x,disp.y,dtp1,dtp1);
				}
				Splash_Pt = Splash_Pt->next;
			}else{
				Splash_Pt->prev->next = Splash_Pt->next;
				if(Splash_Pt->next) Splash_Pt->next->prev = Splash_Pt->prev;
				Splash_Pt = Splash_Pt->next;
				free(splash_del);
				splash_del = 0;
			}
		}
	}

	XDrawLine(d,pix,gcw,9,Window_Y-80,9,Window_Y-52);
	for(i = 0;i<8;i++)
		XDrawLine(d,pix,gcw,accelBar[i][0],Window_Y-80+i*4,accelBar[i][1],Window_Y-80+i*4);
	XDrawLine(d,pix,gcEne[6],4,Window_Y-60-accelBoat*4,14,Window_Y-60-accelBoat*4);
}

void quaker(){
	int i,k,extent_diastrophism;
	extent_diastrophism = 100.0+rand()/(1.0+RAND_MAX)*100.0;
	while(extent_diastrophism--){
		i = 1.0+rand()/(1.0+RAND_MAX)*TerrainSize;
		k = 1.0+rand()/(1.0+RAND_MAX)*TerrainSize;
		Terrain[i][k]+ = rand()>Rand_Half?0.1:-0.1;
	}
	rotterrain(0,0);
}

void rain_cloud(double coeff){
	int i,k;
	i = rand()*1.0/RAND_MAX*(MapSize-1.0)+1.0;
	k = rand()*1.0/RAND_MAX*(MapSize-1.0)+1.0;
	Map[i][k]+ = Strength*coeff;
}

