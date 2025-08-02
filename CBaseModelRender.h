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
		RwRGBA tColor;
		union {
			RwObject* pRwObject;
			RpClump* pRwClump;
			RpAtomic* pRwAtomic;
		};
	};
	std::unordered_map<std::uint32_t, std::array<tModelStruct, 8>> m_Players;
	std::unordered_map<std::uint32_t, CAtomicModelInfo*> m_CustomModels;
	
	CPed* GetPedPointer(std::uint32_t nHandle);
	RwMatrix* GetBoneMatrix(CPed* pPed, std::uint32_t nBoneId);
	void ApplyMatrixOffset(RwMatrix* pMatrix, const RwV3d& vOffset);
	void RotateMatrix(RwMatrix* pMatrix, RwV3d vRotate);

	// Private methods for custom models
	bool LoadTxd(const std::string& szTxdPath, const std::uint16_t& nModelId, CAtomicModelInfo* pModelInfo);
	bool LoadAtomicModel(const std::string& szDffPath, const std::uint16_t& nModelId, CAtomicModelInfo* pModelInfo);
public:
	CBaseModelRender();

	bool AddModel(CBaseModelInfo* pModel, std::uint32_t nPedHandle, std::uint32_t nBoneId, std::uint8_t nSlot);
	void RemoveModel(std::uint32_t nPedHandle, std::uint8_t nSlot);
	void RemoveAllModels(std::uint32_t nPedHandle);
	void RotateModel(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwV3d& vRotate);
	void SetModelOffset(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwV3d& vOffset);
	void SetModelScale(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwV3d& vScale);
	void RenderModels();
	void SetModelColor(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwRGBA& tColor);
	void Cleanup();

	// Public methods for custom models
	std::uint16_t LoadSimpleModel(const std::string& szDffPath, const std::string& szTxdPath);
	CAtomicModelInfo* GetCustomModel(std::uint16_t nModelId);

	// Static methods for callback's
	static RpAtomic* ClumpsForAtomic(RpAtomic* pAtomic, void* pData);
	static RpMaterial* GeometryForMaterials(RpMaterial* pMaterial, void* pData);
	static RpAtomic* SetRelatedModelInfoCB(RpAtomic* atomic, void* data);
};

extern CBaseModelRender* g_BaseModelRender;