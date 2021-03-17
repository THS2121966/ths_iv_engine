//========= Copyright 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Sunlight shadow control entity.
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "c_baseplayer.h"
#include "tier0/vprof.h"
#ifdef MAPBASE
#include "materialsystem/itexture.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


extern ConVar cl_sunlight_ortho_size;
extern ConVar cl_sunlight_depthbias;

ConVar cl_iv_globallight_freeze( "cl_iv_globallight_freeze", "0" );
#ifdef MAPBASE
// I imagine these values might've been designed for the ASW view.
// You can set these as KV anyway.
ConVar cl_iv_globallight_xoffset( "cl_iv_globallight_xoffset", "0" );
ConVar cl_iv_globallight_yoffset( "cl_iv_globallight_yoffset", "0" );
#else
ConVar cl_iv_globallight_xoffset( "cl_iv_globallight_xoffset", "-800" );
ConVar cl_iv_globallight_yoffset( "cl_iv_globallight_yoffset", "1600" );
#endif

//------------------------------------------------------------------------------
// Purpose : Sunlights shadow control entity
//------------------------------------------------------------------------------
class C_IVGlobalLight : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_IVGlobalLight, C_BaseEntity );

	DECLARE_CLIENTCLASS();

	virtual ~C_IVGlobalLight();

	void OnDataChanged( DataUpdateType_t updateType );
	void Spawn();
	bool ShouldDraw();

	void ClientThink();

private:
	Vector m_shadowDirection;
	bool m_bEnabled;
	bool m_bAutoCalculateGlightShadowFSize;
	char m_TextureName[ MAX_PATH ];
#ifdef MAPBASE
	int m_nSpotlightTextureFrame;
#endif
	CTextureReference m_SpotlightTexture;
	color32	m_LightColor;
#ifdef MAPBASE
	float m_flBrightnessScale;
	float m_flCurrentBrightnessScale;
#endif
	Vector m_CurrentLinearFloatLightColor;
	float m_flCurrentLinearFloatLightAlpha;
	float m_flColorTransitionTime;
	float m_flSunDistance;
	float m_flFOV;
	float IVGLShadowRes;
	float IVGLShadowFSize;
	float IVGLShadowAtten;
	float m_flNearZ;
	float m_flNorthOffset;
#ifdef MAPBASE
	float m_flEastOffset;
	float m_flForwardOffset;
	float m_flOrthoSize;
#endif
	bool m_bEnableShadows;
	bool m_bOldEnableShadows;

	static ClientShadowHandle_t m_LocalFlashlightHandle;
};


ClientShadowHandle_t C_IVGlobalLight::m_LocalFlashlightHandle = CLIENTSHADOW_INVALID_HANDLE;


IMPLEMENT_CLIENTCLASS_DT(C_IVGlobalLight, DT_IVGlobalLight, CIVGlobalLight)
	RecvPropVector(RECVINFO(m_shadowDirection)),
	RecvPropBool(RECVINFO(m_bEnabled)),
	RecvPropBool(RECVINFO(m_bAutoCalculateGlightShadowFSize)),
	RecvPropString(RECVINFO(m_TextureName)),
#ifdef MAPBASE
	RecvPropInt(RECVINFO(m_nSpotlightTextureFrame)),
#endif
	/*RecvPropInt(RECVINFO(m_LightColor), 0, RecvProxy_Int32ToColor32),*/
	RecvPropInt(RECVINFO(m_LightColor), 0, RecvProxy_IntToColor32),
#ifdef MAPBASE
	RecvPropFloat(RECVINFO(m_flBrightnessScale)),
#endif
	RecvPropFloat(RECVINFO(m_flColorTransitionTime)),
	RecvPropFloat(RECVINFO(m_flSunDistance)),
	RecvPropFloat(RECVINFO(m_flFOV)),
	RecvPropFloat(RECVINFO(IVGLShadowRes)),
	RecvPropFloat(RECVINFO(IVGLShadowFSize)),
	RecvPropFloat(RECVINFO(IVGLShadowAtten)),
	RecvPropFloat(RECVINFO(m_flNearZ)),
	RecvPropFloat(RECVINFO(m_flNorthOffset)),
#ifdef MAPBASE
	RecvPropFloat(RECVINFO(m_flEastOffset)),
	RecvPropFloat(RECVINFO(m_flForwardOffset)),
	RecvPropFloat(RECVINFO(m_flOrthoSize)),
#endif
	RecvPropBool(RECVINFO(m_bEnableShadows)),
END_RECV_TABLE()


C_IVGlobalLight::~C_IVGlobalLight()
{
	if ( m_LocalFlashlightHandle != CLIENTSHADOW_INVALID_HANDLE )
	{
		g_pClientShadowMgr->DestroyFlashlight( m_LocalFlashlightHandle );
		m_LocalFlashlightHandle = CLIENTSHADOW_INVALID_HANDLE;
	}
}

void C_IVGlobalLight::OnDataChanged( DataUpdateType_t updateType )
{
	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_SpotlightTexture.Init( m_TextureName, TEXTURE_GROUP_OTHER, true );
	}
#ifdef MAPBASE
	else //if ( updateType == DATA_UPDATE_DATATABLE_CHANGED )
	{
		// It could've been changed via input
		if( !FStrEq(m_SpotlightTexture->GetName(), m_TextureName) )
		{
			m_SpotlightTexture.Init( m_TextureName, TEXTURE_GROUP_OTHER, true );
		}
	}
#endif

	BaseClass::OnDataChanged( updateType );
}

void C_IVGlobalLight::Spawn()
{
	BaseClass::Spawn();

	m_bOldEnableShadows = m_bEnableShadows;

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

//------------------------------------------------------------------------------
// We don't draw...
//------------------------------------------------------------------------------
bool C_IVGlobalLight::ShouldDraw()
{
	return false;
}

void C_IVGlobalLight::ClientThink()
{
	VPROF("C_IVGlobalLight::ClientThink");

	bool bSupressWorldLights = false;

	if ( cl_iv_globallight_freeze.GetBool() == true )
	{
		return;
	}

	if ( m_bEnabled )
	{
		Vector vLinearFloatLightColor( m_LightColor.r, m_LightColor.g, m_LightColor.b );
		float flLinearFloatLightAlpha = m_LightColor.a;

#ifdef MAPBASE
		if ( m_CurrentLinearFloatLightColor != vLinearFloatLightColor || m_flCurrentLinearFloatLightAlpha != flLinearFloatLightAlpha || m_flCurrentBrightnessScale != m_flBrightnessScale )
		{
			if (m_flColorTransitionTime != 0.0f)
			{
				float flColorTransitionSpeed = gpGlobals->frametime * m_flColorTransitionTime * 255.0f;

				m_CurrentLinearFloatLightColor.x = Approach( vLinearFloatLightColor.x, m_CurrentLinearFloatLightColor.x, flColorTransitionSpeed );
				m_CurrentLinearFloatLightColor.y = Approach( vLinearFloatLightColor.y, m_CurrentLinearFloatLightColor.y, flColorTransitionSpeed );
				m_CurrentLinearFloatLightColor.z = Approach( vLinearFloatLightColor.z, m_CurrentLinearFloatLightColor.z, flColorTransitionSpeed );
				m_flCurrentLinearFloatLightAlpha = Approach( flLinearFloatLightAlpha, m_flCurrentLinearFloatLightAlpha, flColorTransitionSpeed );
				m_flCurrentBrightnessScale = Approach( m_flBrightnessScale, m_flCurrentBrightnessScale, flColorTransitionSpeed );
			}
			else
			{
				// Just do it instantly
				m_CurrentLinearFloatLightColor.x = vLinearFloatLightColor.x;
				m_CurrentLinearFloatLightColor.y = vLinearFloatLightColor.y;
				m_CurrentLinearFloatLightColor.z = vLinearFloatLightColor.z;
				m_flCurrentLinearFloatLightAlpha = flLinearFloatLightAlpha;
				m_flCurrentBrightnessScale = m_flBrightnessScale;
			}
		}
#else
		if ( m_CurrentLinearFloatLightColor != vLinearFloatLightColor || m_flCurrentLinearFloatLightAlpha != flLinearFloatLightAlpha )
		{
			float flColorTransitionSpeed = gpGlobals->frametime * m_flColorTransitionTime * 255.0f;

			m_CurrentLinearFloatLightColor.x = Approach( vLinearFloatLightColor.x, m_CurrentLinearFloatLightColor.x, flColorTransitionSpeed );
			m_CurrentLinearFloatLightColor.y = Approach( vLinearFloatLightColor.y, m_CurrentLinearFloatLightColor.y, flColorTransitionSpeed );
			m_CurrentLinearFloatLightColor.z = Approach( vLinearFloatLightColor.z, m_CurrentLinearFloatLightColor.z, flColorTransitionSpeed );
			m_flCurrentLinearFloatLightAlpha = Approach( flLinearFloatLightAlpha, m_flCurrentLinearFloatLightAlpha, flColorTransitionSpeed );
		}
#endif

		FlashlightState_t state;

		Vector vDirection = m_shadowDirection;
		VectorNormalize( vDirection );

		//Vector vViewUp = Vector( 0.0f, 1.0f, 0.0f );
		Vector vSunDirection2D = vDirection;
		vSunDirection2D.z = 0.0f;

		/*HACK_GETLOCALPLAYER_GUARD( "C_IVGlobalLight::ClientThink" );*/

		if ( !C_BasePlayer::GetLocalPlayer() )
			return;

		Vector vPos;
		QAngle EyeAngles;
		float flZNear, flZFar, flFov;

		C_BasePlayer::GetLocalPlayer()->CalcView( vPos, EyeAngles, flZNear, flZFar, flFov );
//		Vector vPos = C_BasePlayer::GetLocalPlayer()->GetAbsOrigin();

//		vPos = Vector( 0.0f, 0.0f, 500.0f );
		vPos = ( vPos + vSunDirection2D * m_flNorthOffset ) - vDirection * m_flSunDistance;
#ifdef MAPBASE
		vPos += Vector( m_flEastOffset + cl_iv_globallight_xoffset.GetFloat(), m_flForwardOffset + cl_iv_globallight_yoffset.GetFloat(), 0.0f );
#else
		vPos += Vector( cl_iv_globallight_xoffset.GetFloat(), cl_iv_globallight_yoffset.GetFloat(), 0.0f );
#endif

		QAngle angAngles;
		VectorAngles( vDirection, angAngles );

		Vector vForward, vRight, vUp;
		AngleVectors( angAngles, &vForward, &vRight, &vUp );

		state.m_fHorizontalFOVDegrees = m_flFOV;
		state.m_fVerticalFOVDegrees = m_flFOV;

		state.m_vecLightOrigin = vPos;
		BasisToQuaternion( vForward, vRight, vUp, state.m_quatOrientation );

		state.m_fQuadraticAtten = 0.0f;
		state.m_fLinearAtten = m_flSunDistance * 2.0f;
		state.m_fConstantAtten = 0.0f;
		state.m_FarZAtten = m_flSunDistance * 2.0f;
#ifdef MAPBASE
		float flAlpha = m_flCurrentLinearFloatLightAlpha * (1.0f / 255.0f);
		state.m_Color[0] = (m_CurrentLinearFloatLightColor.x * ( 1.0f / 255.0f ) * flAlpha) * m_flCurrentBrightnessScale;
		state.m_Color[1] = (m_CurrentLinearFloatLightColor.y * ( 1.0f / 255.0f ) * flAlpha) * m_flCurrentBrightnessScale;
		state.m_Color[2] = (m_CurrentLinearFloatLightColor.z * ( 1.0f / 255.0f ) * flAlpha) * m_flCurrentBrightnessScale;
#else
		state.m_Color[0] = m_CurrentLinearFloatLightColor.x * ( 1.0f / 255.0f ) * m_flCurrentLinearFloatLightAlpha;
		state.m_Color[1] = m_CurrentLinearFloatLightColor.y * ( 1.0f / 255.0f ) * m_flCurrentLinearFloatLightAlpha;
		state.m_Color[2] = m_CurrentLinearFloatLightColor.z * ( 1.0f / 255.0f ) * m_flCurrentLinearFloatLightAlpha;
#endif
		state.m_Color[3] = 0.0f; // fixme: need to make ambient work m_flAmbient;
		state.m_NearZ = 4.0f;
		state.m_FarZ = m_flSunDistance * 2.0f;
		state.m_fBrightnessScale = 2.0f;
		state.m_bGlobalLight = true;
		
		//thsdev_new_glight_parms
		state.m_flShadowMapResolution = IVGLShadowRes;
		if(m_bAutoCalculateGlightShadowFSize == false)
		{
		state.m_flShadowFilterSize = IVGLShadowFSize;
		}
		else if(IVGLShadowRes <= 1024)
		{
			state.m_flShadowFilterSize = 1.0f;
		}
		else if(IVGLShadowRes == 2048)
		{
			state.m_flShadowFilterSize = 0.7f;
		}
		else if(IVGLShadowRes >= 4096)
		{
			state.m_flShadowFilterSize = 0.56f;
		}
		state.m_flShadowAtten = IVGLShadowAtten;

#ifdef MAPBASE
		float flOrthoSize = m_flOrthoSize;
#else
		float flOrthoSize = 1000.0f;
#endif

		if ( flOrthoSize > 0 )
		{
			state.m_bOrtho = true;
			state.m_fOrthoLeft = -flOrthoSize;
			state.m_fOrthoTop = -flOrthoSize;
			state.m_fOrthoRight = flOrthoSize;
			state.m_fOrthoBottom = flOrthoSize;
		}
		else
		{
			state.m_bOrtho = false;
		}

#ifndef MAPBASE // Don't draw that huge debug thing
		state.m_bDrawShadowFrustum = true;
#endif
		/*state.m_flShadowSlopeScaleDepthBias = g_pMaterialSystemHardwareConfig->GetShadowSlopeScaleDepthBias();;
		state.m_flShadowDepthBias = g_pMaterialSystemHardwareConfig->GetShadowDepthBias();*/
		state.m_bEnableShadows = m_bEnableShadows;
		state.m_pSpotlightTexture = m_SpotlightTexture;
#ifdef MAPBASE
		state.m_nSpotlightTextureFrame = m_nSpotlightTextureFrame;
#else
		state.m_nSpotlightTextureFrame = 0;
#endif

		state.m_nShadowQuality = 1; // Allow entity to affect shadow quality
//		state.m_bShadowHighRes = true;

		if ( m_bOldEnableShadows != m_bEnableShadows )
		{
			// If they change the shadow enable/disable, we need to make a new handle
			if ( m_LocalFlashlightHandle != CLIENTSHADOW_INVALID_HANDLE )
			{
				g_pClientShadowMgr->DestroyFlashlight( m_LocalFlashlightHandle );
				m_LocalFlashlightHandle = CLIENTSHADOW_INVALID_HANDLE;
			}

			m_bOldEnableShadows = m_bEnableShadows;
		}

		if( m_LocalFlashlightHandle == CLIENTSHADOW_INVALID_HANDLE )
		{
			m_LocalFlashlightHandle = g_pClientShadowMgr->CreateFlashlight( state );
		}
		else
		{
			g_pClientShadowMgr->UpdateFlashlightState( m_LocalFlashlightHandle, state );
			g_pClientShadowMgr->UpdateProjectedTexture( m_LocalFlashlightHandle, true );
		}

		bSupressWorldLights = m_bEnableShadows;
	}
	else if ( m_LocalFlashlightHandle != CLIENTSHADOW_INVALID_HANDLE )
	{
		g_pClientShadowMgr->DestroyFlashlight( m_LocalFlashlightHandle );
		m_LocalFlashlightHandle = CLIENTSHADOW_INVALID_HANDLE;
	}

	//g_pClientShadowMgr->SetShadowFromWorldLightsEnabled( !bSupressWorldLights );

	BaseClass::ClientThink();
} 