#include<GL/glut.h>
#include<iostream>  // exit
#include<cmath>     // abs
#include<unistd.h>  // usleep

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

  void render() {
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
  Pixel* pixels;
  bool hold;

public :

  void initialize(unsigned nx, unsigned ny, unsigned px, unsigned py) {
    hold = false;
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
      glutMotionFunc(motion);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBegin(GL_QUADS);
    for (int j = 0; j < Ny; j++) {
      for (int i = 0; i < Nx; i++) {
	pixels[i+Nx*j].render();
      }
    }
    glEnd();

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

void render() {
  g.render();
}

void idle() {
  glutMouseFunc(mouse);
}

int main(int argc, char** argv) {
  g.initialize(120, 80, 1200, 800); // initialize the grid

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(1200, 800);
  glutCreateWindow("Grid");

  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutDisplayFunc(render);
  glutIdleFunc(idle);
  
  glutMainLoop();
}

