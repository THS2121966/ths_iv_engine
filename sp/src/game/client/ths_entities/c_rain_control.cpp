//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"


extern float  thsdev_rain_splash_chance;

EHANDLE g_RainControlInUse = NULL;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_RainControl : public C_BaseEntity
{
	DECLARE_CLASS( C_RainControl, C_BaseEntity );
public:
	DECLARE_CLIENTCLASS();

	C_RainControl();
	~C_RainControl();
	virtual void	OnDataChanged( DataUpdateType_t updateType );

private:
	float  thsdev_rain_splash_chance_local;

private:
	C_RainControl( const C_RainControl & );
};

IMPLEMENT_CLIENTCLASS_DT( C_RainControl, DT_RainControl, CRainControl )
	RecvPropInt( RECVINFO(thsdev_rain_splash_chance_local) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_RainControl::C_RainControl( void )
:	thsdev_rain_splash_chance_local( 25 )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_RainControl::~C_RainControl( void )
{
	if ( g_RainControlInUse == this )
	{
		thsdev_rain_splash_chance = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_RainControl::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	thsdev_rain_splash_chance = thsdev_rain_splash_chance_local;

	g_RainControlInUse = this;
}