#ifndef _ASA066_H_
#define _ASA066_H_

// https://people.sc.fsu.edu/~jburkardt/c_src/asa066/asa066.html

double alnorm ( double x, bool upper );

void normp ( double z, double *p, double *q, double *pdf );
void nprob ( double z, double *p, double *q, double *pdf );

#endif