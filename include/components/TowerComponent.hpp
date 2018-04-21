#ifndef OBELISK_TOWERCOMPONENT_HPP
#define OBELISK_TOWERCOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace obelisk {

class TowerComponent final : public ashley::Component {
public:
	explicit TowerComponent() = default;

	int level{0};
};

}

#endif
