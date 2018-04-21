#include <utility>

#include "components/PositionComponent.hpp"

namespace obelisk {

PositionComponent::PositionComponent(float x, float y) : position{x, y} {
}

PositionComponent::PositionComponent(glm::vec2 pos) : position{std::move(pos)} {
}

}