#include <nn/gx/CTR/gx_CommandAccess.h>

#include <np/graphics/graphics_MemoryManager.h>

#define DEBUG_PRINT 0

namespace np { namespace graphics {
	namespace {
		inline uptr MathRoundup(uptr x, int base) { return ((x) + ((base)-1)) & ~((base)-1); }
	}  // namespace

	namespace detail {
		MemoryManager::MemoryManager()
		: m_Initialized(false)
		, m_pStartAddrFcram(NULL)
		, m_CurrentAddrVramA(NULL)
		, m_CurrentAddrVramB(NULL)
		, m_HeapOnFcram()
		, m_AllocatedBlockSize(0)
		{
		}

		MemoryManager::~MemoryManager() { Finalize(); }

		void MemoryManager::Initialize(const uptr fcramAddress, const size_t memorySize)
		{
			if (m_Initialized)
			{
				return;
			}

			m_pStartAddrFcram = fcramAddress;
			NN_TASSERT_(m_pStartAddrFcram != NULL);

			m_AllocatedBlockSize = memorySize;

			m_HeapOnFcram.Initialize(m_pStartAddrFcram, memorySize);

			m_CurrentAddrVramA = nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMA);
			m_CurrentAddrVramB = nn::gx::GetVramStartAddr(nn::gx::MEM_VRAMB);

#if DEBUG_PRINT
			NN_TLOG_("[graphics::MemoryManager] Start address on Main Memory (FCRAM): 0x%08X\n", m_pStartAddrFcram);
			NN_TLOG_("[graphics::MemoryManager] Start address on VRAM-A: 0x%08X\n", m_CurrentAddrVramA);
			NN_TLOG_("[graphics::MemoryManager] Start address on VRAM-B: 0x%08X\n", m_CurrentAddrVramB);
#endif

			m_Initialized = true;
		}

		void MemoryManager::Finalize(void)
		{
			m_HeapOnFcram.Finalize();

			m_Initialized = false;
		}

		void MemoryManager::PrintFreeMemorySize(void)
		{
			NN_TLOG_("[graphics::MemoryManager] Free memory size on Main Memory (FCRAM) is 0x%06X(max 0x%06X)\n",
					 m_HeapOnFcram.GetTotalFreeSize(),
					 m_AllocatedBlockSize);
			NN_TLOG_("[graphics::MemoryManager] Free memory size on VRAM-A is 0x%06X(max 0x%06X)\n",
					 nn::gx::GetVramEndAddr(nn::gx::MEM_VRAMA) - m_CurrentAddrVramA,
					 nn::gx::GetVramSize(nn::gx::MEM_VRAMA));
			NN_TLOG_("[graphics::MemoryManager] Free memory size on VRAM-B is 0x%06X(max 0x%06X)\n",
					 nn::gx::GetVramEndAddr(nn::gx::MEM_VRAMB) - m_CurrentAddrVramB,
					 nn::gx::GetVramSize(nn::gx::MEM_VRAMB));
		}

		void* MemoryManager::Allocate(GLenum area, GLenum aim, GLuint id, GLsizei size)
		{
			if (!m_Initialized)
			{
				NN_TPANIC_("Not initialized.\n");
			}

			if (size == 0)
				return 0;

			// When allocating to VRAM, for simplicity treat VRAM like a stack.
			// Also, don't worry about fragmentation.
			// To deal with fragmentation, you may need to retain information about the memory allocated as "area," "aim" and "id" when
			// allocation is successful so they can be subsequently deallocated.
			//

			int	  addrAlign	 = 8;
			void* resultAddr = NULL;

			// Take into account the alignment restrictions on each data set's position
			switch (aim)
			{
				case NN_GX_MEM_SYSTEM:
					addrAlign = nn::gx::SYSTEMBUFFER_ALIGNMENT_MULTIPLEOF4;
					break;
				case NN_GX_MEM_TEXTURE:
					addrAlign = nn::gx::TEXTURE_ALIGNMENT;
					break;
				case NN_GX_MEM_VERTEXBUFFER:
					addrAlign = nn::gx::VERTEXBUFFER_ALIGNMENT_FLOAT;
					break;
				case NN_GX_MEM_RENDERBUFFER:
					// In this sample, we allocate a color buffer, a 16-bit depth buffer, and a 24-bit depth + 8-bit stencil buffer, but
					// allocation of a 24-bit depth buffer is not supported.
					//
					// To allocate a 24-bit depth buffer, the specified buffer must be nn::gx::RENDERBUFFER_ALIGNMENT_DEPTH24-byte aligned
					// (96-byte aligned).
					addrAlign = nn::gx::RENDERBUFFER_ALIGNMENT_COLOR;
					break;
				case NN_GX_MEM_DISPLAYBUFFER:
					addrAlign = nn::gx::DISPLAYBUFFER_ALIGNMENT;
					break;
				case NN_GX_MEM_COMMANDBUFFER:
					addrAlign = nn::gx::COMMANDBUFFER_ALIGNMENT;
					break;
				default:
					NN_TPANIC_("Invalid parameter.(0x%X)\n", aim);
					break;
			}

			switch (area)
			{
				case NN_GX_MEM_FCRAM:
					// When using FCRAM, no need to manage this because it is allocated from the expanded heap that is supported by the SDK
					if ((resultAddr = m_HeapOnFcram.Allocate(size, addrAlign)) == NULL)
					{
						NN_TPANIC_("Lack of resource on Main Memory (FCRAM).\n");
					}
					// Flush Cache
					if (aim == NN_GX_MEM_TEXTURE || aim == NN_GX_MEM_RENDERBUFFER || aim == NN_GX_MEM_DISPLAYBUFFER)
					{
						nngxUpdateBuffer(reinterpret_cast<void*>(resultAddr), size);
					}
					break;
				case NN_GX_MEM_VRAMA:
					{
						if (MathRoundup(m_CurrentAddrVramA, addrAlign) + size > nn::gx::GetVramEndAddr(nn::gx::MEM_VRAMA))
						{
							NN_TPANIC_("Lack of resource on VRAM-A.\n");
						}

						m_CurrentAddrVramA = MathRoundup(m_CurrentAddrVramA, addrAlign);
						resultAddr		   = reinterpret_cast<void*>(m_CurrentAddrVramA);
						m_CurrentAddrVramA += size;
					}
					break;
				case NN_GX_MEM_VRAMB:
					{
						if (MathRoundup(m_CurrentAddrVramB, addrAlign) + size > nn::gx::GetVramEndAddr(nn::gx::MEM_VRAMB))
						{
							NN_TPANIC_("Lack of resource on VRAM-B.\n");
						}

						m_CurrentAddrVramB = MathRoundup(m_CurrentAddrVramB, addrAlign);
						resultAddr		   = reinterpret_cast<void*>(m_CurrentAddrVramB);
						m_CurrentAddrVramB += size;
					}
					break;
				default:
					// This is not likely in normal use; PANIC
					NN_TPANIC_("Invalid parameter. (0x%X)\n", area);
					break;
			}

#if DEBUG_PRINT
			if (aim == NN_GX_MEM_SYSTEM)
			{
				NN_TLOG_("[graphics::MemoryManager] NN_GX_MEM_SYSTEM is allocated at 0x%08X - 0x%08X on %X\n",
						 resultAddr,
						 reinterpret_cast<int>(resultAddr) + size - 1,
						 area);
			}
			else if (aim == NN_GX_MEM_TEXTURE)
			{
				NN_TLOG_("[graphics::MemoryManager] NN_GX_MEM_TEXTURE is allocated at 0x%08X - 0x%08X on %X\n",
						 resultAddr,
						 reinterpret_cast<int>(resultAddr) + size - 1,
						 area);
			}
			else if (aim == NN_GX_MEM_VERTEXBUFFER)
			{
				NN_TLOG_("[graphics::MemoryManager] NN_GX_MEM_VERTEXBUFFER is allocated at 0x%08X - 0x%08X on %X\n",
						 resultAddr,
						 reinterpret_cast<int>(resultAddr) + size - 1,
						 area);
			}
			else if (aim == NN_GX_MEM_DISPLAYBUFFER)
			{
				NN_TLOG_("[graphics::MemoryManager] NN_GX_MEM_DISPLAYBUFFER is allocated at 0x%08X - 0x%08X on %X\n",
						 resultAddr,
						 reinterpret_cast<int>(resultAddr) + size - 1,
						 area);
			}
			else if (aim == NN_GX_MEM_RENDERBUFFER)
			{
				NN_TLOG_("[graphics::MemoryManager] NN_GX_MEM_RENDERBUFFER is allocated at 0x%08X - 0x%08X on %X\n",
						 resultAddr,
						 reinterpret_cast<int>(resultAddr) + size - 1,
						 area);
			}
			else if (aim == NN_GX_MEM_COMMANDBUFFER)
			{
				NN_TLOG_("[graphics::MemoryManager] NN_GX_MEM_COMMANDBUFFER is allocated at 0x%08X - 0x%08X on %X\n",
						 resultAddr,
						 reinterpret_cast<int>(resultAddr) + size - 1,
						 area);
			}
#else
			NN_UNUSED_VAR(id);
#endif	// #if DEBUG_PRINT

			return resultAddr;
		}

		/* Memory deallocator for DMPGL */
		void MemoryManager::Deallocate(GLenum area, GLenum aim, GLuint id, void* addr)
		{
			NN_UNUSED_VAR(aim);
			NN_UNUSED_VAR(id);

			if (!m_Initialized)
			{
				NN_TPANIC_("Not initialized.\n");
			}

#if DEBUG_PRINT
			NN_TLOG_("[graphics::MemoryManager] (id %d) was deallocated at 0x%08X on %X\n", id, addr, area);
#endif

			switch (area)
			{
				case NN_GX_MEM_FCRAM:
					m_HeapOnFcram.Free(addr);
					break;

				case NN_GX_MEM_VRAMA:
				case NN_GX_MEM_VRAMB:
					// Because the buffer in VRAM is simple, do not deallocate
					break;

				default:
					NN_TPANIC_("Invalid parameter.\n");
					break;
			}
		}
	}  // namespace detail

	namespace {
		graphics::detail::MemoryManager s_MemoryManager;
	}

	NN_WEAK_SYMBOL void InitializeMemoryManager(const uptr fcramAddress, const size_t memorySize)
	{
		s_MemoryManager.Initialize(fcramAddress, memorySize);
	}

	NN_WEAK_SYMBOL void FinalizeMemoryManager(void) { s_MemoryManager.Finalize(); }

	NN_WEAK_SYMBOL void PrintMemoryManagerInfo(void) { s_MemoryManager.PrintFreeMemorySize(); }

	NN_WEAK_SYMBOL void* GetAllocator(GLenum area, GLenum aim, GLuint id, GLsizei size)
	{
		return s_MemoryManager.Allocate(area, aim, id, size);
	}

	NN_WEAK_SYMBOL void GetDeallocator(GLenum area, GLenum aim, GLuint id, void* addr) { s_MemoryManager.Deallocate(area, aim, id, addr); }

	NN_WEAK_SYMBOL void* Alloc(size_t size) { return s_MemoryManager.Allocate(NN_GX_MEM_FCRAM, NN_GX_MEM_SYSTEM, 0, size); }

	NN_WEAK_SYMBOL void Free(void* ptr) { s_MemoryManager.Deallocate(NN_GX_MEM_FCRAM, NN_GX_MEM_SYSTEM, 0, ptr); }

}}	// namespace np::graphics
