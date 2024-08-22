#ifndef NP_GRAPHICS_GRAPHICS_API_H_
#define NP_GRAPHICS_GRAPHICS_API_H_

#include <np/graphics/graphics_RenderSystemDrawing.h>

/*!
	@brief Submits a render command to the render queue.

	@param[in] func The function to call.
*/
#define SUBMIT_RENDER_COMMAND(func) np::graphics::SubmitRenderCommand(func)

/*!
	@brief Submits a render command to the render queue.

	@param[in] func  The function to call.
	@param[in] param The parameter to pass to the function.
*/
#define SUBMIT_RENDER_COMMAND_PARAM(func, param) np::graphics::SubmitRenderCommand(func, param)

namespace np { namespace graphics {

     /*!
		@brief Command to render, ensures that all rendering commands are done in the same thread.

		@param[in] renderSystem The render system to use.
		@param[in] param The parameter to pass to the function.
	*/
	typedef void (*RenderCommandFunc)(RenderSystemDrawing*, uptr);

	/*!
		@brief Initializes the graphics API.
	*/
	void Initialize(void);

	/*!
		@brief Finalizes the graphics API.
	*/
	void Finalize(void);

	/*!
		@brief Submit a render command to the render queue.

		@param[in] func The function to call.
		@param[in] param The parameter to pass to the function.
	*/
	void SubmitRenderCommand(RenderCommandFunc func, uptr param = NULL);

	/*!
		@brief Submit a render command to the render queue.

		@tparam T The type of the parameter.

		@param[in] func The function to call.
		@param[in] param The parameter to pass to the function.
	*/
	template<typename T>
	void SubmitRenderCommand(void (*func)(RenderSystemDrawing*, T*), T* param)
	{
		SubmitRenderCommand(reinterpret_cast<RenderCommandFunc>(func), reinterpret_cast<uptr>(param));
	}

}}	// namespace np::graphics

#endif	// NP_GRAPHICS_GRAPHICS_API_H_
