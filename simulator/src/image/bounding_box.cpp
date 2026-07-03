#include "fallguard/image/bounding_box.hpp"

#include <algorithm>

namespace fallguard {

float BoundingBox::aspect_ratio() const
{
    const auto safe_height = std::max<std::uint16_t>(height, 1);
    return static_cast<float>(width) / static_cast<float>(safe_height);
}

std::uint32_t BoundingBox::area() const
{
    return static_cast<std::uint32_t>(width) * static_cast<std::uint32_t>(height);
}

} // namespace fallguard
