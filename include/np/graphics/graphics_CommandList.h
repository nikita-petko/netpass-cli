#ifndef NP_GRAPHICS_GRAPHICS_COMMAND_LIST_H_
#define NP_GRAPHICS_GRAPHICS_COMMAND_LIST_H_

#include <np/graphics/graphics_Utility.h>

namespace np { namespace graphics {

	const u32 GRAPHICS_COMMAND_LIST_NUM = 1;

	/*!
		@brief Represents a command list.
	*/
	class CommandList : private nn::util::NonCopyable<CommandList>
	{
	public:
		CommandList(void);
		virtual ~CommandList(void);

	public:
		void Initialize(const GLsizei bufferSize, const GLsizei requestNum, const bool serialRunMode = false);
		void Finalize(void);

	public:
		void Run(void);

	protected:
		void RunSingle(void);

	public:
		void Swap(void);
		void Stop(void);
		void Wait(void);
		void Clear(void);

	public:
		void SetSerialRun(const bool flag);

	protected:
		bool m_InitializeFlag;
		bool m_SerialRunFlag;
		bool m_Padding[2];

		u32 m_StoreCommandListIndex;
		u32 m_RunningCommandListIndex;

		GLuint m_CommandListIdArray[GRAPHICS_COMMAND_LIST_NUM];

		GLsizei m_BufferSize;
		GLsizei m_RequestNum;
	};
}}	// namespace np::graphics

#endif
