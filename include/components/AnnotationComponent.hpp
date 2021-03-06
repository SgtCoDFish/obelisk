#ifndef OBELISK_ANNOTATIONCOMPONENT_HPP
#define OBELISK_ANNOTATIONCOMPONENT_HPP

#include <Ashley/core/Component.hpp>
#include <APG/graphics/SpriteBase.hpp>

class AnnotationComponent final : public ashley::Component {
public:
	explicit AnnotationComponent(APG::SpriteBase *sprite, bool showCount = false) : sprite{sprite},
																					showCount{showCount} {}

	APG::SpriteBase *sprite;
	bool showCount;
	int lastCount{-1};
};

#endif
