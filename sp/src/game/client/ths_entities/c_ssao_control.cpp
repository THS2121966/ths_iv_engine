//========= Copyright [THS] inc 2020, All rights reserved. ============//
//
// Purpose: SSAO control entity.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//------------------------------------------------------------------------------
// Purpose : SSAO control entity
//------------------------------------------------------------------------------
class C_SSAOControl : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_SSAOControl, C_BaseEntity );

	DECLARE_CLIENTCLASS();

	bool ths_ssao_init;

	void OnDataChanged(DataUpdateType_t updateType);
	bool ShouldDraw();
	void THSSSAOEnable( bool ths_ssao_init = false );

private:
	bool ths_ssao_enable;
};

IMPLEMENT_CLIENTCLASS_DT(C_SSAOControl, DT_SSAOControl, CSSAOControl)
	RecvPropBool(RECVINFO(ths_ssao_enable)),
END_RECV_TABLE()


//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
void C_SSAOControl::OnDataChanged(DataUpdateType_t updateType)
{
	THSSSAOEnable( ths_ssao_enable );
}

//------------------------------------------------------------------------------
// We don't draw...
//------------------------------------------------------------------------------
bool C_SSAOControl::ShouldDraw()
{
	return false;
}