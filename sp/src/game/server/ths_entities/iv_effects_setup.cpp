//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "baseentity.h"
#include "entityoutput.h"
//#include "convar.h"
#include "iv_effects_setup.h"
#include "ai_utils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( iv_effects_main_setup, CIVEffectsMain );

BEGIN_DATADESC( CIVEffectsMain )
	
	DEFINE_KEYFIELD( ths_enable_expfx_local,			FIELD_BOOLEAN,	"EnableEnchancedExpFX" ),

	// Inputs
	DEFINE_INPUT( ths_enable_expfx_local,	FIELD_BOOLEAN, "SetEnchancedExpFXEnabled" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CIVEffectsMain, DT_IVEffectsMain )
	SendPropInt( SENDINFO(ths_enable_expfx_local), 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CIVEffectsMain::Spawn( void )
{
	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CIVEffectsMain::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}