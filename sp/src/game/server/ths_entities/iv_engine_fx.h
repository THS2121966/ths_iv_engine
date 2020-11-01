#if !defined ( IV_ENGINE_FX_H )
#define IV_ENGINE_FX_H
#ifdef _WIN32
#pragma once
#endif


//-----------------------------------------------------------------------------
// Purpose: Entity that controls IV Engine Main FX
//-----------------------------------------------------------------------------
class CIVFXControl : public CPointEntity
{
	DECLARE_CLASS( CIVFXControl, CPointEntity );
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	virtual void	Spawn( void );
	virtual int		UpdateTransmitState( void );

	// Inputs

	CNetworkVar( int, iv_fx_local_fgrain );
	CNetworkVar( int, iv_fx_local_exp );
	CNetworkVar( int, iv_fx_local_flare );
};

#endif//  IV_ENGINE_FX_H