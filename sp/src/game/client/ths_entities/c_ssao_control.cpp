//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"


extern bool  ths_ssao_init;

EHANDLE g_SSAOControlInUse = NULL;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_SSAOControl : public C_BaseEntity
{
	DECLARE_CLASS( C_SSAOControl, C_BaseEntity );
public:
	DECLARE_CLIENTCLASS();

	C_SSAOControl();
	~C_SSAOControl();
	virtual void	OnDataChanged( DataUpdateType_t updateType );

private:
	bool  ths_local_ssao_init;

private:
	C_SSAOControl( const C_SSAOControl & );
};

IMPLEMENT_CLIENTCLASS_DT( C_SSAOControl, DT_SSAOControl, CSSAOControl )
	RecvPropInt( RECVINFO(ths_local_ssao_init) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_SSAOControl::C_SSAOControl( void )
:	ths_local_ssao_init( true )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_SSAOControl::~C_SSAOControl( void )
{
	if ( g_SSAOControlInUse == this )
	{
		ths_ssao_init = false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_SSAOControl::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	ths_ssao_init = ths_local_ssao_init;

	g_SSAOControlInUse = this;
}