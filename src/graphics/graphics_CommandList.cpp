#include <np/graphics/graphics_CommandList.h>

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
		 CommandList Class Member Function
	------------------------------------------------------------------------ */

	CommandList::CommandList(void)
	: m_InitializeFlag(false)
	, m_SerialRunFlag(false)
	, m_StoreCommandListIndex(0)
	, m_RunningCommandListIndex(0)
	, m_BufferSize(0)
	, m_RequestNum(0)
	{
		for (u32 index = 0; index < GRAPHICS_COMMAND_LIST_NUM; index++)
		{
			m_CommandListIdArray[index] = 0;
		}
	}

	CommandList::~CommandList(void) { Finalize(); }

	void CommandList::Initialize(const GLsizei bufferSize, const GLsizei requestNum, const bool serialRunMode)
	{
		if (!m_InitializeFlag)
		{
			GRAPHICS_ASSERT_GL_ERROR();

			m_StoreCommandListIndex	  = 0;
			m_RunningCommandListIndex = 0;

			m_BufferSize = bufferSize;
			m_RequestNum = requestNum;

			nngxGenCmdlists(GRAPHICS_COMMAND_LIST_NUM, &m_CommandListIdArray[0]);
			for (u32 index = 0; index < GRAPHICS_COMMAND_LIST_NUM; index++)
			{
				GLuint id = m_CommandListIdArray[index];
				nngxBindCmdlist(id);
				nngxCmdlistStorage(m_BufferSize, m_RequestNum);
				GRAPHICS_ASSERT_GL_ERROR();

				nngxSetCmdlistParameteri(NN_GX_CMDLIST_RUN_MODE, NN_GX_CMDLIST_SERIAL_RUN);
			}
			nngxBindCmdlist(m_CommandListIdArray[m_StoreCommandListIndex]);
			GRAPHICS_ASSERT_GL_ERROR();

			m_SerialRunFlag = serialRunMode;
			if (m_SerialRunFlag)
			{
				nngxRunCmdlist();
			}
			else
			{
				nngxStopCmdlist();
			}
			GRAPHICS_ASSERT_GL_ERROR();

			m_InitializeFlag = true;
		}
	}

	void CommandList::Finalize(void)
	{
		if (m_InitializeFlag)
		{
			GRAPHICS_ASSERT_GL_ERROR();

			Clear();
			GRAPHICS_ASSERT_GL_ERROR();

			nngxDeleteCmdlists(GRAPHICS_COMMAND_LIST_NUM, &m_CommandListIdArray[0]);
			for (u32 index = 0; index < GRAPHICS_COMMAND_LIST_NUM; index++)
			{
				m_CommandListIdArray[index] = 0;
			}

			GRAPHICS_ASSERT_GL_ERROR();

			m_InitializeFlag = false;
		}
	}

	void CommandList::Run(void)
	{
		RunSingle();

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void CommandList::RunSingle(void)
	{
		nngxBindCmdlist(m_CommandListIdArray[m_StoreCommandListIndex]);
		nngxSplitDrawCmdlist();
		nngxRunCmdlist();
		nngxWaitCmdlistDone();
	}

	void CommandList::Swap(void)
	{
		Clear();

		if (m_SerialRunFlag)
		{
			nngxRunCmdlist();
		}
		else
		{
			nngxStopCmdlist();
		}

		m_RunningCommandListIndex = 0;
		m_StoreCommandListIndex	  = 0;
	}

	void CommandList::Stop(void) { nngxBindCmdlist(m_CommandListIdArray[m_RunningCommandListIndex]); }

	void CommandList::Wait(void)
	{
		nngxBindCmdlist(m_CommandListIdArray[m_RunningCommandListIndex]);
		nngxWaitCmdlistDone();

		if (m_SerialRunFlag)
		{
			nngxRunCmdlist();
		}
		else
		{
			nngxStopCmdlist();
		}
	}

	void CommandList::Clear(void)
	{
		nngxBindCmdlist(m_CommandListIdArray[m_RunningCommandListIndex]);
		nngxClearCmdlist();
	}

	void CommandList::SetSerialRun(const bool flag) { m_SerialRunFlag = flag; }

}}	// namespace np::graphics
