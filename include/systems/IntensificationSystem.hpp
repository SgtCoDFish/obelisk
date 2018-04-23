#ifndef OBELISK_INTENSIFICATIONSYSTEM_HPP
#define OBELISK_INTENSIFICATIONSYSTEM_HPP

#include <cstdint>
#include <memory>

#include <Ashley/systems/IteratingSystem.hpp>
#include <Ashley/core/Entity.hpp>
#include <ObeliskState.hpp>

namespace obelisk {

class IntensificationSystem final : public ashley::IteratingSystem {
public:
	explicit IntensificationSystem(int64_t priority, std::shared_ptr<ObeliskState> state, APG::SpriteBase *speedUpgrade,
								   APG::SpriteBase *levelUpgrade, APG::SpriteBase *cardFront,
								   APG::SpriteBase *smallCardFront);

	void processEntity(ashley::Entity *entity, float deltaTime) override;

private:
	std::shared_ptr<ObeliskState> state;
	APG::SpriteBase *speedUpgrade;
	APG::SpriteBase *levelUpgrade;
	APG::SpriteBase *cardFront;
	APG::SpriteBase *smallCardFront;
};

}

#endif //OBELISK_INTENSIFICATIONSYSTEM_HPP
