//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"


extern bool  ths_env_echanced_expfx;

EHANDLE g_IVEffectsControlInUse = NULL;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_IVEffectsMain : public C_BaseEntity
{
	DECLARE_CLASS( C_IVEffectsMain, C_BaseEntity );
public:
	DECLARE_CLIENTCLASS();

	C_IVEffectsMain();
	~C_IVEffectsMain();
	virtual void	OnDataChanged( DataUpdateType_t updateType );

private:
	bool  ths_enable_expfx_local;

private:
	C_IVEffectsMain( const C_IVEffectsMain & );
};

IMPLEMENT_CLIENTCLASS_DT( C_IVEffectsMain, DT_IVEffectsMain, CIVEffectsMain )
	RecvPropInt( RECVINFO(ths_enable_expfx_local) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_IVEffectsMain::C_IVEffectsMain( void )
:	ths_enable_expfx_local( true )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_IVEffectsMain::~C_IVEffectsMain( void )
{
	if ( g_IVEffectsControlInUse == this )
	{
		ths_env_echanced_expfx = false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_IVEffectsMain::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	ths_env_echanced_expfx = ths_enable_expfx_local;

	g_IVEffectsControlInUse = this;
}