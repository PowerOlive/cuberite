
#pragma once

#include "RedstoneHandler.h"





class cObserverHandler : public cRedstoneHandler
{
public:

	inline static bool IsOn(NIBBLETYPE a_Meta)
	{
		return (a_Meta & 0x8) == 0x8;
	}

	inline static Vector3i Getಠ_ಠOffset(NIBBLETYPE a_Meta)
	{
		return -GetButtOffset(a_Meta);
	}

	inline static Vector3i GetButtOffset(NIBBLETYPE a_Meta)
	{
		switch (a_Meta & 0x7)
		{
			case 0x0: return { 0, 1, 0 };
			case 0x1: return { 0, -1, 0 };
			case 0x2: return { 0, 0, 1 };
			case 0x3: return { 0, 0, -1 };
			case 0x4: return { 1, 0, 0 };
			case 0x5: return { -1, 0, 0 };
			default:
			{
				LOGWARNING("%s: Unknown metadata: %d", __FUNCTION__, a_Meta);
				ASSERT(!"Unknown metadata while determining orientation of observer!");
				return { 0, 0, 0 };
			}
		}
	}

	static bool ShouldPowerOn(cWorld & a_World, const Vector3i & a_Position, NIBBLETYPE a_Meta, cIncrementalRedstoneSimulatorChunkData * a_Data)
	{
		BLOCKTYPE BlockType;
		NIBBLETYPE BlockMeta;
		if (!a_World.GetBlockTypeMeta(a_Position + Getಠ_ಠOffset(a_Meta), BlockType, BlockMeta))
		{
			return false;
		}

		auto ಠ_ಠ = PoweringData(BlockType, BlockMeta);
		auto UpdateInfo = a_Data->ExchangeUpdateOncePowerData(a_Position, ಠ_ಠ);

		// Determine if to signal an update based on the block previously observed changed
		return (UpdateInfo.PoweringBlock != ಠ_ಠ.PoweringBlock) || (UpdateInfo.PowerLevel != ಠ_ಠ.PowerLevel);
	}

	virtual unsigned char GetPowerDeliveredToPosition(cWorld & a_World, Vector3i a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta, Vector3i a_QueryPosition, BLOCKTYPE a_QueryBlockType) const override
	{
		if (IsOn(a_Meta) && (a_QueryPosition == (a_Position + GetButtOffset(a_Meta))))
		{
			return 15;
		}

		return 0;
	}

	virtual unsigned char GetPowerLevel(cWorld & a_World, Vector3i a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta) const override
	{
		return IsOn(a_BlockType) ? 15 : 0;
	}

	virtual cVector3iArray Update(cWorld & a_World, Vector3i a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta, PoweringData a_PoweringData) const override
	{
		// LOGD("Evaluating Lenny the observer (%i %i %i)", a_Position.x, a_Position.y, a_Position.z);

		auto Data = static_cast<cIncrementalRedstoneSimulator *>(a_World.GetRedstoneSimulator())->GetChunkData();
		auto DelayInfo = Data->GetMechanismDelayInfo(a_Position);

		if (DelayInfo == nullptr)
		{
			if (!ShouldPowerOn(a_World, a_Position, a_Meta, Data))
			{
				return {};
			}

			// From rest, we've determined there was a block update
			// Schedule power-on 1 tick in the future
			Data->m_MechanismDelays[a_Position] = std::make_pair(1, true);
		}
		else
		{
			int DelayTicks;
			bool ShouldPowerOn;
			std::tie(DelayTicks, ShouldPowerOn) = *DelayInfo;

			if (DelayTicks == 0)
			{
				if (ShouldPowerOn)
				{
					// Remain on for 1 tick before resetting
					*DelayInfo = std::make_pair(1, false);
					a_World.SetBlockMeta(a_Position.x, a_Position.y, a_Position.z, a_Meta | 0x8, a_Meta);
				}
				else
				{
					// We've reset. Erase delay data in preparation for detecting further updates
					Data->m_MechanismDelays.erase(a_Position);
					a_World.SetBlockMeta(a_Position.x, a_Position.y, a_Position.z, a_Meta & ~0x8, a_Meta);
				}

				return { a_Position + GetButtOffset(a_Meta) };
			}
		}

		return {};
	}

	virtual cVector3iArray GetValidSourcePositions(cWorld & a_World, Vector3i a_Position, BLOCKTYPE a_BlockType, NIBBLETYPE a_Meta) const override
	{
		UNUSED(a_World);
		UNUSED(a_Position);
		UNUSED(a_BlockType);
		UNUSED(a_BlockType);
		return {};
	}
};
