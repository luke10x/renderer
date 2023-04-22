In this file I will try to document where I got the assets

Textures-16.png - https://piiixl.itch.io/textures

I am planning using fonts from here:
fonts: http://www.04.jp.org/

  Steeve from: https://sketchfab.com/3d-models/minecraft-steve-low-poly-14eb9e49389d4f3aa60463bf4c34fcd1
  it says it is necessay to attribute but there is no details about the author

# Prepare OBJ files and textures

While exporting to Wavefron (.obj) use the following settings:

    Forward Axis:          Y
    Up Axis:               Z
    Geometry:
        UV Coordinates:    Checked
        Normals:           Checked
        Triangulated Mesh: Checked
    Materials:             Export

Make sure your material names are matching PPM file basenames.

Files can be saved as PNG in the source. And can be later converted:

    mkdir -p assets/png && mogrify -path assets/ppm -format ppm assets/png/*.png

