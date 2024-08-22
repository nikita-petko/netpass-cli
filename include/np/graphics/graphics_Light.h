#ifndef NP_GRAPHICS_GRAPHICS_LIGHT_H_
#define NP_GRAPHICS_GRAPHICS_LIGHT_H_

#include <np/graphics/graphics_Utility.h>

namespace np { namespace graphics {
	/*!
		@brief Represents a light source.
	*/
	class Light : private nn::util::NonCopyable<Light>
	{
	public:
		Light(void);
		virtual ~Light();

	public:
		void Initialize(void);

	public:
		f32 m_Ambient[4];
		f32 m_Diffuse[4];
		f32 m_Specular0[4];
		f32 m_Specular1[4];
		f32 m_Position[4];
	};
}}	// namespace np::graphics

#endif
