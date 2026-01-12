#include"material.h"

Lambertian::Lambertian(const Color& albedo) : tex(make_shared<Solid_Color>(albedo)) {}

Lambertian::Lambertian(shared_ptr<Texture> tex) : tex(tex) {}

bool Lambertian::scatter(const Ray &r_in, const Hit_Record &rec, Color &attenuation, Ray &scattered) const
{
    vec3 scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (near_zero(scatter_direction)) scatter_direction = rec.normal;
    scattered = Ray(rec.p, scatter_direction, r_in.time());
    attenuation = tex->value(rec.u, rec.v, rec.p);
    return true;
}


//-------------------------------------------------

Metal::Metal(const Color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

bool Metal::scatter(const Ray& r_in, const Hit_Record& rec, Color& attenuation, Ray& scattered) const {
    vec3 reflected = reflect(r_in.get_direction(), rec.normal);
    reflected = reflected.normalized() + (fuzz * random_unit_vector());
    scattered = Ray(rec.p, reflected, r_in.time());
    attenuation = albedo;
    return (scattered.get_direction().dot(rec.normal) > 0);;
}

//-------------------------------------------------

Dielectric::Dielectric(double refraction_index) : refraction_index(refraction_index) {}

bool Dielectric::scatter(const Ray& r_in, const Hit_Record& rec, Color& attenuation, Ray& scattered) const {
    attenuation = Color(1.0, 1.0, 1.0);
    double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

    vec3 unit_direction = r_in.get_direction().normalized();
    double cos_theta = std::fmin((-unit_direction).dot(rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ri) > random_double())
        direction = reflect(unit_direction, rec.normal);
    else
        direction = refract(unit_direction, rec.normal, ri);

    scattered = Ray(rec.p, direction, r_in.time());
    return true;
}

double Dielectric::reflectance(double cosine, double refraction_index) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0*r0;
    return r0 + (1-r0)*std::pow((1 - cosine),5);
}