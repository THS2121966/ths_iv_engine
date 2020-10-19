#ifndef _INCLUDED_C_IV_DYNAMIC_LIGHT_H
#define _INCLUDED_C_IV_DYNAMIC_LIGHT_H

// iv light indices
enum
{
	IV_LIGHT_INDEX_FIRES = 0x64000000,
};

class C_IV_DynamicLight : public C_BaseEntity
{
public:
	DECLARE_CLASS( C_IV_DynamicLight, C_BaseEntity );
	DECLARE_CLIENTCLASS();

	C_IV_DynamicLight();
	virtual ~C_IV_DynamicLight();

public:
	void	OnDataChanged(DataUpdateType_t updateType);
	bool	ShouldDraw();
	void	ClientThink( void );

	unsigned char	m_Flags;
	unsigned char	m_LightStyle;

	float	m_Radius;
	int		m_Exponent;
	float	m_InnerAngle;
	float	m_OuterAngle;
	float	m_SpotRadius;

private:
	dlight_t*	m_pDynamicLight;
	dlight_t*	m_pSpotlightEnd;
};

#endif // _INCLUDED_C_IV_DYNAMIC_LIGHT_H