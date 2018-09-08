#pragma once

#include "ImageMeta.h"
#include "Animation.h"
#include "Color.h"
#include <vector>
#include <cstdint>

struct ArtFile
{
public:
	std::vector<Palette> palettes;
	std::vector<ImageMeta> imageMetas;
	std::vector<Animation> animations;

	void ValidateImageMetadata() const;
};
