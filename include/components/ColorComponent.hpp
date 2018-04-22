#ifndef OBELISK_COLORCOMPONENT_HPP
#define OBELISK_COLORCOMPONENT_HPP

#include <Ashley/core/Component.hpp>
#include <glm/vec4.hpp>

namespace obelisk {

class ColorComponent final : public ashley::Component {
public:
	explicit ColorComponent(glm::vec4 color) :
			color{std::move(color)} {
	}

	glm::vec4 color;
};

}

#endif
