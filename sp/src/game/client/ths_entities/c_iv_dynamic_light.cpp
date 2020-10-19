#include "cbase.h"
#include "dlight.h"
#include "iefx.h"
#include "IViewRender.h"
#include "c_iv_dynamic_light.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT(C_IV_DynamicLight, DT_IV_Dynamic_Light, CIV_Dynamic_Light)
RecvPropInt		(RECVINFO(m_Flags)),
RecvPropInt		(RECVINFO(m_LightStyle)),
RecvPropFloat	(RECVINFO(m_Radius)),
RecvPropInt		(RECVINFO(m_Exponent)),
RecvPropFloat	(RECVINFO(m_InnerAngle)),
RecvPropFloat	(RECVINFO(m_OuterAngle)),
RecvPropFloat	(RECVINFO(m_SpotRadius)),
END_RECV_TABLE()

C_IV_DynamicLight::C_IV_DynamicLight(void) : m_pSpotlightEnd(0)
{
	m_pDynamicLight = NULL;
}

C_IV_DynamicLight::~C_IV_DynamicLight()
{
	if (m_pDynamicLight)
	{
		m_pDynamicLight->die = gpGlobals->curtime;
		m_pDynamicLight = NULL;
	}
}

void C_IV_DynamicLight::OnDataChanged(DataUpdateType_t updateType)
{
	if ( updateType == DATA_UPDATE_CREATED )
	{
		SetNextClientThink(gpGlobals->curtime + 0.05);
	}
}

bool C_IV_DynamicLight::ShouldDraw()
{
	//return false;
	return true;
}

void C_IV_DynamicLight::ClientThink(void)
{
	Vector forward;
	AngleVectors( GetAbsAngles(), &forward );

	if ( (m_Flags & DLIGHT_NO_MODEL_ILLUMINATION) == 0 )
	{
		if (!m_pDynamicLight || m_pDynamicLight->key != IV_LIGHT_INDEX_FIRES + index)
		{
			m_pDynamicLight = effects->CL_AllocDlight( IV_LIGHT_INDEX_FIRES + index );	
		}
		m_pDynamicLight->color.b = m_clrRender->b;
		m_pDynamicLight->color.g = m_clrRender->g;
		m_pDynamicLight->color.r = m_clrRender->r;

		m_pDynamicLight->origin	= GetAbsOrigin();
		m_pDynamicLight->radius = m_Radius;
		m_pDynamicLight->color.exponent = m_Exponent;

		m_pDynamicLight->die		= gpGlobals->curtime + 30.0f;
	}
	else
	{
		// In this case, the m_Flags could have changed; which is how we turn the light off
		if (m_pDynamicLight)
		{
			m_pDynamicLight->die = gpGlobals->curtime;
			m_pDynamicLight = 0;
		}
	}

	SetNextClientThink(gpGlobals->curtime + 0.001);
}