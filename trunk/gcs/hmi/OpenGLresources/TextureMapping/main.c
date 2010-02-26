#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL.h"
#include "texture.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BPP 24

/* BINDING TEXTURES */

/* Main points to remember:
   - Enable textuers (glEnable(GL_TEXTURE_2D))
   - Load the texture (int a = LoadGLTexture("a.bmp"))
   - Bind the texture (glBindTexture(GL_TEXTURE_2D, texture))
   - Set texture coordinates for each verticy (glTexCoord2f(0.0f, 0.0f))
   
   Set texture coordinates in the same order you set verticies, for example,
   If you specifiy verticies as top left, top right, bottom right, bottom left,
   then you would set textures up like this:
   glTexCoord2f(0.0f, 1.0f); (x = 0, y = 1 (top left))
   glTexCoord2f(1.0f, 1.0f); (x = 1, y = 1 (top right))
   glTexCoord2f(1.0f, 0.0f); (x = 1, y = 0 (bottom right))
   glTexCoord2f(0.0f, 0.0f); (x = 0, y = 0 (bottom left))
*/   

/* create window */
SDL_Surface* surface;
int fullscreen = 0;
float amount = 0.0f;
GLuint texture;

int SDLGLSetup()
{
  /* this holds some info about our display */
  const SDL_VideoInfo *videoInfo;
  int videoFlags;

  /* initialize SDL */
  if (SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 0;
  }

  /* Fetch the video info */
  videoInfo = SDL_GetVideoInfo();

  if (!videoInfo)
  {
    fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 0;
  }

  /* the flags to pass to SDL_SetVideoMode */
  videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
  videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
  videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
  videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */
  if (fullscreen)
    videoFlags |= SDL_FULLSCREEN;       /* Fullscreen */
 
  /* This checks to see if surfaces can be stored in device memory */
  if (videoInfo->hw_available)
    videoFlags |= SDL_HWSURFACE;
  else
    videoFlags |= SDL_SWSURFACE;

  /* This checks if hardware blits can be done */
  if ( videoInfo->blit_hw )
    videoFlags |= SDL_HWACCEL;

  /* Sets up OpenGL double buffering */
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  /* hide cursor */
  SDL_ShowCursor(SDL_DISABLE);

  /* get a SDL surface */
  surface = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, BPP,
			     videoFlags);

  /* Verify there is a surface */
  if (!surface )
  {
    fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
    SDL_Quit();
    return 0;
  }

  /* set the title bar */
  SDL_WM_SetCaption("SDL Test", "An SDL Test");

  /* initialize OpenGL */

  /* Enable Texture Mapping */
  glEnable(GL_TEXTURE_2D);

  /* Enable smooth shading */
  glShadeModel( GL_SMOOTH );

  /* Set the background black */
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

  /* Depth buffer setup */
  glClearDepth( 1.0f );

  /* Enables Depth Testing */
  glEnable( GL_DEPTH_TEST );

  /* The Type Of Depth Test To Do */
  glDepthFunc( GL_LEQUAL );

  /* Really Nice Perspective Calculations */
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );


  /* Lighting */
  /*
  GLfloat LightAmbient[]= { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat LightPosition[]= { 0.0f, 100.0f, 0.0f, 1.0f };

  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);
  */

  /* resize the initial window */
  ResizeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

  return 1;
}

void DrawScene()
{
  /* Clear The Screen And The Depth Buffer */
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glLoadIdentity();

  glTranslatef(0.0f,0.0f,-1.0f);
  glRotatef(amount, 1.0f, 0.5f, 0.0f);

  amount += 0.05;

  /* ALWAYS REMEMBER, draw shapes with textures LAST */
  /* Once you have bound a texture, it will use that
     texture on every rectangle or triangle you draw
     until the next glLoadIdentity is called for the
     next frame, or the next time you bind a texture */

  /* Draw a rectangle, and bind the texture */
  glColor3f(1.0f,1.0f,1.0f);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 1.0f);glVertex3f(0.0f, 0.5f, 0.0F);
  glTexCoord2f(1.0f, 1.0f);glVertex3f(0.5f, 0.5f, 0.0F);
  glTexCoord2f(1.0f, 0.0f);glVertex3f(0.5f, 0.0f, 0.0F);
  glTexCoord2f(0.0f, 0.0f);glVertex3f(0.0f, 0.0f, 0.0F);
  glEnd();

  /* Draw it to the screen */
  SDL_GL_SwapBuffers();
}

int main()
{
  int i = 0, fps = 0;
  char buff[100]; 
  time_t seconds;
  const SDL_VideoInfo *videoInfo;
  int videoFlags;

  SDLGLSetup();

  SDL_Event event;
  int gameover = 0;

  /* Load Textures */
  texture = LoadGLTexture("grass36.bmp");

  /* message pump */
  while (!gameover)
  {
    /* look for an event */
    if (SDL_PollEvent(&event)) 
    {
      /* an event was found */
      switch (event.type) 
      {
        /* close button clicked */
        case SDL_QUIT:
          gameover = 1;
          break;

        /* handle the keyboard */
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) 
          {
            case SDLK_ESCAPE:
            case SDLK_q:
              gameover = 1;
              break;
          }
          break;
      }
    }

    /* Draw the scene */
    DrawScene();
  }

  /* cleanup SDL */
  SDL_Quit();
}
