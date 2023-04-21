

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

void calc_triangle_angles(float *a1, float *a2, float *a3, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
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
  
    // Define the triangle vertices in 3D space
    // float v1[3] = {1.0, 0.0, 0.0};
    // float v2[3] = {0.0, 1.0, 0.0};
    // float v3[3] = {0.0, 0.0, 1.0};

    // float v1[3] = { x1, y1, z1 };
    // float v2[3] = { x2, y2, z2 };
    // float v3[3] = { x3, y3, z3 };

    // // Calculate the vectors for two sides of the triangle
    // float v12[3], v13[3];
    // v12[0] = v2[0] - v1[0];
    // v12[1] = v2[1] - v1[1];
    // v12[2] = v2[2] - v1[2];
    // v13[0] = v3[0] - v1[0];
    // v13[1] = v3[1] - v1[1];
    // v13[2] = v3[2] - v1[2];

    // // Calculate the dot product and cross product of the two sides
    // float dot = dot_product(v12, v13);
    // float cross[3];
    // cross_product(v12, v13, cross);

    // // Calculate the magnitudes of the two sides and the cross product
    // float mag12 = sqrt(dot_product(v12, v12));
    // float mag13 = sqrt(dot_product(v13, v13));
    // float mag_cross = sqrt(dot_product(cross, cross));

    // // Calculate the angles of the triangle corners
    // float a1 = asin(mag_cross / (mag12 * mag13));
    // float a2 = asin(sin(a1) * mag13 / mag_cross);
    // float a3 = asin(sin(a1) * mag12 / mag_cross);

    // // Convert the angles from radians to degrees
    // float degrees_per_radian = 180.0 / M_PI;
    // a1 *= degrees_per_radian;
    // a2 *= degrees_per_radian;
    // a3 *= degrees_per_radian;

    // Print the angles
    printf("Angle 1: %f degrees\n", a1);
    printf("Angle 2: %f degrees\n", a2);
    printf("Angle 3: %f degrees\n", a3);

    return 0;
}