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
	DEFINE_KEYFIELD( thsdev_mapping_numb_shadows_local,			FIELD_INTEGER,	"SetMaxShadowedLights" ),
	DEFINE_KEYFIELD( thsdev_mapping_shadows_res_local,			FIELD_INTEGER,	"PJShadowRes" ),
//	DEFINE_KEYFIELD( m_flShadowFilterSize_local,			FIELD_FLOAT,	"SetDynShadowFilterSize" ),

	DEFINE_KEYFIELD( ths_water_fx_init_local,			FIELD_BOOLEAN,	"EnableScreenWaterFX" ),
	DEFINE_KEYFIELD( ths_blood_fx_init_local,			FIELD_BOOLEAN,	"EnableScreenBloodFX" ),

	// Inputs
	DEFINE_INPUT( ths_enable_expfx_local,	FIELD_BOOLEAN, "SetEnchancedExpFXEnabled" ),
//	DEFINE_INPUT( m_flShadowFilterSize_local,	FIELD_FLOAT, "InputSetDynShadowsFilterSize" ),

	DEFINE_INPUT( ths_water_fx_init_local,	FIELD_BOOLEAN, "SetEnabledScreenWaterFX" ),
	DEFINE_INPUT( ths_blood_fx_init_local,	FIELD_BOOLEAN, "SetEnabledScreenBloodFX" ),
	DEFINE_INPUT( thsdev_mapping_shadows_res_local,	FIELD_INTEGER, "SetPJShadowRes" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CIVEffectsMain, DT_IVEffectsMain )
	SendPropInt( SENDINFO(ths_enable_expfx_local), 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO(thsdev_mapping_numb_shadows_local), 5, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO(thsdev_mapping_shadows_res_local), 2048, SPROP_UNSIGNED ),
//	SendPropInt( SENDINFO(m_flShadowFilterSize_local), 0.7, SPROP_UNSIGNED ),

	SendPropInt( SENDINFO(ths_water_fx_init_local), 0, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO(ths_blood_fx_init_local), 0, SPROP_UNSIGNED ),
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