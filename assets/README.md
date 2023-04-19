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
