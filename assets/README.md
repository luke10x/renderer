In this file I will try to document where I got the assets

Textures-16.png - https://piiixl.itch.io/textures

I am planning using fonts from here:
fonts: http://www.04.jp.org/

From png to ppm:

    mkdir -p assets/png && mogrify -path assets/ppm -format ppm assets/png/*.png

  
# Prepare OBJ files and textures

While exporting to OBJ use the following settings:

    Forward Axis:          Y
    Up Axis:               Z
    Geometry:
        UV Coordinates:    Checked
        Triangulated Mesh: Checked
    Materials:             Export


  for (int i = 0; i < tCount; i++) {
      printf("Triangle %d:\n", i + 1);
      printf("  Vertex 1: (%f, %f, %f)\n", (*triangles)[i].x1, (*triangles)[i].y1, (*triangles)[i].z1);
      printf("  Vertex 2: (%f, %f, %f)\n", (*triangles)[i].x2, (*triangles)[i].y2, (*triangles)[i].z2);
      printf("  Vertex 3: (%f, %f, %f)\n", (*triangles)[i].x3, (*triangles)[i].y3, (*triangles)[i].z3);
      printf("  Texture Coordinates:\n");
      printf("    Vertex 1: (%f, %f)\n",   (*triangles)[i].u1, (*triangles)[i].v1);
      printf("    Vertex 2: (%f, %f)\n",   (*triangles)[i].u2, (*triangles)[i].v2);
      printf("    Vertex 3: (%f, %f)\n",   (*triangles)[i].u3, (*triangles)[i].v3);
      printf("  Material: %d (%s). \n", triMats[i], materialNames[triMats[i]]);
  }