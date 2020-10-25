//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"


extern bool  ths_nightvision_init;

EHANDLE g_NVControlInUse = NULL;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_NVControl : public C_BaseEntity
{
	DECLARE_CLASS( C_NVControl, C_BaseEntity );
public:
	DECLARE_CLIENTCLASS();

	C_NVControl();
	~C_NVControl();
	virtual void	OnDataChanged( DataUpdateType_t updateType );

private:
	bool  ths_local_nightvision_init;

private:
	C_NVControl( const C_NVControl & );
};

IMPLEMENT_CLIENTCLASS_DT( C_NVControl, DT_NVControl, CNVControl )
	RecvPropInt( RECVINFO(ths_local_nightvision_init) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_NVControl::C_NVControl( void )
:	ths_local_nightvision_init( true )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_NVControl::~C_NVControl( void )
{
	if ( g_NVControlInUse == this )
	{
		ths_nightvision_init = false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_NVControl::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	ths_nightvision_init = ths_local_nightvision_init;

	g_NVControlInUse = this;
}