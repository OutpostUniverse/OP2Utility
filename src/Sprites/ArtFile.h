#pragma once

#include "ImageMeta.h"
#include "Animation.h"
#include "Color.h"
#include <vector>
#include <cstdint>
#include <cstddef>

struct ArtFile
{
public:
	std::vector<Palette> palettes;
	std::vector<ImageMeta> imageMetas;
	std::vector<Animation> animations;

	void ValidateImageMetadata() const;

//private:
	void CountFrames(std::size_t& frameCount, std::size_t& subframeCount, std::size_t& unknownCount) const;
};
