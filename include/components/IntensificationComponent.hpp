#ifndef OBELISK_INTENSIFICATIONCOMPONENT_HPP
#define OBELISK_INTENSIFICATIONCOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace obelisk {

class IntensificationComponent final : public ashley::Component {
public:
	explicit IntensificationComponent(int level) : level{level} {}

	const int level;
};

}

#endif //OBELISK_INTENSIFICATIONCOMPONENT_HPP
