//Floodgate module by Jackson Cannon - https://github.com/cannon/gmsv_floodgate
//Used to limit console messages sent by GMOD server to prevent many lag exploits

//Lots of code from gm_spew - https://github.com/LennyPenny/gm_spew

//taking heavy inspiration from Chris (STEAM_0:1:26262689)
//https://christopherthorne.googlecode.com/svn/trunk/gm_enginespew/gm_enginespew/main.cpp

#include "deps.h"

using namespace GarrysMod::Lua;

ILuaBase* LAU; /*since the spew handler func doesn't get provided with the LUA state / interface / w/e
			   we need to grab it on module load and store it here*/

//the old spew func will be stored here before we replace it with ours
SpewOutputFunc_t oldSpewFunc;

//will be our thread so weird things don't happen when stuff gets called from another thread
int gThread;

//how many messages can be sent
int floodgateConsoleLimit = 100000;

SpewRetval_t spewHandler(SpewType_t spewType, const char *msg) {
	#ifdef _WIN32
		if (GetCurrentThreadId() != gThread)
			return SPEW_CONTINUE;
	#endif // _WIN32
	
	if (!msg) //checking if the message is valid
		return SPEW_CONTINUE;

	floodgateConsoleLimit--;

	if (floodgateConsoleLimit < 0)
		return SPEW_CONTINUE;

	if (floodgateConsoleLimit == 0)
		return oldSpewFunc(spewType, "Stopping console flood (floodgate module)");

	return oldSpewFunc(spewType, msg); //pass it back to the default handler
	
}

int ConsoleFloodgate(lua_State* state)
{
	if (LUA->IsType(1, Type::NUMBER))
	{
		floodgateConsoleLimit = (int)LUA->GetNumber(1);
	}
	return 1;
}

GMOD_MODULE_OPEN() {
	LAU = LUA; //making it available in our whole module

	#ifdef _WIN32
		gThread = GetCurrentThreadId();
	#endif // _WIN32


	oldSpewFunc = GetSpewOutputFunc(); //saving the old func

	SpewOutputFunc((SpewOutputFunc_t)spewHandler); //making our function the spew handler


	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->PushString("ConsoleFloodgate");
	LUA->PushCFunction(ConsoleFloodgate);
	LUA->SetTable(-3);	

	return 0;
}

GMOD_MODULE_CLOSE() {
	SpewOutputFunc(oldSpewFunc); //making sure we get out before out lua thingy gets closed and everything breaks

	return 0;
}