#include <math.h>
#include <stdlib.h>

#include "dataload.h"
#include "display.h"

void pixel_put(int x, int y, int r, int g, int b);

void clearBackground() {
  int x, y;
  for (y = 0; y < SH; y++) {
    for (x = 0; x < SW; x++) {
      pixel_put(x, y, 0, 61, 30 );
    }
  }
}

void testTextures(t_face*  faces, int f_count) {
  for (int i = 0; i < f_count; i++) {
    t_material* m = faces[i].m;

    int x, y;
    for (y = 0; y < m->height; y++) {
      for (x = 0; x < m->width; x++) {
        int pixel = (m->height - y -1)  * m->width + x ;

        int r = m->pixels[pixel].r;
        int g = m->pixels[pixel].g; 
        int b = m->pixels[pixel].b;
        pixel_put(x, y, r, g, b);
      }
    }
  }
}

#define EPSILON 0.000001

t_face* raycast(float x, float y, float z, float dir_x, float dir_y, float dir_z, t_face* faces, int num_faces,
unsigned char* r, unsigned char* g, unsigned char* b
) {
    float min_distance = INFINITY;
    t_face* closest_face = NULL;
    for (int i = 0; i < num_faces; i++) {
        float x1 = faces[i].v1.x;
        float y1 = faces[i].v1.y;
        float z1 = faces[i].v1.z;
        float x2 = faces[i].v2.x;
        float y2 = faces[i].v2.y;
        float z2 = faces[i].v2.z;
        float x3 = faces[i].v3.x;
        float y3 = faces[i].v3.y;
        float z3 = faces[i].v3.z;

        float e1x = x2 - x1;
        float e1y = y2 - y1;
        float e1z = z2 - z1;
        float e2x = x3 - x1;
        float e2y = y3 - y1;
        float e2z = z3 - z1;
        float px = dir_y * e2z - dir_z * e2y;
        float py = dir_z * e2x - dir_x * e2z;
        float pz = dir_x * e2y - dir_y * e2x;
        float det = e1x * px + e1y * py + e1z * pz;
        if (det > -EPSILON && det < EPSILON) {
            continue;
        }
        float inv_det = 1.0f / det;
        float tx = x - x1;
        float ty = y - y1;
        float tz = z - z1;
        float u = (tx * px + ty * py + tz * pz) * inv_det;
        if (u < 0.0f || u > 1.0f) {
            continue;
        }
        float qx = ty * e1z - tz * e1y;
        float qy = tz * e1x - tx * e1z;
        float qz = tx * e1y - ty * e1x;
        float v = (dir_x * qx + dir_y * qy + dir_z * qz) * inv_det;
        if (v < 0.0f || u + v > 1.0f) {
            continue;
        }
        float t = (e2x * qx + e2y * qy + e2z * qz) * inv_det;
        if (t < 0.0f || t > min_distance) {
            continue;
        }
        min_distance = t;
        closest_face = &faces[i];

        float u1 = closest_face->vt1.u;
        float v1 = closest_face->vt1.v;
        float u2 = closest_face->vt2.u;
        float v2 = closest_face->vt2.v;
        float u3 = closest_face->vt3.u;
        float v3 = closest_face->vt3.v;

        // Compute u and v texture coordinates
        float u_tex = u1 * (1 - u - v) + u2 * u + u3 * v;
        float v_tex = v1 * (1 - u - v) + v2 * u + v3 * v;

        // Tile the texture coordinates
        u_tex = fmodf(u_tex, 1.0f);
        v_tex = fmodf(v_tex, 1.0f);

        // Handle negative values of tiled texture coordinates
        if (u_tex < 0.0f) {
            u_tex += 1.0f;
        }
        if (v_tex < 0.0f) {
            v_tex += 1.0f;
        }

        // Flip the texture vertically
        v_tex = 1.0f - v_tex;

        t_material* m = closest_face->m;
        if (m->pixels == NULL) {
          // Return face so that we can draww in default color
          return closest_face;
        }

        int pixel_row = (int)(v_tex * (m->height - 1));
        int pixel_col = (int)(u_tex * (m->width - 1));
        int pixel_index = pixel_row * m->width + pixel_col;

        *r = m->pixels[pixel_index].r;
        *g = m->pixels[pixel_index].g; 
        *b = m->pixels[pixel_index].b;
    }
    return closest_face;
}

void project(float x, float y, float z, float rotation_z, float rotation_x, t_face* faces, int num_faces) {

    int u, v;
    uint8_t r, g, b;

    // Calculate the field of view in radians
    float fov = 60.0f * M_PI / 180.0f;
    // Calculate the angle step size for both horizontal and vertical directions
    float angle_step = fov / SW;
    // Calculate the vertical angle step size based on the aspect ratio
    float aspect_ratio = (float)SH / (float)SW;
    float v_angle_step = angle_step * aspect_ratio;
    // Calculate the initial horizontal and vertical angles
    float h_angle = -(fov / 2.0f);
    float v_angle = -(fov / 2.0f) * aspect_ratio;

    // Loop through every pixel in the screen
    for (int i = 0; i < SW; i++) {
        for (int j = 0; j < SH; j++) {

            // Calculate the direction vector based on the current horizontal and vertical angles and rotation
            float dir_y = cos(h_angle) * cos(v_angle);
            float dir_x = sin(v_angle);
            float dir_z = sin(h_angle) * cos(v_angle);

            // Rotate the direction vector around the x-axis
            float cos_rot_x = cos(-rotation_x);
            float sin_rot_x = sin(-rotation_x);
            float new_dir_y = dir_y * cos_rot_x - dir_z * sin_rot_x;
            float new_dir_z = dir_y * sin_rot_x + dir_z * cos_rot_x;

            // Rotate the direction vector around the z-axis
            float cos_rot_z = cos(-rotation_z);
            float sin_rot_z = sin(-rotation_z);
            float new_dir_x = dir_x * cos_rot_z - new_dir_y * sin_rot_z;
            new_dir_y = dir_x * sin_rot_z + new_dir_y * cos_rot_z;

            // Normalize the direction vector
            float length = sqrt(new_dir_x * new_dir_x + new_dir_y * new_dir_y + new_dir_z * new_dir_z);
            new_dir_x /= length;
            new_dir_y /= length;
            new_dir_z /= length;

            // Call the raycast function for the current pixel and direction
            t_face* found = raycast(
              x, y, z,
              new_dir_x, new_dir_y, new_dir_z, 
              faces, num_faces,
              &r, &g, &b
            );
            if (found != NULL) {
              if (found->m->pixels != NULL) {
                pixel_put(i, j, r, g, b);
              } else {
                pixel_put(i, j, 100, 100, 50);
              }
            }

            // Increment the horizontal angle by the angle step size
            h_angle += angle_step;
        }
        // Increment the vertical angle by the angle step size
        v_angle += v_angle_step;
        // Reset the horizontal angle to the initial value
        h_angle = -(fov / 2.0f);
    }
}
