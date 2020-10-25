#if !defined ( NIGHTVISION_CONTROL_H )
#define NIGHTVISION_CONTROL_H
#ifdef _WIN32
#pragma once
#endif


//-----------------------------------------------------------------------------
// Purpose: Entity that controls NightVision postprocessing
//-----------------------------------------------------------------------------
class CNVControl : public CPointEntity
{
	DECLARE_CLASS( CNVControl, CPointEntity );
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	virtual void	Spawn( void );
	virtual int		UpdateTransmitState( void );

	// Inputs


	CNetworkVar( bool, ths_local_nightvision_init );
};

#endif//  NIGHTVISION_CONTROL_H