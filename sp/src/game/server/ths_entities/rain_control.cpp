//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "baseentity.h"
#include "entityoutput.h"
//#include "convar.h"
#include "rain_control.h"
#include "ai_utils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( rain_control, CRainControl );

BEGIN_DATADESC( CRainControl )
	
	DEFINE_KEYFIELD( thsdev_rain_splash_chance_local,			FIELD_FLOAT,	"EnableRainSplash" ),

	// Inputs
	DEFINE_INPUT( thsdev_rain_splash_chance_local,	FIELD_FLOAT, "SetRainSplashEnabled" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CRainControl, DT_RainControl )
	SendPropInt( SENDINFO(thsdev_rain_splash_chance_local), 25, SPROP_UNSIGNED ),
END_SEND_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CRainControl::Spawn( void )
{
	SetSolid( SOLID_NONE );
	SetMoveType( MOVETYPE_NONE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CRainControl::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}