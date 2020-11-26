#include "shaderlib/cshader.h"
class ths_blurfilter_ps20_Static_Index
{
private:
	int m_nKERNEL;
#ifdef _DEBUG
	bool m_bKERNEL;
#endif
public:
	void SetKERNEL( int i )
	{
		Assert( i >= 0 && i <= 4 );
		m_nKERNEL = i;
#ifdef _DEBUG
		m_bKERNEL = true;
#endif
	}
	void SetKERNEL( bool i )
	{
		m_nKERNEL = i ? 1 : 0;
#ifdef _DEBUG
		m_bKERNEL = true;
#endif
	}
private:
	int m_nCLEAR_COLOR;
#ifdef _DEBUG
	bool m_bCLEAR_COLOR;
#endif
public:
	void SetCLEAR_COLOR( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nCLEAR_COLOR = i;
#ifdef _DEBUG
		m_bCLEAR_COLOR = true;
#endif
	}
	void SetCLEAR_COLOR( bool i )
	{
		m_nCLEAR_COLOR = i ? 1 : 0;
#ifdef _DEBUG
		m_bCLEAR_COLOR = true;
#endif
	}
public:
	ths_blurfilter_ps20_Static_Index( )
	{
#ifdef _DEBUG
		m_bKERNEL = false;
#endif // _DEBUG
		m_nKERNEL = 0;
#ifdef _DEBUG
		m_bCLEAR_COLOR = false;
#endif // _DEBUG
		m_nCLEAR_COLOR = 0;
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
		bool bAllStaticVarsDefined = m_bKERNEL && m_bCLEAR_COLOR;
		Assert( bAllStaticVarsDefined );
#endif // _DEBUG
		return ( 1 * m_nKERNEL ) + ( 5 * m_nCLEAR_COLOR ) + 0;
	}
};
#define shaderStaticTest_ths_blurfilter_ps20 psh_forgot_to_set_static_KERNEL + psh_forgot_to_set_static_CLEAR_COLOR + 0
class ths_blurfilter_ps20_Dynamic_Index
{
public:
	ths_blurfilter_ps20_Dynamic_Index()
	{
	}
	int GetIndex()
	{
		// Asserts to make sure that we aren't using any skipped combinations.
		// Asserts to make sure that we are setting all of the combination vars.
#ifdef _DEBUG
#endif // _DEBUG
		return 0;
	}
};
#define shaderDynamicTest_ths_blurfilter_ps20 0