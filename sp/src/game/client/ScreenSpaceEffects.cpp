//========= Copyright Valve Corporation, All rights reserved. ============//
#include "cbase.h"

#include "KeyValues.h"
#include "cdll_client_int.h"
#include "view_scene.h"
#include "viewrender.h"
#include "tier0/icommandline.h"
#include "materialsystem/imesh.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialsystemhardwareconfig.h"
#include "materialsystem/imaterialvar.h"
#include "hl2/c_basehlplayer.h"

#include "ScreenSpaceEffects.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//------------------------------------------------------------------------------
// CScreenSpaceEffectRegistration code
// Used to register and effect with the IScreenSpaceEffectManager
//------------------------------------------------------------------------------
CScreenSpaceEffectRegistration *CScreenSpaceEffectRegistration::s_pHead = NULL;

CScreenSpaceEffectRegistration::CScreenSpaceEffectRegistration( const char *pName, IScreenSpaceEffect *pEffect )
{
	m_pEffectName = pName;
	m_pEffect = pEffect;
	m_pNext = s_pHead;
	s_pHead = this;
}


//------------------------------------------------------------------------------
// CScreenSpaceEffectManager - Implementation of IScreenSpaceEffectManager
//------------------------------------------------------------------------------
class CScreenSpaceEffectManager : public IScreenSpaceEffectManager
{
public:

	virtual void InitScreenSpaceEffects( );
	virtual void ShutdownScreenSpaceEffects( );

	virtual IScreenSpaceEffect *GetScreenSpaceEffect( const char *pEffectName );

	virtual void SetScreenSpaceEffectParams( const char *pEffectName, KeyValues *params );
	virtual void SetScreenSpaceEffectParams( IScreenSpaceEffect *pEffect, KeyValues *params );
    
	virtual void EnableScreenSpaceEffect( const char *pEffectName );
	virtual void EnableScreenSpaceEffect( IScreenSpaceEffect *pEffect );

	virtual void DisableScreenSpaceEffect( const char *pEffectName );
	virtual void DisableScreenSpaceEffect( IScreenSpaceEffect *pEffect );

	virtual void DisableAllScreenSpaceEffects( );

	virtual void RenderEffects( int x, int y, int w, int h );
};

CScreenSpaceEffectManager g_ScreenSpaceEffectManager;
IScreenSpaceEffectManager *g_pScreenSpaceEffects = &g_ScreenSpaceEffectManager;

//---------------------------------------------------------------------------------------
// CScreenSpaceEffectManager::InitScreenSpaceEffects - Initialise all registered effects
//---------------------------------------------------------------------------------------
void CScreenSpaceEffectManager::InitScreenSpaceEffects( )
{
	if ( CommandLine()->FindParm( "-filmgrain" ) )
	{
		GetScreenSpaceEffect( "filmgrain" )->Enable( true );
	}

	for( CScreenSpaceEffectRegistration *pReg=CScreenSpaceEffectRegistration::s_pHead; pReg!=NULL; pReg=pReg->m_pNext )
	{
		IScreenSpaceEffect *pEffect = pReg->m_pEffect;
		if( pEffect )
		{
			bool bIsEnabled = pEffect->IsEnabled( );
			pEffect->Init( );
			pEffect->Enable( bIsEnabled );
		}
	}
}


//----------------------------------------------------------------------------------------
// CScreenSpaceEffectManager::ShutdownScreenSpaceEffects - Shutdown all registered effects
//----------------------------------------------------------------------------------------
void CScreenSpaceEffectManager::ShutdownScreenSpaceEffects( )
{
	for( CScreenSpaceEffectRegistration *pReg=CScreenSpaceEffectRegistration::s_pHead; pReg!=NULL; pReg=pReg->m_pNext )
	{
		IScreenSpaceEffect *pEffect = pReg->m_pEffect;
		if( pEffect )
		{
			pEffect->Shutdown( );
		}
	}
}


//---------------------------------------------------------------------------------------
// CScreenSpaceEffectManager::GetScreenSpaceEffect - Returns a point to the named effect
//---------------------------------------------------------------------------------------
IScreenSpaceEffect *CScreenSpaceEffectManager::GetScreenSpaceEffect( const char *pEffectName )
{
	for( CScreenSpaceEffectRegistration *pReg=CScreenSpaceEffectRegistration::s_pHead; pReg!=NULL; pReg=pReg->m_pNext )
	{
		if( !Q_stricmp( pReg->m_pEffectName, pEffectName ) )
		{
			IScreenSpaceEffect *pEffect = pReg->m_pEffect;
			return pEffect;
		}
	}

	Warning( "Could not find screen space effect %s\n", pEffectName );

	return NULL;
}


//---------------------------------------------------------------------------------------
// CScreenSpaceEffectManager::SetScreenSpaceEffectParams 
//	- Assign parameters to the specified effect
//---------------------------------------------------------------------------------------
void CScreenSpaceEffectManager::SetScreenSpaceEffectParams( const char *pEffectName, KeyValues *params )
{
	IScreenSpaceEffect *pEffect = GetScreenSpaceEffect( pEffectName );
	if( pEffect )
		SetScreenSpaceEffectParams( pEffect, params );
}

void CScreenSpaceEffectManager::SetScreenSpaceEffectParams( IScreenSpaceEffect *pEffect, KeyValues *params )
{
	if( pEffect )
		pEffect->SetParameters( params );
}


//---------------------------------------------------------------------------------------
// CScreenSpaceEffectManager::EnableScreenSpaceEffect
//	- Enables the specified effect
//---------------------------------------------------------------------------------------
void CScreenSpaceEffectManager::EnableScreenSpaceEffect( const char *pEffectName )
{
	IScreenSpaceEffect *pEffect = GetScreenSpaceEffect( pEffectName );
	if( pEffect )
		EnableScreenSpaceEffect( pEffect );
}

void CScreenSpaceEffectManager::EnableScreenSpaceEffect( IScreenSpaceEffect *pEffect )
{
	if( pEffect )
		pEffect->Enable( true );
}


//---------------------------------------------------------------------------------------
// CScreenSpaceEffectManager::DisableScreenSpaceEffect
//	- Disables the specified effect
//---------------------------------------------------------------------------------------
void CScreenSpaceEffectManager::DisableScreenSpaceEffect( const char *pEffectName )
{
	IScreenSpaceEffect *pEffect = GetScreenSpaceEffect( pEffectName );
	if( pEffect )
		DisableScreenSpaceEffect( pEffect );
}

void CScreenSpaceEffectManager::DisableScreenSpaceEffect( IScreenSpaceEffect *pEffect )
{
	if( pEffect )
		pEffect->Enable( false );
}


//---------------------------------------------------------------------------------------
// CScreenSpaceEffectManager::DisableAllScreenSpaceEffects
//	- Disables all registered screen space effects
//---------------------------------------------------------------------------------------
void CScreenSpaceEffectManager::DisableAllScreenSpaceEffects( )
{
	for( CScreenSpaceEffectRegistration *pReg=CScreenSpaceEffectRegistration::s_pHead; pReg!=NULL; pReg=pReg->m_pNext )
	{
		IScreenSpaceEffect *pEffect = pReg->m_pEffect;
		if( pEffect )
		{
			pEffect->Enable( false );
		}
	}
}


//---------------------------------------------------------------------------------------
// CScreenSpaceEffectManager::RenderEffects
//	- Renders all registered screen space effects
//---------------------------------------------------------------------------------------
void CScreenSpaceEffectManager::RenderEffects( int x, int y, int w, int h )
{
	for( CScreenSpaceEffectRegistration *pReg=CScreenSpaceEffectRegistration::s_pHead; pReg!=NULL; pReg=pReg->m_pNext )
	{
		IScreenSpaceEffect *pEffect = pReg->m_pEffect;
		if( pEffect )
		{
			pEffect->Render( x, y, w, h );
		}
	}
}

//------------------------------------------------------------------------------
// Example post-processing effect
//------------------------------------------------------------------------------
class CExampleEffect : public IScreenSpaceEffect
{
public:
	CExampleEffect( );
   ~CExampleEffect( );

	void Init( );
	void Shutdown( );

	void SetParameters( KeyValues *params );

	void Render( int x, int y, int w, int h );

	void Enable( bool bEnable );
	bool IsEnabled( );

private:

	bool				m_bEnable;

	CMaterialReference	m_Material;
};

ADD_SCREENSPACE_EFFECT( CExampleEffect, exampleeffect );

//------------------------------------------------------------------------------
// CExampleEffect constructor
//------------------------------------------------------------------------------
CExampleEffect::CExampleEffect( )
{
	m_bEnable = false;
}


//------------------------------------------------------------------------------
// CExampleEffect destructor
//------------------------------------------------------------------------------
CExampleEffect::~CExampleEffect( )
{
}


//------------------------------------------------------------------------------
// CExampleEffect init
//------------------------------------------------------------------------------
void CExampleEffect::Init( )
{
	// This is just example code, init your effect material here
	//m_Material.Init( "engine/exampleeffect", TEXTURE_GROUP_OTHER );

	m_bEnable = false;
}


//------------------------------------------------------------------------------
// CExampleEffect shutdown
//------------------------------------------------------------------------------
void CExampleEffect::Shutdown( )
{
	m_Material.Shutdown();
}

//------------------------------------------------------------------------------
// CExampleEffect enable
//------------------------------------------------------------------------------
void CExampleEffect::Enable( bool bEnable )
{
	// This is just example code, don't enable it
	// m_bEnable = bEnable;
}

bool CExampleEffect::IsEnabled( )
{
	return m_bEnable;
}

//------------------------------------------------------------------------------
// CExampleEffect SetParameters
//------------------------------------------------------------------------------
void CExampleEffect::SetParameters( KeyValues *params )
{
	if( params->GetDataType( "example_param" ) == KeyValues::TYPE_STRING )
	{
		// ...
	}
}

//------------------------------------------------------------------------------
// CExampleEffect render
//------------------------------------------------------------------------------
void CExampleEffect::Render( int x, int y, int w, int h )
{
	if ( !IsEnabled() )
		return;

	// Render Effect
	Rect_t actualRect;
	UpdateScreenEffectTexture( 0, x, y, w, h, false, &actualRect );
	ITexture *pTexture = GetFullFrameFrameBufferTexture( 0 );

	CMatRenderContextPtr pRenderContext( materials );

	pRenderContext->DrawScreenSpaceRectangle( m_Material, x, y, w, h,
											actualRect.x, actualRect.y, actualRect.x+actualRect.width-1, actualRect.y+actualRect.height-1, 
											pTexture->GetActualWidth(), pTexture->GetActualHeight() );
}

//------------------------------------------------------------------------------
// Water post-processing effects
//------------------------------------------------------------------------------
class CWaterEffects : public IScreenSpaceEffect
{
public:
	CWaterEffects( void ) { };

	virtual void Init( void );
	virtual void Shutdown( void );
	virtual void SetParameters( KeyValues *params );
	virtual void Enable( bool bEnable ) { m_bEnabled = bEnable; }
	virtual bool IsEnabled( ) { return m_bEnabled; }

	virtual void Render( int x, int y, int w, int h );

	virtual float GetViscosity( ) { return fViscosity; }
	virtual float GetAmount( ) { return fAmount; }
	virtual void SetViscosity( float fNewViscosity ) { fViscosity = fNewViscosity; }
	virtual void SetAmount( float fNewAmount ) { fAmount = fNewAmount; }
	virtual bool IsUnderwater( ) { return m_bUnderwater; }

private:
	bool	m_bEnabled;
	bool	m_bUnderwater;

	float	fViscosity;
	float	fAmount;

	CMaterialReference	m_ChromaticDisp;
	CMaterialReference	m_WaterFX;
	CMaterialReference	m_BlurX;
	CMaterialReference	m_BlurY;
};

ADD_SCREENSPACE_EFFECT( CWaterEffects, c17_waterfx );

ConVar r_post_watereffects_underwater_chromaticoffset( "r_post_watereffects_underwater_chromaticoffset", "1.0", FCVAR_CHEAT );
ConVar r_post_watereffects_underwater_amount( "r_post_watereffects_underwater_amount", "0.1", FCVAR_CHEAT );
ConVar r_post_watereffects_underwater_viscosity( "r_post_watereffects_underwater_viscosity", "1.0", FCVAR_CHEAT );
ConVar r_post_watereffects_lerp_viscosity( "r_post_watereffects_lerp_viscosity", "0.01", FCVAR_CHEAT );
ConVar r_post_watereffects_lerp_amount( "r_post_watereffects_lerp_amount", "0.005", FCVAR_CHEAT );
ConVar r_post_watereffects_underwater_gaussianamount( "r_post_watereffects_underwater_gaussianamount", "1.5", FCVAR_CHEAT );

//------------------------------------------------------------------------------
// CWaterEffects init
//------------------------------------------------------------------------------
void CWaterEffects::Init( void )
{
	fViscosity = 0.01;
	fAmount = 0;
	m_bUnderwater = false;

	PrecacheMaterial( "dev/chromaticDisp" );
	PrecacheMaterial( "dev/screenwater" );
	PrecacheMaterial( "dev/blurfilterx" );
	PrecacheMaterial( "dev/blurfiltery" );

	m_ChromaticDisp.Init( materials->FindMaterial("dev/chromaticDisp", TEXTURE_GROUP_PIXEL_SHADERS, true) );
	m_WaterFX.Init( materials->FindMaterial("dev/screenwater", TEXTURE_GROUP_PIXEL_SHADERS, true) );
	m_BlurX.Init( materials->FindMaterial("dev/screen_blurx", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
	m_BlurY.Init( materials->FindMaterial("dev/screen_blury", TEXTURE_GROUP_PIXEL_SHADERS, true ) );
}

//------------------------------------------------------------------------------
// CWaterEffects shutdown
//------------------------------------------------------------------------------
void CWaterEffects::Shutdown( void )
{
	m_ChromaticDisp.Shutdown();
	m_WaterFX.Shutdown();
	m_BlurX.Shutdown();
	m_BlurY.Shutdown();
}

//------------------------------------------------------------------------------
// CWaterEffects SetParameters
//------------------------------------------------------------------------------
void CWaterEffects::SetParameters( KeyValues *params )
{
	if( IsUnderwater() )
		return;

	float in, temp;

	if( params->FindKey( "amount" ) )
	{
		in = params->GetFloat( "amount" );
		temp = GetAmount();
		temp += in;
		if( temp > 0.1f )
			temp = 0.1f;

		SetAmount( temp );
	}

	if( params->FindKey( "viscosity" ) )
	{
		in = params->GetFloat( "viscosity" );
		temp = GetViscosity();
		temp += in;
		if( temp > 1.0f )
			temp = 1.0f;

		SetViscosity( temp );
	}
}

ConVar r_post_watereffects( "r_post_watereffects", "1", FCVAR_ARCHIVE );
ConVar r_post_watereffects_debug( "r_post_watereffects_debug", "0", FCVAR_CHEAT );

//------------------------------------------------------------------------------
// CWaterEffects render
//------------------------------------------------------------------------------
void CWaterEffects::Render( int x, int y, int w, int h )
{
	if( !r_post_watereffects.GetBool() || ( IsEnabled() == false ) )
		return;

	C_BaseHLPlayer *pPlayer = (C_BaseHLPlayer *)C_BasePlayer::GetLocalPlayer();
	if(!pPlayer)
		return;

	IMaterialVar *var;

	if(pPlayer->GetWaterLevel() >= 3)
	{
		m_bUnderwater = true;
		fViscosity = r_post_watereffects_underwater_viscosity.GetFloat();
		fAmount = r_post_watereffects_underwater_amount.GetFloat();

		//Gaussian Blur the screen
		var = m_BlurX->FindVar( "$BLURSIZE", NULL );
		var->SetFloatValue( r_post_watereffects_underwater_gaussianamount.GetFloat() );
		var = m_BlurX->FindVar( "$RESDIVISOR", NULL );
		var->SetIntValue( 1 );
		DrawScreenEffectMaterial( m_BlurX, x, y, w, h );
		var = m_BlurY->FindVar( "$BLURSIZE", NULL );
		var = m_BlurY->FindVar( "$RESDIVISOR", NULL );
		var->SetIntValue( 1 );
		var->SetFloatValue( r_post_watereffects_underwater_gaussianamount.GetFloat() );
		DrawScreenEffectMaterial( m_BlurY, x, y, w, h );

		//Render Chromatic Dispersion
		var = m_ChromaticDisp->FindVar( "$FOCUSOFFSET", NULL );
		var->SetFloatValue( r_post_watereffects_underwater_chromaticoffset.GetFloat() );
		var = m_ChromaticDisp->FindVar( "$radial", NULL );
		var->SetIntValue( 0 );
		DrawScreenEffectMaterial( m_ChromaticDisp, x, y, w, h );
	}
	else
	{
		m_bUnderwater = false;

		if( fViscosity != 0.01 )
			fViscosity = FLerp( fViscosity, 0.01, r_post_watereffects_lerp_viscosity.GetFloat() );

		if( fAmount != 0 )
			fAmount = FLerp( fAmount, 0, r_post_watereffects_lerp_amount.GetFloat() );

		if( fAmount < 0.01 )
		{
			if( r_post_watereffects_debug.GetBool() )
			{
				DevMsg( "Water Effects Stopped.\n" );
			}
			return;
		}
	}

	var = m_WaterFX->FindVar( "$AMOUNT", NULL );
	var->SetFloatValue( fAmount );
	var = m_WaterFX->FindVar( "$VISCOSITY", NULL );
	var->SetFloatValue( fViscosity );
	DrawScreenEffectMaterial( m_WaterFX, x, y, w, h );

	if( r_post_watereffects_debug.GetBool() )
	{
		DevMsg( "Water Amount: %.2f\n", fAmount );
		DevMsg( "Water Viscosity: %.2f\n", fViscosity );
	}
}