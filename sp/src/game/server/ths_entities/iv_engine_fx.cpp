//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "baseentity.h"
#include "entityoutput.h"
//#include "convar.h"
#include "iv_engine_fx.h"
#include "ai_utils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( iv_engine_fx, CIVFXControl );

BEGIN_DATADESC( CIVFXControl )
	
	DEFINE_KEYFIELD( iv_fx_local_fgrain,			FIELD_INTEGER,	"SetupFilmGrainFX" ),
	DEFINE_KEYFIELD( iv_fx_local_exp,			FIELD_INTEGER,	"SetupExponentFX" ),
	DEFINE_KEYFIELD( iv_fx_local_flare,			FIELD_INTEGER,	"SetupFlareFX" ),

	// Inputs
	DEFINE_INPUT( iv_fx_local_fgrain,	FIELD_INTEGER, "SetParmFilmGrainFX" ),
	DEFINE_INPUT( iv_fx_local_exp,	FIELD_INTEGER, "SetParmExponentFX" ),
	DEFINE_INPUT( iv_fx_local_flare,	FIELD_INTEGER, "SetParmFlareFX" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CIVFXControl, DT_IVFXControl )
	SendPropInt( SENDINFO(iv_fx_local_fgrain), 0, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO(iv_fx_local_exp), 0, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO(iv_fx_local_flare), 0, SPROP_UNSIGNED ),
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CIVFXControl::Spawn( void )
{
	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CIVFXControl::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}