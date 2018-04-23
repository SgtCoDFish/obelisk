#ifndef OBELISK_MONSTERSTAT_HPP
#define OBELISK_MONSTERSTAT_HPP

namespace obelisk {

struct MonsterStats {
	explicit MonsterStats(float timeToMove) : timeToMove{timeToMove} {}

	const float timeToMove;
};

}

#endif
