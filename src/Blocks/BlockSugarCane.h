
#pragma once

#include "BlockPlant.h"





class cBlockSugarCaneHandler final :
	public cBlockPlant<false>
{
	using Super = cBlockPlant<false>;

public:

	using Super::Super;

private:

	virtual cItems ConvertToPickups(const NIBBLETYPE a_BlockMeta, const cItem * const a_Tool) const override
	{
		return cItem(E_ITEM_SUGARCANE, 1, 0);
	}





	virtual bool CanBeAt(const cChunk & a_Chunk, const Vector3i a_Position, const NIBBLETYPE a_Meta) const override
	{
		const auto BelowPos = a_Position.addedY(-1);
		if (!cChunkDef::IsValidHeight(BelowPos))
		{
			return false;
		}

		switch (a_Chunk.GetBlock(BelowPos))
		{
			case E_BLOCK_DIRT:
			case E_BLOCK_GRASS:
			case E_BLOCK_FARMLAND:
			case E_BLOCK_SAND:
			{
				static const Vector3i Coords[] =
				{
					{-1, 0,  0},
					{ 1, 0,  0},
					{ 0, 0, -1},
					{ 0, 0,  1},
				} ;
				for (size_t i = 0; i < ARRAYCOUNT(Coords); i++)
				{
					BLOCKTYPE BlockType;
					NIBBLETYPE BlockMeta;
					if (!a_Chunk.UnboundedRelGetBlock(BelowPos + Coords[i], BlockType, BlockMeta))
					{
						// Too close to the edge, cannot simulate
						return true;
					}
					if (IsBlockWater(BlockType) || (BlockType == E_BLOCK_FROSTED_ICE))
					{
						return true;
					}
				}  // for i - Coords[]
				// Not directly neighboring a water block
				return false;
			}
			case E_BLOCK_SUGARCANE:
			{
				return true;
			}
		}
		return false;
	}





	virtual ColourID GetMapBaseColourID(NIBBLETYPE a_Meta) const override
	{
		UNUSED(a_Meta);
		return 7;
	}





	virtual int Grow(cChunk & a_Chunk, Vector3i a_RelPos, int a_NumStages = 1) const override
	{
		// Check the total height of the sugarcane blocks here:
		auto top = a_RelPos.addedY(1);
		while (
			cChunkDef::IsValidHeight(top) &&
			(a_Chunk.GetBlock(top) == E_BLOCK_SUGARCANE)
		)
		{
			++top.y;
		}
		auto bottom = a_RelPos.addedY(-1);
		while (
			cChunkDef::IsValidHeight(bottom) &&
			(a_Chunk.GetBlock(bottom) == E_BLOCK_SUGARCANE)
		)
		{
			--bottom.y;
		}

		// Grow by at most a_NumStages, but no more than max height:
		auto toGrow = std::min(a_NumStages, a_Chunk.GetWorld()->GetMaxSugarcaneHeight() + 1 - (top.y - bottom.y));
		for (int i = 0; i < toGrow; i++)
		{
			const auto NewTop = top.addedY(i);
			if (!cChunkDef::IsValidHeight(NewTop))
			{
				return i;
			}

			if (a_Chunk.GetBlock(NewTop) == E_BLOCK_AIR)
			{
				a_Chunk.SetBlock(NewTop, E_BLOCK_SUGARCANE, 0);
			}
			else
			{
				return i;
			}
		}  // for i
		return toGrow;
	}

	virtual PlantAction CanGrow(cChunk & a_Chunk, Vector3i a_RelPos) const override
	{
		// Only allow growing if there's an air block above:
		if (((a_RelPos.y + 1) < cChunkDef::Height) && (a_Chunk.GetBlock(a_RelPos.addedY(1)) == E_BLOCK_AIR))
		{
			return Super::CanGrow(a_Chunk, a_RelPos);
		}
		return paStay;
	}
} ;




