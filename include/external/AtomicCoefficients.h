/*! \file AtomicCoefficients.h
 \brief Evaluate matrix elements of Coulomb interaction from Slater integrals

 A set of functions to compute
 the matrix elements of Coulomb interaction between atomic orbitals
 with angular momentum l:
 U_{m1, m2; m3, m4} c_{m1}^+ c_{m2}^+ c_{m4} c_{m3},
 where
 U_{m1, m2; m3, m4} = sum_k F_k * V^k_{m1, m2; m3, m4}
 with F_k being the Slater integrals, F0, F2, ... , F_{2l}.
*/

#ifndef _ATOMIC_COEF_H
#define _ATOMIC_COEF_H

#include <vector>

/*!
 evaluate V^k_{m1, m2; m3, m4}
 for angular momentum l.
*/
double coef_vk(int l, int k, int m1, int m2, int m3, int m4);

/*!
 evaluate U_{m1, m2; m3, m4}
 for angular momentum l
 using the Slater integral fk[l+1] = { F0, F2, ... , F_{2l} }
*/
double atomic_u(int l, std::vector<double> &fk, int m1, int m2, int m3, int m4);


/*!
 evaluate matrix element of spin-orbit coupling
 < l m1, s1 | L.S | l m2, s2 >.
  m1, m2 = +l, ... , -l.
  s1, s2 = +1, -1  ( for up, down).
*/
double coef_ls(int l, int m1, int s1, int m2, int s2);

/*!
 3j-symbol for integer angular momentum
*/
double wigner_3j(int l1, int l2, int l3, int m1, int m2, int m3);

#endif // _ATOMIC_COEF_H
