#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dataload.h"

#define MAX_LINE_SIZE 1024
#define MAX_MATERIALS 128
#define MAX_OBJECTS 128
#define MAX_FACES 2048
#define MAX_VERTICES 2048

void material_load_from_ppm(const char* file_basename, t_material** material, int flip) {
  const char* directory_path = "./assets/ppm/";
  const char* extension = ".ppm";
  char* filename = malloc(strlen(directory_path) + strlen(file_basename) + strlen(extension) + 1);
  sprintf(filename, "%s%s%s", directory_path, file_basename, extension);

  // Even if falis to load we want to return an empty material 
  (*material)->name = strdup(file_basename);
  (*material)->pixels = NULL;
  (*material)->width = 0;
  (*material)->height = 0;

  FILE* fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Error opening file %s\n", filename);

    return;
  }

  // Read PPM header
  char magic[3];
  int width, height, max_val;
  
  if (fscanf(fp, "%2s", magic) != 1 || strcmp(magic, "P6") != 0) {
      fprintf(stderr, "Invalid magic number %s\n", filename);
      return;
  }
  // Skip over any comments or blank lines after magic number
  int c;
  while ((c = fgetc(fp)) == ' ' || c == '\n' || c == '#') {
      if (c == '#') {
          // Skip over comment line
          while ((c = fgetc(fp)) != '\n' && c != EOF) {}
      }
  }

  // Read dimensions and max_val from PPM file
  ungetc(c, fp);

  // read dimensions and maximum value
  int nread = fscanf(fp, "%d", &width);
  if (nread != 1) {
      fprintf(stderr, "Could not read width %s\n", filename);
      return;
  }
  while ((c = fgetc(fp)) == '\n'); // skip newlines
  ungetc(c, fp);

  nread = fscanf(fp, "%d", &height);
  if (nread != 1) {
      fprintf(stderr, "Could not read height %s\n", filename);
      return;
  }
  while ((c = getc(fp)) == '\n'); // skip newlines
  ungetc(c, fp);

  nread = fscanf(fp, "%d", &max_val);
  if (nread != 1) {
      fprintf(stderr, "Could not read maximum value %s\n", filename);
      return;
  }
  while ((c = getc(fp)) == '\n'); // skip newlines
  ungetc(c, fp);

  // printf("image %s %dx%d *%d \n", filename, width, height, max_val);
  // Allocate memory for pixel data
  t_pixel* pixels = malloc(sizeof(t_pixel) * width * height);
  if (!pixels) {
    fprintf(stderr, "Error allocating memory for pixels\n");
    fclose(fp);
    return;
  }

// Read pixel data
int i = 0;
uint8_t buffer[3];
int row_offset = (flip ? height - 1 : 0);
int row_step = (flip ? -1 : 1);
for (int row = row_offset; row >= 0 && row < height; row += row_step) {
  for (int col = 0; col < width && fread(buffer, 1, 3, fp) == 3; col++) {
    pixels[row * width + col].r = buffer[0] * 255 / max_val;
    pixels[row * width + col].g = buffer[1] * 255 / max_val;
    pixels[row * width + col].b = buffer[2] * 255 / max_val;
    i++;
  }
}
  // // Read pixel data
  // int i = 0;
  // uint8_t buffer[3];
  // while (i < width * height && fread(buffer, 1, 3, fp) == 3) {
  //   pixels[i].r = buffer[0] * 255 / max_val;
  //   pixels[i].g = buffer[1] * 255 / max_val;
  //   pixels[i].b = buffer[2] * 255 / max_val;
  //   i++;
  // }

  (*material)->name = strdup(file_basename);
  (*material)->pixels = pixels;
  (*material)->width = width;
  (*material)->height = height;

  fclose(fp);
}

/**
 * @Returns number of faces
 */
int faces_load_from_file(const char* filename, t_face** faces) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("Error: Failed to open file %s\n", filename);
    return 0;
  }

  t_face*     F = malloc(MAX_FACES     *sizeof(t_face));
  t_object*   O = malloc(MAX_OBJECTS   *sizeof(t_object));
  t_material* M = malloc(MAX_MATERIALS *sizeof(t_material));
  t_vertex*   V = malloc(MAX_VERTICES  *sizeof(t_vertex));
  t_vt*      VT = malloc(MAX_VERTICES  *sizeof(t_vt));

  int v_count  = 0;
  int vt_count = 0;
  int o_count  = -1;
  int m_count  = -1;
  int f_count  = 0;

  while (!feof(file)) {
      char line[128];
      if (!fgets(line, 128, file)) break;

      // Vertices
      if (line[0] == 'v' && line[1] == ' ') {
          sscanf(line, "v %f %f %f",
            &(V[v_count].x), &(V[v_count].y), &(V[v_count].z));
          v_count++;
      }
      
      // Texture vertices
      else if (line[0] == 'v' && line[1] == 't') {
        sscanf(line, "vt %f %f", &(VT[vt_count].u), &(VT[vt_count].v));

        vt_count++;
      }

      // Objects
      else if (line[0] == 'o' && line[1] == ' ') {
        o_count++;

        int nameLength = strlen(line) - 2;
        O[o_count].name = strdup(line + 2); // Skip "o " prefix
        O[o_count].name[nameLength - 1] = '\0';
      }

      // Materials (starting with "usemtl " prefix)
      else if (line[0] == 'u' && line[1] == 's' && line[2] == 'e') {
        m_count++; 
        
        int nameLength = strlen(line) - 7;
        char* filename = strndup(line + 7,  strlen(line + 7) - 1); // Skip "usemtl " prefix
        // TODO Optimize, do not load pixes of already loaded files
        t_material* material = &M[m_count];
        material_load_from_ppm(filename, &material, 0);
      }

      // Finally faces
      else if (line[0] == 'f' && line[1] == ' ') {
        int v1, vt1, vn1, v2, vt2, vn2, v3, vt3, vn3;
        sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
          &v1, &vt1, &vn1, &v2, &vt2, &vn2, &v3, &vt3, &vn3);
        
        // Create face from scanned data
        F[f_count].v1.x = V[v1 - 1].x;
        F[f_count].v1.y = V[v1 - 1].y;
        F[f_count].v1.z = V[v1 - 1].z;
        F[f_count].v2.x = V[v2 - 1].x;
        F[f_count].v2.y = V[v2 - 1].y;
        F[f_count].v2.z = V[v2 - 1].z;
        F[f_count].v3.x = V[v3 - 1].x;
        F[f_count].v3.y = V[v3 - 1].y;
        F[f_count].v3.z = V[v3 - 1].z;

        F[f_count].vt1.u = VT[vt1 - 1].u;
        F[f_count].vt1.v = VT[vt1 - 1].v;
        F[f_count].vt2.u = VT[vt2 - 1].u;
        F[f_count].vt2.v = VT[vt2 - 1].v;
        F[f_count].vt3.u = VT[vt3 - 1].u;
        F[f_count].vt3.v = VT[vt3 - 1].v;

        // Current object and material too...
        F[f_count].m   = &M[m_count];
        F[f_count].obj = &O[o_count];

        f_count++;
      }
  }
  fclose(file);

  free(VT);
  free(V);

  *faces = F;

  // This could be usefull for debug
  for (int i = 0; i < f_count; i++) {
    printf("F: %d/%d in '%s' using '%s' (%dx%d) \n",
    i+1, f_count, F[i].obj->name, F[i].m->name, F[i].m->width, F[i].m->height );
    printf(" - Vertex (%3.3f, %3.3f, %3.3f) with UV (%.3f, %.3f)\n",
      F[i] .v1.x,
      F[i] .v1.y,
      F[i] .v1.z,
      F[i].vt1.u,
      F[i].vt1.v
    );
    printf(" - Vertex (%3.3f, %.3f, %3.3f) with UV (%.3f, %.3f)\n",
      F[i] .v2.x,
      F[i] .v2.y,
      F[i] .v2.z,
      F[i].vt2.u,
      F[i].vt2.v
    );
    printf(" - Vertex (%3.3f, %3.3f, %3.3f) with UV (%.3f, %.3f)\n",
      F[i] .v3.x,
      F[i] .v3.y,
      F[i] .v3.z,
      F[i].vt3.u,
      F[i].vt3.v
    );
  }
  return f_count;
}