#include <utility>

#include "ObeliskCard.hpp"

namespace obelisk {

ObeliskCard::ObeliskCard(std::string title, std::string description) :
		title{std::move(title)},
		description{std::move(description)} {
}

}