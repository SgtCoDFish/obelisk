#ifndef OBELISK_ENTITYSPAWNSYSTEM_HPP
#define OBELISK_ENTITYSPAWNSYSTEM_HPP

#include <vector>

#include <Ashley/systems/IntervalSystem.hpp>
#include <ObeliskState.hpp>
#include <MonsterStat.hpp>

namespace ashley {
class Engine;
}

namespace obelisk {

class EntitySpawnSystem final : public ashley::IntervalSystem {
public:
	explicit EntitySpawnSystem(int64_t priority, float interval, ObeliskState *state,
							   std::vector<APG::SpriteBase *> sprites, std::vector<MonsterStats> monsterStats);

	void addedToEngine(ashley::Engine &engine) override;

	void removedFromEngine(ashley::Engine &engine) override;

protected:
	void updateInterval() override;

private:
	ashley::Engine *engine{nullptr};
	ObeliskState *state{nullptr};

	std::vector<APG::SpriteBase *> sprites;
	std::vector<MonsterStats> monsterStats;
	APG::SpriteBase * lastSprite;
};

}

#endif
