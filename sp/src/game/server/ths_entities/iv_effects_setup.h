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
	CNetworkVar( int, thsdev_mapping_numb_shadows_local );
//	CNetworkVar( float, m_flShadowFilterSize_local );

	CNetworkVar( bool, ths_water_fx_init_local );
	CNetworkVar( int, ths_blood_fx_init_local );
};

#endif//  IV_EFFECTS_SETUP_H