#pragma once

#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>

#include "../../Common/include/mpi_structure.hpp"

#include "../../Common/include/geometry_structure.hpp"
#include "../../Common/include/config_structure.hpp"
#include "../../Common/include/matrix_structure.hpp"
#include "../../Common/include/vector_structure.hpp"
#include "../../Common/include/linear_solvers_structure.hpp"
#include "../../Common/include/grid_movement_structure.hpp"
#include "../../SU2_CFD/include/solver_structure.hpp"

using namespace std;

class SUBoom{
public:
  unsigned long n_prof;
  unsigned long **pointID_original;
  su2double ***Coord_original;
  bool *startline, *endline;

  /*---Flight variables---*/
  su2double flt_h;
  su2double flt_M;
  su2double flt_psi, flt_gamma, flt_mu;
  su2double flt_heading[3];

  /*---Atmosphere variables---*/
  su2double atm_g;
  unsigned int atm_noise_flag;

  su2double T_inf, a_inf, p_inf, rho_inf;
  su2double *z, *a_of_z, *rho_of_z;//, T_of_z[65001], rho_of_z[65001];

  /*---Scale factors---*/
  su2double scale_L, scale_T, scale_p, scale_m, scale_z;
  su2double scale_C1, scale_C2;

  /*---Ray variables---*/
  unsigned short ray_N_phi;
  su2double ray_r0;
  su2double ray_t0;
  su2double *ray_phi;
  su2double **ray_nu;
  su2double **ray_theta0;
  su2double **ray_c0;
  su2double **theta;
  su2double **x_of_z, **y_of_z, **t_of_z;
  su2double **dxdt, **dydt, **dzdt;
  su2double *ray_A;
  su2double *ray_C1, *ray_C2, *ray_dC1, *ray_dC2;

  /*---Tolerance variables---*/
  su2double tol_l, tol_m;
  su2double tol_dp, tol_dr, tol_dphi;
  su2double tol_integrate;

  /*---Boom strength---*/
  su2double *p_rise, *p_max, *p_rise2, *p_int2;

  /*---Sensitivity---*/
  unsigned short nDim;
  unsigned long *nPanel, nSig, *nPointID, nPointID_tot;
  unsigned long **PointID;
  su2double ***dJdU;

  /*---Ray data class for passing into RK4---*/
  class RayData{
    public:
      su2double L;
      su2double T;
      su2double c0;
      su2double nu;

      su2double diff_m2, diff_p2;
      su2double scale_C1, scale_C2;
      su2double *t, *C1, *C2, *dC1, *dC2;
      unsigned long M;

      //void GetAtmosphericData(su2double& a, su2double& rho, su2double& p, su2double h);
  };

  /*---Signal class for storing pressure signal---*/
  class Signal{
    public:
      unsigned long *original_len, M, *final_len;
      su2double *m, *dp, *l, *fvec;
      su2double **x, **original_T, **original_p;
      su2double **final_T, **final_p;
  };

  Signal signal;

  /*---Main functions for boom calculations---*/
  SUBoom();
  SUBoom(CSolver *solver, CConfig *config, CGeometry *geometry);
  ~SUBoom(void);

  void AtmosISA(su2double& h0, su2double& T, su2double& a, su2double& p,
                su2double& rho, su2double& g);
  void SearchLinear(CConfig *config, CGeometry *geometry, 
                    const su2double r0, const su2double *phi, unsigned short nPhi);
  void ExtractLine(CGeometry *geometry, const su2double r0, unsigned short iPhi);
  void ExtractPressure(CSolver *solver, CConfig *config, CGeometry *geometry, unsigned short iPhi);
  bool InsideElem(CGeometry *geometry, su2double r0, su2double phi, unsigned long jElem, su2double *p0, su2double *p1);
  int Intersect2D(su2double r0, su2double *Coord_i, su2double *Coord_ip1, su2double *p0, su2double *p1);
  int Intersect3D(su2double r0, su2double phi, int nCoord, su2double **Coord_i, su2double *p1);
  void ConditionAtmosphericData();
  void ScaleFactors();
  void DistanceToTime();
  void InitialWaveNormals();
  void RayTracer(unsigned short iPhi);
  void RayTubeArea(unsigned short iPhi);
  void FindInitialRayTime();
  void ODETerms(unsigned short iPhi);
  void CreateSignature(unsigned short iPhi); // Note: p is pressure disturbance (relative)
  void PropagateSignal(unsigned short iPhi);
  void WriteSensitivities();

  void GetAtmosphericData(su2double& a, su2double& rho, su2double& p, su2double h);
  void Sph2Cart(su2double& nx, su2double& ny, su2double& nz, su2double az, su2double elev,
                  su2double r);
  su2double *rk4(su2double x0, int m, su2double y0[], su2double dx, RayData data,
              su2double *f(su2double x, int m, su2double y[], RayData data));
  su2double *SplineGetDerivs(su2double x[], su2double y[], int N);
  su2double matchr(int j, su2double h_L, su2double r0);
  su2double *ClipLambdaZeroSegment(su2double fvec[], int &M);
};
void MergeSort(su2double x[], su2double p[], int l, int r);
void MergeSort(su2double y[], unsigned long k[], int l, int r);
void merge(su2double x[], su2double p[], int l, int m, int r);
void merge(su2double y[], unsigned long p[], int l, int m, int r);
void QuickSort(su2double x[], su2double p[], int l, int r);
su2double *derivs(su2double x, int m, su2double y[], SUBoom::RayData data);
su2double *derivsProp(su2double t, int m, su2double y[], SUBoom::RayData data);
su2double **WaveformToPressureSignal(su2double fvec[], unsigned int M, int &Msig);
su2double EvaluateSpline(su2double x, int N, su2double t[], su2double fit[], su2double coeffs[]);
void Isoparameters(unsigned short nDim, unsigned short nDonor, su2double *X, su2double *xj, su2double *isoparams);