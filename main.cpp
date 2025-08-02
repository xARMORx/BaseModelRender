#include "main.h"

sol::table open(sol::this_state ts) {
    sol::state_view lua{ ts };
    lua["VERSION"] = 0.1;
    sol::table myModule = lua.create_table();

    

    myModule.set_function("cleanup", []() {
        g_BaseModelRender->Cleanup();
    }); 

    myModule.set_function("addModel", [](std::uintptr_t pModel, std::uint32_t nPedHandle, std::uint32_t nBoneId, std::uint8_t nSlot) {
        return g_BaseModelRender->AddModel((CBaseModelInfo*)pModel, nPedHandle, nBoneId, nSlot);
    });

    myModule.set_function("setModelScale", [](std::uint32_t nPedHandle, std::uint8_t nSlot, float fX, float fY, float fZ) {
        RwV3d vScale = { fX, fY, fZ };
        g_BaseModelRender->SetModelScale(nPedHandle, nSlot, vScale);
    });

    myModule.set_function("setModelOffset", [](std::uint32_t nPedHandle, std::uint8_t nSlot, float fX, float fY, float fZ) {
        RwV3d vOffset = { fX, fY, fZ };
        g_BaseModelRender->SetModelOffset(nPedHandle, nSlot, vOffset);
    });

    myModule.set_function("setModelRotate", [](std::uint32_t nPedHandle, std::uint8_t nSlot, float fX, float fY, float fZ) {
        RwV3d vRotate = { fX, fY, fZ };
        g_BaseModelRender->RotateModel(nPedHandle, nSlot, vRotate);
    });

    myModule.set_function("removeModel", [](std::uint32_t nPedHandle, std::uint8_t nSlot) {
        g_BaseModelRender->RemoveModel(nPedHandle, nSlot);
    });

    myModule.set_function("setModelColor", [](std::uint32_t nPedHandle, std::uint8_t nSlot, std::uint8_t nR, std::uint8_t nG, std::uint8_t nB, std::uint8_t nA) {
        g_BaseModelRender->SetModelColor(nPedHandle, nSlot, {nR, nG, nB, nA});
    });

    myModule.set_function("removeAllModels", [](std::uint32_t nPedHandle) {
        g_BaseModelRender->RemoveAllModels(nPedHandle);
    });

    myModule.set_function("getGameBaseModel", [](std::uint32_t nModelIndex) {
        return (std::uintptr_t)CModelInfo::GetModelInfo(nModelIndex);
    });

    myModule.set_function("loadSimpleModel", [](const std::string& szDffPath, const std::string& szTxdPath)
    {
        return g_BaseModelRender->LoadSimpleModel(szDffPath, szTxdPath);
    });

    myModule.set_function("getSimpleBaseModel", [](std::uint16_t nCustomModelId)
    {
        return (std::uintptr_t)(g_BaseModelRender->GetCustomModel(nCustomModelId));
    });

    return myModule;
}

SOL_MODULE_ENTRYPOINT(open);