#ifndef CAMERA_H
#define CAMERA_H

#include "global.h"

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "Walnut/Image.h"
#include <iostream>

class camera {
public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width = 100;  // Rendered image width in pixel count
    int    max_depth = 10;   // Maximum number of ray bounces into scene
    double vfov = 90;  // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0);   // Point camera is looking from
    point3 lookat = point3(0, 0, -1);  // Point camera is looking at
    vec3   vup = vec3(0, 1, 0);     // Camera-relative "up" direction
    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus
    int    samples_per_pixel = 10;   // Count of random samples for each pixel




    //void render(const hittable& world) {
    //    initialize();

    //    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    //    for (int j = 0; j < image_height; ++j) {
    //        std::clog << "Scanlines remaining: " << (image_height - j) << "\n";
    //        for (int i = 0; i < image_width; ++i) {

    //            color pixel_color(0, 0, 0);

    //            for (int sample = 0; sample < samples_per_pixel; sample++) {
    //                // auto offset = sample_square();
    //                auto offset = vec3(0, 0, 0);
    //                auto pixel_center = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

    //                auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    //                auto ray_direction = pixel_center - ray_origin;
    //                ray r(ray_origin, ray_direction);

    //                pixel_color += ray_color(r, max_depth, world);
    //            }


    //            write_color(std::cout, pixel_color / samples_per_pixel);
    //        }
    //    }

    //    std::clog << "Done.                 \n";
    //}
    void render(const hittable& world) {
        initialize();
        for (int j = 0; j < image_height; ++j) {
            std::clog << "Scanlines remaining: " << (image_height - j) << "\n";
            for (int i = 0; i < image_width; ++i) {

                color pixel_color(0, 0, 0);

                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    // auto offset = sample_square();
                    auto offset = vec3(0, 0, 0);
                    auto pixel_center = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

                    auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
                    auto ray_direction = pixel_center - ray_origin;
                    ray r(ray_origin, ray_direction);

                    pixel_color += ray_color(r, max_depth, world);
                }

                // Let ray_color handle clamping, gamma correction, and conversion to int
                pixel_color /= samples_per_pixel;
                // Pack the RGBA value into a uint32_t
                m_ImageData[j * image_width + i] = compute_color_rgba(pixel_color);
            }
        }

        std::clog << "Done.                 \n";
        for (int i = 0;i < 10;i++)std::clog << m_ImageData[i];
        m_Image->SetData(m_ImageData);
    }

    void onResize(uint32_t width, uint32_t height) {
        if (m_Image) {
			if (width == m_Image->GetWidth() && height == m_Image->GetHeight()) return;
			m_Image->Resize(width, height);
        }
        else {
			m_Image = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
        }

		image_width = width;
        image_height = height;
        delete[] m_ImageData;
        m_ImageData = new uint32_t[width * height];
    }

    std::shared_ptr<Walnut::Image> getImage()const { return m_Image; }
private:
    int    image_height;   // Rendered image height
    point3 center;         // Camera center
    point3 pixel00_loc;    // Location of pixel 0, 0
    vec3   pixel_delta_u;  // Offset to pixel to the right
    vec3   pixel_delta_v;  // Offset to pixel below
    vec3   u, v, w;              // Camera frame basis vectors
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius

	std::shared_ptr<Walnut::Image> m_Image;
    uint32_t* m_ImageData = nullptr;
    void initialize() {
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        center = lookfrom;

        // Determine viewport dimensions.

        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left =
            center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        hit_record rec;

        if (depth == 0) {
            return color(0, 0, 0);
        }

        if (world.hit(r, interval(0.000001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                return attenuation * ray_color(scattered, depth - 1, world);
            }
            else {
                return color(0, 0, 0);
            }
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif