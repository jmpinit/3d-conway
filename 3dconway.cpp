#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
using namespace std;

#include "SDL/SDL.h"
#include "SDL/SDL_Draw.h"

const int BUMP_COUNT = 64;

//Screen attributes
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

SDL_Surface *screen = NULL;

SDL_Event event;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow) {
    return main(__argc, __argv);
}

struct point {
  double x;
  double y;
  double z;
};

double t = 0;

double cx = 0;
double cy = 0;
double cz = 50;

double ex = 0;
double ey = 0;
double ez = 300;

double tx = 0;
double ty = 0;
double tz = 0;

point the_bumps[BUMP_COUNT*BUMP_COUNT];

point get2DPoint(double ax, double ay, double az) {
    double dx = cos(tx)*(sin(tz)*(ay-cy)+cos(tz)*(ax-cx))-sin(ty)*(az-cz);
    double dy = sin(tx)*(cos(ty)*(az-cz)+sin(ty)*(sin(tz)*(ay-cy)+cos(tz)*(ax-cx)))+cos(tx)*(cos(tz)*(ay-cy)-sin(tz)*(ax-cx));
    double dz = cos(tx)*(cos(ty)*(az-cz)+sin(ty)*(sin(tz)*(ay-cy)+cos(tz)*(ax-cx)))-sin(tx)*(cos(tz)*(ay-cy)-sin(tz)*(ax-cx));
    
    point p;
    
    p.x = (dx-ex)*(ez/dz);
    p.y = (dy-ey)*(ez/dz);
    
    return p;
}

void Checked_Line(SDL_Surface* screen, int x1, int y1, int x2, int y2, Uint32 color) {
    if(x1>=0&&x1<SCREEN_WIDTH&&y1>=0&&y1<SCREEN_HEIGHT && x2>=0&&x2<SCREEN_WIDTH&&y2>=0&&y2<SCREEN_HEIGHT)
        Draw_Line(screen, x1, y1, x2, y2, color);
}

void drawBumpMap(SDL_Surface* screen, point* bumps, int width, int height) {
    for(int y=1; y<height-1; y += 2) {
        for(int x=1; x<width-1; x += 2) {
            //get the position of the point
            point a = bumps[y*width+x];
            point b = get2DPoint(a.x, a.y, a.z);
            
            Uint32 c_white  = SDL_MapRGB(screen->format, 255, 255, 255);
            
            point c;
            a = bumps[(y-1)*width+x-1];
            c = get2DPoint(a.x, a.y, a.z);
            Checked_Line(screen, b.x+SCREEN_WIDTH/2, b.y+SCREEN_HEIGHT/2, c.x+SCREEN_WIDTH/2, c.y+SCREEN_HEIGHT/2, c_white);
            a = bumps[(y-1)*width+x+1];
            c = get2DPoint(a.x, a.y, a.z);
            Checked_Line(screen, b.x+SCREEN_WIDTH/2, b.y+SCREEN_HEIGHT/2, c.x+SCREEN_WIDTH/2, c.y+SCREEN_HEIGHT/2, c_white);
            a = bumps[(y+1)*width+x-1];
            c = get2DPoint(a.x, a.y, a.z);
            Checked_Line(screen, b.x+SCREEN_WIDTH/2, b.y+SCREEN_HEIGHT/2, c.x+SCREEN_WIDTH/2, c.y+SCREEN_HEIGHT/2, c_white);
            a = bumps[(y+1)*width+x+1];
            c = get2DPoint(a.x, a.y, a.z);
            Checked_Line(screen, b.x+SCREEN_WIDTH/2, b.y+SCREEN_HEIGHT/2, c.x+SCREEN_WIDTH/2, c.y+SCREEN_HEIGHT/2, c_white);
        }
    }
}

void drawMap(SDL_Surface* screen, point* bumps, int width, int height) {
    for(int y=0; y<height; y++) {
        for(int x=0; x<width; x++) {
            //get the position of the point
            point a = bumps[y*width+x];
            
            Uint32 c_white  = SDL_MapRGB(screen->format, 255, 255, 255);
            
            int zoomx = SCREEN_WIDTH/BUMP_COUNT;
            int zoomy = SCREEN_HEIGHT/BUMP_COUNT;
            if(a.z!=0) {
                Draw_FillRect(screen, x*zoomx, y*zoomy, zoomx, zoomy, c_white);
            } else {
                Draw_FillRect(screen, x*zoomx, y*zoomy, zoomx, zoomy, 0);
            }
        }
    }
}

bool get_field(int x, int y) {
    if(the_bumps[y*BUMP_COUNT+x].z>2) return true; else return false;
}

int main(int argc, char* args[]) {
    SDL_Surface* screen = NULL;
    
    //Set up screen
    screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_FULLSCREEN);
    
    //Set the window caption
    SDL_WM_SetCaption("Rockets", NULL );
    
    Uint32 c_white  = SDL_MapRGB(screen->format, 255,255,255);
    Uint32 c_gray   = SDL_MapRGB(screen->format, 200,200,200);
    Uint32 c_dgray  = SDL_MapRGB(screen->format, 64,64,64);
    Uint32 c_cyan   = SDL_MapRGB(screen->format, 32,255,255);
    
	//Start SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    
    //generate a bump map
    srand(time(NULL));
    
    for(int y=0; y<BUMP_COUNT; y++) {
        for(int x=0; x<BUMP_COUNT; x++) {
            the_bumps[y*BUMP_COUNT+x].x = x-BUMP_COUNT/2;
            the_bumps[y*BUMP_COUNT+x].y = y-BUMP_COUNT/2;
            if(rand()%32>18) { the_bumps[y*BUMP_COUNT+x].z = 4; }
        }
    }
    
	//While there's an event to handle
	bool quit = false;
	
	while(!quit) {
        //calculate next frame of Conway
        bool newfield[BUMP_COUNT*BUMP_COUNT];
        for(int y=1; y<BUMP_COUNT-1; y++) {
            for(int x=1; x<BUMP_COUNT-1; x++) {
                int n = 0;
                if(get_field(x-1, y-1)) n++;
                if(get_field(x, y-1)) n++;
                if(get_field(x+1, y-1)) n++;
                if(get_field(x-1, y)) n++;
                if(get_field(x+1, y)) n++;
                if(get_field(x-1,  y+1)) n++;
                if(get_field(x, y+1)) n++;
                if(get_field(x+1, y+1)) n++;
                                
                if(n<2 || n>3) {
                    //death
                    newfield[y*BUMP_COUNT+x] = false;
                } else if(n==3) {
                    //birth
                    newfield[y*BUMP_COUNT+x] = true;
                } else {
                    newfield[y*BUMP_COUNT+x] = get_field(x, y);
                }
            }
        }
        
        for(int y=0; y<BUMP_COUNT; y++) {
            for(int x=0; x<BUMP_COUNT; x++) {
                newfield[y*BUMP_COUNT+x]? the_bumps[y*BUMP_COUNT+x].z = 4: the_bumps[y*BUMP_COUNT+x].z = 0;
            }
        }
        
        int x, y;
        SDL_GetMouseState(&x, &y);
        
        ty = (((double)x-(double)SCREEN_WIDTH/2)/(double)SCREEN_WIDTH)*2.0*3.1415;
        tx = (((double)y-(double)SCREEN_HEIGHT/2)/(double)SCREEN_HEIGHT)*2.0*3.1415;
        
        SDL_FillRect(screen, NULL, 0x000000);
        
        drawBumpMap(screen, the_bumps, BUMP_COUNT, BUMP_COUNT);
        
        //Update Screen
    	SDL_Flip(screen);
        
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                //Quit the program
                quit = true;
            } else if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT:
                        cx -= 1;
                        break;
                    case SDLK_RIGHT:
                        cx += 1;
                        break;
                    case SDLK_UP:
                        cz += 1;
                        break;
                    case SDLK_DOWN:
                        cz -= 1;
                        break;
                    case SDLK_0:
                        ez += 10;
                        break;
                    case SDLK_1:
                        ez -= 10;
                        break;
                    case SDLK_q:
                        quit = true;
                        break;
                }
            }
        }
    }

    //Quit SDL
    SDL_Quit();

    return 0;
}
