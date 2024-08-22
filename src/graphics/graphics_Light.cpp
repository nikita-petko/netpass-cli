#include <np/graphics/graphics_Light.h>

namespace np { namespace graphics {

	Light::Light(void) { Initialize(); }

	Light::~Light() {}

	void Light::Initialize(void)
	{
		m_Ambient[0] = 0.0f;
		m_Ambient[1] = 0.0f;
		m_Ambient[2] = 0.0f;
		m_Ambient[3] = 0.0f;

		m_Diffuse[0] = 0.0f;
		m_Diffuse[1] = 0.0f;
		m_Diffuse[2] = 0.0f;
		m_Diffuse[3] = 0.0f;

		m_Specular0[0] = 0.0f;
		m_Specular0[1] = 0.0f;
		m_Specular0[2] = 0.0f;
		m_Specular0[3] = 0.0f;

		m_Specular1[0] = 0.0f;
		m_Specular1[1] = 0.0f;
		m_Specular1[2] = 0.0f;
		m_Specular1[3] = 0.0f;

		m_Position[0] = 0.0f;
		m_Position[1] = 0.0f;
		m_Position[2] = 0.0f;
		m_Position[3] = 1.0f;
	}

}}	// namespace np::graphics
