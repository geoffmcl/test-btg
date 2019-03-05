#include <stdio.h>

#ifndef BTG_RAW_H
#define BTG_RAW_H

int read_uchar   (FILE *f, unsigned char *uc);
int write_uchar  (FILE *f, unsigned char *uc);
int read_ushort  (FILE *f, unsigned short *us);
int write_ushort (FILE *f, unsigned short *us);
int read_uint    (FILE *f, unsigned int *ui);
int write_uint   (FILE *f, unsigned int *ui);
int read_ulong   (FILE *f, unsigned long *ul);
int write_ulong  (FILE *f, unsigned long *ul);
int read_float   (FILE *f, float *fl);
int write_float  (FILE *f, float *fl);
int read_double  (FILE *f, double *db);
int write_double (FILE *f, double *db);

#endif
