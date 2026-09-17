/****************************************************************************/
//    Copyright (C) 2009 Aali132                                            //
//    Copyright (C) 2018 quantumpencil                                      //
//    Copyright (C) 2018 Maxime Bacoux                                      //
//    Copyright (C) 2024 myst6re                                            //
//    Copyright (C) 2020 Chris Rizzitello                                   //
//    Copyright (C) 2020 John Pritchard                                     //
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/
#pragma once

#include <steamworkssdk/steam_api.h>

bool FFNx_SteamAPI_RestartAppIfNecessary(uint32 unOwnAppID);
#define SteamAPI_RestartAppIfNecessary FFNx_SteamAPI_RestartAppIfNecessary
bool FFNx_SteamAPI_Init();
#define SteamAPI_Init FFNx_SteamAPI_Init
void FFNx_SteamAPI_RunCallbacks();
#define SteamAPI_RunCallbacks FFNx_SteamAPI_RunCallbacks
void FFNx_SteamAPI_Shutdown();
#define SteamAPI_Shutdown FFNx_SteamAPI_Shutdown
void FFNx_SteamAPI_RegisterCallback(class CCallbackBase *pCallback, int iCallback);
void FFNx_SteamAPI_UnregisterCallback(class CCallbackBase *pCallback);

ISteamUser *FFNx_SteamUser();
#define SteamUser FFNx_SteamUser
ISteamUtils *FFNx_SteamUtils();
#define SteamUtils FFNx_SteamUtils
ISteamUserStats *FFNx_SteamUserStats();
#define SteamUserStats FFNx_SteamUserStats

//-----------------------------------------------------------------------------
// Purpose: maps a steam callback to a class member function
//			template params: T = local class, P = parameter struct
//-----------------------------------------------------------------------------
template< class T, class P >
class FFNxCCallback : protected CCallbackBase
{
public:
	typedef void (T::*func_t)( P* );

	// If you can't support constructing a callback with the correct parameters
	// then uncomment the empty constructor below and manually call
	// ::Register() for your object
	// Or, just call the regular constructor with (NULL, NULL)
	// FFNxCCallback() {}

	// constructor for initializing this object in owner's constructor
	FFNxCCallback( T *pObj, func_t func ) : m_pObj( pObj ), m_Func( func )
	{
		if ( pObj && func )
			Register( pObj, func );
	}

	~FFNxCCallback()
	{
		if ( m_nCallbackFlags & k_ECallbackFlagsRegistered )
			Unregister();
	}

	// manual registration of the callback
	void Register( T *pObj, func_t func )
	{
		if ( !pObj || !func )
			return;

		if ( m_nCallbackFlags & k_ECallbackFlagsRegistered )
			Unregister();

		m_pObj = pObj;
		m_Func = func;
		// SteamAPI_RegisterCallback sets k_ECallbackFlagsRegistered
		FFNx_SteamAPI_RegisterCallback( this, P::k_iCallback );
	}

	void Unregister()
	{
		// SteamAPI_UnregisterCallback removes k_ECallbackFlagsRegistered
		FFNx_SteamAPI_UnregisterCallback( this );
	}
protected:
	virtual void Run( void *pvParam )
	{
		(m_pObj->*m_Func)( (P *)pvParam );
	}
	virtual void Run( void *pvParam, bool, SteamAPICall_t )
	{
		(m_pObj->*m_Func)( (P *)pvParam );
	}
	int GetCallbackSizeBytes()
	{
		return sizeof( P );
	}

	T *m_pObj;
	func_t m_Func;
};

// utility macro for declaring the function and callback object together
#undef STEAM_CALLBACK
#define STEAM_CALLBACK( thisclass, func, param, var ) FFNxCCallback< thisclass, param > var; void func( param *pParam )
