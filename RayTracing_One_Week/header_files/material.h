#pragma once
#include "hittable.h"
#include "texture.h"

class Material {
    public:
        virtual ~Material() = default;

        virtual bool scatter(
            const Ray& r_in, const Hit_Record& rec, Color& attenuation, Ray& scattered
        ) const {
            return false;
    }
};

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

class Lambertian : public Material {
  public:
    Lambertian(const Color& albedo);
    Lambertian(shared_ptr<Texture> tex);

    bool scatter(const Ray& r_in, const Hit_Record& rec, Color& attenuation, Ray& scattered) const override;

  private:
    shared_ptr<Texture> tex;
};

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------

class Metal : public Material {
  public:
    Metal(const Color& albedo, double fuzz);

    bool scatter(const Ray& r_in, const Hit_Record& rec, Color& attenuation, Ray& scattered) const override;

  private:
    Color albedo;
    double fuzz;
};

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
class Dielectric : public Material {
  public:
    Dielectric(double refraction_index);

    bool scatter(const Ray& r_in, const Hit_Record& rec, Color& attenuation, Ray& scattered) const override;

  private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;
    static double reflectance(double cosine, double refraction_index);
};
