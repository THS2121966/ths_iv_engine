#if !defined ( RAIN_CONTROL_H )
#define RAIN_CONTROL_H
#ifdef _WIN32
#pragma once
#endif


//-----------------------------------------------------------------------------
// Purpose: Entity that controls rain splashes
//-----------------------------------------------------------------------------
class CRainControl : public CPointEntity
{
	DECLARE_CLASS( CRainControl, CPointEntity );
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	virtual void	Spawn( void );
	virtual int		UpdateTransmitState( void );

	// Inputs

	CNetworkVar( float, thsdev_rain_splash_chance_local );
};

#endif//  RAIN_CONTROL_H