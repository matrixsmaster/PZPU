/*
 *  Copyright (C) 2014  Soloviov Dmitry
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef DOSCARD_H_
#define DOSCARD_H_

/* Specific Inclusion */
#ifdef DOSCARD_SOURCE
#include <llvm/IR/LLVMContext.h>
#include <llvm/ADT/Triple.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/CodeGen/LinkAllCodegenComponents.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/JITEventListener.h>
#include <llvm/ExecutionEngine/JITMemoryManager.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/Format.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/MathExtras.h>
#include <llvm/Support/Memory.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/PluginLoader.h>
#include <llvm/Support/PrettyStackTrace.h>
#include <llvm/Support/Process.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/Signals.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Instrumentation.h>
#endif

/* Common Inclusion */
#include <vector>
#include <string>
#include "ldbwrap.h"

/* ****************** DosCard Generic Constants ****************** */

#define DEFAULTLIBNAME "libdbwrap.bc"
#define VERSTRMAXLEN 1536
#define VERSIONSTR "0.0.5"
#define VERINFOTEMPL "libDosCard ver. %s build %s\nCompiled with %s on %s\nwrapper: %s"

namespace doscard {

/* ****************** DosCard data types ****************** */

enum EDOSCRDState {
	DOSCRD_NOT_READY = 0,
	DOSCRD_LOADED,
	DOSCRD_INITED,
	DOSCRD_RUNNING,
	DOSCRD_SHUTDOWN,
	DOSCRD_LOADFAIL,
	DOSCRD_PAUSED
};

#ifdef DOSCARD_SOURCE

typedef std::vector<llvm::Function*> DCFuncs;

typedef struct {
	llvm::LLVMContext* context;
	llvm::Module* module;
	llvm::EngineBuilder* engbld;
	llvm::ExecutionEngine* engine;
	DCFuncs* funcs;
} DCPHolder;

typedef std::vector<llvm::GenericValue> DCArgs;

#else

//Fillers
typedef int DCPHolder;
typedef std::vector<std::string> DCArgs;

#endif

/* ****************** DosCard Main Class Definition ****************** */

class CDosCard {
public:
	CDosCard(bool autoload);
	~CDosCard();

	/// Try to load binary blob with filename provided.
	bool TryLoad(const char* filename);

	/// Returns current state of class instance.
	EDOSCRDState GetCurrentState();

	/// Returns a pointer to libdoscard settings block.
	dosbox::LDB_Settings* GetSettings();

	/// Returns a safe pointer (not needed to be destroyed) to 0-terminated string.
	char* GetVersionStringSafe();

	/// Try to apply settings provided.
	bool ApplySettings(dosbox::LDB_Settings* pset);

	/// Prepare instance for execution.
	bool Prepare();

	/// Run instance (which eventually will create a thread).
	int Run();

	/// Pause/Resume VM.
	void SetPause(bool paused);

	/// Do NOT use this function.
	void DoNotCallRunner();

	/// Try to set capabilities flags.
	int SetCapabilities(LDBI_caps flags);

	/// Returns a pointer to a valid VM video framebuffer and modifies width and height args.
	uint32_t* GetFramebuffer(int* w, int* h);

	/// Puts event to a FIFO buffer of VM.
	void PutEvent(dosbox::LDB_UIEvent e);

	/// Puts a string to VM's TTY interface.
	void PutString(char* str);

	/// Returns a pointer to a vector filled with VM's messages.
	LDBI_MesgVec* GetMessages();

	/// Fills a sound format request structure.
	bool GetSoundFormat(dosbox::LDB_SoundInfo* format);

	/// Fills up a buffer with sound data generated by VM.
	uint32_t FillSound(LDBI_SndSample* buf, uint32_t samples);

private:
	EDOSCRDState state;
	dosbox::LDB_Settings* settings;
	char* verstr;
	DCPHolder* phld;
	uint32_t* framebuffer;
	uint64_t framebufsz;
	pthread_t dosthread;
	LDBI_MesgVec msgbuff;

	void FreeModule();
	bool LoadFunctions();
	//(void)
	DCArgs GenArgs(void);
	//(LDB_Settings*)
	DCArgs GenArgs(dosbox::LDB_Settings* pset);
	//(void*,uint64_t)
	DCArgs GenArgs(void* ptr, uint64_t len);
};

/* ****************** DosCard library control exports ****************** */

/**
 *  LibDosCardInit(int verb) should be called before using any
 *  other function/class/veriable/reference/etc.
 */
void LibDosCardInit(int verb);

/**
 * LibDosCardExit(void) should be called before terminating host
 * process to properly finalize all modules.
 */
void LibDosCardExit(void);

} //namespace doscard

#endif /* DOSCARD_H_ */
