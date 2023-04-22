#include <stdio.h>
#include <math.h>

// Helper function to calculate the dot product of two vectors
float dot_product(float v1[], float v2[]) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

// Helper function to calculate the cross product of two vectors
void cross_product(float v1[], float v2[], float result[]) {
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void calc_triangle_angles(
    float *a1, float *a2, float *a3,
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3
){
    float a, b, c;
    float dotproduct, angle;
    
    // Calculate the lengths of the sides of the triangle
    a = sqrt(pow(x2-x1, 2) + pow(y2-y1, 2) + pow(z2-z1, 2));
    b = sqrt(pow(x3-x2, 2) + pow(y3-y2, 2) + pow(z3-z2, 2));
    c = sqrt(pow(x1-x3, 2) + pow(y1-y3, 2) + pow(z1-z3, 2));
    
    // Calculate the angles of the corners of the triangle
    dotproduct = (b*b + c*c - a*a) / (2*b*c);
    *a1 = acos(dotproduct) * 180 / M_PI;
    
    dotproduct = (c*c + a*a - b*b) / (2*c*a);
    *a2 = acos(dotproduct) * 180 / M_PI;
    
    dotproduct = (a*a + b*b - c*c) / (2*a*b);
    *a3 = acos(dotproduct) * 180 / M_PI;
}

int print_angles (
  float x1, float y1, float z1,
  float x2, float y2, float z2,
  float x3, float y3, float z3
) {
    float a1, a2, a3;
    calc_triangle_angles(&a1, &a2, &a3, x1, y1, z1, x2, y2, z2, x3, y3, z3);
    
    // Print the angles
    printf("Angle 1: %f degrees\n", a1);
    printf("Angle 2: %f degrees\n", a2);
    printf("Angle 3: %f degrees\n", a3);

    return 0;
}