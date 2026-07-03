#include "fallguard/image/connected_components.hpp"

#include <algorithm>
#include <limits>
#include <vector>

namespace fallguard {

namespace {

struct Point {
    std::uint16_t x;
    std::uint16_t y;
};

// Pushes unvisited foreground neighbors of (x, y) onto stack and marks them
// visited immediately (not when popped), so a pixel can never be enqueued
// twice.
void push_neighbors(const SilhouetteFrame& silhouette,
                    std::vector<std::uint8_t>& visited,
                    Connectivity connectivity,
                    std::uint16_t x,
                    std::uint16_t y,
                    std::vector<Point>& stack)
{
    const std::uint16_t width = silhouette.width();
    const std::uint16_t height = silhouette.height();

    // 4-connected neighbors.
    const int dx4[] = {-1, 1, 0, 0};
    const int dy4[] = {0, 0, -1, 1};
    // Diagonal neighbors, added on top of the 4-connected set for 8-connectivity.
    const int dxd[] = {-1, -1, 1, 1};
    const int dyd[] = {-1, 1, -1, 1};

    auto try_push = [&](int nx, int ny) {
        if (nx < 0 || ny < 0 || nx >= static_cast<int>(width) || ny >= static_cast<int>(height)) {
            return;
        }
        const auto ux = static_cast<std::uint16_t>(nx);
        const auto uy = static_cast<std::uint16_t>(ny);
        const auto index = static_cast<std::size_t>(uy) * width + ux;
        if (visited[index] || !silhouette.at(ux, uy)) {
            return;
        }
        visited[index] = 1;
        stack.push_back({ux, uy});
    };

    for (int i = 0; i < 4; ++i) {
        try_push(static_cast<int>(x) + dx4[i], static_cast<int>(y) + dy4[i]);
    }

    if (connectivity == Connectivity::Eight) {
        for (int i = 0; i < 4; ++i) {
            try_push(static_cast<int>(x) + dxd[i], static_cast<int>(y) + dyd[i]);
        }
    }
}

} // namespace

std::vector<ConnectedComponent> find_connected_components(
    const SilhouetteFrame& silhouette,
    Connectivity connectivity,
    std::vector<std::int32_t>* labels_out)
{
    const std::uint16_t width = silhouette.width();
    const std::uint16_t height = silhouette.height();
    const std::size_t pixel_count = static_cast<std::size_t>(width) * height;

    std::vector<std::uint8_t> visited(pixel_count, 0);
    std::vector<std::int32_t> labels(pixel_count, -1);
    std::vector<ConnectedComponent> components;

    std::vector<Point> stack;
    stack.reserve(pixel_count);

    for (std::uint16_t y = 0; y < height; ++y) {
        for (std::uint16_t x = 0; x < width; ++x) {
            const auto seed_index = static_cast<std::size_t>(y) * width + x;
            if (visited[seed_index] || !silhouette.at(x, y)) {
                continue;
            }

            const auto component_index = static_cast<std::int32_t>(components.size());
            ConnectedComponent component;

            auto min_x = std::numeric_limits<std::uint16_t>::max();
            auto min_y = std::numeric_limits<std::uint16_t>::max();
            std::uint16_t max_x = 0;
            std::uint16_t max_y = 0;

            stack.clear();
            stack.push_back({x, y});
            visited[seed_index] = 1;

            while (!stack.empty()) {
                const Point p = stack.back();
                stack.pop_back();

                const auto index = static_cast<std::size_t>(p.y) * width + p.x;
                labels[index] = component_index;
                ++component.pixel_count;
                min_x = std::min(min_x, p.x);
                min_y = std::min(min_y, p.y);
                max_x = std::max(max_x, p.x);
                max_y = std::max(max_y, p.y);

                push_neighbors(silhouette, visited, connectivity, p.x, p.y, stack);
            }

            component.bounds.x = min_x;
            component.bounds.y = min_y;
            component.bounds.width = static_cast<std::uint16_t>(max_x - min_x + 1);
            component.bounds.height = static_cast<std::uint16_t>(max_y - min_y + 1);
            components.push_back(component);
        }
    }

    if (labels_out != nullptr) {
        *labels_out = std::move(labels);
    }

    return components;
}

const ConnectedComponent* largest_component(const std::vector<ConnectedComponent>& components)
{
    if (components.empty()) {
        return nullptr;
    }

    return &*std::max_element(
        components.begin(),
        components.end(),
        [](const ConnectedComponent& a, const ConnectedComponent& b) {
            return a.pixel_count < b.pixel_count;
        });
}

} // namespace fallguard
