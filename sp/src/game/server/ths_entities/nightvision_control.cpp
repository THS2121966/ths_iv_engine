//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "baseentity.h"
#include "entityoutput.h"
//#include "convar.h"
#include "nightvision_control.h"
#include "ai_utils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( nightvision_control, CNVControl );

BEGIN_DATADESC( CNVControl )
	
	DEFINE_KEYFIELD( ths_local_nightvision_init,			FIELD_BOOLEAN,	"EnableNVEffect" ),

	// Inputs
	DEFINE_INPUT( ths_local_nightvision_init,	FIELD_BOOLEAN, "SetNVEnabled" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CNVControl, DT_NVControl )
	SendPropInt( SENDINFO(ths_local_nightvision_init), 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CNVControl::Spawn( void )
{
	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CNVControl::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}