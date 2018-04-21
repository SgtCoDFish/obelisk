#ifndef OBELISK_OBELISKCARD_HPP
#define OBELISK_OBELISKCARD_HPP

#include <string>

namespace obelisk {

class ObeliskCard final {
public:
	explicit ObeliskCard(std::string title, std::string description);

private:
	std::string title;
	std::string description;
};

}

#endif
