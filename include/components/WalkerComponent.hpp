#ifndef OBELISK_WALKERCOMPONENT_HPP
#define OBELISK_WALKERCOMPONENT_HPP

#include <Ashley/core/Component.hpp>

namespace obelisk {

class WalkerComponent : public ashley::Component {
public:
	explicit WalkerComponent(float stepTime) :
			stepTime{stepTime},
			timeToStep{stepTime} {
	}

	const float stepTime;
	float timeToStep;
};

}

#endif
