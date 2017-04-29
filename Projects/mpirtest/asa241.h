#ifndef _ASA241_H_
#define _ASA241_H_

// Downloaded from: https://people.sc.fsu.edu/~jburkardt/c_src/asa241/asa241.html

// functions used for testing
void normal_01_cdf_values ( int *n_data, double *x, double *fx ); // Reference exact values from the noraml CDF
void asa241Test ( );

// Inverse CDF
float r4_normal_01_cdf_inverse ( float p );
double r8_normal_01_cdf_inverse ( double p );


// helper functions
float r4poly_value ( int n, float a[], float x );
double r8_huge ( );
double r8poly_value ( int n, double a[], double x );
float r4_huge ( );
void timestamp ( );

#endif