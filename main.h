#pragma once
#define LUAOPEN_MODULE_EXPAND(name) luaopen_##name
#define LUAOPEN_MODULE(name) LUAOPEN_MODULE_EXPAND name
#define LUA_MODULE_ENTRYPOINT extern "C" __declspec(dllexport) int LUAOPEN_MODULE((MODULE_NAME))
#define SOL_MODULE_ENTRYPOINT(func) LUA_MODULE_ENTRYPOINT(lua_State* L) { return (sol::c_call<decltype(&func), &func>)(L); }

#pragma warning(disable : 26495) // Variable '%variable%' is uninitialized. Always initialize a member variable (type.6).
#pragma warning(disable : 26819) // Unannotated fallthrough between switch labels (es.78).
#pragma warning(disable : 26439) // This kind of function may not throw. Declare it 'noexcept'.
#pragma warning(disable : 26800)
#include "sol2/sol.hpp"
#pragma warning(default : 26800)
#pragma warning(default : 26439)
#pragma warning(default : 26819)
#pragma warning(default : 26495)

#include "plugin.h"
#include "CModelInfo.h"

using namespace plugin;

#include "CBaseModelRender.h"
#include "CScene.h"