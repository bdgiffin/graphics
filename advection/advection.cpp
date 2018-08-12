// include glut.h with cross-platform support
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// include project headers
#include "mesh.h"   // Mesh

// include standard C/C++ libraries
#include<iostream>  // exit
#include<cmath>     // abs
#include<unistd.h>  // usleep

// include CImg for reading image files
#include "CImg.h"
using namespace cimg_library;

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

  float value(void) {
    return color[0];
  }

  void addValue(float c) {
    color[0] += c;
    color[1] += c;
    color[2] += c;
  }

  void render(void) {
    // vertices are draw in normalized device coordinates (NDC)
    glColor3f(color[0], color[1], color[2]); // color vertex 1
    glVertex2f(coordinate[0]-0.5*dimension[0],coordinate[1]-0.5*dimension[1]);

    glColor3f(color[0], color[1], color[2]); // color vertex 2
    glVertex2f(coordinate[0]+0.5*dimension[0],coordinate[1]-0.5*dimension[1]);

    glColor3f(color[0], color[1], color[2]); // color vertex 3
    glVertex2f(coordinate[0]+0.5*dimension[0],coordinate[1]+0.5*dimension[1]);

    glColor3f(color[0], color[1], color[2]); // color vertex 4
    glVertex2f(coordinate[0]-0.5*dimension[0],coordinate[1]+0.5*dimension[1]);
  }
};

class Grid {
private :
  int Nx, Ny, Px, Py;
  Mesh* mesh;
  Pixel* pixels;
  float time;

public :

  void initialize(CImg<float>& image) {
    mesh = new Mesh(image);
    time = 0.0;
    std::cout << image.spectrum() << std::endl;
    Nx = image.width();
    Ny = image.height();
    Px = 12*Nx;
    Py = 12*Ny;
    float w = 2.0 / Nx;
    float h = 2.0 / Ny;
    pixels = new Pixel[Nx*Ny];
    for (int j = 0; j < Ny; j++) {
      float y = h * (j + 0.5) - 1.0;
      for (int i = 0; i < Nx; i++) {
	float c = mesh->He[Nx*j+i];
	float x = w * (i + 0.5) - 1.0;
	pixels[i+Nx*j].setCoordinate(x, y);
	pixels[i+Nx*j].setDimension(w, h);
	pixels[i+Nx*j].setColor(c,c,c);
      }
    }
  } // initialize

  float getTime(void) {
    return time;
  }

  void setTime(float t) {
    time = t;
  }

  void update(float dt) {
    // update the solution
    mesh->UpdateFields(dt);

    // update time
    time += dt;
  }

  int width(void) {
    return Px;
  }

  int height(void) {
    return Py;
  }

  void setWidth(int p) {
    Px = p;
  }

  void setHeight(int p) {
    Py = p;
  }

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
      mesh->He[Nx*j+i] = 1.0;
      glutMotionFunc(motion);
      glutPostRedisplay(); // refresh the display
    }
  }

  void m_motion(int x, int y) {
    float w = 2.0 / Nx;
    float h = 2.0 / Ny;
    int i = std::min(std::max(int(floor(((2.0 / Px) * x) / w)),0),Nx-1);
    int j = std::min(std::max(int(floor((2.0 - (2.0 / Py) * y) / h)),0),Ny-1);
    mesh->He[Nx*j+i] = 1.0;
    glutPostRedisplay(); // refresh the display
  }

  void render(void) {
    // clear the current bit buffers, restoring them to their preset values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update pixel colors, and draw quadrilaterals
    glBegin(GL_QUADS);
    for (int j = 0; j < Ny; j++) {
      for (int i = 0; i < Nx; i++) {
	float c = mesh->He[Nx*j+i];
	pixels[i+Nx*j].setColor(c,c,c);
	pixels[i+Nx*j].render();
      }
    }
    glEnd(); // GL_QUADS

    // for double buffering: display buffer that was just rendered
    glutSwapBuffers();

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

void render(void) {
  g.render();
}

void idle(void) {
  float t = glutGet(GLUT_ELAPSED_TIME);
  float dt = (t - g.getTime()) / 1000;
  g.setTime(t);
  g.update(dt);
  glutMouseFunc(mouse);
  glutPostRedisplay(); // refresh the display
}

void reshape(int w, int h) {
  // Prevent a divide by zero, when window is too short
  // (you cant make a window of zero width).
  if (h == 0) {h = 1;}
  float ratio = (1.0 * w) / h;

  // change pixel dimension of the window
  g.setWidth(glutGet(GLUT_WINDOW_WIDTH));
  g.setHeight(glutGet(GLUT_WINDOW_HEIGHT));

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
  CImg<float> image("initial_conditions.png");
  g.initialize(image); // initialize the grid

  // initialize glut
  glutInit(&argc, argv);

  // specify the initial position of the display window
  //  -default value is (-1,-1), leading to automatic window placement
  glutInitWindowPosition(100, 100);

  // specify the initial size of the display window (in pixels)
  glutInitWindowSize(g.width(), g.height());

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

