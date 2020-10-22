#if !defined ( IV_EFFECTS_SETUP_H )
#define IV_EFFECTS_SETUP_H
#ifdef _WIN32
#pragma once
#endif


//-----------------------------------------------------------------------------
// Purpose: Entity that controls iv_engine_main_effects.
//-----------------------------------------------------------------------------
class CIVEffectsMain : public CPointEntity
{
	DECLARE_CLASS( CIVEffectsMain, CPointEntity );
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	virtual void	Spawn( void );
	virtual int		UpdateTransmitState( void );

	// Inputs


	CNetworkVar( bool, ths_enable_expfx_local );
};

#endif//  IV_EFFECTS_SETUP_H