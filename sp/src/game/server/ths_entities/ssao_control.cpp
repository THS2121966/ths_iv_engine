//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "baseentity.h"
#include "entityoutput.h"
//#include "convar.h"
#include "ssao_control.h"
#include "ai_utils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( ssao_control, CSSAOControl );

BEGIN_DATADESC( CSSAOControl )
	
	DEFINE_KEYFIELD( ths_local_ssao_init,			FIELD_BOOLEAN,	"EnableSSAOEffect" ),

	// Inputs
	DEFINE_INPUT( ths_local_ssao_init,	FIELD_BOOLEAN, "SetSSAOEnabled" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CSSAOControl, DT_SSAOControl )
	SendPropInt( SENDINFO(ths_local_ssao_init), 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CSSAOControl::Spawn( void )
{
	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CSSAOControl::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}