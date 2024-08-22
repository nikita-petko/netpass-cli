#ifndef NP_GRAPHICS_GRAPHICS_COMMAND_CACHE_H_
#define NP_GRAPHICS_GRAPHICS_COMMAND_CACHE_H_

#include <np/graphics/graphics_Utility.h>

namespace np { namespace graphics {

	namespace detail {
		/*!
			@brief Contains 3D commands (64 bits).
		*/
		struct CommandBuffer3d
		{
			unsigned data : 32;	 // Data to write to the register.
			unsigned addr : 16;	 // Address of the register where writing is performed.
			unsigned be	  : 4;	 // Byte enabled
			unsigned size : 7;	 // Data count - 1. Single access if 0; burst access if 1 or greater.
			unsigned rsv  : 4;	 // Represents a reserved region.
			unsigned seq  : 1;	 // Access mode during burst access.
								 // Single register writing when 0, continuous register writing when 1 */.
		};

		// Address of the register setting the index of the vertex shader floating point register.
		const u32 VS_UNIFORM_FLOAT_REG_INDEX = 0x2c0;
		// Start address of the register to write the floating point uniform value of the vertex shader.
		const u32 VS_UNIFORM_FLOAT_REG_VALUE_BEGIN = 0x2c1;
		// End address of the register to write the floating point uniform value of the vertex shader.
		const u32 VS_UNIFORM_FLOAT_REG_VALUE_END = 0x2c8;

		// Number of floating-point registers for the vertex shader.
		const u32 VS_UNIFORM_FLOAT_REG_NUM = 96;

		// Maximum number when the vertex shader floating point register is used as a 4x4 matrix.
		const u32 VS_UNIFORM_FLOAT_REG_MATRIX_NUM = VS_UNIFORM_FLOAT_REG_NUM / 16;

		/*!
			@brief Represents a matrix of vertex shader uniforms.
		*/
		class VertexShaderUniformMatrix : private nn::util::NonCopyable<VertexShaderUniformMatrix>
		{
		public:
			VertexShaderUniformMatrix(void);
			virtual ~VertexShaderUniformMatrix();

		public:
			void Initialize(void);
			void Finalize(void);

		public:
			bool m_Valid;
			bool m_Padding[3];

			// Index of floating-point registers for the vertex shader.
			u32 m_RegisterIndex;
			// Offset in the 3D command buffer.
			u32 m_Offset[4][4];
		};

		void GetVSUniformOffset(u8*			  bufferAddress,
								const u32	  startOffset,
								const GLsizei bufferSize,
								const u32	  uniformFloatRegIndex,
								u32*		  uniformOffsetArray);
	}  // namespace detail

	enum
	{
		COMMAND_CACHE_UNINITIALIZED,  // Defines the command list as uninitialized.
		COMMAND_CACHE_INITIALIZED,	  // Defines the command list as initialized.
		COMMAND_CACHE_FINALIZED,	  // Defines the command list as finalized.
		COMMAND_CACHE_BEGIN_SAVE,	  // Defines that saving the command list has started.
		COMMAND_CACHE_END_SAVE		  // Defines that the command list has been saved.
	};

	const u32 GRAPHICS_MAX_COMMAND_CACHE_MODELS_NUM = 128;
	const u32 GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM = np::graphics::detail::VS_UNIFORM_FLOAT_REG_MATRIX_NUM;

	/*!
		@brief Represents a command cache for saving and reusing commands.
	*/
	class CommandCache : private nn::util::NonCopyable<CommandCache>
	{
	public:
		CommandCache(void);
		virtual ~CommandCache(void);

	public:
		void Initialize(const u32		 CommandListSize,
						const u32		 RequestNum,
						const GLboolean	 copyCommandBuffer,
						const GLbitfield stateMask,
						const u32		 totalModelNum = 1);
		void Finalize(void);

	public:
		// The index of the vertex shader floating point register is saved in the specified array index.
		void SetVSUniformMatrixRegisterIndex(const u32 modelIndex, const u32 uniformMatrixIndex, const u32 regsiterIndex);

	public:
		// Start saving the command list.
		void BeginSave(void);
		// Save the offset to the start of the command list when rendering one model.
		void SaveCommandBufferStartOffset(const u32 modelIndex);
		// End saving the command list.
		void EndSave(void);

	public:
		// Update the vertex shader floating point matrix values specified by the array index of the specified matrix.
		void UpdateVSUniformMatrix(const u32 modelIndex, const u32 uniformMatrixIndex, const nn::math::MTX44& matrix);
		// Add the saved command list to the current command list.
		void Append(void);

	public:
		void Print(void);

	protected:
		u32 m_State;

		GLsizei	   m_CommandListSize;
		GLsizei	   m_CommandListRequestNum;
		GLboolean  m_CopyCommandBuffer;
		GLboolean  m_Padding[3];
		GLbitfield m_StateMask;

		u32 m_TotalModelNum;
		u32 m_ModelIndex;

		GLuint	m_CurrentCommandListId;
		GLuint	m_CommandListId;
		GLint	m_CommandBufferStartOffsetArray[GRAPHICS_MAX_COMMAND_CACHE_MODELS_NUM];
		GLuint	m_CommandBufferOffset;
		GLsizei m_CommandBufferSize;
		GLuint	m_CommandRequestBeginId;
		GLsizei m_CommandRequestSize;
		u8*		m_CommandBufferAddr;

		// Array used to store the 3D command buffer offset of the vertex shader uniform matrix.
		detail::VertexShaderUniformMatrix m_VertexShaderUniformMatrixArray2d[GRAPHICS_MAX_COMMAND_CACHE_MODELS_NUM]
																			[GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM];
	};

	void PrintCurrentCmdlist(void);

}}	// namespace np::graphics

#endif
