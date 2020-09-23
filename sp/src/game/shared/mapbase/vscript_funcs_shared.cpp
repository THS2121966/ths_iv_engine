//========= Mapbase - https://github.com/mapbase-source/source-sdk-2013 ============//
//
// Purpose: Due to this being a custom integration of VScript based on the Alien Swarm SDK, we don't have access to
//			some of the code normally available in games like L4D2 or Valve's original VScript DLL.
//			Instead, that code is recreated here, shared between server and client.
//
//			It also contains other functions unique to Mapbase.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "matchers.h"
#include "takedamageinfo.h"
#include "ammodef.h"
#include <vgui_controls/Controls.h> 
#include <vgui/ILocalize.h>

#ifndef CLIENT_DLL
#include "globalstate.h"
#include "vscript_server.h"
#endif

#include "vscript_funcs_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class CScriptNetPropManager
{
public:

#ifdef CLIENT_DLL
	RecvProp *RecurseTable( RecvTable *pTable, const char *pszPropName )
#else
	SendProp *RecurseTable( SendTable *pTable, const char *pszPropName )
#endif
	{
#ifdef CLIENT_DLL
		RecvProp *pProp = NULL;
#else
		SendProp *pProp = NULL;
#endif
		for (int i = 0; i < pTable->GetNumProps(); i++)
		{
			pProp = pTable->GetProp( i );
			if (pProp->GetType() == DPT_DataTable)
			{
				pProp = RecurseTable(pProp->GetDataTable(), pszPropName);
				if (pProp)
					return pProp;
			}
			else
			{
				if (FStrEq( pProp->GetName(), pszPropName ))
					return pProp;
			}
		}

		return NULL;
	}

#ifdef CLIENT_DLL
	RecvProp *RecurseNetworkClass( ClientClass *pClass, const char *pszPropName )
#else
	SendProp *RecurseNetworkClass( ServerClass *pClass, const char *pszPropName )
#endif
	{
#ifdef CLIENT_DLL
		RecvProp *pProp = RecurseTable( pClass->m_pRecvTable, pszPropName );
#else
		SendProp *pProp = RecurseTable( pClass->m_pTable, pszPropName );
#endif
		if (pProp)
			return pProp;

		if (pClass->m_pNext)
			return RecurseNetworkClass( pClass->m_pNext, pszPropName );
		else
			return NULL;
	}

#ifdef CLIENT_DLL
	RecvProp *GetPropByName( CBaseEntity *pEnt, const char *pszPropName )
	{
		if (pEnt)
		{
			return RecurseNetworkClass( pEnt->GetClientClass(), pszPropName );
		}

		return NULL;
	}
#else
	SendProp *GetPropByName( CBaseEntity *pEnt, const char *pszPropName )
	{
		if (pEnt)
		{
			return RecurseNetworkClass( pEnt->GetServerClass(), pszPropName );
		}

		return NULL;
	}
#endif

	int GetPropArraySize( HSCRIPT hEnt, const char *pszPropName )
	{
		CBaseEntity *pEnt = ToEnt( hEnt );
		auto *pProp = GetPropByName( pEnt, pszPropName );
		if (pProp)
		{
			// TODO: Is this what this function wants?
			return pProp->GetNumElements();
		}

		return -1;
	}

	#define GetPropFunc( name, varType, propType, defaultval ) \
	varType name( HSCRIPT hEnt, const char *pszPropName ) \
	{ \
		CBaseEntity *pEnt = ToEnt( hEnt ); \
		auto *pProp = GetPropByName( pEnt, pszPropName ); \
		if (pProp && pProp->GetType() == propType) \
		{ \
			return *(varType*)((char *)pEnt + pProp->GetOffset()); \
		} \
		return defaultval; \
	} \

	#define GetPropFuncArray( name, varType, propType, defaultval ) \
	varType name( HSCRIPT hEnt, const char *pszPropName, int iArrayElement ) \
	{ \
		CBaseEntity *pEnt = ToEnt( hEnt ); \
		auto *pProp = GetPropByName( pEnt, pszPropName ); \
		if (pProp && pProp->GetType() == propType) \
		{ \
			return ((varType*)((char *)pEnt + pProp->GetOffset()))[iArrayElement]; \
		} \
		return defaultval; \
	} \

	GetPropFunc( GetPropFloat, float, DPT_Float, -1 );
	GetPropFuncArray( GetPropFloatArray, float, DPT_Float, -1 );
	GetPropFunc( GetPropInt, int, DPT_Int, -1 );
	GetPropFuncArray( GetPropIntArray, int, DPT_Int, -1 );
	GetPropFunc( GetPropVector, Vector, DPT_Vector, vec3_invalid );
	GetPropFuncArray( GetPropVectorArray, Vector, DPT_Vector, vec3_invalid );

	HSCRIPT GetPropEntity( HSCRIPT hEnt, const char *pszPropName )
	{
		CBaseEntity *pEnt = ToEnt( hEnt );
		auto *pProp = GetPropByName( pEnt, pszPropName );
		if (pProp && pProp->GetType() == DPT_Int)
		{
			return ToHScript( *(CHandle<CBaseEntity>*)((char *)pEnt + pProp->GetOffset()) );
		}

		return NULL;
	}

	HSCRIPT GetPropEntityArray( HSCRIPT hEnt, const char *pszPropName, int iArrayElement )
	{
		CBaseEntity *pEnt = ToEnt( hEnt );
		auto *pProp = GetPropByName( pEnt, pszPropName );
		if (pProp && pProp->GetType() == DPT_Int)
		{
			return ToHScript( ((CHandle<CBaseEntity>*)((char *)pEnt + pProp->GetOffset()))[iArrayElement] );
		}

		return NULL;
	}

	const char *GetPropString( HSCRIPT hEnt, const char *pszPropName )
	{
		CBaseEntity *pEnt = ToEnt( hEnt );
		auto *pProp = GetPropByName( pEnt, pszPropName );
		if (pProp && pProp->GetType() == DPT_Int)
		{
			return (const char*)((char *)pEnt + pProp->GetOffset());
		}

		return NULL;
	}

	const char *GetPropStringArray( HSCRIPT hEnt, const char *pszPropName, int iArrayElement )
	{
		CBaseEntity *pEnt = ToEnt( hEnt );
		auto *pProp = GetPropByName( pEnt, pszPropName );
		if (pProp && pProp->GetType() == DPT_Int)
		{
			return ((const char**)((char *)pEnt + pProp->GetOffset()))[iArrayElement];
		}

		return NULL;
	}

	const char *GetPropType( HSCRIPT hEnt, const char *pszPropName )
	{
		CBaseEntity *pEnt = ToEnt( hEnt );
		auto *pProp = GetPropByName( pEnt, pszPropName );
		if (pProp)
		{
			switch (pProp->GetType())
			{
			case DPT_Int:		return "integer";
			case DPT_Float:		return "float";
			case DPT_Vector:	return "vector";
			case DPT_VectorXY:	return "vector2d";
			case DPT_String:	return "string";
			case DPT_Array:		return "array";
			case DPT_DataTable:	return "datatable";
			}
		}

		return NULL;
	}

	bool HasProp( HSCRIPT hEnt, const char *pszPropName )
	{
		CBaseEntity *pEnt = ToEnt( hEnt );
		return GetPropByName( pEnt, pszPropName ) != NULL;
	}

	#define SetPropFunc( name, varType, propType ) \
	void name( HSCRIPT hEnt, const char *pszPropName, varType value ) \
	{ \
		CBaseEntity *pEnt = ToEnt( hEnt ); \
		auto *pProp = GetPropByName( pEnt, pszPropName ); \
		if (pProp && pProp->GetType() == propType) \
		{ \
			*(varType*)((char *)pEnt + pProp->GetOffset()) = value; \
		} \
	} \

	#define SetPropFuncArray( name, varType, propType ) \
	void name( HSCRIPT hEnt, const char *pszPropName, varType value, int iArrayElement ) \
	{ \
		CBaseEntity *pEnt = ToEnt( hEnt ); \
		auto *pProp = GetPropByName( pEnt, pszPropName ); \
		if (pProp && pProp->GetType() == propType) \
		{ \
			((varType*)((char *)pEnt + pProp->GetOffset()))[iArrayElement] = value; \
		} \
	} \

	SetPropFunc( SetPropFloat, float, DPT_Float );
	SetPropFuncArray( SetPropFloatArray, float, DPT_Float );
	SetPropFunc( SetPropInt, int, DPT_Int );
	SetPropFuncArray( SetPropIntArray, int, DPT_Int );
	SetPropFunc( SetPropVector, Vector, DPT_Vector );
	SetPropFuncArray( SetPropVectorArray, Vector, DPT_Vector );
	SetPropFunc( SetPropString, const char*, DPT_String );
	SetPropFuncArray( SetPropStringArray, const char*, DPT_String );

	void SetPropEntity( HSCRIPT hEnt, const char *pszPropName, HSCRIPT value )
	{
		CBaseEntity *pEnt = ToEnt( hEnt );
		auto *pProp = GetPropByName( pEnt, pszPropName );
		if (pProp && pProp->GetType() == DPT_Int)
		{
			*((CHandle<CBaseEntity>*)((char *)pEnt + pProp->GetOffset())) = ToEnt(value);
		}
	}

	HSCRIPT SetPropEntityArray( HSCRIPT hEnt, const char *pszPropName, HSCRIPT value, int iArrayElement )
	{
		CBaseEntity *pEnt = ToEnt( hEnt );
		auto *pProp = GetPropByName( pEnt, pszPropName );
		if (pProp && pProp->GetType() == DPT_Int)
		{
			((CHandle<CBaseEntity>*)((char *)pEnt + pProp->GetOffset()))[iArrayElement] = ToEnt(value);
		}

		return NULL;
	}

private:
} g_ScriptNetPropManager;

BEGIN_SCRIPTDESC_ROOT_NAMED( CScriptNetPropManager, "CNetPropManager", SCRIPT_SINGLETON "Allows reading and updating the network properties of an entity." )
	DEFINE_SCRIPTFUNC( GetPropArraySize, "Returns the size of an netprop array, or -1." )
	DEFINE_SCRIPTFUNC( GetPropEntity, "Reads an EHANDLE valued netprop (21 bit integer). Returns the script handle of the entity." )
	DEFINE_SCRIPTFUNC( GetPropEntityArray, "Reads an EHANDLE valued netprop (21 bit integer) from an array. Returns the script handle of the entity." )
	DEFINE_SCRIPTFUNC( GetPropFloat, "Reads a float valued netprop." )
	DEFINE_SCRIPTFUNC( GetPropFloatArray, "Reads a float valued netprop from an array." )
	DEFINE_SCRIPTFUNC( GetPropInt, "Reads an integer valued netprop." )
	DEFINE_SCRIPTFUNC( GetPropIntArray, "Reads an integer valued netprop from an array." )
	DEFINE_SCRIPTFUNC( GetPropString, "Reads a string valued netprop." )
	DEFINE_SCRIPTFUNC( GetPropStringArray, "Reads a string valued netprop from an array." )
	DEFINE_SCRIPTFUNC( GetPropVector, "Reads a 3D vector valued netprop." )
	DEFINE_SCRIPTFUNC( GetPropVectorArray, "Reads a 3D vector valued netprop from an array." )
	DEFINE_SCRIPTFUNC( GetPropType, "Returns the name of the netprop type as a string." )
	DEFINE_SCRIPTFUNC( HasProp, "Checks if a netprop exists." )
	DEFINE_SCRIPTFUNC( SetPropEntity, "Sets an EHANDLE valued netprop (21 bit integer) to reference the specified entity." )
	DEFINE_SCRIPTFUNC( SetPropEntityArray, "Sets an EHANDLE valued netprop (21 bit integer) from an array to reference the specified entity." )
	DEFINE_SCRIPTFUNC( SetPropFloat, "Sets a netprop to the specified float." )
	DEFINE_SCRIPTFUNC( SetPropFloatArray, "Sets a netprop from an array to the specified float." )
	DEFINE_SCRIPTFUNC( SetPropInt, "Sets a netprop to the specified integer." )
	DEFINE_SCRIPTFUNC( SetPropIntArray, "Sets a netprop from an array to the specified integer." )
	DEFINE_SCRIPTFUNC( SetPropString, "Sets a netprop to the specified string." )
	DEFINE_SCRIPTFUNC( SetPropStringArray, "Sets a netprop from an array to the specified string." )
	DEFINE_SCRIPTFUNC( SetPropVector, "Sets a netprop to the specified vector." )
	DEFINE_SCRIPTFUNC( SetPropVectorArray, "Sets a netprop from an array to the specified vector." )
END_SCRIPTDESC();

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class CScriptConvarLookup
{
public:

#ifndef CLIENT_DLL
	const char *GetClientConvarValue( const char *pszConVar, int entindex )
	{
		return engine->GetClientConVarValue( entindex, pszConVar );
	}
#endif

	const char *GetStr( const char *pszConVar )
	{
		ConVarRef cvar( pszConVar );
		return cvar.GetString();
	}

	float GetFloat( const char *pszConVar )
	{
		ConVarRef cvar( pszConVar );
		return cvar.GetFloat();
	}

	void SetValue( const char *pszConVar, const char *pszValue )
	{
		ConVarRef cvar( pszConVar );
		if (!cvar.IsValid())
			return;

		// FCVAR_NOT_CONNECTED can be used to protect specific convars from nefarious interference
		if (cvar.IsFlagSet(FCVAR_NOT_CONNECTED))
			return;

		cvar.SetValue( pszValue );
	}

private:
} g_ScriptConvarLookup;

BEGIN_SCRIPTDESC_ROOT_NAMED( CScriptConvarLookup, "CConvars", SCRIPT_SINGLETON "Provides an interface for getting and setting convars." )
#ifndef CLIENT_DLL
	DEFINE_SCRIPTFUNC( GetClientConvarValue, "Returns the convar value for the entindex as a string. Only works with client convars with the FCVAR_USERINFO flag." )
#endif
	DEFINE_SCRIPTFUNC( GetStr, "Returns the convar as a string. May return null if no such convar." )
	DEFINE_SCRIPTFUNC( GetFloat, "Returns the convar as a float. May return null if no such convar." )
	DEFINE_SCRIPTFUNC( SetValue, "Sets the value of the convar. Supported types are bool, int, float, string." )
END_SCRIPTDESC();

#ifndef CLIENT_DLL
void EmitSoundOn( const char *pszSound, HSCRIPT hEnt )
{
	CBaseEntity *pEnt = ToEnt( hEnt );
	if (!hEnt || !pEnt)
		return;

	pEnt->EmitSound( pszSound );
}

void EmitSoundOnClient( const char *pszSound, HSCRIPT hEnt, HSCRIPT hPlayer )
{
	CBaseEntity *pEnt = ToEnt( hEnt );
	CBasePlayer *pPlayer = ToBasePlayer( ToEnt( hPlayer ) );
	if (!hPlayer || !pEnt || !pPlayer)
		return;

	CSingleUserRecipientFilter filter( pPlayer );

	EmitSound_t params;
	params.m_pSoundName = pszSound;
	params.m_flSoundTime = 0.0f;
	params.m_pflSoundDuration = NULL;
	params.m_bWarnOnDirectWaveReference = true;

	pEnt->EmitSound( filter, pEnt->entindex(), params );
}

void AddThinkToEnt( HSCRIPT entity, const char *pszFuncName )
{
	CBaseEntity *pEntity = ToEnt( entity );
	if (!pEntity)
		return;

	pEntity->ScriptSetThinkFunction(pszFuncName, 0.f);
}

HSCRIPT EntIndexToHScript( int index )
{
	return ToHScript( UTIL_EntityByIndex( index ) );
}

void ParseScriptTableKeyValues( CBaseEntity *pEntity, HSCRIPT hKV )
{
	int nIterator = -1;
	ScriptVariant_t varKey, varValue;
	while ((nIterator = g_pScriptVM->GetKeyValue( hKV, nIterator, &varKey, &varValue )) != -1)
	{
		switch (varValue.m_type)
		{
			case FIELD_CSTRING:		pEntity->KeyValue( varKey.m_pszString, varValue.m_pszString ); break;
			case FIELD_FLOAT:		pEntity->KeyValue( varKey.m_pszString, varValue.m_float ); break;
			case FIELD_VECTOR:		pEntity->KeyValue( varKey.m_pszString, *varValue.m_pVector ); break;
		}

		g_pScriptVM->ReleaseValue( varKey );
		g_pScriptVM->ReleaseValue( varValue );
	}
}

void PrecacheEntityFromTable( const char *pszClassname, HSCRIPT hKV )
{
	if ( IsEntityCreationAllowedInScripts() == false )
	{
		Warning( "VScript error: A script attempted to create an entity mid-game. Due to the server's settings, entity creation from scripts is only allowed during map init.\n" );
		return;
	}

	// This is similar to UTIL_PrecacheOther(), but we can't check if we can only precache it once.
	// Probably for the best anyway, as similar classes can still have different precachable properties.
	CBaseEntity *pEntity = CreateEntityByName( pszClassname );
	if (!pEntity)
	{
		Assert( !"PrecacheEntityFromTable: only works for CBaseEntities" );
		return;
	}

	ParseScriptTableKeyValues( pEntity, hKV );

	pEntity->Precache();

	UTIL_RemoveImmediate( pEntity );
}

HSCRIPT SpawnEntityFromTable( const char *pszClassname, HSCRIPT hKV )
{
	if ( IsEntityCreationAllowedInScripts() == false )
	{
		Warning( "VScript error: A script attempted to create an entity mid-game. Due to the server's settings, entity creation from scripts is only allowed during map init.\n" );
		return NULL;
	}

	CBaseEntity *pEntity = CreateEntityByName( pszClassname );
	if ( !pEntity )
	{
		Assert( !"SpawnEntityFromTable: only works for CBaseEntities" );
		return NULL;
	}

	gEntList.NotifyCreateEntity( pEntity );

	ParseScriptTableKeyValues( pEntity, hKV );

	DispatchSpawn( pEntity );
	pEntity->Activate();

	return ToHScript( pEntity );
}
#endif

//-----------------------------------------------------------------------------
// Mapbase-specific functions start here
//-----------------------------------------------------------------------------

static void ScriptColorPrint( int r, int g, int b, const char *pszMsg )
{
	const Color clr(r, g, b, 255);
	ConColorMsg( clr, "%s", pszMsg );
}

static void ScriptColorPrintL( int r, int g, int b, const char *pszMsg )
{
	const Color clr(r, g, b, 255);
	ConColorMsg( clr, "%s\n", pszMsg );
}

#ifndef CLIENT_DLL
HSCRIPT SpawnEntityFromKeyValues( const char *pszClassname, HSCRIPT hKV )
{
	if ( IsEntityCreationAllowedInScripts() == false )
	{
		Warning( "VScript error: A script attempted to create an entity mid-game. Due to the server's settings, entity creation from scripts is only allowed during map init.\n" );
		return NULL;
	}

	CBaseEntity *pEntity = CreateEntityByName( pszClassname );
	if ( !pEntity )
	{
		Assert( !"SpawnEntityFromKeyValues: only works for CBaseEntities" );
		return NULL;
	}

	gEntList.NotifyCreateEntity( pEntity );

	CScriptKeyValues *pScriptKV = hKV ? HScriptToClass<CScriptKeyValues>( hKV ) : NULL;
	if (pScriptKV)
	{
		KeyValues *pKV = pScriptKV->m_pKeyValues;
		for (pKV = pKV->GetFirstSubKey(); pKV != NULL; pKV = pKV->GetNextKey())
		{
			pEntity->KeyValue( pKV->GetName(), pKV->GetString() );
		}
	}

	DispatchSpawn( pEntity );
	pEntity->Activate();

	return ToHScript( pEntity );
}

void ScriptDispatchSpawn( HSCRIPT hEntity )
{
	CBaseEntity *pEntity = ToEnt( hEntity );
	if (pEntity)
	{
		DispatchSpawn( pEntity );
	}
}
#endif

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class CScriptLocalize
{
public:

	const char *GetTokenAsUTF8( const char *pszToken )
	{
		const char *pText = g_pVGuiLocalize->FindAsUTF8( pszToken );
		if ( pText )
		{
			return pText;
		}

		return NULL;
	}

	void AddStringAsUTF8( const char *pszToken, const char *pszString )
	{
		wchar_t wpszString[256];
		g_pVGuiLocalize->ConvertANSIToUnicode( pszString, wpszString, sizeof(wpszString) );

		// TODO: This is a fake file name! Should "fileName" mean anything?
		g_pVGuiLocalize->AddString( pszToken, wpszString, "resource/vscript_localization.txt" );
	}

private:
} g_ScriptLocalize;

BEGIN_SCRIPTDESC_ROOT_NAMED( CScriptLocalize, "CLocalize", SCRIPT_SINGLETON "Accesses functions related to localization strings." )

	DEFINE_SCRIPTFUNC( GetTokenAsUTF8, "Gets the current language's token as a UTF-8 string (not Unicode)." )

	DEFINE_SCRIPTFUNC( AddStringAsUTF8, "Adds a new localized token as a UTF-8 string (not Unicode)." )

END_SCRIPTDESC();

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static HSCRIPT CreateDamageInfo( HSCRIPT hInflictor, HSCRIPT hAttacker, const Vector &vecForce, const Vector &vecDamagePos, float flDamage, int iDamageType )
{
	// The script is responsible for deleting this via DestroyDamageInfo().
	CTakeDamageInfo *damageInfo = new CTakeDamageInfo(ToEnt(hInflictor), ToEnt(hAttacker), flDamage, iDamageType);
	HSCRIPT hScript = g_pScriptVM->RegisterInstance( damageInfo, true );

	damageInfo->SetDamagePosition( vecDamagePos );
	damageInfo->SetDamageForce( vecForce );

	return hScript;
}

static void DestroyDamageInfo( HSCRIPT hDamageInfo )
{
	if (hDamageInfo)
	{
		CTakeDamageInfo *pInfo = (CTakeDamageInfo*)g_pScriptVM->GetInstanceValue( hDamageInfo, GetScriptDescForClass( CTakeDamageInfo ) );
		if (pInfo)
		{
			g_pScriptVM->RemoveInstance( hDamageInfo );
			delete pInfo;
		}
	}
}

void ScriptCalculateExplosiveDamageForce( HSCRIPT info, const Vector &vecDir, const Vector &vecForceOrigin, float flScale ) { CalculateExplosiveDamageForce( HScriptToClass<CTakeDamageInfo>(info), vecDir, vecForceOrigin, flScale ); }
void ScriptCalculateBulletDamageForce( HSCRIPT info, int iBulletType, const Vector &vecBulletDir, const Vector &vecForceOrigin, float flScale ) { CalculateBulletDamageForce( HScriptToClass<CTakeDamageInfo>(info), iBulletType, vecBulletDir, vecForceOrigin, flScale ); }
void ScriptCalculateMeleeDamageForce( HSCRIPT info, const Vector &vecMeleeDir, const Vector &vecForceOrigin, float flScale ) { CalculateMeleeDamageForce( HScriptToClass<CTakeDamageInfo>( info ), vecMeleeDir, vecForceOrigin, flScale ); }
void ScriptGuessDamageForce( HSCRIPT info, const Vector &vecForceDir, const Vector &vecForceOrigin, float flScale ) { GuessDamageForce( HScriptToClass<CTakeDamageInfo>( info ), vecForceDir, vecForceOrigin, flScale ); }

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BEGIN_SCRIPTDESC_ROOT_NAMED( CTraceInfoAccessor, "CGameTrace", "Handle for accessing trace_t info." )
	DEFINE_SCRIPT_CONSTRUCTOR()

	DEFINE_SCRIPTFUNC( DidHitWorld, "Returns whether the trace hit the world entity or not." )
	DEFINE_SCRIPTFUNC( DidHitNonWorldEntity, "Returns whether the trace hit something other than the world entity." )
	DEFINE_SCRIPTFUNC( GetEntityIndex, "Returns the index of whatever entity this trace hit." )
	DEFINE_SCRIPTFUNC( DidHit, "Returns whether the trace hit anything." )

	DEFINE_SCRIPTFUNC( FractionLeftSolid, "If this trace started within a solid, this is the point in the trace's fraction at which it left that solid." )
	DEFINE_SCRIPTFUNC( HitGroup, "Returns the specific hit group this trace hit if it hit an entity." )
	DEFINE_SCRIPTFUNC( PhysicsBone, "Returns the physics bone this trace hit if it hit an entity." )
	DEFINE_SCRIPTFUNC( Entity, "Returns the entity this trace has hit." )
	DEFINE_SCRIPTFUNC( HitBox, "Returns the hitbox of the entity this trace has hit. If it hit the world entity, this returns the static prop index." )

	DEFINE_SCRIPTFUNC( IsDispSurface, "Returns whether this trace hit a displacement." )
	DEFINE_SCRIPTFUNC( IsDispSurfaceWalkable, "Returns whether DISPSURF_FLAG_WALKABLE is ticked on the displacement this trace hit." )
	DEFINE_SCRIPTFUNC( IsDispSurfaceBuildable, "Returns whether DISPSURF_FLAG_BUILDABLE is ticked on the displacement this trace hit." )
	DEFINE_SCRIPTFUNC( IsDispSurfaceProp1, "Returns whether DISPSURF_FLAG_SURFPROP1 is ticked on the displacement this trace hit." )
	DEFINE_SCRIPTFUNC( IsDispSurfaceProp2, "Returns whether DISPSURF_FLAG_SURFPROP2 is ticked on the displacement this trace hit." )

	DEFINE_SCRIPTFUNC( StartPos, "Gets the trace's start position." )
	DEFINE_SCRIPTFUNC( EndPos, "Gets the trace's end position." )

	DEFINE_SCRIPTFUNC( Fraction, "Gets the fraction of the trace completed. For example, if the trace stopped exactly halfway to the end position, this would be 0.5." )
	DEFINE_SCRIPTFUNC( Contents, "Gets the contents of the surface the trace has hit." )
	DEFINE_SCRIPTFUNC( DispFlags, "Gets the displacement flags of the surface the trace has hit." )

	DEFINE_SCRIPTFUNC( AllSolid, "Returns whether the trace is completely within a solid." )
	DEFINE_SCRIPTFUNC( StartSolid, "Returns whether the trace started within a solid." )

	DEFINE_SCRIPTFUNC( Surface, "Returns the trace's surface." )
	DEFINE_SCRIPTFUNC( Plane, "Returns the trace's plane." )

	DEFINE_SCRIPTFUNC( Destroy, "Deletes this instance. Important for preventing memory leaks." )
END_SCRIPTDESC();

BEGIN_SCRIPTDESC_ROOT_NAMED( surfacedata_t, "surfacedata_t", "Handle for accessing surface data." )
	DEFINE_SCRIPTFUNC( GetFriction, "The surface's friction." )
	DEFINE_SCRIPTFUNC( GetThickness, "The surface's thickness." )

	DEFINE_SCRIPTFUNC( GetJumpFactor, "The surface's jump factor." )
	DEFINE_SCRIPTFUNC( GetMaterialChar, "The surface's material character." )
END_SCRIPTDESC();

BEGIN_SCRIPTDESC_ROOT_NAMED( CSurfaceScriptAccessor, "csurface_t", "Handle for accessing csurface_t info." )
	DEFINE_SCRIPTFUNC( Name, "The surface's name." )
	DEFINE_SCRIPTFUNC( SurfaceProps, "The surface's properties." )

	DEFINE_SCRIPTFUNC( Destroy, "Deletes this instance. Important for preventing memory leaks." )
END_SCRIPTDESC();

CPlaneTInstanceHelper g_PlaneTInstanceHelper;

BEGIN_SCRIPTDESC_ROOT( cplane_t, "Handle for accessing cplane_t info." )
	DEFINE_SCRIPT_INSTANCE_HELPER( &g_PlaneTInstanceHelper )
END_SCRIPTDESC();

static HSCRIPT ScriptTraceLineComplex( const Vector &vecStart, const Vector &vecEnd, HSCRIPT entIgnore, int iMask, int iCollisionGroup )
{
	// The script is responsible for deleting this via Destroy().
	CTraceInfoAccessor *traceInfo = new CTraceInfoAccessor();
	HSCRIPT hScript = g_pScriptVM->RegisterInstance( traceInfo, true );

	CBaseEntity *pLooker = ToEnt(entIgnore);
	UTIL_TraceLine( vecStart, vecEnd, iMask, pLooker, iCollisionGroup, &traceInfo->GetTrace());

	// The trace's destruction should destroy this automatically
	CSurfaceScriptAccessor *surfaceInfo = new CSurfaceScriptAccessor( traceInfo->GetTrace().surface );
	HSCRIPT hSurface = g_pScriptVM->RegisterInstance( surfaceInfo );
	traceInfo->SetSurface( hSurface );

	HSCRIPT hPlane = g_pScriptVM->RegisterInstance( &(traceInfo->GetTrace().plane) );
	traceInfo->SetPlane( hPlane );

	return hScript;
}

static HSCRIPT ScriptTraceHullComplex( const Vector &vecStart, const Vector &vecEnd, const Vector &hullMin, const Vector &hullMax,
	HSCRIPT entIgnore, int iMask, int iCollisionGroup )
{
	// The script is responsible for deleting this via Destroy().
	CTraceInfoAccessor *traceInfo = new CTraceInfoAccessor();
	HSCRIPT hScript = g_pScriptVM->RegisterInstance( traceInfo, true );

	CBaseEntity *pLooker = ToEnt(entIgnore);
	UTIL_TraceHull( vecStart, vecEnd, hullMin, hullMax, iMask, pLooker, iCollisionGroup, &traceInfo->GetTrace());

	// The trace's destruction should destroy this automatically
	CSurfaceScriptAccessor *surfaceInfo = new CSurfaceScriptAccessor( traceInfo->GetTrace().surface );
	HSCRIPT hSurface = g_pScriptVM->RegisterInstance( surfaceInfo );
	traceInfo->SetSurface( hSurface );

	HSCRIPT hPlane = g_pScriptVM->RegisterInstance( &(traceInfo->GetTrace().plane) );
	traceInfo->SetPlane( hPlane );

	return hScript;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BEGIN_SCRIPTDESC_ROOT( FireBulletsInfo_t, "Handle for accessing FireBulletsInfo_t info." )
	DEFINE_SCRIPT_CONSTRUCTOR()

	DEFINE_SCRIPTFUNC( GetShots, "Gets the number of shots which should be fired." )
	DEFINE_SCRIPTFUNC( SetShots, "Sets the number of shots which should be fired." )

	DEFINE_SCRIPTFUNC( GetSource, "Gets the source of the bullets." )
	DEFINE_SCRIPTFUNC( SetSource, "Sets the source of the bullets." )
	DEFINE_SCRIPTFUNC( GetDirShooting, "Gets the direction of the bullets." )
	DEFINE_SCRIPTFUNC( SetDirShooting, "Sets the direction of the bullets." )
	DEFINE_SCRIPTFUNC( GetSpread, "Gets the spread of the bullets." )
	DEFINE_SCRIPTFUNC( SetSpread, "Sets the spread of the bullets." )

	DEFINE_SCRIPTFUNC( GetDistance, "Gets the distance the bullets should travel." )
	DEFINE_SCRIPTFUNC( SetDistance, "Sets the distance the bullets should travel." )

	DEFINE_SCRIPTFUNC( GetAmmoType, "Gets the ammo type the bullets should use." )
	DEFINE_SCRIPTFUNC( SetAmmoType, "Sets the ammo type the bullets should use." )

	DEFINE_SCRIPTFUNC( GetTracerFreq, "Gets the tracer frequency." )
	DEFINE_SCRIPTFUNC( SetTracerFreq, "Sets the tracer frequency." )

	DEFINE_SCRIPTFUNC( GetDamage, "Gets the damage the bullets should deal. 0 = use ammo type" )
	DEFINE_SCRIPTFUNC( SetDamage, "Sets the damage the bullets should deal. 0 = use ammo type" )
	DEFINE_SCRIPTFUNC( GetPlayerDamage, "Gets the damage the bullets should deal when hitting the player. 0 = use regular damage" )
	DEFINE_SCRIPTFUNC( SetPlayerDamage, "Sets the damage the bullets should deal when hitting the player. 0 = use regular damage" )

	DEFINE_SCRIPTFUNC( GetFlags, "Gets the flags the bullets should use." )
	DEFINE_SCRIPTFUNC( SetFlags, "Sets the flags the bullets should use." )

	DEFINE_SCRIPTFUNC( GetDamageForceScale, "Gets the scale of the damage force applied by the bullets." )
	DEFINE_SCRIPTFUNC( SetDamageForceScale, "Sets the scale of the damage force applied by the bullets." )

	DEFINE_SCRIPTFUNC_NAMED( ScriptGetAttacker, "GetAttacker", "Gets the entity considered to be the one who fired the bullets." )
	DEFINE_SCRIPTFUNC_NAMED( ScriptSetAttacker, "SetAttacker", "Sets the entity considered to be the one who fired the bullets." )
	DEFINE_SCRIPTFUNC_NAMED( ScriptGetAdditionalIgnoreEnt, "GetAdditionalIgnoreEnt", "Gets the optional entity which the bullets should ignore." )
	DEFINE_SCRIPTFUNC_NAMED( ScriptSetAdditionalIgnoreEnt, "SetAdditionalIgnoreEnt", "Sets the optional entity which the bullets should ignore." )

	DEFINE_SCRIPTFUNC( GetPrimaryAttack, "Gets whether the bullets came from a primary attack." )
	DEFINE_SCRIPTFUNC( SetPrimaryAttack, "Sets whether the bullets came from a primary attack." )

	//DEFINE_SCRIPTFUNC( Destroy, "Deletes this instance. Important for preventing memory leaks." )
END_SCRIPTDESC();

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
HSCRIPT FireBulletsInfo_t::ScriptGetAttacker()
{
	return ToHScript( m_pAttacker );
}

void FireBulletsInfo_t::ScriptSetAttacker( HSCRIPT value )
{
	m_pAttacker = ToEnt( value );
}

HSCRIPT FireBulletsInfo_t::ScriptGetAdditionalIgnoreEnt()
{
	return ToHScript( m_pAdditionalIgnoreEnt );
}

void FireBulletsInfo_t::ScriptSetAdditionalIgnoreEnt( HSCRIPT value )
{
	m_pAdditionalIgnoreEnt = ToEnt( value );
}

static HSCRIPT CreateFireBulletsInfo( int cShots, const Vector &vecSrc, const Vector &vecDirShooting,
	const Vector &vecSpread, float iDamage, HSCRIPT pAttacker )
{
	// The script is responsible for deleting this via DestroyFireBulletsInfo().
	FireBulletsInfo_t *info = new FireBulletsInfo_t();
	HSCRIPT hScript = g_pScriptVM->RegisterInstance( info, true );

	info->SetShots( cShots );
	info->SetSource( vecSrc );
	info->SetDirShooting( vecDirShooting );
	info->SetSpread( vecSpread );
	info->SetDamage( iDamage );
	info->ScriptSetAttacker( pAttacker );

	return hScript;
}

static void DestroyFireBulletsInfo( HSCRIPT hBulletsInfo )
{
	g_pScriptVM->RemoveInstance( hBulletsInfo );
}

// For the function in baseentity.cpp
FireBulletsInfo_t *GetFireBulletsInfoFromInfo( HSCRIPT hBulletsInfo )
{
	return HScriptToClass<FireBulletsInfo_t>( hBulletsInfo );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BEGIN_SCRIPTDESC_ROOT( CUserCmd, "Handle for accessing CUserCmd info." )
	DEFINE_SCRIPTFUNC( GetCommandNumber, "For matching server and client commands for debugging." )

	DEFINE_SCRIPTFUNC_NAMED( ScriptGetTickCount, "GetTickCount", "The tick the client created this command." )

	DEFINE_SCRIPTFUNC( GetViewAngles, "Player instantaneous view angles." )
	DEFINE_SCRIPTFUNC( SetViewAngles, "Sets player instantaneous view angles." )

	DEFINE_SCRIPTFUNC( GetForwardMove, "Forward velocity." )
	DEFINE_SCRIPTFUNC( SetForwardMove, "Sets forward velocity." )
	DEFINE_SCRIPTFUNC( GetSideMove, "Side velocity." )
	DEFINE_SCRIPTFUNC( SetSideMove, "Sets side velocity." )
	DEFINE_SCRIPTFUNC( GetUpMove, "Up velocity." )
	DEFINE_SCRIPTFUNC( SetUpMove, "Sets up velocity." )

	DEFINE_SCRIPTFUNC( GetButtons, "Attack button states." )
	DEFINE_SCRIPTFUNC( SetButtons, "Sets attack button states." )
	DEFINE_SCRIPTFUNC( GetImpulse, "Impulse command issued." )
	DEFINE_SCRIPTFUNC( SetImpulse, "Sets impulse command issued." )

	DEFINE_SCRIPTFUNC( GetWeaponSelect, "Current weapon id." )
	DEFINE_SCRIPTFUNC( SetWeaponSelect, "Sets current weapon id." )
	DEFINE_SCRIPTFUNC( GetWeaponSubtype, "Current weapon subtype id." )
	DEFINE_SCRIPTFUNC( SetWeaponSubtype, "Sets current weapon subtype id." )

	DEFINE_SCRIPTFUNC( GetRandomSeed, "For shared random functions." )

	DEFINE_SCRIPTFUNC( GetMouseX, "Mouse accum in x from create move." )
	DEFINE_SCRIPTFUNC( SetMouseX, "Sets mouse accum in x from create move." )
	DEFINE_SCRIPTFUNC( GetMouseY, "Mouse accum in y from create move." )
	DEFINE_SCRIPTFUNC( SetMouseY, "Sets mouse accum in y from create move." )
END_SCRIPTDESC();

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
BEGIN_SCRIPTDESC_ROOT( IPhysicsObject, "VPhysics object class." )

	DEFINE_SCRIPTFUNC( IsStatic, "" )
	DEFINE_SCRIPTFUNC( IsAsleep, "" )
	DEFINE_SCRIPTFUNC( IsTrigger, "" )
	DEFINE_SCRIPTFUNC( IsFluid, "" )
	DEFINE_SCRIPTFUNC( IsHinged, "" )
	DEFINE_SCRIPTFUNC( IsCollisionEnabled, "" )
	DEFINE_SCRIPTFUNC( IsGravityEnabled, "" )
	DEFINE_SCRIPTFUNC( IsDragEnabled, "" )
	DEFINE_SCRIPTFUNC( IsMotionEnabled, "" )
	DEFINE_SCRIPTFUNC( IsMoveable, "" )
	DEFINE_SCRIPTFUNC( IsAttachedToConstraint, "" )

	DEFINE_SCRIPTFUNC( EnableCollisions, "" )
	DEFINE_SCRIPTFUNC( EnableGravity, "" )
	DEFINE_SCRIPTFUNC( EnableDrag, "" )
	DEFINE_SCRIPTFUNC( EnableMotion, "" )

	DEFINE_SCRIPTFUNC( Wake, "" )
	DEFINE_SCRIPTFUNC( Sleep, "" )

	DEFINE_SCRIPTFUNC( SetMass, "" )
	DEFINE_SCRIPTFUNC( GetMass, "" )
	DEFINE_SCRIPTFUNC( GetInvMass, "" )
	DEFINE_SCRIPTFUNC( GetInertia, "" )
	DEFINE_SCRIPTFUNC( GetInvInertia, "" )
	DEFINE_SCRIPTFUNC( SetInertia, "" )

	DEFINE_SCRIPTFUNC( ApplyForceCenter, "" )
	DEFINE_SCRIPTFUNC( ApplyForceOffset, "" )
	DEFINE_SCRIPTFUNC( ApplyTorqueCenter, "" )

	DEFINE_SCRIPTFUNC( GetName, "" )

END_SCRIPTDESC();

static const Vector &GetPhysVelocity( HSCRIPT hPhys )
{
	IPhysicsObject *pPhys = HScriptToClass<IPhysicsObject>( hPhys );
	if (!pPhys)
		return vec3_origin;

	static Vector vecVelocity;
	pPhys->GetVelocity( &vecVelocity, NULL );
	return vecVelocity;
}

static const Vector &GetPhysAngVelocity( HSCRIPT hPhys )
{
	IPhysicsObject *pPhys = HScriptToClass<IPhysicsObject>( hPhys );
	if (!pPhys)
		return vec3_origin;

	static Vector vecAngVelocity;
	pPhys->GetVelocity( NULL, &vecAngVelocity );
	return vecAngVelocity;
}

static void SetPhysVelocity( HSCRIPT hPhys, const Vector& vecVelocity, const Vector& vecAngVelocity )
{
	IPhysicsObject *pPhys = HScriptToClass<IPhysicsObject>( hPhys );
	if (!pPhys)
		return;

	if (pPhys)
		pPhys->SetVelocity( &vecVelocity, &vecAngVelocity );
}

static void AddPhysVelocity( HSCRIPT hPhys, const Vector& vecVelocity, const Vector& vecAngVelocity )
{
	IPhysicsObject *pPhys = HScriptToClass<IPhysicsObject>( hPhys );
	if (!pPhys)
		return;

	if (pPhys)
		pPhys->AddVelocity( &vecVelocity, &vecAngVelocity );
}

//=============================================================================
//=============================================================================

static int ScriptPrecacheModel( const char *modelname )
{
	return CBaseEntity::PrecacheModel( modelname );
}

static void ScriptPrecacheOther( const char *classname )
{
	UTIL_PrecacheOther( classname );
}

//=============================================================================
//=============================================================================

static void ScriptEntitiesInBox( HSCRIPT hTable, int listMax, const Vector &hullMin, const Vector &hullMax, int iMask )
{
	CBaseEntity *list[1024];
	int count = UTIL_EntitiesInBox( list, listMax, hullMin, hullMax, iMask );

	for ( int i = 0; i < count; i++ )
	{
		g_pScriptVM->SetValue( hTable, STRING(list[i]->GetEntityName()), ToHScript( list[i] ) );
	}
}

static void ScriptEntitiesAtPoint( HSCRIPT hTable, int listMax, const Vector &point, int iMask )
{
	CBaseEntity *list[1024];
	int count = UTIL_EntitiesAtPoint( list, listMax, point, iMask );

	for ( int i = 0; i < count; i++ )
	{
		g_pScriptVM->SetValue( hTable, STRING(list[i]->GetEntityName()), ToHScript( list[i] ) );
	}
}

static void ScriptEntitiesInSphere( HSCRIPT hTable, int listMax, const Vector &center, float radius, int iMask )
{
	CBaseEntity *list[1024];
	int count = UTIL_EntitiesInSphere( list, listMax, center, radius, iMask );

	for ( int i = 0; i < count; i++ )
	{
		g_pScriptVM->SetValue( hTable, STRING(list[i]->GetEntityName()), ToHScript( list[i] ) );
	}
}

//-----------------------------------------------------------------------------

static void ScriptDecalTrace( HSCRIPT hTrace, const char *decalName )
{
	CTraceInfoAccessor *traceInfo = HScriptToClass<CTraceInfoAccessor>(hTrace);
	UTIL_DecalTrace( &traceInfo->GetTrace(), decalName );
}

//=============================================================================
//=============================================================================

bool ScriptMatcherMatch( const char *pszQuery, const char *szValue ) { return Matcher_Match( pszQuery, szValue ); }

//=============================================================================
//=============================================================================

#ifndef CLIENT_DLL
bool IsDedicatedServer()
{
	return engine->IsDedicatedServer();
}
#endif

bool ScriptIsServer()
{
#ifdef GAME_DLL
	return true;
#else
	return false;
#endif
}

bool ScriptIsClient()
{
#ifdef CLIENT_DLL
	return true;
#else
	return false;
#endif
}

// Notification printing on the right edge of the screen
void NPrint(int pos, const char* fmt)
{
	engine->Con_NPrintf(pos, fmt);
}

//=============================================================================
//=============================================================================

extern void RegisterMathScriptFunctions();

void RegisterSharedScriptFunctions()
{
	// 
	// Due to this being a custom integration of VScript based on the Alien Swarm SDK, we don't have access to
	// some of the code normally available in games like L4D2 or Valve's original VScript DLL.
	// Instead, that code is recreated here, shared between server and client.
	// 

#ifndef CLIENT_DLL
	ScriptRegisterFunction( g_pScriptVM, EmitSoundOn, "Play named sound on an entity." );
	ScriptRegisterFunction( g_pScriptVM, EmitSoundOnClient, "Play named sound only on the client for the specified player." );

	ScriptRegisterFunction( g_pScriptVM, AddThinkToEnt, "This will put a think function onto an entity, or pass null to remove it. This is NOT chained, so be careful." );
	ScriptRegisterFunction( g_pScriptVM, EntIndexToHScript, "Returns the script handle for the given entity index." );
	ScriptRegisterFunction( g_pScriptVM, PrecacheEntityFromTable, "Precache an entity from KeyValues in a table." );
	ScriptRegisterFunction( g_pScriptVM, SpawnEntityFromTable, "Native function for entity spawning." );
#endif

	g_pScriptVM->RegisterInstance( &g_ScriptConvarLookup, "Convars" );
	g_pScriptVM->RegisterInstance( &g_ScriptNetPropManager, "NetProps" );

	//-----------------------------------------------------------------------------
	// Functions, singletons, etc. unique to Mapbase
	//-----------------------------------------------------------------------------

	g_pScriptVM->RegisterInstance( GetAmmoDef(), "AmmoDef" );
	g_pScriptVM->RegisterInstance( &g_ScriptLocalize, "Localize" );

	//-----------------------------------------------------------------------------

	ScriptRegisterFunction( g_pScriptVM, NPrint, "" );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptColorPrint, "printc", "Version of print() which takes a color before the message." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptColorPrintL, "printcl", "Version of printl() which takes a color before the message." );

#ifndef CLIENT_DLL
	ScriptRegisterFunction( g_pScriptVM, SpawnEntityFromKeyValues, "Spawns an entity with the keyvalues in a CScriptKeyValues handle." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptDispatchSpawn, "DispatchSpawn", "Spawns an unspawned entity." );
#endif

	ScriptRegisterFunction( g_pScriptVM, CreateDamageInfo, "Creates damage info." );
	ScriptRegisterFunction( g_pScriptVM, DestroyDamageInfo, "Destroys damage info." );
	ScriptRegisterFunction( g_pScriptVM, ImpulseScale, "Returns an impulse scale required to push an object." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptCalculateExplosiveDamageForce, "CalculateExplosiveDamageForce", "Fill out a damage info handle with a damage force for an explosive." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptCalculateBulletDamageForce, "CalculateBulletDamageForce", "Fill out a damage info handle with a damage force for a bullet impact." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptCalculateMeleeDamageForce, "CalculateMeleeDamageForce", "Fill out a damage info handle with a damage force for a melee impact." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptGuessDamageForce, "GuessDamageForce", "Try and guess the physics force to use." );

	ScriptRegisterFunction( g_pScriptVM, CreateFireBulletsInfo, "Creates FireBullets info." );
	ScriptRegisterFunction( g_pScriptVM, DestroyFireBulletsInfo, "Destroys FireBullets info." );

	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptTraceLineComplex, "TraceLineComplex", "Complex version of TraceLine which takes 2 points, an ent to ignore, a trace mask, and a collision group. Returns a handle which can access all trace info." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptTraceHullComplex, "TraceHullComplex", "Takes 2 points, min/max hull bounds, an ent to ignore, a trace mask, and a collision group to trace to a point using a hull. Returns a handle which can access all trace info." );

	// 
	// VPhysics
	// 
	ScriptRegisterFunction( g_pScriptVM, GetPhysVelocity, "Gets physics velocity for the given VPhysics object" );
	ScriptRegisterFunction( g_pScriptVM, GetPhysAngVelocity, "Gets physics angular velocity for the given VPhysics object" );
	ScriptRegisterFunction( g_pScriptVM, SetPhysVelocity, "Sets physics velocity for the given VPhysics object" );
	ScriptRegisterFunction( g_pScriptVM, AddPhysVelocity, "Adds physics velocity for the given VPhysics object" );

	// 
	// Precaching
	// 
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptPrecacheModel, "PrecacheModel", "Precaches a model for later usage." );
	ScriptRegisterFunction( g_pScriptVM, PrecacheMaterial, "Precaches a material for later usage." );
	ScriptRegisterFunction( g_pScriptVM, PrecacheParticleSystem, "Precaches a particle system for later usage." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptPrecacheOther, "PrecacheOther", "Precaches an entity class for later usage." );

	// 
	// Misc. Utility
	// 
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptEntitiesInBox, "EntitiesInBox", "Gets all entities which are within a worldspace box." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptEntitiesAtPoint, "EntitiesAtPoint", "Gets all entities which are intersecting a point in space." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptEntitiesInSphere, "EntitiesInSphere", "Gets all entities which are within a sphere." );

	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptDecalTrace, "DecalTrace", "Creates a dynamic decal based on the given trace info. The trace information can be generated by TraceLineComplex() and the decal name must be from decals_subrect.txt." );

	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptMatcherMatch, "Matcher_Match", "Compares a string to a query using Mapbase's matcher system, supporting wildcards, RS matchers, etc." );
	ScriptRegisterFunction( g_pScriptVM, Matcher_NamesMatch, "Compares a string to a query using Mapbase's matcher system using wildcards only." );
	ScriptRegisterFunction( g_pScriptVM, AppearsToBeANumber, "Checks if the given string appears to be a number." );

#ifndef CLIENT_DLL
	ScriptRegisterFunction( g_pScriptVM, IsDedicatedServer, "Is this a dedicated server?" );
#endif
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptIsServer, "IsServer", "Returns true if the script is being run on the server." );
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptIsClient, "IsClient", "Returns true if the script is being run on the client." );

	RegisterMathScriptFunctions();

#ifdef CLIENT_DLL
	VScriptRunScript( "vscript_client", true );
#else
	VScriptRunScript( "vscript_server", true );
#endif
}
