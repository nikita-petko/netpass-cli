#include <np/graphics/graphics_CommandCache.h>

namespace np { namespace graphics {

	namespace detail {
		VertexShaderUniformMatrix::VertexShaderUniformMatrix(void) : m_Valid(false), m_RegisterIndex(0)
		{
			for (u32 i = 0; i < 4; i++)
			{
				for (u32 j = 0; j < 4; j++)
				{
					m_Offset[i][j] = 0;
				}
			}
		}

		VertexShaderUniformMatrix::~VertexShaderUniformMatrix() {}

		void VertexShaderUniformMatrix::Initialize(void)
		{
			m_Valid			= false;
			m_RegisterIndex = 0;
			for (u32 i = 0; i < 4; i++)
			{
				for (u32 j = 0; j < 4; j++)
				{
					m_Offset[i][j] = 0;
				}
			}
		}

		void VertexShaderUniformMatrix::Finalize(void) {}
	}  // namespace detail

	/* ------------------------------------------------------------------------
		CommandCache Class Member Function
	------------------------------------------------------------------------ */

	CommandCache::CommandCache(void)
	: m_State(COMMAND_CACHE_UNINITIALIZED)
	, m_CommandListSize(0x100000)
	, m_CommandListRequestNum(32)
	, m_CopyCommandBuffer(GL_TRUE)
	, m_StateMask(0)
	, m_TotalModelNum(0)
	, m_ModelIndex(0)
	, m_CurrentCommandListId(0)
	, m_CommandListId(0)
	, m_CommandBufferOffset(0)
	, m_CommandBufferSize(0)
	, m_CommandRequestBeginId(0)
	, m_CommandRequestSize(0)
	, m_CommandBufferAddr(NULL)
	{
		for (u32 modelIndex = 0; modelIndex < GRAPHICS_MAX_COMMAND_CACHE_MODELS_NUM; modelIndex++)
		{
			m_CommandBufferStartOffsetArray[modelIndex] = 0;

			for (u32 matrixIndex = 0; matrixIndex < GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM; matrixIndex++)
			{
				m_VertexShaderUniformMatrixArray2d[modelIndex][matrixIndex].Initialize();
			}
		}
	}

	CommandCache::~CommandCache() { CommandCache::Finalize(); }

	void CommandCache::Initialize(const u32		   CommandListSize,
								  const u32		   RequestNum,
								  const GLboolean  copyCommandBuffer,
								  const GLbitfield stateMask,
								  const u32		   totalModelNum)
	{
		if (totalModelNum >= GRAPHICS_MAX_COMMAND_CACHE_MODELS_NUM)
		{
			NN_TPANIC_("totalModelNum(%d) is too large. (Max = %d)\n", totalModelNum, GRAPHICS_MAX_COMMAND_CACHE_MODELS_NUM);
		}

		m_ModelIndex = 0;

		m_CommandListSize		= CommandListSize;
		m_CommandListRequestNum = RequestNum;
		m_TotalModelNum			= totalModelNum;
		m_CopyCommandBuffer		= copyCommandBuffer;
		m_StateMask				= stateMask;

		m_State = COMMAND_CACHE_INITIALIZED;
	}

	void CommandCache::Finalize(void)
	{
		GRAPHICS_ASSERT_GL_ERROR();

		if (m_CommandListId != 0)
		{
			nngxDeleteCmdlists(1, &m_CommandListId);
			m_CommandListId = 0;

			GRAPHICS_ASSERT_GL_ERROR();
		}

		for (u32 modelIndex = 0; modelIndex < GRAPHICS_MAX_COMMAND_CACHE_MODELS_NUM; modelIndex++)
		{
			m_CommandBufferStartOffsetArray[modelIndex] = 0;

			for (u32 matrixIndex = 0; matrixIndex < GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM; matrixIndex++)
			{
				m_VertexShaderUniformMatrixArray2d[modelIndex][matrixIndex].Initialize();
			}
		}

		m_State = COMMAND_CACHE_FINALIZED;
	}

	void CommandCache::SetVSUniformMatrixRegisterIndex(const u32 modelIndex, const u32 uniformMatrixIndex, const u32 registerIndex)
	{
		if (!(modelIndex < m_TotalModelNum))
		{
			NN_TPANIC_("modelIndex(%d) must be < m_TotalModelNum(%d).\n", modelIndex, m_TotalModelNum);
		}

		if (!(uniformMatrixIndex < GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM))
		{
			NN_TPANIC_("uniformMatrixIndex(%d) must be < GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM(%d).\n",
					   uniformMatrixIndex,
					   GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM);
		}

		m_VertexShaderUniformMatrixArray2d[modelIndex][uniformMatrixIndex].m_Valid		   = true;
		m_VertexShaderUniformMatrixArray2d[modelIndex][uniformMatrixIndex].m_RegisterIndex = registerIndex;
	}

	void CommandCache::BeginSave(void)
	{
		if (m_State != COMMAND_CACHE_INITIALIZED)
		{
			NN_TPANIC_("m_State != COMMAND_CACHE_INITIALIZED.\n");
		}

		GRAPHICS_ASSERT_GL_ERROR();

		// Save the ID of the command list that is currently bound.
		nngxGetCmdlistParameteri(NN_GX_CMDLIST_BINDING, (GLint*)&m_CurrentCommandListId);

		// Allocate the command list at the save destination.
		nngxGenCmdlists(1, &m_CommandListId);
		nngxBindCmdlist(m_CommandListId);
		nngxCmdlistStorage(m_CommandListSize, m_CommandListRequestNum);

		// Start saving the command list.
		nngxStartCmdlistSave();

		m_State = COMMAND_CACHE_BEGIN_SAVE;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void CommandCache::SaveCommandBufferStartOffset(const u32 modelIndex)
	{
		if (m_State != COMMAND_CACHE_BEGIN_SAVE)
		{
			NN_TPANIC_("m_State != COMMAND_CACHE_BEGIN_SAVE.\n");
		}

		if (modelIndex < m_TotalModelNum)
		{
			nngxGetCmdlistParameteri(NN_GX_CMDLIST_USED_BUFSIZE, &m_CommandBufferStartOffsetArray[m_ModelIndex]);
			m_ModelIndex += 1;
		}
		else
		{
			NN_TPANIC_("modelIndex(%d) must be < m_TotalModelNum(%d)\n", modelIndex, m_TotalModelNum);
		}
	}

	void CommandCache::EndSave(void)
	{
		GRAPHICS_ASSERT_GL_ERROR();

		if (m_State != COMMAND_CACHE_BEGIN_SAVE)
		{
			NN_TPANIC_("m_State != COMMAND_CACHE_BEGIN_SAVE.\n");
		}

		if (m_ModelIndex == 0)
		{
			NN_TPANIC_("SaveCommandBufferStartOffset() must be called more than once.\n");
		}

		if (!m_CopyCommandBuffer)
		{
			nngxSplitDrawCmdlist();
		}

		GRAPHICS_ASSERT_GL_ERROR();

		// End saving the command list.
		nngxStopCmdlistSave(&m_CommandBufferOffset, &m_CommandBufferSize, &m_CommandRequestBeginId, &m_CommandRequestSize);

		GRAPHICS_ASSERT_GL_ERROR();

		// Get the start address of the 3D command buffer of the save destination command list.
		nngxGetCmdlistParameteri(NN_GX_CMDLIST_TOP_BUFADDR, (GLint*)&m_CommandBufferAddr);
		// Add the byte offset for the save start address to it.
		m_CommandBufferAddr += m_CommandBufferOffset;

		GRAPHICS_ASSERT_GL_ERROR();

		for (u32 modelIndex = 0; modelIndex < m_TotalModelNum; modelIndex++)
		{
			// Get the matrix offset value for the 3D command buffer of the save destination command list.
			for (u32 uniformMatrixIndex = 0; uniformMatrixIndex < GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM; uniformMatrixIndex++)
			{
				if (m_VertexShaderUniformMatrixArray2d[modelIndex][uniformMatrixIndex].m_Valid)
				{
					for (u32 index = 0; index < 4; index++)
					{
						graphics::detail::GetVSUniformOffset(
							m_CommandBufferAddr,
							m_CommandBufferStartOffsetArray[modelIndex] - m_CommandBufferOffset,
							m_CommandBufferSize,
							m_VertexShaderUniformMatrixArray2d[modelIndex][uniformMatrixIndex].m_RegisterIndex + index,
							&m_VertexShaderUniformMatrixArray2d[modelIndex][uniformMatrixIndex].m_Offset[index][0]);
					}
				}
			}
		}

		m_ModelIndex = 0;

		// Bind the command list again.
		nngxBindCmdlist(m_CurrentCommandListId);

		m_State = COMMAND_CACHE_END_SAVE;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void CommandCache::UpdateVSUniformMatrix(const u32 modelIndex, const u32 uniformMatrixIndex, const nn::math::MTX44& matrix)
	{
		if (m_State != COMMAND_CACHE_END_SAVE)
		{
			NN_TPANIC_("m_State != COMMAND_CACHE_END_SAVE.\n");
		}

		if (!(modelIndex < m_TotalModelNum))
		{
			NN_TPANIC_("modelIndex(%d) must be < m_TotalModelNum(%d).\n", modelIndex, m_TotalModelNum);
		}

		if (!(uniformMatrixIndex < GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM))
		{
			NN_TPANIC_("uniformMatrixIndex(%d) must be < GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM(%d)\n",
					   uniformMatrixIndex,
					   GRAPHICS_MAX_COMMAND_CACHE_MATRIX_NUM);
		}

		GLfloat matrixArray[16];
		for (u32 index = 0; index < 16; index++)
		{
			matrixArray[index] = matrix.m[index & 3][index >> 2];
		}

		for (u32 index = 0; index < 4; index++)
		{
			*(reinterpret_cast<u32*>(
				&m_CommandBufferAddr[m_VertexShaderUniformMatrixArray2d[modelIndex][uniformMatrixIndex].m_Offset[index][3]])) =
				*(reinterpret_cast<u32*>(&matrixArray[0 + index]));
			*(reinterpret_cast<u32*>(
				&m_CommandBufferAddr[m_VertexShaderUniformMatrixArray2d[modelIndex][uniformMatrixIndex].m_Offset[index][2]])) =
				*(reinterpret_cast<u32*>(&matrixArray[4 + index]));
			*(reinterpret_cast<u32*>(
				&m_CommandBufferAddr[m_VertexShaderUniformMatrixArray2d[modelIndex][uniformMatrixIndex].m_Offset[index][1]])) =
				*(reinterpret_cast<u32*>(&matrixArray[8 + index]));
			*(reinterpret_cast<u32*>(
				&m_CommandBufferAddr[m_VertexShaderUniformMatrixArray2d[modelIndex][uniformMatrixIndex].m_Offset[index][0]])) =
				*(reinterpret_cast<u32*>(&matrixArray[12 + index]));
		}
	}

	void CommandCache::Append(void)
	{
		GRAPHICS_ASSERT_GL_ERROR();

		if (m_State != COMMAND_CACHE_END_SAVE)
		{
			NN_TPANIC_("m_State != COMMAND_CACHE_END_SAVE.\n");
		}

		// Add the saved command list to the current command list.
		nngxUseSavedCmdlist(m_CommandListId,
							m_CommandBufferOffset,
							m_CommandBufferSize,
							m_CommandRequestBeginId,
							m_CommandRequestSize,
							m_StateMask,
							m_CopyCommandBuffer);

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void CommandCache::Print(void)
	{
		if (m_State == COMMAND_CACHE_UNINITIALIZED)
		{
			NN_LOG("CommandCache : State = COMMAND_CACHE_UNINITIALIZED\n");
		}
		else if (m_State == COMMAND_CACHE_INITIALIZED)
		{
			NN_LOG("CommandCache : State = COMMAND_CACHE_INITIALIZED\n");
		}
		else if (m_State == COMMAND_CACHE_FINALIZED)
		{
			NN_LOG("CommandCache : State = COMMAND_CACHE_FINALIZED\n");
		}
		else if (m_State == COMMAND_CACHE_BEGIN_SAVE)
		{
			NN_LOG("CommandCache : State = COMMAND_CACHE_BEGIN_SAVE\n");
		}
		else if (m_State == COMMAND_CACHE_END_SAVE)
		{
			NN_LOG("CommandCache : State = COMMAND_CACHE_END_SAVE \n");
		}

		NN_LOG("  commandListId  = %d\n", m_CommandListId);
		if (m_CopyCommandBuffer)
		{
			NN_LOG("  Copy 3d command buffer\n");
		}
		else
		{
			NN_LOG("  Refer 3d command buffer\n");
		}
		NN_LOG("  bufferOffset   = %d, bufferSize  = %d\n", m_CommandBufferOffset, m_CommandBufferSize);
		NN_LOG("  requestBeginId = %d, requestSize = %d\n", m_CommandRequestBeginId, m_CommandRequestSize);
		NN_LOG("  modelIndex = %d, totalModelNum = %d\n", m_ModelIndex, m_TotalModelNum);
	}

	namespace detail {
		void GetVSUniformOffset(u8*			  bufferAddress,
								const u32	  startOffset,
								const GLsizei bufferSize,
								const u32	  uniformFloatRegIndex,
								u32*		  uniformOffsetArray)
		{
			u32 const_current_addr	= 0xffffffff;
			u32 const_current_index = 0;
			u32 const_f32_mode		= 0;

			for (u32 i = 0; i < 4; i++)
			{
				uniformOffsetArray[i] = 0xffffffff;
			}

			for (u32 offset = startOffset; offset < bufferSize;)
			{
				CommandBuffer3d* command;
				command = (CommandBuffer3d*)&bufferAddress[offset];

				if (command->size == 0)
				{
					// Single command.
					if (command->be)
					{
						if (command->addr == VS_UNIFORM_FLOAT_REG_INDEX)
						{
							const_current_addr	= command->data & 0xff;
							const_current_index = 0;
							const_f32_mode		= (command->data & 0x80000000);
						}
						else if ((command->addr >= VS_UNIFORM_FLOAT_REG_VALUE_BEGIN) && (command->addr <= VS_UNIFORM_FLOAT_REG_VALUE_END))
						{
							if (const_current_addr == uniformFloatRegIndex)
							{
								uniformOffsetArray[const_current_index] = offset;
								if (const_current_index == 3)
								{
									break;
								}
							}

							++const_current_index;
							if (const_f32_mode ? (const_current_index == 4) : (const_current_index == 3))
							{
								const_current_addr++;
								const_current_index = 0;
							}
						}
					}
					offset += 8;
				}
				else
				{
					// Burst command.
					if (command->be)
					{
						if (command->seq)
						{
							// Sequential mode.
							if (!((VS_UNIFORM_FLOAT_REG_INDEX > command->addr + command->size) ||
								  command->addr > VS_UNIFORM_FLOAT_REG_VALUE_END))
							{
								u32 i = 0;
								for (i = 0; i < command->size + 1; i++)
								{
									unsigned addr = command->addr + i;
									if (addr == VS_UNIFORM_FLOAT_REG_INDEX)
									{
										const_current_addr	= ((i == 0) ? command->data : bufferAddress[offset + 4 + 4 * i]) & 0xff;
										const_current_index = 0;
										const_f32_mode		= (((i == 0) ? command->data : bufferAddress[offset + 4 + 4 * i]) & 0x80000000);
									}
									else if ((addr >= VS_UNIFORM_FLOAT_REG_VALUE_BEGIN) && (addr <= VS_UNIFORM_FLOAT_REG_VALUE_END))
									{
										if (const_current_addr == uniformFloatRegIndex)
										{
											uniformOffsetArray[const_current_index] = (i == 0) ? offset : (offset + 4 + 4 * i);
											if (const_current_index == 3)
											{
												break;
											}
										}
										++const_current_index;
										if (const_f32_mode ? (const_current_index == 4) : (const_current_index == 3))
										{
											const_current_addr++;
											const_current_index = 0;
										}
									}
								}
								if (i != command->size + 1)
								{
									break;
								}
							}
						}
						else
						{
							// Non-sequential mode.
							if (command->addr == VS_UNIFORM_FLOAT_REG_INDEX)
							{
								const_current_addr	= (bufferAddress[offset + 4 + 4 * command->size]) & 0xff;
								const_current_index = 0;
								const_f32_mode		= (bufferAddress[offset + 4 + 4 * command->size] & 0x80000000);
							}
							else if (command->addr >= VS_UNIFORM_FLOAT_REG_VALUE_BEGIN && command->addr <= VS_UNIFORM_FLOAT_REG_VALUE_END)
							{
								u32 i = 0;
								for (i = 0; i < command->size + 1; i++)
								{
									if (const_current_addr == uniformFloatRegIndex)
									{
										uniformOffsetArray[const_current_index] = (i == 0) ? offset : (offset + 4 + 4 * i);
										if (const_current_index == 3)
										{
											break;
										}
									}
									++const_current_index;
									if (const_f32_mode ? (const_current_index == 4) : (const_current_index == 3))
									{
										const_current_addr++;
										const_current_index = 0;
									}
								}

								if (i != command->size + 1)
								{
									break;
								}
							}
						}
					}
					offset += (command->size + 1) * 4 + 4;
					if (offset & 4)
					{
						offset += 4;
					}
				}
			}
		}
	}  // namespace detail

	void PrintCurrentCmdlist(void)
	{
		GLint bufferSize = 0;
		nngxGetCmdlistParameteri(NN_GX_CMDLIST_USED_BUFSIZE, &bufferSize);

		GLint maxBufferSize = 0;
		nngxGetCmdlistParameteri(NN_GX_CMDLIST_MAX_BUFSIZE, &maxBufferSize);

		GLint reqCount = 0;
		nngxGetCmdlistParameteri(NN_GX_CMDLIST_USED_REQCOUNT, &reqCount);

		GLint maxReqCount = 0;
		nngxGetCmdlistParameteri(NN_GX_CMDLIST_MAX_REQCOUNT, &maxReqCount);

		NN_LOG("CommandList information\n");
		NN_LOG("  bufferSize = %d/%d\n", bufferSize, maxBufferSize);
		NN_LOG("  reqCount = %d/%d\n", reqCount, maxReqCount);
	}

}}	// namespace np::graphics
