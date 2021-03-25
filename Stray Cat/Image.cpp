#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <cmath>

Image::Image(const std::string &a_path)
{
  if((data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, 4)) != nullptr)
  {
    size = width * height * channels;
  }
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height ];

  if(data != nullptr)
  {
    width = a_width;
    height = a_height;
    size = a_width * a_height * a_channels;
    channels = a_channels;
    self_allocated = true;
  }
}


int Image::Save(const std::string &a_path)
{
  auto extPos = a_path.find_last_of('.');
  if(a_path.substr(extPos, std::string::npos) == ".png" || a_path.substr(extPos, std::string::npos) == ".PNG")
  {
    stbi_write_png(a_path.c_str(), width, height, channels, data, width * channels);
  }
  else if(a_path.substr(extPos, std::string::npos) == ".jpg" || a_path.substr(extPos, std::string::npos) == ".JPG" ||
          a_path.substr(extPos, std::string::npos) == ".jpeg" || a_path.substr(extPos, std::string::npos) == ".JPEG")
  {
    stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
  }
  else
  {
    std::cerr << "Unknown file extension: " << a_path.substr(extPos, std::string::npos) << "in file name" << a_path << "\n";
    return 1;
  }

  return 0;
}

Image::~Image()
{
  if(self_allocated)
    delete [] data;
  else
  {
    stbi_image_free(data);
  }
}

static Coords4 MatrixVectorMult(Coords4 v, std::vector<float> m) {
    Coords4 res = {0.0, 0.0, 0.0, 1.0};
    res.x += m[0] * v.x + m[1] * v.y + m[2] * v.z + m[3] * v.u;
    res.y += m[4] * v.x + m[5] * v.y + m[6] * v.z + m[7] * v.u;
    res.z += m[8] * v.x + m[9] * v.y + m[10] * v.z + m[11] * v.u;
    res.u += m[12] * v.x + m[13] * v.y + m[14] * v.z + m[15] * v.u;
    return res;
}

static std::vector<float> ConstructRotationMatrix(float phi, int axis) {
    std::vector<float> res;
    for (int i = 0; i < 16; i++) {
        if (i % 4 != i / 4) {
            res.push_back(0.0);
        } else {
            res.push_back(1.0);
        }
    }
    if (axis == 0) {
        res[5] = cos(phi * pi / 180);
        res[6] = -sin(phi * pi / 180);
        res[9] = sin(phi * pi / 180);
        res[10] = cos(phi * pi / 180);
    } else if (axis == 1) {
        res[0] = cos(phi * pi / 180);
        res[2] = sin(phi * pi / 180);
        res[8] = -sin(phi * pi / 180);
        res[10] = cos(phi * pi / 180);
    } else {
        res[0] = cos(phi * pi / 180);
        res[1] = -sin(phi * pi / 180);
        res[4] = sin(phi * pi / 180);
        res[5] = cos(phi * pi / 180);
    }
    return res;
}

void Triangle::Draw (Image &screen) {
    Coords4 right_apex;
    Coords4 left_apex;
    Coords4 down_apex;
    Coords4 upper_apex;
    if ((apex1.x >= apex2.x) && (apex1.x >= apex3.x)) {
        right_apex = apex1;
    } else if ((apex2.x >= apex1.x) && (apex2.x >= apex3.x)) {
        right_apex = apex2;
    } else {
        right_apex = apex3;
    }
    if ((apex1.x <= apex2.x) && (apex1.x <= apex3.x)) {
        left_apex = apex1;
    } else if ((apex2.x <= apex1.x) && (apex2.x <= apex3.x)) {
        left_apex = apex2;
    } else {
        left_apex = apex3;
    }
    if ((apex1.y >= apex2.y) && (apex1.y >= apex3.y)) {
        upper_apex = apex1;
    } else if ((apex2.y >= apex1.y) && (apex2.y >= apex3.y)) {
        upper_apex = apex2;
    } else {
        upper_apex = apex3;
    }
    if ((apex1.y <= apex2.y) && (apex1.y <= apex3.y)) {
        down_apex = apex1;
    } else if ((apex2.y <= apex1.y) && (apex2.y <= apex3.y)) {
        down_apex = apex2;
    } else {
        down_apex = apex3;
    }
    
    float cond1;
    float cond2;
    float cond3;

    for (int j = round(down_apex.y); j < round(upper_apex.y); j++) {
        for (int i = round(left_apex.x); i < round(right_apex.x); i++) {
            cond1 = (apex1.x - ((float)i + 0.5)) * (apex2.y - apex1.y) - (apex2.x - apex1.x) * (apex1.y - ((float)j + 0.5));
            cond2 = (apex2.x - ((float)i + 0.5)) * (apex3.y - apex2.y) - (apex3.x - apex2.x) * (apex2.y - ((float)j + 0.5));
            cond3 = (apex3.x - ((float)i + 0.5)) * (apex1.y - apex3.y) - (apex1.x - apex3.x) * (apex3.y - ((float)j + 0.5));
            if (((cond1 > 0) && (cond2 > 0) && (cond3 > 0)) ||
                ((cond1 < 0) && (cond2 < 0) && (cond3 < 0))) {
                if ((i >= 0) && (i < screen.Width()) && (j >= 0) && (j < screen.Height())) {
                    screen.PutPixel(i, j, Blend(screen.GetPixel(i, j), color));
                }
            }
        }
    }
}

void Triangle::Rotate(float phi_x, float phi_y, float phi_z) {
    apex1 = MatrixVectorMult(apex1, ConstructRotationMatrix(phi_x, 0));
    apex1 = MatrixVectorMult(apex1, ConstructRotationMatrix(phi_y, 1));
    apex1 = MatrixVectorMult(apex1, ConstructRotationMatrix(phi_z, 2));
    
    apex2 = MatrixVectorMult(apex2, ConstructRotationMatrix(phi_x, 0));
    apex2 = MatrixVectorMult(apex2, ConstructRotationMatrix(phi_y, 1));
    apex2 = MatrixVectorMult(apex2, ConstructRotationMatrix(phi_z, 2));
    
    apex3 = MatrixVectorMult(apex3, ConstructRotationMatrix(phi_x, 0));
    apex3 = MatrixVectorMult(apex3, ConstructRotationMatrix(phi_y, 1));
    apex3 = MatrixVectorMult(apex3, ConstructRotationMatrix(phi_z, 2));
}

Icosahedron::Icosahedron(Coords4 c, float scale, float phi_x, float phi_y, float phi_z) {
    center = c;
    
    float phi = (1 + sqrt(5)) / 2;
    apexes.push_back((Coords4){ phi,   1.0,  0.0, 1.0});
    apexes.push_back((Coords4){ phi,  -1.0,  0.0, 1.0});
    apexes.push_back((Coords4){-phi,  -1.0,  0.0, 1.0});
    apexes.push_back((Coords4){-phi,   1.0,  0.0, 1.0});
    apexes.push_back((Coords4){ 0.0,   phi,  1.0, 1.0});
    apexes.push_back((Coords4){ 0.0,  -phi,  1.0, 1.0});
    apexes.push_back((Coords4){ 0.0,  -phi, -1.0, 1.0});
    apexes.push_back((Coords4){ 0.0,   phi, -1.0, 1.0});
    apexes.push_back((Coords4){ 1.0,   0.0,  phi, 1.0});
    apexes.push_back((Coords4){ 1.0,   0.0, -phi, 1.0});
    apexes.push_back((Coords4){-1.0,   0.0, -phi, 1.0});
    apexes.push_back((Coords4){-1.0,   0.0,  phi, 1.0});
    
    for(int i = 0; i < apexes.size(); i++) {
        // scale
        apexes[i].x *= scale;
        apexes[i].y *= scale;
        apexes[i].z *= scale;
        
        // rotate
        apexes[i] = MatrixVectorMult(apexes[i], ConstructRotationMatrix(phi_x, 0));
        apexes[i] = MatrixVectorMult(apexes[i], ConstructRotationMatrix(phi_y, 1));
        apexes[i] = MatrixVectorMult(apexes[i], ConstructRotationMatrix(phi_z, 2));
        
        // translate
        apexes[i].x += c.x;
        apexes[i].y += c.y;
        apexes[i].z += c.z;
    }
    
    triangles.push_back(Triangle(apexes[7],  apexes[0],  apexes[4],  (Pixel){252, 65,  3,   128})); // red
    triangles.push_back(Triangle(apexes[7],  apexes[4],  apexes[3],  (Pixel){3,   49,  252, 128})); // dark blue
    triangles.push_back(Triangle(apexes[7],  apexes[3],  apexes[10], (Pixel){28,  252, 3,   128})); // green
    triangles.push_back(Triangle(apexes[7],  apexes[10], apexes[9],  (Pixel){165, 3,   252, 128})); // purple
    triangles.push_back(Triangle(apexes[7],  apexes[9],  apexes[0],  (Pixel){240, 252, 3,   128})); // yellow
    triangles.push_back(Triangle(apexes[5],  apexes[8],  apexes[11], (Pixel){3,   223, 252, 128})); // light blue
    triangles.push_back(Triangle(apexes[5],  apexes[11], apexes[2],  (Pixel){250, 160, 184, 128})); // pink
    triangles.push_back(Triangle(apexes[5],  apexes[2],  apexes[6],  (Pixel){255, 174, 0,   128})); // orange
    triangles.push_back(Triangle(apexes[5],  apexes[6],  apexes[1],  (Pixel){12,  133, 36,  128})); // dark green
    triangles.push_back(Triangle(apexes[5],  apexes[1],  apexes[8],  (Pixel){0,   121, 196, 128})); // blue
    triangles.push_back(Triangle(apexes[0],  apexes[8],  apexes[4],  (Pixel){217, 82,  82,  128})); // light red
    triangles.push_back(Triangle(apexes[8],  apexes[4],  apexes[11], (Pixel){125, 74,  7,   128})); // brown
    triangles.push_back(Triangle(apexes[4],  apexes[11], apexes[3],  (Pixel){255, 0,   230, 128})); // magenta
    triangles.push_back(Triangle(apexes[11], apexes[3],  apexes[2],  (Pixel){0,   255, 238, 128})); // turquoise
    triangles.push_back(Triangle(apexes[3],  apexes[2],  apexes[10], (Pixel){204, 159, 82,  128})); // ocher
    triangles.push_back(Triangle(apexes[2],  apexes[10], apexes[6],  (Pixel){187, 224, 99,  128})); // light green
    triangles.push_back(Triangle(apexes[10], apexes[6],  apexes[9],  (Pixel){173, 0,   0,   128})); // dark red
    triangles.push_back(Triangle(apexes[6],  apexes[9],  apexes[1],  (Pixel){118, 120, 13,  128})); // swampy
    triangles.push_back(Triangle(apexes[9],  apexes[1],  apexes[0],  (Pixel){135, 3,   56,  128})); // wine
    triangles.push_back(Triangle(apexes[1],  apexes[0],  apexes[8],  (Pixel){13,  108, 120, 128})); // deep water
}

void Icosahedron::Draw(Image &screen) {
    for (int i = 0; i < triangles.size(); i++) {
        triangles[i].Draw(screen);
    }
}
