#include "fallguard/image/morphology.hpp"

namespace fallguard {

namespace {

SilhouetteFrame erode_once(const SilhouetteFrame& input)
{
    SilhouetteFrame output(input.width(), input.height());

    for (std::uint16_t y = 0; y < input.height(); ++y) {
        for (std::uint16_t x = 0; x < input.width(); ++x) {
            if (!input.at(x, y)) {
                continue;
            }

            const bool has_left = x == 0 || input.at(static_cast<std::uint16_t>(x - 1), y);
            const bool has_right = x + 1 >= input.width() || input.at(static_cast<std::uint16_t>(x + 1), y);
            const bool has_up = y == 0 || input.at(x, static_cast<std::uint16_t>(y - 1));
            const bool has_down = y + 1 >= input.height() || input.at(x, static_cast<std::uint16_t>(y + 1));

            output.set(x, y, has_left && has_right && has_up && has_down);
        }
    }

    return output;
}

SilhouetteFrame dilate_once(const SilhouetteFrame& input)
{
    SilhouetteFrame output(input.width(), input.height());

    for (std::uint16_t y = 0; y < input.height(); ++y) {
        for (std::uint16_t x = 0; x < input.width(); ++x) {
            if (input.at(x, y)) {
                output.set(x, y, true);
                continue;
            }

            const bool left = x > 0 && input.at(static_cast<std::uint16_t>(x - 1), y);
            const bool right = x + 1 < input.width() && input.at(static_cast<std::uint16_t>(x + 1), y);
            const bool up = y > 0 && input.at(x, static_cast<std::uint16_t>(y - 1));
            const bool down = y + 1 < input.height() && input.at(x, static_cast<std::uint16_t>(y + 1));

            output.set(x, y, left || right || up || down);
        }
    }

    return output;
}

} // namespace

SilhouetteFrame erode(const SilhouetteFrame& input, std::uint8_t iterations)
{
    SilhouetteFrame result = input;
    for (std::uint8_t i = 0; i < iterations; ++i) {
        result = erode_once(result);
    }
    return result;
}

SilhouetteFrame dilate(const SilhouetteFrame& input, std::uint8_t iterations)
{
    SilhouetteFrame result = input;
    for (std::uint8_t i = 0; i < iterations; ++i) {
        result = dilate_once(result);
    }
    return result;
}

SilhouetteFrame morphological_open(const SilhouetteFrame& input, std::uint8_t iterations)
{
    return dilate(erode(input, iterations), iterations);
}

SilhouetteFrame morphological_close(const SilhouetteFrame& input, std::uint8_t iterations)
{
    return erode(dilate(input, iterations), iterations);
}

} // namespace fallguard
