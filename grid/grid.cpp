// include glut.h with cross-platform support
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// include standard C/C++ libraries
#include<iostream>  // exit
#include<cmath>     // abs
#include<unistd.h>  // usleep

// forward declarations
void motion(int x, int y);

class Pixel {
private :
  float coordinate[2];
  float dimension[2];
  float color[3];
  
public :

  void setCoordinate(float x, float y) {
    coordinate[0] = x;
    coordinate[1] = y;
  }
   
  void setDimension(float w, float h) {
    dimension[0] = w;
    dimension[1] = h;
  }

  void setColor(float r, float g, float b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
  }

  void render(int time) {
    float c[3];
    c[0] = color[0] * (0.75 + 0.25*std::sin(0.1*time));
    c[1] = color[1] * (0.75 + 0.25*std::sin(0.1*time));
    c[2] = color[2] * (0.75 + 0.25*std::sin(0.1*time));
    glColor3f(c[0], c[1], c[2]); // color vertex 1
    glVertex2f(coordinate[0]-0.5*dimension[0],coordinate[1]-0.5*dimension[1]);

    glColor3f(c[0], c[1], c[2]); // color vertex 2
    glVertex2f(coordinate[0]+0.5*dimension[0],coordinate[1]-0.5*dimension[1]);

    glColor3f(c[0], c[1], c[2]); // color vertex 3
    glVertex2f(coordinate[0]+0.5*dimension[0],coordinate[1]+0.5*dimension[1]);

    glColor3f(c[0], c[1], c[2]); // color vertex 4
    glVertex2f(coordinate[0]-0.5*dimension[0],coordinate[1]+0.5*dimension[1]);
  }
};

class Grid {
private :
  int Nx, Ny, Px, Py;
  Pixel* pixels;
  int time;

public :

  void initialize(unsigned nx, unsigned ny, unsigned px, unsigned py) {
    time = 0;
    Nx = nx;
    Ny = ny;
    Px = px;
    Py = py;
    float w = 2.0 / Nx;
    float h = 2.0 / Ny;
    pixels = new Pixel[Nx*Ny];
    for (int j = 0; j < Ny; j++) {
      float y = h * (j + 0.5) - 1.0;
      for (int i = 0; i < Nx; i++) {
	float x = w * (i + 0.5) - 1.0;
	pixels[i+Nx*j].setCoordinate(x, y);
	pixels[i+Nx*j].setDimension(w, h);
	pixels[i+Nx*j].setColor(std::abs(x),std::abs(y),0.5);
      }
    }
  } // initialize

  void keyboard(unsigned char c, int x, int y) {
    if (c == 27) { // ASCII code for the escape key
      exit(0);
    }
  }

  void mouse(int button, int state, int x, int y) {
    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
      float w = 2.0 / Nx;
      float h = 2.0 / Ny;
      int i = std::min(std::max(int(floor(((2.0 / Px) * x) / w)),0),Nx-1);
      int j = std::min(std::max(int(floor((2.0 - (2.0 / Py) * y) / h)),0),Ny-1);
      pixels[i+Nx*j].setColor(1.0,0.0,0.0);
      glutMotionFunc(motion);
      glutPostRedisplay(); // refresh the display
    }
  }

  void m_motion(int x, int y) {
    float w = 2.0 / Nx;
    float h = 2.0 / Ny;
    int i = std::min(std::max(int(floor(((2.0 / Px) * x) / w)),0),Nx-1);
    int j = std::min(std::max(int(floor((2.0 - (2.0 / Py) * y) / h)),0),Ny-1);
    pixels[i+Nx*j].setColor(1.0,0.0,0.0);
    glutPostRedisplay(); // refresh the display
  }

  void render() {
    // clear the current bit buffers, restoring them to their preset values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();
    // Set the camera
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
    gluLookAt(	0.0f, 0.0f, -1.0f,   // eye position (x,y,z)
		0.0f, 0.0f,  0.0f,   // center position (x,y,z)
		0.0f, 1.0f,  0.0f);  // up direction (x,y,z)
    // rotate everything by the specified angle, about the specified axis
    // -1st arg: angle of total rotation
    // -2nd arg: x-component of rotation axis
    // -3rd arg: y-component of rotation axis
    // -4th arg: z-component of rotation axis
    glRotatef(1.0*time, 0.0, 0.0, 1.0);

    // draw quadrilaterals
    glBegin(GL_QUADS);
    for (int j = 0; j < Ny; j++) {
      for (int i = 0; i < Nx; i++) {
	pixels[i+Nx*j].render(time);
      }
    }
    glEnd(); // GL_QUADS

    // for double buffering: display buffer that was just rendered
    glutSwapBuffers();

    // update the time variable
    time+=1;

  } // render
};

// declare global grid instance
Grid g;

void keyboard(unsigned char c, int x, int y) {
  g.keyboard(c, x, y);
}

void mouse(int button, int state, int x, int y) {
  g.mouse(button, state, x, y);
}

void motion(int x, int y) {
  g.m_motion(x, y);
}

void render() {
  g.render();
}

void idle() {
  glutMouseFunc(mouse);
  glutPostRedisplay(); // refresh the display
}

void reshape(int w, int h) {
  // Prevent a divide by zero, when window is too short
  // (you cant make a window of zero width).
  if (h == 0) {h = 1;}
  float ratio = (1.0 * w) / h;

  // Use the Projection Matrix
  glMatrixMode(GL_PROJECTION);

  // Reset Matrix
  glLoadIdentity();

  // Set the viewport to be the entire window
  // -1st & 2nd args: (x,y) coordinate of the bottom-left viewport corner
  // -3rd & 4th args: (x,y) coordinate of the top-right viewport corner
  glViewport(0, 0, w, h);

  // Set the correct perspective.
  // -1st arg: field of view angle in yz plane
  // -2nd arg: (re-)define width-to-height ratio of the viewport
  // -3rd arg: depth value of near clipping plane
  // -4th arg: depth value of far clipping plane
  gluPerspective(0,ratio,0,1000);

  // Get Back to the Modelview
  // -uses the scaling matrix in other glut functions, as needed
  glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
  g.initialize(60, 60, 600, 600); // initialize the grid

  // initialize glut
  glutInit(&argc, argv);

  // specify the initial position of the display window
  //  -default value is (-1,-1), leading to automatic window placement
  glutInitWindowPosition(100, 100);

  // specify the initial size of the display window (in pixels)
  glutInitWindowSize(600, 600);

  // initialize the display modes:
  // -define colors via RGBA values
  // -use double buffering (for smoother animations)
  // -use depth buffering (to correctly occlude objects with greater depth)
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // create the display window with the specified name
  glutCreateWindow("Grid");

  // define call-back functions:
  // -main display function to run if there is a state change
  glutDisplayFunc(render);
  // -function to run in response to window resizing
  glutReshapeFunc(reshape);
  // -function to run in response to keyboard input
  glutKeyboardFunc(keyboard);
  // -function to run in response to mouse input
  glutMouseFunc(mouse);
  // -function to run if no active input is provided
  glutIdleFunc(idle);
  
  // enter main display loop
  glutMainLoop();

  // the program should never reach this point; return 1 if it does
  return 1;
}

