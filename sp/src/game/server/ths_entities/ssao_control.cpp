//========= Copyright [THS] inc 2020, All rights reserved. ============//
//
// Purpose: SSAO control entity.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//------------------------------------------------------------------------------
// Purpose : SSAO control entity
//------------------------------------------------------------------------------
class CSSAOControl : public CBaseEntity
{
public:
	DECLARE_CLASS( CSSAOControl, CBaseEntity );

	CSSAOControl();

	void Spawn( void );
	int  UpdateTransmitState();

	virtual int	ObjectCaps( void ) { return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

private:
	CNetworkVar( bool, ths_ssao_enable );
};

LINK_ENTITY_TO_CLASS(ssao_control, CSSAOControl);

BEGIN_DATADESC( CSSAOControl )

	DEFINE_KEYFIELD( ths_ssao_enable, FIELD_BOOLEAN, "ssaoenable" ),

	// Inputs
	DEFINE_INPUT( ths_ssao_enable,	FIELD_BOOLEAN, "SetSSAOEnabled" ),

END_DATADESC()


IMPLEMENT_SERVERCLASS_ST_NOBASE(CSSAOControl, DT_SSAOControl)
	SendPropBool(SENDINFO(ths_ssao_enable)),
END_SEND_TABLE()


CSSAOControl::CSSAOControl()
{
	ths_ssao_enable = false;
}


//------------------------------------------------------------------------------
// Purpose : Send even though we don't have a model
//------------------------------------------------------------------------------
int CSSAOControl::UpdateTransmitState()
{
	// ALWAYS transmit to all clients.
	return SetTransmitState( FL_EDICT_ALWAYS );
}


//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CSSAOControl::Spawn( void )
{
	Precache();
	SetSolid( SOLID_NONE );
}