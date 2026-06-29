#include "fallguard/frame.hpp"

namespace fallguard {

namespace {

std::size_t index_for(std::uint16_t width, std::uint16_t height,
                      std::uint16_t x, std::uint16_t y)
{
    if (x >= width || y >= height) {
        throw std::out_of_range("frame coordinate is outside frame bounds");
    }

    return static_cast<std::size_t>(y) * width + x;
}

} // namespace

ImageFrame::ImageFrame(std::uint16_t width, std::uint16_t height, std::uint8_t fill)
    : width_(width)
    , height_(height)
    , pixels_(static_cast<std::size_t>(width) * height, fill)
{
}

std::uint16_t ImageFrame::width() const
{
    return width_;
}

std::uint16_t ImageFrame::height() const
{
    return height_;
}

bool ImageFrame::empty() const
{
    return pixels_.empty();
}

std::uint8_t ImageFrame::at(std::uint16_t x, std::uint16_t y) const
{
    return pixels_.at(index_for(width_, height_, x, y));
}

void ImageFrame::set(std::uint16_t x, std::uint16_t y, std::uint8_t value)
{
    pixels_.at(index_for(width_, height_, x, y)) = value;
}

SilhouetteFrame::SilhouetteFrame(std::uint16_t width, std::uint16_t height)
    : width_(width)
    , height_(height)
    , pixels_(static_cast<std::size_t>(width) * height, 0)
{
}

std::uint16_t SilhouetteFrame::width() const
{
    return width_;
}

std::uint16_t SilhouetteFrame::height() const
{
    return height_;
}

bool SilhouetteFrame::at(std::uint16_t x, std::uint16_t y) const
{
    return pixels_.at(index_for(width_, height_, x, y)) != 0;
}

void SilhouetteFrame::set(std::uint16_t x, std::uint16_t y, bool foreground)
{
    pixels_.at(index_for(width_, height_, x, y)) = foreground ? 1 : 0;
}

} // namespace fallguard
