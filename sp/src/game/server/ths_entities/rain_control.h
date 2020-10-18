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
	void	UpdateRainOutputThink( void );
	virtual int		UpdateTransmitState( void );

	// Inputs

	CNetworkVar( float, thsdev_rain_splash_chance_local );
	
	// Outputs
	COutputEvent thsdev_rain_splash_output;
	COutputEvent thsdev_rain_splash_output_none;
};

#endif//  RAIN_CONTROL_H