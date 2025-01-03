
#pragma once

#include "BlockPlant.h"
#include "../FastRandom.h"





class cBlockNetherWartHandler final :
	public cBlockPlant<false>
{
	using Super = cBlockPlant<false>;

public:

	using Super::Super;

private:

	virtual cItems ConvertToPickups(const NIBBLETYPE a_BlockMeta, const cItem * const a_Tool) const override
	{
		if (a_BlockMeta == 0x03)
		{
			// Fully grown, drop the entire produce:
			const auto DropNum = FortuneDiscreteRandom(2, 4, ToolFortuneLevel(a_Tool));
			return cItem(E_ITEM_NETHER_WART, DropNum);
		}

		return cItem(E_ITEM_NETHER_WART);
	}





	virtual int Grow(cChunk & a_Chunk, Vector3i a_RelPos, int a_NumStages = 1) const override
	{
		auto oldMeta = a_Chunk.GetMeta(a_RelPos);
		auto meta = std::min(oldMeta + a_NumStages, 3);
		if ((oldMeta < 3) && (CanGrow(a_Chunk, a_RelPos) == paGrowth))
		{
			a_Chunk.SetBlock(a_RelPos, m_BlockType, static_cast<NIBBLETYPE>(meta));
			return meta - oldMeta;
		}

		// In older versions of cuberite, there was a bug which made wart grow too much. This check fixes previously saved buggy warts.
		if (oldMeta > 3)
		{
			a_Chunk.FastSetBlock(a_RelPos, m_BlockType, 3);
		}
		return 0;
	}





	virtual bool CanBeAt(const cChunk & a_Chunk, const Vector3i a_Position, const NIBBLETYPE a_Meta) const override
	{
		// Needs to be placed on top of a Soulsand block:
		const auto BasePos = a_Position.addedY(-1);
		return cChunkDef::IsValidHeight(BasePos) && (a_Chunk.GetBlock(BasePos) == E_BLOCK_SOULSAND);
	}





	virtual ColourID GetMapBaseColourID(NIBBLETYPE a_Meta) const override
	{
		UNUSED(a_Meta);
		return 35;
	}
} ;
