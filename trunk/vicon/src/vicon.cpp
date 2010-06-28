/* $Author: Luis Mejias$
 * 
 *  This program can be used to connect to the vicon system and visualise its
 *  output, position and  orientation at this point. For a more details 
 *  about the type of information the vicon system can provide please see the 
 *  API/SDK on the ARCAA wiki.
 *
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

// system OpenGL includes
#include <GL/gl.h> 
#include <GL/glu.h>
#include <GL/glut.h>

//Vicon SDK header
#include "Client.h"




//define the view for the  openGL camera
typedef enum {
	view_stationary,
	view_walk_behind,
	view_fly_behind,
	view_cockpit,
	view_north_up,
	view_track_up,
	heli_cam,
	view_exploring_right,
	view_exploring_left,
	view_exploring_up
} viewpoint_t;



//main method to display/visualisation
void DrawScene( viewpoint_t		viewpoint,
		double			north,
		double			east,
		double			down,
		double			phi,
		double			theta,
		double			psi,
		double			roll_moment,
		double			pitch_moment );

//traslation/orientation of the openGL camera camera/view 
static void lookat( viewpoint_t		viewpoint,
		    double			x,
		    double			y,
		    double			z,
		    double			phi,
		    double			theta,
		    double			psi
		    );
//draw a small red cube on the display 
void drawBox(void);
//draw text in the main window
void drawString (int x,int y, int z,char *s);

//openGL definitions
static int		main_window;
static int		WIDTH		= 600; //width of the openGL window
static int		HEIGHT		= 300;//height of the openGL window
viewpoint_t	viewpoint   = view_stationary;//view of the openGL camera
#define C_DEG2RAD           0.0175            // degree to radian
#define C_RAD2DEG           57.2958           // radian to degree
#define mm2M                0.01  
//main position and orientation of the vicon object
float X,Y,Z;   //position
float phi,theta,psi; //orientation

//vicon data stream namespace
using namespace ViconDataStreamSDK::CPP;

// Make a new client
Client MyClient;

/**
 * reads incoming data stream from the vicon server. is called in the main 
 * visualisation loop 
 */
static void process_state(void){
  
  std::string SegmentName;
  std::string SubjectName;
  unsigned int SubjectCount;
  unsigned int SegmentCount;


  while( MyClient.GetFrame().Result != Result::Success )
    {
      // Sleep a little so that we don't lumber the CPU with a busy poll
      sleep(1);
      std::cout << ".";
    }
  
  //number of objects currently represented/tracked by the vicon system 
  SubjectCount = MyClient.GetSubjectCount().SubjectCount;
  
  
  unsigned int SubjectIndex = 0 ; //we just plot the first object in the list
                                  //incrementing until subjectCount will display
                                  //several object simultaneously


  SubjectName = MyClient.GetSubjectName( SubjectIndex ).SubjectName;
      
  // Get the root segment
  std::string RootSegment = MyClient.GetSubjectRootSegmentName( SubjectName ).SegmentName;
  
  //Get the number of Segments
  SegmentCount = MyClient.GetSegmentCount( SubjectName ).SegmentCount;
      

  
  unsigned int SegmentIndex = 0; //we just plot the first object in the list
                                  //incrementing until segmentCount will display
                                  //several object simultaneously

   //Get the name of Segment
  SegmentName = MyClient.GetSegmentName( SubjectName, SegmentIndex ).SegmentName;
  //Get the name of subject
  SubjectName = MyClient.GetSubjectName( SubjectIndex ).SubjectName;


  // Get the global segment translation
  Output_GetSegmentGlobalTranslation _Output_GetSegmentGlobalTranslation = MyClient.GetSegmentGlobalTranslation( SubjectName, SegmentName );
      
  //store locally X,Y,Z. data comes by defauls in mm and radians
  X = _Output_GetSegmentGlobalTranslation.Translation[ 0 ]*mm2M;
  Y = _Output_GetSegmentGlobalTranslation.Translation[ 1 ]*mm2M;
  Z = -1.0 * (_Output_GetSegmentGlobalTranslation.Translation[ 2 ])*mm2M;
  
  // Get the global segment orientation
  Output_GetSegmentGlobalRotationEulerXYZ _Output_GetSegmentGlobalRotationEulerXYZ = MyClient.GetSegmentGlobalRotationEulerXYZ( SubjectName, SegmentName );
  
  //store locally roll, pitch, yaw
  phi= _Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ 0 ];
  theta= _Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ 1 ];
  psi=  _Output_GetSegmentGlobalRotationEulerXYZ.Rotation[ 2 ];
  
  //print in console current position and attitude
  //std::cout << "Global Translation: (" << X << ", " << Y << ", " << Z << ") " << std::endl;
  //std::cout << "Global Orientation(" << phi << ", " << theta << ", " << psi << ") " << std::endl;
  
  
}

/**
 *  rezise_window() resize main window
 *  
 */

static void resize_window( GLsizei  w,
			   GLsizei  h
			   )
{
  if( h == 0 )
    h = 1;
  if( w == 0 )
    w = 1;
  
  glViewport( 0, 0, w, h );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective(
		 30.0,				// Field of view
		 (GLfloat)w/(GLfloat)h,		// Aspect ratio
		 1.0,				// Near
		 1000.0				// Far
		 );
  
  // select the Modelview matrix
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
}


/**
 *  animate() main CALLBACK visualisation function
 */

static void
animate( void )
{
	
  //read incoming data (from the socket)
  process_state();
	
	
  //draws openGL environment
  DrawScene(viewpoint,
	    X,
	    Y,
	    Z,
	    phi,
	    theta,
	    psi,
	    0.0,
	    0.0);
  
  
  //redisplay
  glutPostRedisplay();
  glutSwapBuffers();
  
  
}


/**
 *  init_window() sets up the GL environment and schedules the
 * user callbacks for animation and periodic timing.
 */
void
init_window(
	    int *   argc,
	    char ** argv,
	    void    (*animate)( void ),
	    void    (*keyboard)(
				unsigned char  c,
				int	       x,
				int	       y
				),
	    void    (*idle)( void )
	    )
{
  // Call these first so that glutInit can override if necessary
  glutInit( argc, argv );
  glutInitWindowSize( WIDTH, HEIGHT ); 
  glutInitWindowPosition( 320, 100 );
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );	
  main_window = glutCreateWindow( argv[0] );
  
  glShadeModel(GL_FLAT);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  glEnable( GL_BLEND );
  glEnable( GL_ALPHA_TEST );
  
  //callbacks for display, keyboard and resize window
  glutDisplayFunc( animate );
  glutReshapeFunc( resize_window ); 
  glutKeyboardFunc( keyboard );
  //glutIdleFunc( idle ); //not used at the moment. could be used to call process_state
  
  
}


/**
 * keyboard() read the keyboards and set the view
 *  of the display/window.
 */

static void
keyboard(
	 unsigned char		c,
	 int			x,
	 int			y
	 )
{
  
  
  switch( c )
    {
    case ' ':
      printf( "reset\n" );
      break;
    case 'q':
      printf( "quit\n" );
      // Disconnect and dispose
      MyClient.Disconnect();
      exit( EXIT_SUCCESS );
    case '1':
      viewpoint = view_stationary;
      break;
    case '2':
      viewpoint = view_walk_behind;
      break;
    case '3':
      viewpoint = view_fly_behind;
      break;
    case '4':
      viewpoint = heli_cam;
      break;
    case '5':
      viewpoint = view_north_up;
      break;
    case '6':
      viewpoint = view_track_up;
      break;
    case '7':
      viewpoint =  view_exploring_left;
      break;
    case 'v':   
      break;
    default:
      /* Nothing */
      break;
    }
  
	
	fflush( stdout );
}


/**
 * mai() main function
 *  
 */

int main(int argc, char **argv )
{
  
  bool TransmitMulticast        = false;
  std::string HostName = "131.181.87.186:801";

  
  while( !MyClient.IsConnected().Connected )
    {
      // Direct connection
      MyClient.Connect( HostName );
      std::cout << ".";
      sleep(1);
      
    }
  
  // Enable some different data types
  MyClient.EnableSegmentData();
  MyClient.EnableMarkerData();
  MyClient.EnableUnlabeledMarkerData();
  MyClient.EnableDeviceData();
  
  // Set the streaming mode
  MyClient.SetStreamMode( ViconDataStreamSDK::CPP::StreamMode::ClientPull );
  
  
  // Set the global up axis
  MyClient.SetAxisMapping( Direction::Forward, 
                           Direction::Left, 
                           Direction::Up ); // Z-up
  
  
  
  //initialise the entire program
  init_window(	&argc,
		argv,
		animate,
		keyboard,
		process_state);



	// Process remaining arguments
	fprintf( stderr, "%d args remaining\n", argc );
	while( *++argv )
	{
		const char *		arg = *argv;
		if( arg[0] != '-' )
			break;

		if( isdigit( arg[1] ) )
		{
			viewpoint = (viewpoint_t) (arg[1] - '1');
			continue;
		}

		fprintf( stderr,
			"heli-3d: Unknown argument '%s'\n",
			arg
		);
			
	}


	glutMainLoop();
}


/**
 *  DrawScene() creates the main visualization objects
 *  accepts: viewpoint,traslation and orientation.
 */


void DrawScene( viewpoint_t		viewpoint,
		double			north,
		double			east,
		double			down,
		double			phi,
		double			theta,
		double			psi,
		double			roll_moment,
		double			pitch_moment )
{


  static GLfloat floorVertices[4][3] = {
    { -20.0, 0.0, 20.0 },
    { 20.0, 0.0, 20.0 },
    { 20.0, 0.0, -20.0 },
    { -20.0, 0.0, -20.0 },
  };
  
  float x = north; // 0.6858 + 0.00;
  float y = -down; // 0.6858 + 0.40;
  float z =  east; // 0.6858 - 4.00;
  GLfloat xtrans, ytrans, ztrans;
  GLfloat sceneroty;
  char *text_traslation;
  char *text_orientation;
    
  text_traslation=(char *)malloc(64*sizeof(char*));
  text_orientation=(char *)malloc(64*sizeof(char*));

  // all of this stuff is just for the lighting of the scene.
    GLfloat groundAmbient[4]	= { 0.20, 0.30, 0.10, 1.00 };
    GLfloat local_ambient[4]	= { 0.70, 0.70, 0.70, 1.00 };
    
    GLfloat ambient0[4]		= { 0.00, 0.00, 0.00, 1.00 };
    GLfloat diffuse0[4]		= { 1.00, 1.00, 1.00, 1.00 };
    GLfloat specular0[4]		= { 1.00, 0.00, 0.00, 1.00 };
    GLfloat position0[4]		= { 0.00, 100.50, 0.0, 1.00 };
    GLfloat red_face[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */



    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, local_ambient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
    
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_POSITION, position0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
     
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    

    //changes camera view
    lookat( viewpoint, x, y, z, phi, theta, psi );

    //this is the ground
    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, groundAmbient);
    
    int e=0;
    glDisable(GL_LIGHTING);
    for(e=-1000; e<=1000; e+=50)
      {
	glColor3ub(0, 255, 0);
	glBegin(GL_LINES);
	glVertex3f( e, 0.00, -1000.00);
	glVertex3f( e, 0.00,  1000.00);
	
	glVertex3f( -1000.00, 0.00, e);
	glVertex3f( 1000.00, 0.00, e);
	glEnd();
      }
    
    
    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_face);

    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef( psi   * C_RAD2DEG, 0.00,  -1.00, 0.00);
    glRotatef( theta * C_RAD2DEG, 0.00,  0.00, 1.00);
    glRotatef( phi   * C_RAD2DEG, 1.00,  0.00, 0.00);
    
    //creates the red cube
    drawBox();

    //draw some information in the openGL window
    sprintf(text_traslation,"traslation: X:%f Y:%f Z:%f",north, east, down);
    sprintf(text_orientation,"orientation: roll:%f pitch:%f yaw:%f",phi*C_RAD2DEG, theta*C_RAD2DEG, psi*C_RAD2DEG);
    drawString (north,east,down+10,text_traslation);
    drawString (north,east,down+5,text_orientation);

    glPopMatrix();
    glPushMatrix();
	

 
}


/**
 *  lookat() changes the view of the openGL camera
 *  accepts: viewpoint,traslation and orientation.
 */

static void lookat( viewpoint_t		viewpoint,
		    double			x,
		    double			y,
		    double			z,
		    double			phi,
		    double			theta,
		    double			psi
    )
{
    double  camera[3]	= { 1.00, 6.00, 30.00 };
    double  dest[3]		= {    x,    y,     z };
    double  up[3]		= { 0.00, 1.00,  0.00 };
    
    double camera_onboard[3]={1.00,6.00,30.00};

    double  dx	= 10 * cos(psi);
    double  dz	= 10 * sin(psi);
    


    
      
    switch( viewpoint )
      {
      default:
    case view_stationary:
      // Nothing to do. 0 Use the defaults
      break;
    
	case heli_cam: 
	camera[0] = x;
	camera[1] = y;
	camera[2] = z;
	
	dest[0] = x + dx; 
	dest[1] = y ; 
	dest[2] = z + dz;  
	
	break; 
	
      case view_fly_behind: 
	// Position the camera just behind and above the boom
	camera[0] = x - dx;
	camera[1] = y + 1;
	camera[2] = z - dz;
	break;
      case view_walk_behind:
	// Position the camera at eye height just behind the boom
	camera[0] = x - dx;
	camera[2] = z - dz;
	break;
	
      case view_cockpit:
	{
	 
	  break;
	}
      case view_north_up:
	// Camera is in the sky
	camera[0] = x;
	camera[1] = 100;
	camera[2] = z;
	
	// North is "up"
	up[0] = 1;
	break;
      case view_track_up:
	// Camera is in the sky
	camera[0] = x;
	camera[1] = 100;
	camera[2] = z;
	
	// Up is along the boom
	up[0] = dx;
	up[2] = dz;
	break;
      case view_exploring_right:
	
	camera[0] = x ; 
	camera[1] = y ;
	camera[2] = z ;
	
	dest[0] = 0.0;
	dest[1] = 0.0;
	dest[2] = psi+C_DEG2RAD;
       	break;
	
      case view_exploring_left:
	
	camera[0] = x ; 
	camera[1] = y ;
	camera[2] = z ;
	
	dest[0] = 0.0;
	dest[1] = 0.0;
	dest[2] = psi-C_DEG2RAD;
       

	break; 
	
	
      }
    
    gluLookAt(
	      camera[0],	camera[1],	camera[2],
	      dest[0],	dest[1],	dest[2],
	      up[0],		up[1],		up[2]
	      );
  
}


/**
 *  few routines to display text on the screen
 *  
 */

void drawString (int x,int y, int z,char *s)
{
  unsigned int i;
  glRasterPos3f(x, y,z);

  for (i = 0; i < strlen (s); i++)
    glutBitmapCharacter (GLUT_BITMAP_HELVETICA_10, s[i]);
}
void drawStringBig (char *s)
{
  unsigned int i;
  for (i = 0; i < strlen (s); i++)
    glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, s[i]);
}




/**
 *  drawBox() creates the main red cube
 *  .
 */



GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
  {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertexes. */


void drawBox(void)
{
  int i;

/* Setup cube vertex data. */
  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;


  for (i = 0; i < 6; i++) {
    glBegin(GL_QUADS);
    glNormal3fv(&n[i][0]);
    glVertex3fv(&v[faces[i][0]][0]);
    glVertex3fv(&v[faces[i][1]][0]);
    glVertex3fv(&v[faces[i][2]][0]);
    glVertex3fv(&v[faces[i][3]][0]);
    glEnd();
  }
}

