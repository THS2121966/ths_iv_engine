//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"


extern int  ths_filmgrain;
extern int  ths_exp;
extern int  ths_flare;

EHANDLE g_IVFXInUse = NULL;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_IVFXControl : public C_BaseEntity
{
	DECLARE_CLASS( C_IVFXControl, C_BaseEntity );
public:
	DECLARE_CLIENTCLASS();

	C_IVFXControl();
	~C_IVFXControl();
	virtual void	OnDataChanged( DataUpdateType_t updateType );

private:
	int  iv_fx_local_fgrain;
	int  iv_fx_local_exp;
	int  iv_fx_local_flare;

private:
	C_IVFXControl( const C_IVFXControl & );
};

IMPLEMENT_CLIENTCLASS_DT( C_IVFXControl, DT_IVFXControl, CIVFXControl )
	RecvPropInt( RECVINFO(iv_fx_local_fgrain) ),
	RecvPropInt( RECVINFO(iv_fx_local_exp) ),
	RecvPropInt( RECVINFO(iv_fx_local_flare) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_IVFXControl::C_IVFXControl( void )
:	iv_fx_local_fgrain( 0 ),
	iv_fx_local_exp( 0 ),
	iv_fx_local_flare( 0 )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_IVFXControl::~C_IVFXControl( void )
{
	if ( g_IVFXInUse == this )
	{
		ths_filmgrain = 50;
		ths_exp = 50;
		ths_flare = 25;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_IVFXControl::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	ths_filmgrain = iv_fx_local_fgrain;
	ths_exp = iv_fx_local_exp;
	ths_flare = iv_fx_local_flare;

	g_IVFXInUse = this;
}