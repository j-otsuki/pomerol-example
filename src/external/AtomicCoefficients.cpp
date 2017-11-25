/*! \file AtomicCoefficients.cpp
 \brief Definition of atomic_coef.h
*/

#include <stdlib.h>
#include <math.h>
#include <cassert>
#include "gsl/gsl_sf_coupling.h"
#include "external/AtomicCoefficients.h"

double wigner_3j(int l1, int l2, int l3, int m1, int m2, int m3)
{
	return gsl_sf_coupling_3j(2*l1, 2*l2, 2*l3, 2*m1, 2*m2, 2*m3);
}

static double pow_minus(int i)
{
	if( abs(i) % 2 )  return -1;
	else  return +1;
}

// V^k_{m1, m2; m3, m4}
double coef_vk(int l, int k, int m1, int m2, int m3, int m4)
{
	if( m1 + m2 != m3 + m4 )  return 0;

	double r = pow(2*l+1, 2) * pow_minus(m1 + m4 + k);

	r *= pow( wigner_3j(l, k, l, 0, 0, 0), 2 );
	r *= wigner_3j(l, k, l, -m1, m1-m3, m3);
	r *= wigner_3j(l, k, l, -m4, m4-m2, m2);

	return r;
}

// U_{m1, m2; m3, m4} = sum_k F_k * V^k_{m1, m2; m3, m4}
//  fk[l+1] = { F0, F2, ... , F_{2l} }
double atomic_u(int l, std::vector<double> &fk, int m1, int m2, int m3, int m4)
{
	assert( fk.size() >= l+1 );
	double r = 0;
	for(int k=0; k<=2*l; k+=2){
		r += fk[k/2] * coef_vk(l, k, m1, m2, m3, m4);
	}
	return r;
}

// double coef_Gaunt(int k, int l1, int m1, int l2, int m2)
// {
// 	double r = sqrt( (2*l1+1) * (2*l2+1) );
// 	if( (l1+k+l2) % 2 )  return 0;
// 	r *= pow_minus(m1);
// 	r *= pow_minus((l1+k+l2) / 2);
// 	r *= wigner_3j(l1, k, l2, 0, 0, 0);
// 	r *= wigner_3j(l1, k, l2, -m1, m1-m2, m2);
// 	return r;
// }


// <l m1 | L^z | l m2>
static double l_z(int l, int m1, int m2)
{
	if( m1 != m2 )  return 0;
	else  return double(m1);
}
// <l m1 | L^+ | l m2>
static double l_plus(int l, int m1, int m2)
{
	if( m1 != m2+1 )  return 0;
	else  return sqrt( (l-m2) * (l+m2+1) );
}
// <l m1 | L^- | l m2>
static double l_minus(int l, int m1, int m2)
{
	return l_plus(l, m2, m1);
}
// < s1 | s^z | s2 >
static double s_z(int s1, int s2)
{
	if( s1 == s2 )  return 0.5 * double(s1);
	else   return 0;
}
// < s1 | s^+ | s2 >
static double s_plus(int s1, int s2)
{
	if( s1 == +1 && s2 == -1 )  return 1.0;
	else   return 0;
}
// < s1 | s^- | s2 >
static double s_minus(int s1, int s2)
{
	return s_plus(s2, s1);
}

// < l m1, s1 | L.S | l m2, s2 >
double coef_ls(int l, int m1, int s1, int m2, int s2)
{
	double r = l_z(l, m1, m2) * s_z(s1, s2);
	r += 0.5 * l_plus(l, m1, m2) * s_minus(s1, s2);
	r += 0.5 * l_minus(l, m1, m2) * s_plus(s1, s2);

	return r;
}
