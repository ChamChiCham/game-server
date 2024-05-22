#include <iostream>
#include "include/lua.hpp"

#pragma comment (lib, "lua54.lib")

int LUAapi_add(lua_State* L)
{
	int a = lua_tonumber(L, -1);
	int b = lua_tonumber(L, -2);
	int c = a + b;
	lua_pushnumber(L, c);
	lua_pop(L, 2);
	return 1;
}

int main()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	int ret = luaL_loadfile(L, "dragon_move.lua");
	if (ret) {
		std::cout << "Error: " << lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	//lua_getglobal(L, "rows");
	//lua_getglobal(L, "cols");
	//rows = (int)lua_tonumber(L, -2);
	//cols = (int)lua_tonumber(L, -1);

	lua_register(L, "callC_add", LUAapi_add);

	lua_getglobal(L, "event_add");
	lua_pushnumber(L, 10);
	lua_pushnumber(L, 20);
	ret = lua_pcall(L, 2, 1, 0);
	if (0 != ret) {
		std::cout << "Error: " << lua_tostring(L, -1);
		lua_pop(L, 1);
	}
	int res = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);

	std::cout << res;
	lua_close(L);
}