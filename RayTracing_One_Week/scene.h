#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"

void bouncing_sheperes();
void checkered_spheres();
void earth();
void perlin_spheres();
void quads();

enum class Scene {
    BOUNCING_SPHERES,
    CHECKERED_SPHERES,
    EARTH,
    PERLIN_NOISE,
    QUADS
};

void scene_to_render(Scene scene) {
    switch (scene)
    {
    case Scene::BOUNCING_SPHERES:
        bouncing_sheperes();
        break;
    
    case Scene::CHECKERED_SPHERES:
        checkered_spheres();
        break;
    case Scene::EARTH:
        earth();
        break;
    case Scene::PERLIN_NOISE:
        perlin_spheres();
        break;
    case Scene::QUADS:
        quads();
        break;
    }
}

void bouncing_sheperes() {
    Hittable_List world;

    auto checker = make_shared<Checker_Texture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));
    world.add(make_shared<Sphere>(Point3(0,-1000,0), 1000, make_shared<Lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            Point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - Point3(4, 0.2, 0)).norm() > 0.9) {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random_vector().cwiseProduct(random_vector());
                    sphere_material = make_shared<Lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random_vector(0.5, 1.0);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    world = Hittable_List(make_shared<BVH_Node>(world));

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = Point3(13,2,3);
    cam.lookat   = Point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.Render(world);
}

void checkered_spheres() {
    Hittable_List world;

    auto checker = make_shared<Checker_Texture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));

    world.add(make_shared<Sphere>(Point3(0,-10, 0), 10, make_shared<Lambertian>(checker)));
    world.add(make_shared<Sphere>(Point3(0, 10, 0), 10, make_shared<Lambertian>(checker)));

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = Point3(13,2,3);
    cam.lookat   = Point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.Render(world);
}

void earth() {
    auto earth_texture = make_shared<Image_Texture>("earthmap.jpg");
    auto earth_surface = make_shared<Lambertian>(earth_texture);
    auto globe = make_shared<Sphere>(Point3(0,0,0), 2, earth_surface);

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = Point3(0,0,12);
    cam.lookat   = Point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.Render(Hittable_List(globe));
}

void perlin_spheres() {
    Hittable_List world;

    auto pertext = make_shared<Noise_Texture>(4);
    world.add(make_shared<Sphere>(Point3(0,-1000,0), 1000, make_shared<Lambertian>(pertext)));
    world.add(make_shared<Sphere>(Point3(0,2,0), 2, make_shared<Lambertian>(pertext)));

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = Point3(13,2,3);
    cam.lookat   = Point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.Render(world);
}

void quads() {
    Hittable_List world;

    // Materials
    auto left_red     = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<Quad>(Point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<Quad>(Point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<Quad>(Point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<Quad>(Point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<Quad>(Point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    Camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 80;
    cam.lookfrom = Point3(0,0,9);
    cam.lookat   = Point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.Render(world);
}

