#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846

#define A   (150.0)
#define B   (240.0)
#define C   (200.0)
#define D   (120.0)
#define E   (90.0)

#define M1  (237.0-10.0)
#define N1  (50.0)
#define M2  (237.0-10.0)
#define N2  (50.0)

#define K1_MIN   (187.0)
#define K1_MAX   (287.0-10.0)
#define K2_MIN   (187.0)
#define K2_MAX   (287.0-10.0)

double X, Y;
double nmk1, nmk2;

double calc_degree(double a, double b, double c) {
    return acos((a*a+b*b-c*c)/a/b/2.0);
}

//正向计算
void calc_xy(double k1, double k2) {

    double d1 = calc_degree(N1, M1, k1) - nmk1;
    double d2 = calc_degree(N2, M2, k2) - nmk2;
    double x = sin(d1)*B;
    double y = cos(d1)*B+D;

    double dt = PI - d1 -d2;

    X = x+sin(dt)*C;
    Y = y-cos(dt)*C;

    if(d1 > PI/3*2 && d2 > PI/3*2) {
        X = 0;
        Y = 0;
    }
}


int main() {

    double i,j;
    nmk1 = calc_degree(N1, M1, K1_MIN);
    nmk2 = calc_degree(N2, M2, K2_MIN);

    for(i=K1_MIN; i<K1_MAX; i+=1.0) {
        for(j=K2_MIN; j<K2_MAX; j+=1.0) {
            calc_xy(i, j);
            printf("%lf, %lf, %lf, %lf\n", i, j, X, Y);
        }
    }
}















/*
           n/--...k1-k2
-----------/_____```--..
                m
m + r = max

                 ^
                 |
             /\  |
          c /  \b|
           /    \|
          /     /|
       (x, y)  /a|d
       <------/--|---
               e |
*/










