#include "CBaseModelRender.h"


CBaseModelRender::CBaseModelRender()
{
	this->m_Players = {};
	Events::drawingEvent += [this] {
		this->RenderModels();
	};
}

CPed* CBaseModelRender::GetPedPointer(std::uint32_t nHandle)
{
	return CPools::GetPed(nHandle);
}

RwMatrix* CBaseModelRender::GetBoneMatrix(CPed* pPed, std::uint32_t nBoneId)
{
	nBoneId += 20;
	if (!pPed)
		return nullptr;

	if ((pPed->m_nPedFlags.bHasGroupDriveTask & 0x400) == 0) 
	{
		pPed->UpdateRpHAnim();
		pPed->m_nPedFlags.bHasGroupDriveTask |= 0x400;
	}

	RpHAnimHierarchy* pAnimHierarcy = GetAnimHierarchyFromSkinClump(pPed->m_pRwClump);

	if (pAnimHierarcy) 
	{
		unsigned int nId = RpHAnimIDGetIndex(pAnimHierarcy, nBoneId) << 6;
		return &pAnimHierarcy->pMatrixArray[nBoneId];
	}

	return nullptr;
}

void CBaseModelRender::ApplyMatrixOffset(RwMatrix* pMatrix, const RwV3d& vOffset)
{
	pMatrix->pos.x += pMatrix->right.x * vOffset.x + pMatrix->up.x * vOffset.y + pMatrix->at.x * vOffset.z;
	pMatrix->pos.y += pMatrix->right.y * vOffset.x + pMatrix->up.y * vOffset.y + pMatrix->at.y * vOffset.z;
	pMatrix->pos.z += pMatrix->right.z * vOffset.x + pMatrix->up.z * vOffset.y + pMatrix->at.z * vOffset.z;
}

void CBaseModelRender::RotateMatrix(RwMatrix* pMatrix, RwV3d vRotate)
{
	if (!pMatrix)
		return;

	RwV3d vPos = pMatrix->pos;

	RwV3d fAxisZ = { 0.0f, 0.0f, 1.0f };
	RwV3d fAxisY = { 0.0f, 1.0f, 0.0f };
	RwV3d fAxisX = { 1.0f, 0.0f, 0.0f };

	RwMatrixRotate(pMatrix, &fAxisZ, vRotate.z, rwCOMBINEPRECONCAT);
	RwMatrixRotate(pMatrix, &fAxisY, vRotate.y, rwCOMBINEPRECONCAT);
	RwMatrixRotate(pMatrix, &fAxisX, vRotate.x, rwCOMBINEPRECONCAT);

	pMatrix->pos = vPos;
}


bool CBaseModelRender::AddModel(CBaseModelInfo* pModel, std::uint32_t nPedHandle, std::uint32_t nBoneId, std::uint8_t nSlot)
{

	if (nSlot > 7 || !pModel || !GetPedPointer(nPedHandle))
		return false;

	tModelStruct model;
	model.pModelInfo = pModel;
	model.nBoneId = nBoneId;
	model.vScale = { 1.0f, 1.0f, 1.0f };
	model.vOffset = { 0.f, 0.f, 0.f };
	model.vRotate = { 0.f, 0.f, 0.f };

	RwMatrix* pMatrix = this->GetBoneMatrix(this->GetPedPointer(nPedHandle), nBoneId);

	RwMatrix mObjectMatrix{};

	memcpy(&mObjectMatrix, pMatrix, sizeof(RwMatrix));

	model.pRwObject = pModel->CreateInstance(&mObjectMatrix);

	auto it = this->m_Players.find(nPedHandle);
	if (it == this->m_Players.end())
	{
		std::array<tModelStruct, 8> newArray{};
		this->m_Players.emplace(std::make_pair(nPedHandle, newArray));

		this->AddModel(pModel, nPedHandle, nBoneId, nSlot);
	}
	else {
		it->second[nSlot] = model;
	}

	return true;
}
void CBaseModelRender::RemoveModel(std::uint32_t nPedHandle, std::uint8_t nSlot)
{
	if (nSlot > 7)
		return;

	auto it = this->m_Players.find(nPedHandle);
	if (it == this->m_Players.end())
		return;

	it->second[nSlot].pRwAtomic = nullptr;
	it->second[nSlot].pModelInfo = nullptr;
	it->second[nSlot].nBoneId = -1;
}

void CBaseModelRender::RemoveAllModels(std::uint32_t nPedHandle)
{
	auto it = this->m_Players.find(nPedHandle);
	if (it == this->m_Players.end())
		return;

	this->m_Players.erase(it->first);
}

void CBaseModelRender::RotateModel(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwV3d& vRotate)
{
	auto it = this->m_Players.find(nPedHandle);
	if (it == this->m_Players.end())
		return;

	it->second[nSlot].vRotate = vRotate;
}

void CBaseModelRender::SetModelOffset(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwV3d& vOffset)
{
	auto it = this->m_Players.find(nPedHandle);
	if (it == this->m_Players.end())
		return;

	it->second[nSlot].vOffset = vOffset;
}

void CBaseModelRender::SetModelScale(std::uint32_t nPedHandle, std::uint8_t nSlot, const RwV3d& vScale)
{
	auto it = this->m_Players.find(nPedHandle);
	if (it == this->m_Players.end())
		return;

	it->second[nSlot].vScale = vScale;
}

void CBaseModelRender::RenderModels()
{
	if (this->m_Players.empty())
		return;
	int RENDERSTATECULLMODE;
	int RENDERSTATEZTESTENABLE;
	int RENDERSTATEZWRITEENABLE;
	int RENDERSTATESHADEMODE;
	int RENDERSTATEFOGENABLE;

	RwRenderStateGet(rwRENDERSTATECULLMODE, RWRSTATE(&RENDERSTATECULLMODE));
	RwRenderStateGet(rwRENDERSTATEZTESTENABLE, RWRSTATE(&RENDERSTATEZTESTENABLE));
	RwRenderStateGet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(&RENDERSTATEZWRITEENABLE));
	RwRenderStateGet(rwRENDERSTATESHADEMODE, RWRSTATE(&RENDERSTATESHADEMODE));
	RwRenderStateGet(rwRENDERSTATEFOGENABLE, RWRSTATE(&RENDERSTATEFOGENABLE));

	RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
	RwRenderStateSet(rwRENDERSTATESHADEMODE, RWRSTATE(rwSHADEMODEGOURAUD));
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(FALSE));
	for (auto& it : this->m_Players) 
	{
		if (!GetPedPointer(it.first)) 
		{
			this->m_Players.erase(it.first);
			continue;
		}

		for (std::uint8_t i{}; i < 8; i++) 
		{
			if (!it.second[i].pRwObject)
				continue;

			RwFrame* pFrame = (RwFrame*)it.second[i].pRwAtomic->object.object.parent;

			RwMatrix* pMatrix = this->GetBoneMatrix(this->GetPedPointer(it.first), it.second[i].nBoneId);

			RwMatrix mObjectMatrix{};
			
			memcpy(&mObjectMatrix, pMatrix, sizeof(RwMatrix));

			this->ApplyMatrixOffset(&mObjectMatrix, it.second[i].vOffset);
			this->RotateMatrix(&mObjectMatrix, it.second[i].vRotate);

			memcpy(&pFrame->modelling, &mObjectMatrix, sizeof(RwMatrix));
			RwMatrixUpdate(&pFrame->modelling);
			RwMatrixScale(&pFrame->modelling, &it.second[i].vScale, rwCOMBINEPRECONCAT);
			RwFrameUpdateObjects(pFrame);

			if (it.second[i].pRwObject->type == 1)
				it.second[i].pRwAtomic->renderCallBack(it.second[i].pRwAtomic);
			else if (it.second[i].pRwObject->type == 2) {
				RpClumpRender(it.second[i].pRwClump);
			}
		}
	}
	RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(RENDERSTATECULLMODE));
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(RENDERSTATEZTESTENABLE));
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(RENDERSTATEZWRITEENABLE));
	RwRenderStateSet(rwRENDERSTATESHADEMODE, RWRSTATE(RENDERSTATESHADEMODE));
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(RENDERSTATEFOGENABLE));
}

void CBaseModelRender::Cleanup()
{
	this->m_Players.clear();
}
