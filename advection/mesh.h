#ifndef MESH_H
#define MESH_H

// include standard C/C++ libraries
#include <iostream> // exit
#include <cmath>    // sqrt
#include <cstring>  // memset

class Mesh {
  // Discretization parameters
  int Nx, Ny; // Number of nodes in the x- and y-directions
  int Ex, Ey; // Number of elements in the x- and y-directions
  float dx;   // Grid spacing
  float dt;   // Time step

  // Field variables
  float* Vxn; // Nodal x-velocity      [Nx*Ny]
  float* Vyn; // Nodal y-velocity      [Nx*Ny]
  float* He;  // Element pressure head [Ex*Ey]

  // Transfer operators
  float* Re;  // Remap integral operator [4*Ex*Ey]

  // Workspace arrays
  float* Un;  // Nodal field     [Nx*Ny]
  float* Ue;  // Element field   [Ex*Ey]
  float* Fn;  // Nodal residual  [Nx*Ny]
  float* dUn; // Nodal increment [Nx*Ny]

  Mesh(int ex, int ey, float width) {
    Ex = ex;
    Ey = ey;
    Nx = Ex + 1;
    Ny = Ey + 1;
    dx = width;
    dt = 1.0; // default initialization
    Vxn = new float[Nx*Ny]();
    Vyn = new float[Nx*Ny]();
    He  = new float[Ex*Ey]();
    Re  = new float[4*Ex*Ey];
    Un  = new float[Nx*Ny];
    Ue  = new float[Ex*Ey];
    Fn  = new float[Nx*Ny];
    dUn = new float[Nx*Ny];
  } // Mesh

  void UpdateFields(void) {
    UpdateIntegralOperator();

    // Update velocity field
    UpdateNodalField(Vxn);
    UpdateNodalField(Vyn);

    // Update pressure head field
    UpdateElementField(He);

    // Enforce BCs
    EnforceNodalBCs();
  } // UpdateFields

  void UpdateIntegralOperator(void) {
    const float scale = 0.5*dt/dx;
    float w, xi, eta;
    for (int j = 0; j < Ey; j++) {
      for (int i = 0; i < Ex; i++) {
	int e = Ex*j+i;
	w = 0.25*dx*dx*(1.0+scale*(-Vxn[Nx*j+i]      -Vyn[Nx*j+i]
                                   +Vxn[Nx*j+i+1]    -Vyn[Nx*j+i+1]
                                   +Vxn[Nx*(j+1)+i+1]+Vyn[Nx*(j+1)+i+1]
				   -Vxn[Nx*(j+1)+i]  +Vyn[Nx*(j+1)+i]));
	xi  = scale*(Vxn[Nx*j+i]
                    +Vxn[Nx*j+i+1]
                    +Vxn[Nx*(j+1)+i+1]
		    +Vxn[Nx*(j+1)+i]);
	eta = scale*(Vyn[Nx*j+i]
                    +Vyn[Nx*j+i+1]
                    +Vyn[Nx*(j+1)+i+1]
		    +Vyn[Nx*(j+1)+i]);
	Re[4*e]   = w*(1.0-xi)*(1.0-eta);
	Re[4*e+1] = w*(1.0+xi)*(1.0-eta);
	Re[4*e+2] = w*(1.0+xi)*(1.0+eta);
	Re[4*e+3] = w*(1.0-xi)*(1.0+eta);
      }
    }
  } // UpdateIntegralOperator

  void UpdateNodalField(float* Xn) {
                     // Xn[Nx*Ny]
    Interpolate(Xn, Ue);
    Integrate(Ue); Remap(Xn);
  } // UpdateNodalField

  void UpdateElementField(float* Xe) {
                       // Xe[Nx*Ny]
    Integrate(Xe); Remap(Un);
    Interpolate(Un, Xe);
  } // UpdateElementField

  void EnforceNodalBCs(void) {
    // Enforce zero normal velocity BCs
    for (int j = 0; j < Ny; j++) {
      Vxn[Nx*j]        = 0.0;
      Vxn[Nx*j+(Nx-1)] = 0.0;
    }
    for (int i = 0; i < Nx; i++) {
      Vyn[i]           = 0.0;
      Vyn[Nx*(Ny-1)+i] = 0.0;
    }
  } // EnforceNodalBCs

  void Interpolate(float* Xn, float* Xe) {
                // Xn[Nx*Ny], Xe[Ex*Ey]
    for (int j = 0; j < Ey; j++) {
      for (int i = 0; i < Ex; i++) {
	Xe[Ex*j+i] = 0.25*(Xn[Nx*j+i]
                          +Xn[Nx*j+i+1]
                          +Xn[Nx*(j+1)+i+1]
                          +Xn[Nx*(j+1)+i]);
      }
    }
  } // Interpolate

  void Integrate(float* Xe) {
              // Xe[Ex*Ey]
    memset(Fn, 0.0, Nx*Ny); // zero out Fn
    for (int j = 0; j < Ey; j++) {
      for (int i = 0; i < Ex; i++) {
	int e = Ex*j+i;
	Fn[Nx*j+i]       += Re[4*e]   * Xe[e];
	Fn[Nx*j+i+1]     += Re[4*e+1] * Xe[e];
	Fn[Nx*(j+1)+i+1] += Re[4*e+2] * Xe[e];
	Fn[Nx*(j+1)+i]   += Re[4*e+3] * Xe[e];
      }
    }
  } // Integrate

  void Remap(float* Xn) {
          // Xn[Nx*Ny]
    // Solve M * Xn = Fn through Jacobi relaxation

    // set constant(s)
    const float tol = 1.0e-5;

    // iterate on the residual
    UpdateResidual(Xn);
    while (Norm() > tol) {
      UpdateIncrement();
      UpdateResidual(dUn);
      for (int i = 0; i < Nx*Ny; i++) {
	Xn[i] += dUn[i];
      }
    }
  } // Remap

  void UpdateResidual(float* Xn) {
                   // Fn[Nx*Ny], Xn[Nx*Ny]
    // Compute Fn -= M * Xn

    // set constant(s)
    const float w = dx*dx/36.0;

    // (-1,-1)
    for (int j = 1; j < Ny; j++) {
      for (int i = 1; i < Nx; i++) {
	Fn[Nx*j+i] -= w * Xn[Nx*(j-1)+i-1];
      }
    }
    // (0,-1)
    for (int j = 1; j < Ny; j++) {
      for (int i = 0; i < Nx; i++) {
	Fn[Nx*j+i] -= 4.0 * w * Xn[Nx*(j-1)+i];
      }
    }
    // (+1,-1)
    for (int j = 1; j < Ny; j++) {
      for (int i = 0; i < (Nx-1); i++) {
	Fn[Nx*j+i] -= w * Xn[Nx*(j-1)+i+1];
      }
    }
    // (-1,0)
    for (int j = 0; j < Ny; j++) {
      for (int i = 1; i < Nx; i++) {
	Fn[Nx*j+i] -= 4.0 * w * Xn[Nx*j+i-1];
      }
    }
    // (0,0)
    for (int j = 0; j < Ny; j++) {
      for (int i = 0; i < Nx; i++) {
	Fn[Nx*j+i] -= 16.0 * w * Xn[Nx*j+i];
      }
    }
    // (+1,0)
    for (int j = 0; j < Ny; j++) {
      for (int i = 0; i < (Nx-1); i++) {
	Fn[Nx*j+i] -= 4.0 * w * Xn[Nx*j+i+1];
      }
    }
    // (-1,+1)
    for (int j = 0; j < (Ny-1); j++) {
      for (int i = 1; i < Nx; i++) {
	Fn[Nx*j+i] -= w * Xn[Nx*(j+1)+i-1];
      }
    }
    // (0,+1)
    for (int j = 0; j < (Ny-1); j++) {
      for (int i = 0; i < Nx; i++) {
	Fn[Nx*j+i] -= 4.0 * w * Xn[Nx*(j+1)+i];
      }
    }
    // (+1,+1)
    for (int j = 0; j < (Ny-1); j++) {
      for (int i = 0; i < (Nx-1); i++) {
	Fn[Nx*j+i] -= w * Xn[Nx*(j+1)+i+1];
      }
    }
  } // UpdateResidual

  float Norm(void) {
    // Compute the normalized L2 norm of the residual, where
    // Norm = sqrt(Fn' * M * Fn) / sqrt(Ex*Ey*dx^2)
    // However: use the diagonalized (approximate row-averaged) M, for speed

    float norm = 0.0;
    for (int i = 0; i < Nx*Ny; i++) {
      norm += Fn[i] * Fn[i];
    }
    return std::sqrt(norm/(Ex*Ey));
  } // Norm

  void UpdateIncrement(void) {
    // Compute dUn = P * Fn (P is an approximation to inv(M))

    // Use Jacobi relaxation (divide by the diagonal entries of M)
    
    // corners
    float w = 1.0/(4.0*dx*dx);
    dUn[0]                = w * Fn[0];
    dUn[Nx-1]             = w * Fn[Nx-1];
    dUn[Nx*(Ny-1)+(Nx-1)] = w * Fn[Nx*(Ny-1)+(Nx-1)];
    dUn[Nx*(Ny-1)]        = w * Fn[Nx*(Ny-1)];

    // edges
    float w = 1.0/(8.0*dx*dx);
    for (int i = 1; i < (Nx-1); i++) {
      dUn[i]           = w * Fn[i];
      dUn[Nx*(Ny-1)+i] = w * Fn[Nx*(Ny-1)+i];
    }
    for (int j = 1; j < (Ny-1); j++) {
      dUn[Nx*j+(Nx-1)] = w * Fn[Nx*j+(Nx-1)];
      dUn[Nx*j]        = w * Fn[Nx*j];
    }

    // middle
    float w = 1.0/(16.0*dx*dx);
    for (int j = 1; j < (Ny-1); j++) {
      for (int i = 1; i < (Nx-1); i++) {
	dUn[Nx*j+i] = w * Fn[Nx*j+i];
      }
    }
  } // UpdateIncrement
  
};

#endif // MESH_H
