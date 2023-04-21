#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 1024
#define MAX_MATERIALS 128
#define MAX_OBJECTS 128
#define MAX_FACES 2048
#define MAX_VERTICES 2048

typedef struct {
  char* name;
  void* map_kd;
} t_material;

typedef struct {
  float x, y, z;
} t_vertex;

typedef struct {
  float u, v;
} t_vt;

typedef struct t_object t_object;
typedef struct t_face t_face;

struct t_face {
  t_vertex v1, v2, v3;
  t_vt vt1, vt2, vt3;
  t_object* obj;
  t_material *m;
};

struct t_object {
  char* name;
};

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
            M[m_count].name = strdup(line + 7); // Skip "usemtl " prefix
            M[m_count].name[nameLength - 1] = '\0';
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
        F[f_count].vt2.u = VT[vt1 - 2].u;
        F[f_count].vt2.v = VT[vt1 - 2].v;
        F[f_count].vt3.u = VT[vt1 - 3].u;
        F[f_count].vt3.v = VT[vt1 - 3].v;

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
  return f_count;

  // This could be usefull for debug
  for (int i = 0; i <= f_count; i++) {
    printf("F-%d in '%s' using '%s'\n", i, F[i].obj->name, F[i].m->name);
    printf(" - Vertex (%.3f, %.3f, %3.f) with UV (%.3f, %.3f)\n",
      F[i] .v1.x,
      F[i] .v1.y,
      F[i] .v1.z,
      F[i].vt1.u,
      F[i].vt1.v
    );
    printf(" - Vertex (%.3f, %.3f, %3.f) with UV (%.3f, %.3f)\n",
      F[i] .v2.x,
      F[i] .v2.y,
      F[i] .v2.z,
      F[i].vt2.u,
      F[i].vt2.v
    );
    printf(" - Vertex (%.3f, %.3f, %3.f) with UV (%.3f, %.3f)\n",
      F[i] .v3.x,
      F[i] .v3.y,
      F[i] .v3.z,
      F[i].vt3.u,
      F[i].vt3.v
    );

  }
  return f_count;

}