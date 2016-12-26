#pragma once
#include "Meshsegmenter.h"
#include "Common.h"
#include "ClothSegment.h"
class ClothSegmenter :
	public MeshSegmenter
{
public:
	ClothSegmenter(void);
	~ClothSegmenter(void);

	/* Override */
	void onDifferentLevelSet(size_t seq, LevelSet& levelSet){

	}

	/* Override */
	Segment_ createSegment(){
		return smartNew(ClothSegment);
	}
};

