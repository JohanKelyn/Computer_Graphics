#include "texture.h"

Solid_Color::Solid_Color(const Color& albedo) : albedo(albedo) {}

Solid_Color::Solid_Color(double red, double green, double blue) : Solid_Color(Color(red,green,blue)) {}

Color Solid_Color::value(double u, double v, const Point3& p) const {
        return albedo;
}



Checker_Texture::Checker_Texture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
: inv_scale(1.0 / scale), even(even), odd(odd) {}

Checker_Texture::Checker_Texture(double scale, const Color& c1, const Color& c2)
: Checker_Texture(scale, make_shared<Solid_Color>(c1), make_shared<Solid_Color>(c2)) {}

Color Checker_Texture::value(double u, double v, const Point3& p) const {
    auto xInteger = int(std::floor(inv_scale * p.x()));
    auto yInteger = int(std::floor(inv_scale * p.y()));
    auto zInteger = int(std::floor(inv_scale * p.z()));

    bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

    return isEven ? even->value(u, v, p) : odd->value(u, v, p);
}



Image_Texture::Image_Texture(const char* filename) : image(filename) {}

Color Image_Texture::value(double u, double v, const Point3& p) const {
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (image.height() <= 0) return Color(0,1,1);

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = Interval(0,1).clamp(u);
    v = 1.0 - Interval(0,1).clamp(v);  // Flip V to image coordinates

    auto i = int(u * image.width());
    auto j = int(v * image.height());
    auto pixel = image.pixel_data(i,j);

    auto color_scale = 1.0 / 255.0;
    return Color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
}




Noise_Texture::Noise_Texture(double scale) : scale(scale) {}

Color Noise_Texture::value(double u, double v, const Point3& p) const  {
    return Color(.5, .5, .5) * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
}

