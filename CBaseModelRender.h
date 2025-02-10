#pragma once
#include "main.h"

class CBaseModelRender
{
private:
	struct tModelStruct {
		CBaseModelInfo* pModelInfo;
		std::uint32_t nBoneId;
		RwV3d vOffset;
		RwV3d vRotate;
		RwV3d vScale;
		union {
			RwObject* pRwObject;
			RpClump* pRwClump;
			RpAtomic* pRwAtomic;
		};
	};
	std::unordered_map<std::uint32_t, std::array<tModelStruct, 8>> m_Players;

	
	CPed* GetPedPointer(std::uint32_t nHandle);
	RwMatrix* GetBoneMatrix(CPed* pPed, std::uint32_t nBoneId);
	void ApplyMatrixOffset(RwMatrix* pMatrix, const RwV3d& vOffset);
	void RotateMatrix(RwMatrix* pMatrix, RwV3d vRotate);
public:
	CBaseModelRender();

	
	bool AddModel(CBaseModelInfo* pModel, std::uint32_t nPedHandle, std::uint32_t nBoneId, std::uint8_t nSlot);
	void RemoveModel(std::uint32_t nPedHandle, std::uint8_t nSlot);
	void RemoveAllModels(std::uint32_t nPedHandle);
	void RotateModel(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwV3d& vRotate);
	void SetModelOffset(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwV3d& vOffset);
	void SetModelScale(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwV3d& vScale);
	void RenderModels();
	void Cleanup();
};