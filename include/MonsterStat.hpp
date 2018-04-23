#ifndef OBELISK_MONSTERSTAT_HPP
#define OBELISK_MONSTERSTAT_HPP

namespace obelisk {

struct MonsterStats {
	explicit MonsterStats(int hp, float timeToMove) : hp{hp}, timeToMove{timeToMove} {}

	const int hp;
	const float timeToMove;
};

}

#endif
