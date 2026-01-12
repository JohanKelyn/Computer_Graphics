#pragma once
#include "interval.h"
#include "rtw_stb_image.h"
#include "perlin.h"

class Texture {
  public:
    virtual ~Texture() = default;

    virtual Color value(double u, double v, const Point3& p) const = 0;
};

class Solid_Color : public Texture {
  public:
    Solid_Color(const Color& albedo);

    Solid_Color(double red, double green, double blue);

    Color value(double u, double v, const Point3& p) const override;

  private:
    Color albedo;
};

class Checker_Texture : public Texture {
  public:
    Checker_Texture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd);

    Checker_Texture(double scale, const Color& c1, const Color& c2);

    Color value(double u, double v, const Point3& p) const override;

  private:
    double inv_scale;
    shared_ptr<Texture> even;
    shared_ptr<Texture> odd;
};

class Image_Texture : public Texture {
  public:
    Image_Texture(const char* filename);

    Color value(double u, double v, const Point3& p) const override;

  private:
    RTW_Image image;
};


class Noise_Texture : public Texture {
  public:
    Noise_Texture(double scale);

    Color value(double u, double v, const Point3& p) const override;

  private:
    Perlin noise;
    double scale;
};