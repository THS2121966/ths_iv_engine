//=============== [THS] inc 2020. =============================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"


extern bool  ths_env_echanced_expfx;
extern int  thsdev_mapping_numb_shadows;
//extern float  m_flShadowFilterSize;

extern bool  ths_water_fx_init;
extern bool  ths_blood_fx_init;

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
	int  thsdev_mapping_numb_shadows_local;
//	float  m_flShadowFilterSize_local;

	bool  ths_water_fx_init_local;
	bool  ths_blood_fx_init_local;

private:
	C_IVEffectsMain( const C_IVEffectsMain & );
};

IMPLEMENT_CLIENTCLASS_DT( C_IVEffectsMain, DT_IVEffectsMain, CIVEffectsMain )
	RecvPropInt( RECVINFO(ths_enable_expfx_local) ),
	RecvPropInt( RECVINFO(thsdev_mapping_numb_shadows_local) ),
//	RecvPropInt( RECVINFO(m_flShadowFilterSize_local) ),

	RecvPropInt( RECVINFO(ths_water_fx_init_local) ),
	RecvPropInt( RECVINFO(ths_blood_fx_init_local) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_IVEffectsMain::C_IVEffectsMain( void )
:	ths_enable_expfx_local( true ),
	thsdev_mapping_numb_shadows_local( 5 ),
//	m_flShadowFilterSize_local( 0.7 )
	ths_water_fx_init_local( false ),
	ths_blood_fx_init_local( false )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_IVEffectsMain::~C_IVEffectsMain( void )
{
	if ( g_IVEffectsControlInUse == this )
	{
		ths_enable_expfx_local = true;
		thsdev_mapping_numb_shadows_local = 5;
//		m_flShadowFilterSize_local = 0.7;

		ths_water_fx_init_local = false;
		ths_blood_fx_init_local = false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_IVEffectsMain::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	ths_env_echanced_expfx = ths_enable_expfx_local;
	thsdev_mapping_numb_shadows = thsdev_mapping_numb_shadows_local;
//	m_flShadowFilterSize = m_flShadowFilterSize_local;

	ths_water_fx_init = ths_water_fx_init_local;
	ths_blood_fx_init = ths_blood_fx_init_local;

	g_IVEffectsControlInUse = this;
}