#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#include <string>
#include <vector>

constexpr int tileSize = 64;

struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

struct Coords4 {
    float x;
    float y;
    float z;
    float u;
};

constexpr Pixel backgroundColor{0, 0, 0, 0};
constexpr Pixel brown {218, 165, 32, 255};

const float pi = 3.14159265358;

struct Image {
  explicit Image(const std::string &a_path);
  Image(int a_width, int a_height, int a_channels);

  int Save(const std::string &a_path);

  int Width()    const { return width; }
  int Height()   const { return height; }
  int Channels() const { return channels; }
  size_t Size()  const { return size; }
  Pixel* Data()        { return data; }

  Pixel GetPixel(int x, int y) { return data[width * y + x];}
  void  PutPixel(int x, int y, const Pixel &pix) { data[width * y + x] = pix; }

  ~Image();

private:
  int width = -1;
  int height = -1;
  int channels = 3;
  size_t size = 0;
  Pixel *data = nullptr;
  bool self_allocated = false;
};

static Pixel Blend(Pixel old_pixel, Pixel new_pixel) {
    new_pixel.r = new_pixel.a / 255.0 * (new_pixel.r - old_pixel.r) + old_pixel.r;
    new_pixel.g = new_pixel.a / 255.0 * (new_pixel.g - old_pixel.g) + old_pixel.g;
    new_pixel.b = new_pixel.a / 255.0 * (new_pixel.b - old_pixel.b) + old_pixel.b;
    new_pixel.a = 255;
    return new_pixel;
}

static Coords4 MatrixVectorMult(Coords4 v, std::vector<float> m);

static std::vector<float> ConstructRotationMatrix(float phi, int axis);

struct Triangle {
    Triangle(struct Coords4 a1, struct Coords4 a2, struct Coords4 a3, struct Pixel c) :
            apex1(a1), apex2(a2), apex3(a3), color(c) {}
    
    Coords4 Apex1() const { return apex1; }
    Coords4 Apex2() const { return apex2; }
    Coords4 Apex3() const { return apex3; }
    
    void Draw(Image &screen);
    void Rotate(float phi_x, float phi_y, float phi_z);
    
private:
    Coords4 apex1 = {0.0, 0.0, 0.0, 1.0};
    Coords4 apex2 = {0.0, 0.0, 1.0, 1.0};
    Coords4 apex3 = {0.0, 1.0, 0.0, 1.0};
    Pixel color = {0, 0, 0, 0};
};

struct Icosahedron {
    Icosahedron(Coords4 c, float scale, float phi_x, float phi_y, float phi_z);
    
    void Draw(Image &screen);
    
private:
    Coords4 center = {0.0, 0.0, 0.0, 1.0};
    std::vector<Coords4> apexes;
    std::vector<Triangle> triangles;
};

#endif //MAIN_IMAGE_H
