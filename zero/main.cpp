#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <vecmath.h>

#include<string>

using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;

static int color_counter = 0;

// Light position
GLfloat Lt0pos[] = {1.0f, 1.0f, 5.0f, 1.0f};

void drawFaces();

void tokenize(std::string str, std::vector<string> &token_v);

// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }


// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
	color_counter = (color_counter + 1) & 0x3;
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
		Lt0pos[1] = Lt0pos[1] - 0.5;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
                Lt0pos[1] = Lt0pos[1] + 0.5;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
		Lt0pos[0] = Lt0pos[0] - 0.5;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
		Lt0pos[0] = Lt0pos[0] + 0.5;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

    

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };
                                 
    cout << " DRAW SCENE ! TRIGGERED color_counter " << color_counter << endl;
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[color_counter]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	drawFaces();
    
    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

#define MAX_BUFFER_SIZE 1024

// a/b/c d/e/f g/h/i

void drawFaces()
{
#if 1
  for(int i=0;i<vecf.size();i++)
  {
    vector<unsigned> vecf_elem = vecf[i];
    glBegin(GL_TRIANGLES);
    glNormal3d(vecn[vecf_elem[2]][0], vecn[vecf_elem[2]][1], vecn[vecf_elem[2]][2]);
    glVertex3d(vecv[vecf_elem[0]][0], vecv[vecf_elem[0]][1], vecv[vecf_elem[0]][2]);
    glNormal3d(vecn[vecf_elem[5]][0], vecn[vecf_elem[5]][1], vecn[vecf_elem[5]][2]);
    glVertex3d(vecv[vecf_elem[3]][0], vecv[vecf_elem[3]][1], vecv[vecf_elem[3]][2]);
    glNormal3d(vecn[vecf_elem[8]][0], vecn[vecf_elem[8]][1], vecn[vecf_elem[8]][2]);
    glVertex3d(vecv[vecf_elem[6]][0], vecv[vecf_elem[6]][1], vecv[vecf_elem[6]][2]);
    glEnd();
  }
#endif  
}

void loadInput()
{
#if 1
	// load the OBJ file here
  char buffer_read[MAX_BUFFER_SIZE];
  while(cin.getline(buffer_read, MAX_BUFFER_SIZE))
  {
    stringstream ss(buffer_read);
    Vector3f v;
    vector<unsigned> vec_data;
    string s;
    string s_f1,s_f2,s_f3;
    ss >> s;
    if(s == "v")
    {
          
      ss >> v[0] >> v[1] >> v[2];
      vecv.push_back(Vector3f(v[0],v[1],v[2]));     
    }
    else if(s == "vn")
    {
      ss >> v[0] >> v[1] >> v[2];    
      vecn.push_back(Vector3f(v[0],v[1],v[2]));
    }
    else if(s == "f")
    {
      ss >> s_f1 >> s_f2 >> s_f3;
      vector<string> s_f1_tokens;
      tokenize(s_f1,s_f1_tokens);
      for(int i=0;i<s_f1_tokens.size();i++)
      {
        vec_data.push_back(stoi(s_f1_tokens[i]));
      }
      vector<string> s_f2_tokens;
      tokenize(s_f2,s_f2_tokens);
      for(int i=0;i<s_f2_tokens.size();i++)
      {
        vec_data.push_back(stoi(s_f2_tokens[i]));
      }
      vector<string> s_f3_tokens;
      tokenize(s_f3,s_f3_tokens);
      for(int i=0;i<s_f3_tokens.size();i++)
      {
        vec_data.push_back(stoi(s_f3_tokens[i]));
      }
      vecf.push_back(vec_data);
    }

  }
#endif  
}


void tokenize(std::string str, std::vector<string> &token_v){
    size_t start = str.find_first_not_of("/"), end=start;

    while (start != std::string::npos){
        // Find next occurence of delimiter
        end = str.find("/", start);
        // Push back the token found into vector
        token_v.push_back(str.substr(start, end-start));
        // Skip all occurences of the delimiter to find new start
        start = str.find_first_not_of("/", end);
    }
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
