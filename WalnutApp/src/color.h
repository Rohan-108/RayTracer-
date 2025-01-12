#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

uint32_t compute_color_rgba(const color& pixel_color) {
    // Helper function to apply gamma correction
    auto linear_to_gamma = [](double linear_component) {
        return sqrt(linear_component);
        };

    // Clamp values to the interval [0.0, 0.999]
    static const interval intensity(0.000, 0.999);

    // Apply gamma correction and convert to [0,255]
    uint8_t r = static_cast<uint8_t>(256 * intensity.clamp(linear_to_gamma(pixel_color.x())));
    uint8_t g = static_cast<uint8_t>(256 * intensity.clamp(linear_to_gamma(pixel_color.y())));
    uint8_t b = static_cast<uint8_t>(256 * intensity.clamp(linear_to_gamma(pixel_color.z())));
    uint8_t a = 255; // Fully opaque

    // Pack into 0xff0000ff RGBA format
    return (a << 24) | (b << 16) | (g << 8) | r;
}

#endif