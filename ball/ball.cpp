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
void keyboard(unsigned char c, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void render(void);
void idle(void);
void reshape(int w, int h);

// define a ball class object
class Ball {
private :
  float coordinate[2];
  float velocity[2];
  float radius;
  float color[3];
  
public :

  void setCoordinate(float x, float y) {
    coordinate[0] = x;
    coordinate[1] = y;
  } // setCoordinate

  void setVelocity(float vx, float vy) {
    velocity[0] = vx;
    velocity[1] = vy;
  } // setCoordinate
   
  void setRadius(float r) {
    radius = r;
  } // setRadius

  void setColor(float r, float g, float b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
  } // setColor

  void update(float dt) {
    // handle contact conditions
    float friction = 1.0 - 0.02;
    if ((coordinate[0] - radius) <= -1.0) {
      coordinate[0] = -1.0 + radius;
      velocity[0] = friction * std::abs(velocity[0]);
      velocity[1] = friction * velocity[1];
    } else if ((coordinate[0] + radius) >= +1.0) {
      coordinate[0] = 1.0 - radius;
      velocity[0] = -friction * std::abs(velocity[0]);
      velocity[1] = friction * velocity[1];
    }
    if ((coordinate[1] - radius) <= -1.0) {
      coordinate[1] = -1.0 + radius;
      velocity[0] = friction * velocity[0];
      velocity[1] = friction * std::abs(velocity[1]);
    } else if ((coordinate[1] + radius) >= +1.0) {
      coordinate[1] = 1.0 - radius;
      velocity[0] = friction * velocity[0];
      velocity[1] = -friction * std::abs(velocity[1]);
    }

    // set particle mass
    float mass = 1.0;

    // set particle force
    float f[2];
    f[0] = 0.0;
    f[1] = -9.8*mass;
    float drag = 0.5;
    if (velocity[0] > 0) {
      f[0] -= drag * velocity[0]*velocity[0];
    } else if (velocity[0] < 0) {
      f[0] += drag * velocity[0]*velocity[0];
    }
    if (velocity[1] > 0) {
      f[1] -= drag * velocity[1]*velocity[1];
    } else if (velocity[1] < 0) {
      f[1] += drag * velocity[1]*velocity[1];
    }

    // set particle acceleration
    float a[2];
    a[0] = f[0] / mass;
    a[1] = f[1] / mass;

    // set particle velocity
    velocity[0] += a[0] * dt;
    velocity[1] += a[1] * dt;

    // set particle coordinates
    coordinate[0] += velocity[0] * dt;
    coordinate[1] += velocity[1] * dt;
  }

  void render() {
    glBegin(GL_TRIANGLE_FAN);

    glColor3f(color[0]+0.5, color[1]+0.5, color[2]+0.5);
    glVertex2f(coordinate[0], coordinate[1]); // center of circle

    float pi = 4.0 * std::atan(1.0);
    for (int i = 0; i <= 20; i++)   {
      glColor3f(color[0], color[1], color[2]);
      glVertex2f ((coordinate[0] + (radius * cos(i * 2.0 * pi / 20))),
                  (coordinate[1] + (radius * sin(i * 2.0 * pi / 20))));
    }

    glEnd(); // GL_TRIANGLE_FAN
  }
};

class Grid {
private :
  int Px, Py;
  Ball ball;
  float time;

public :

  void initialize(float x, float y, float r, unsigned px, unsigned py) {
    time = 0.0;
    Px = px;
    Py = py;
    ball.setCoordinate(x,y);
    ball.setRadius(r);
    ball.setVelocity(10.0,5.0);
    ball.setColor(1.0,0.0,0.0);
  } // initialize

  float getTime(void) {
    return time;
  }

  void setTime(float t) {
    time = t;
  }

  void update(float dt) {
    ball.update(dt);
  }

  void keyboard(unsigned char c, int x, int y) {
    if (c == 27) { // ASCII code for the escape key
      exit(0);
    }
  }

  void mouse(int button, int state, int x, int y) {
    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
      glutMotionFunc(motion);
      glutPostRedisplay(); // refresh the display
    }
  }

  void m_motion(int x, int y) {
    glutPostRedisplay(); // refresh the display
  }

  void render(void) {
    // clear the current bit buffers, restoring them to their preset values
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw ball
    ball.render();

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
  int px, py;
  px = py = 600;
  g.initialize(0.0, 0.0, 0.1, px, py); // initialize the grid

  // initialize glut
  glutInit(&argc, argv);

  // specify the initial position of the display window
  //  -default value is (-1,-1), leading to automatic window placement
  glutInitWindowPosition(100, 100);

  // specify the initial size of the display window (in pixels)
  glutInitWindowSize(px, py);

  // initialize the display modes:
  // -define colors via RGBA values
  // -use double buffering (for smoother animations)
  // -use depth buffering (to correctly occlude objects with greater depth)
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

  // create the display window with the specified name
  glutCreateWindow("Ball");

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

