#if !defined ( SSAO_CONTROL_H )
#define SSAO_CONTROL_H
#ifdef _WIN32
#pragma once
#endif


//-----------------------------------------------------------------------------
// Purpose: Entity that controls depth of field postprocessing
//-----------------------------------------------------------------------------
class CSSAOControl : public CPointEntity
{
	DECLARE_CLASS( CSSAOControl, CPointEntity );
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	virtual void	Spawn( void );
	virtual int		UpdateTransmitState( void );

	// Inputs


	CNetworkVar( bool, ths_local_ssao_init );
};

#endif//  SSAO_CONTROL_H