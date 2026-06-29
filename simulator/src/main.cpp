#include "fallguard/core.hpp"

#include <cctype>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

void draw_rect(fallguard::ImageFrame& frame,
               std::uint16_t x,
               std::uint16_t y,
               std::uint16_t width,
               std::uint16_t height,
               std::uint8_t value)
{
    for (std::uint16_t py = y; py < y + height && py < frame.height(); ++py) {
        for (std::uint16_t px = x; px < x + width && px < frame.width(); ++px) {
            frame.set(px, py, value);
        }
    }
}

fallguard::ImageFrame synthetic_image(const fallguard::FallGuardConfig& config,
                                      std::uint32_t timestamp_ms)
{
    fallguard::ImageFrame frame(config.frame_width, config.frame_height, 10);

    if (timestamp_ms < 2000) {
        draw_rect(frame, 42, 24, 12, 44, 220);
    } else {
        draw_rect(frame, 24, 62, 50, 12, 220);
    }

    return frame;
}

class SyntheticFrameSource final : public fallguard::CameraFrameSource {
public:
    explicit SyntheticFrameSource(fallguard::FallGuardConfig config)
        : config_(config)
    {
    }

    bool read(fallguard::CapturedFrame& frame) override
    {
        if (next_timestamp_ms_ > end_timestamp_ms_) {
            return false;
        }

        frame.image = synthetic_image(config_, next_timestamp_ms_);
        frame.timestamp_ms = next_timestamp_ms_;
        next_timestamp_ms_ += frame_interval_ms_;
        return true;
    }

    std::uint16_t width() const override
    {
        return config_.frame_width;
    }

    std::uint16_t height() const override
    {
        return config_.frame_height;
    }

private:
    fallguard::FallGuardConfig config_{};
    std::uint32_t next_timestamp_ms_{0};
    static constexpr std::uint32_t frame_interval_ms_ = 1000;
    static constexpr std::uint32_t end_timestamp_ms_ = 6000;
};

std::string read_pgm_token(std::istream& input)
{
    std::string token;
    char ch = '\0';

    while (input.get(ch)) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            continue;
        }

        if (ch == '#') {
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        token.push_back(ch);
        break;
    }

    while (input.get(ch)) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            break;
        }

        if (ch == '#') {
            input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }

        token.push_back(ch);
    }

    if (token.empty()) {
        throw std::runtime_error("unexpected end of PGM file");
    }

    return token;
}

std::uint16_t parse_dimension(const std::string& token, const char* name)
{
    const auto value = std::stoul(token);
    if (value == 0 || value > 65535) {
        throw std::runtime_error(std::string("invalid PGM ") + name);
    }

    return static_cast<std::uint16_t>(value);
}

fallguard::ImageFrame load_pgm(const std::string& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("failed to open image file: " + path);
    }

    const auto magic = read_pgm_token(input);
    if (magic != "P2" && magic != "P5") {
        throw std::runtime_error("unsupported image format in " + path + " (expected P2 or P5 PGM)");
    }

    const auto width = parse_dimension(read_pgm_token(input), "width");
    const auto height = parse_dimension(read_pgm_token(input), "height");
    const auto max_value = std::stoul(read_pgm_token(input));
    if (max_value == 0 || max_value > 255) {
        throw std::runtime_error("only 8-bit PGM images are supported: " + path);
    }

    fallguard::ImageFrame frame(width, height, 0);
    for (std::uint16_t y = 0; y < height; ++y) {
        for (std::uint16_t x = 0; x < width; ++x) {
            std::uint8_t value = 0;
            if (magic == "P2") {
                value = static_cast<std::uint8_t>(std::stoul(read_pgm_token(input)) * 255 / max_value);
            } else {
                char byte = '\0';
                if (!input.get(byte)) {
                    throw std::runtime_error("truncated PGM image: " + path);
                }
                value = static_cast<std::uint8_t>(
                    static_cast<unsigned char>(byte) * 255U / max_value);
            }

            frame.set(x, y, value);
        }
    }

    return frame;
}

class PgmSequenceFrameSource final : public fallguard::CameraFrameSource {
public:
    explicit PgmSequenceFrameSource(std::vector<std::string> paths)
        : paths_(std::move(paths))
    {
        if (paths_.empty()) {
            throw std::runtime_error("no input image files were provided");
        }

        pending_image_ = load_pgm(paths_.front());
        width_ = pending_image_.width();
        height_ = pending_image_.height();
        next_path_index_ = 1;
        has_pending_image_ = true;
    }

    bool read(fallguard::CapturedFrame& frame) override
    {
        if (!has_pending_image_) {
            if (next_path_index_ >= paths_.size()) {
                return false;
            }

            pending_image_ = load_pgm(paths_[next_path_index_]);
            ++next_path_index_;

            if (pending_image_.width() != width_ || pending_image_.height() != height_) {
                throw std::runtime_error("all input images must have the same dimensions");
            }

            has_pending_image_ = true;
        }

        frame.image = pending_image_;
        frame.timestamp_ms = next_timestamp_ms_;
        next_timestamp_ms_ += frame_interval_ms_;
        has_pending_image_ = false;
        return true;
    }

    std::uint16_t width() const override
    {
        return width_;
    }

    std::uint16_t height() const override
    {
        return height_;
    }

private:
    std::vector<std::string> paths_{};
    fallguard::ImageFrame pending_image_{};
    std::size_t next_path_index_{0};
    std::uint16_t width_{0};
    std::uint16_t height_{0};
    std::uint32_t next_timestamp_ms_{0};
    bool has_pending_image_{false};
    static constexpr std::uint32_t frame_interval_ms_ = 1000;
};

std::unique_ptr<fallguard::CameraFrameSource> make_frame_source(
    int argc,
    char* argv[],
    const fallguard::FallGuardConfig& config)
{
    if (argc <= 1) {
        return std::make_unique<SyntheticFrameSource>(config);
    }

    std::vector<std::string> paths;
    for (int i = 1; i < argc; ++i) {
        paths.emplace_back(argv[i]);
    }

    return std::make_unique<PgmSequenceFrameSource>(std::move(paths));
}

} // namespace

int main(int argc, char* argv[])
{
    try {
        fallguard::FallGuardConfig config{};
        config.frame_width = 96;
        config.frame_height = 96;
        config.fall_confirmation_ms = 3000;

        auto source = make_frame_source(argc, argv, config);
        config.frame_width = source->width();
        config.frame_height = source->height();

        fallguard::Logger logger(std::cout);
        fallguard::VisionEngine vision(config);
        fallguard::TinyMlEngine classifier(config);
        fallguard::DecisionEngine decision(config);
        fallguard::AlertManager alerts;

        logger.log(fallguard::LogLevel::Info, "FallGuard Edge simulator started");
        std::cout << "time_ms,pose,confidence,state,alert,width,height\n";

        fallguard::CapturedFrame frame;
        while (source->read(frame)) {
            auto silhouette = vision.extract_silhouette(frame.image);
            auto result = classifier.classify(silhouette);
            auto state = decision.update(result, frame.timestamp_ms);
            alerts.update(state);

            std::cout << frame.timestamp_ms << ','
                      << fallguard::to_string(result.pose) << ','
                      << std::fixed << std::setprecision(2) << result.confidence << ','
                      << fallguard::to_string(state) << ','
                      << (alerts.is_active() ? "on" : "off") << ','
                      << frame.image.width() << ','
                      << frame.image.height() << '\n';
        }
    } catch (const std::exception& error) {
        std::cerr << "fallguard_simulator: " << error.what() << '\n';
        return 2;
    }

    return 0;
}
