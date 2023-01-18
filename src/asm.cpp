#include "asm.h"

#define PUSH a.push(x86::r15);\
			 a.mov(x86::r15, x86::rsp);\
			 a.and_(x86::rsp, -0x10);\
			 a.sub(x86::rsp, 0x60);\
			 a.movaps(x86::ptr(x86::rsp, 0x50), x86::xmm0);\
			 a.movaps(x86::ptr(x86::rsp, 0x40), x86::xmm1);\
			 a.movaps(x86::ptr(x86::rsp, 0x30), x86::xmm2);\
			 a.movaps(x86::ptr(x86::rsp, 0x20), x86::xmm3);\
			 a.movaps(x86::ptr(x86::rsp, 0x10), x86::xmm4);\
			 a.movaps(x86::ptr(x86::rsp), x86::xmm5);\
			 a.push(x86::rax);\
			 a.push(x86::rcx);\
			 a.push(x86::rdx);\
			 a.push(x86::r8);\
			 a.push(x86::r9);\
			 a.push(x86::r10);\
			 a.push(x86::r11);\
			 a.sub(x86::rsp, 0x28)

#define POP  a.add(x86::rsp, 0x28);\
			 a.pop(x86::r11);\
			 a.pop(x86::r10);\
			 a.pop(x86::r9);\
			 a.pop(x86::r8);\
			 a.pop(x86::rdx);\
			 a.pop(x86::rcx);\
			 a.pop(x86::rax);\
			 a.movaps(x86::xmm5, x86::ptr(x86::rsp));\
			 a.movaps(x86::xmm4, x86::ptr(x86::rsp, 0x10));\
			 a.movaps(x86::xmm3, x86::ptr(x86::rsp, 0x20));\
			 a.movaps(x86::xmm2, x86::ptr(x86::rsp, 0x30));\
			 a.movaps(x86::xmm1, x86::ptr(x86::rsp, 0x40));\
			 a.movaps(x86::xmm0, x86::ptr(x86::rsp, 0x50));\
			 a.mov(x86::rsp, x86::r15);\
			 a.pop(x86::r15)

#define PUSHR a.push(x86::rax)

#define POPR  a.pop(x86::rax)

using namespace asmjit;

void InitAsm(CodeHolder& pCode, char* InjPoint, char*& CodeMemFree, const size_t NopSize = 0)
{
	char* bCodeMem = CodeMemFree;
	CodeMemFree = reinterpret_cast<char*>(reinterpret_cast<uint64_t>(CodeMemFree + 0x0F) & ~0x0F);
	size_t dCodeMem = static_cast<size_t>(CodeMemFree - bCodeMem);
	std::memset(bCodeMem, 0xCC, dCodeMem);
	TrampolineJmp(InjPoint, CodeMemFree, NopSize);
	pCode.relocateToBase(reinterpret_cast<uint64_t>(CodeMemFree));
	size_t CodeSize = pCode.codeSize();
	pCode.copySectionData(CodeMemFree, CodeSize, 0, CopySectionFlags::kPadTargetBuffer | CopySectionFlags::kPadSectionBuffer);
	CodeMemFree += CodeSize;
	pCode.reset();
}

extern void InjectAsm()
{
	const size_t alloc_size = 0x1000;
	auto pVirtualAlloc2 = reinterpret_cast<decltype(&::VirtualAlloc2)>(GetProcAddress(LoadLibrary(L"kernelbase.dll"), "VirtualAlloc2"));
	SYSTEM_INFO SystemInfo = {};
	GetSystemInfo(&SystemInfo);
	if (pVirtualAlloc2 != nullptr && use_virtualalloc2)
	{
		MEM_ADDRESS_REQUIREMENTS MemAddressRequirements = {};
		const uint64_t align = ~(static_cast<uint64_t>(SystemInfo.dwAllocationGranularity) - 1);
		MemAddressRequirements.LowestStartingAddress = reinterpret_cast<void*>((reinterpret_cast<uint64_t>(mInfo.lpBaseOfDll) + mInfo.SizeOfImage) & align);
		MemAddressRequirements.HighestEndingAddress = reinterpret_cast<void*>(((reinterpret_cast<uint64_t>(mInfo.lpBaseOfDll) - alloc_size + INT_MAX) & align) - 1);
		MemAddressRequirements.Alignment = 0;
		MEM_EXTENDED_PARAMETER ExtendedParameters = {};
		ExtendedParameters.Type = MemExtendedParameterAddressRequirements;
		ExtendedParameters.Pointer = &MemAddressRequirements;
		CodeMem = pVirtualAlloc2(GetCurrentProcess(), nullptr, alloc_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE, &ExtendedParameters, 1);
	}
	else
	{
		CodeMem = reinterpret_cast<void*>((reinterpret_cast<uint64_t>(mInfo.lpBaseOfDll) + mInfo.SizeOfImage));
		MCF::VirtualAllocNear(MCF::MemRegion("DarkSoulsIII.exe"), alloc_size, PAGE_EXECUTE_READWRITE, CodeMem, SystemInfo.dwAllocationGranularity);
	}
	static char* CodeMemFree = reinterpret_cast<char*>(CodeMem);

	if (test_out)
	{
		AllocConsoleOnce();
		std::cout << "CodeMem: " << CodeMem << "\n";
	}

	JitRuntime rt;
	CodeHolder code;
	Environment env = rt.environment();

	code.init(env);
	x86::Assembler a(&code);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(LockOnCodeAOB + 10);
	a.movss(x86::xmm1, x86::ptr(x86::rax, 0x8));
	a.call(LOC_CallAOB);
	a.jmp(LockOnCodeAOB + 10);
	a.int3();
	InitAsm(code, LockOnCodeAOB, CodeMemFree, 5);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, &BoneOffsetBasePointer);
	a.mov(x86::qword_ptr(x86::rax), x86::rcx);
	POPR;
	a.call(BOBP_CallAOB);
	a.jmp(BoneOffsetBasePointerAOB + 5);
	a.int3();
	InitAsm(code, BoneOffsetBasePointerAOB, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSH;
	a.mov(x86::rax, reinterpret_cast<uint64_t>(&pInitializeCamera));
	a.call(x86::qword_ptr(x86::rax));
	POP;
	a.call(CSLE_CallAOB);
	a.jmp(CallSaveLoadEndAOB + 5);
	a.int3();
	InitAsm(code, CallSaveLoadEndAOB, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSH;
	a.mov(x86::rax, reinterpret_cast<uint64_t>(&pDeinitializeCamera));
	a.call(x86::qword_ptr(x86::rax));
	POP;
	a.mov(x86::qword_ptr(x86::rax, -0x48), -2);
	a.jmp(DtorStartAOB + 8);
	a.int3();
	InitAsm(code, DtorStartAOB, CodeMemFree, 3);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(StrafeCheckAOB1 + 7);
	a.mov(x86::ptr(x86::rbx, 0x2C1), x86::dil);
	a.jmp(StrafeCheckAOB1 + 7);
	a.int3();
	InitAsm(code, StrafeCheckAOB1, CodeMemFree, 2);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(StrafeCheckAOB2 + 7);
	a.mov(x86::ptr(x86::rcx, 0x2C1), x86::dil);
	a.jmp(StrafeCheckAOB2 + 7);
	a.int3();
	InitAsm(code, StrafeCheckAOB2, CodeMemFree, 2);

	code.init(env);
	code.attach(&a);
	Label TAE_retjmp = a.newLabel();
	PUSH;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 0);
	a.je(TAE_retjmp);
	a.mov(x86::rcx, WorldChrMan);
	a.mov(x86::rcx, x86::ptr(x86::rcx));
	a.mov(x86::rcx, x86::ptr(x86::rcx, 0x80));
	a.mov(x86::rcx, x86::ptr(x86::rcx, 0x1F90));
	a.mov(x86::rcx, x86::ptr(x86::rcx, 0x10));
	a.cmp(x86::rbx, x86::rcx);
	a.jne(TAE_retjmp);
	a.mov(x86::rcx, &TAE_PtrBase);
	a.mov(x86::qword_ptr(x86::rcx), x86::rdi);
	a.mov(x86::rcx, x86::rdi);
	a.mov(x86::rcx, x86::ptr(x86::rcx, 0x18));
	a.mov(x86::rcx, x86::ptr(x86::rcx, 0x10));
	a.mov(x86::rax, &TAE_PtrNameOffset);
	a.mov(x86::qword_ptr(x86::rax), x86::rcx);
	a.mov(x86::rcx, x86::rdi);
	a.mov(x86::rcx, x86::ptr(x86::rcx, 0x8));
	a.mov(x86::rcx, x86::ptr(x86::rcx, 0x8));
	a.mov(x86::rcx, x86::ptr(x86::rcx));
	a.mov(x86::rax, &TAE_PtrOffset);
	a.mov(x86::qword_ptr(x86::rax), x86::rcx);
	a.mov(x86::rax, reinterpret_cast<uint64_t>(&pGetTAEData));
	a.call(x86::qword_ptr(x86::rax));
	a.bind(TAE_retjmp);
	POP;
	a.jmp(GetTAEAOB + 7);
	a.int3();
	InitAsm(code, GetTAEAOB, CodeMemFree, 2);

	/*
	code.init(env);
	code.attach(&a);
	a.mov(x86::rax, x86::ptr(x86::r14));
	a.mov(x86::ptr(x86::rcx), x86::rax);
	a.test(x86::r15, x86::r15);
	a.je(GenInputPtrAOB + 6);
	PUSH;
	a.mov(x86::rcx, &InputPtrBase);
	a.mov(x86::qword_ptr(x86::rcx), x86::rax);
	a.mov(x86::rax, reinterpret_cast<uint64_t>(&pGenerateInputPointers));
	a.call(x86::qword_ptr(x86::rax));
	POP;
	a.jmp(GenInputPtrAOB + 6);
	a.int3();
	InitAsm(code, GenInputPtrAOB, CodeMemFree, 1);
	*/

	code.init(env);
	code.attach(&a);
	a.mov(x86::ecx, 0x3E);
	a.cmp(x86::edx, 0x1770);
	a.jne(GenMsgCodeAOB + 5);
	a.mov(x86::edx, 0xA8F);
	a.jmp(GenMsgCodeAOB + 5);
	a.int3();
	InitAsm(code, GenMsgCodeAOB, CodeMemFree);

	if (FallDeathCodeAOB != nullptr)
	{
		code.init(env);
		code.attach(&a);
		PUSHR;
		a.mov(x86::rax, pIsFPS);
		a.cmp(x86::byte_ptr(x86::rax), 1);
		POPR;
		a.je(FallDeathCodeAOB + 7);
		a.mov(x86::byte_ptr(x86::rax, 0x90), 1);
		a.jmp(FallDeathCodeAOB + 7);
		a.int3();
		InitAsm(code, FallDeathCodeAOB, CodeMemFree, 2);
	}

	code.init(env);
	code.attach(&a);
	const uint8_t HSCAOB_buf[6] = { 0xF3, 0x0F, 0x10, 0x44, 0x90, 0x08 };
	a.mov(x86::rax, x86::ptr(x86::rcx, 0x50));
	a.movsxd(x86::rdx, x86::edx);
	a.embed(HSCAOB_buf, 6);
	a.mov(x86::r8, WorldChrMan);
	a.mov(x86::r8, x86::ptr(x86::r8));
	a.mov(x86::r8, x86::ptr(x86::r8, 0x80));
	a.add(x86::r8, 0x20C8);
	a.cmp(x86::rax, x86::r8);
	a.jne(HeadSizeCodeAOB + 13);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.jne(HeadSizeCodeAOB + 13);
	a.test(x86::edx, x86::edx);
	a.jne(HeadSizeCodeAOB + 13);
	a.mov(x86::ecx, 0xC1A00000); // -20.0f
	a.movd(x86::xmm0, x86::ecx);
	a.jmp(HeadSizeCodeAOB + 13);
	a.int3();
	InitAsm(code, HeadSizeCodeAOB, CodeMemFree, 8);

	code.init(env);
	code.attach(&a);
	a.mov(x86::esi, x86::ptr(x86::rax, 0x124));
	a.test(x86::edx, x86::edx);
	a.jne(UpperHandCodeAOB + 5);
	PUSHR;
	a.xor_(x86::eax, x86::eax);
	a.mov(x86::rcx, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rcx), 1);
	a.cmove(x86::esi, x86::eax);
	POPR;
	a.jmp(UpperHandCodeAOB + 5);
	a.int3();
	InitAsm(code, UpperHandCodeAOB, CodeMemFree, 1);

	code.init(env);
	code.attach(&a);
	Label kMul = a.newLabel();
	Label kMul_retjmp = a.newLabel();
	a.movss(x86::xmm2, x86::dword_ptr(x86::rsi, 0x280));
	PUSHR;
	a.mov(x86::rax, CSLang);
	a.mov(x86::eax, x86::dword_ptr(x86::rax));
	a.cmp(x86::eax, 8);
	a.jne(kMul_retjmp);
	a.mov(x86::rax, CSPcKeyConfig);
	a.mov(x86::rax, x86::ptr(x86::rax));
	a.movss(x86::xmm2, x86::dword_ptr(x86::rax, 0x710));
	a.mulss(x86::xmm2, x86::dword_ptr(kMul));
	a.bind(kMul_retjmp);
	POPR;
	a.jmp(CamSensCodeAOB + 8);
	a.int3();
	a.align(AlignMode::kZero, 0x10);
	a.bind(kMul);
	a.embedFloat(0.1);
	InitAsm(code, CamSensCodeAOB, CodeMemFree, 3);
		
	code.init(env);
	code.attach(&a);
	uint64_t MagicData[4] = { 0x6361707376003E54, 0x6E656D65766F6D20, 0x3E544E4F462F3C2E, 0x7765697620666F20 }; //Don't ask me. It works.
	Label MagicCode = a.newLabel();
	Label MagicCodeSpecial = a.newLabel();
	Label MagicInt64 = a.newLabel();
	a.mov(x86::edx, x86::dword_ptr(x86::rbx, 0xBC8));
	a.mov(x86::rax, CSLang);
	a.mov(x86::eax, x86::dword_ptr(x86::rax));
	a.cmp(x86::eax, 8);
	a.jne(MenuValCodeAOB + 6);
	a.mov(x86::rax, MagicPtr);
	a.mov(x86::rax, x86::qword_ptr(x86::rax, 0x48));
	a.cmp(x86::qword_ptr(MagicInt64), x86::rax);
	a.je(MagicCode);
	a.cmp(x86::qword_ptr(MagicInt64, 0x8), x86::rax);
	a.je(MagicCode);
	a.cmp(x86::qword_ptr(MagicInt64, 0x10), x86::rax);
	a.je(MagicCodeSpecial);
	a.jmp(MenuValCodeAOB + 6);
	a.bind(MagicCodeSpecial);
	a.mov(x86::rax, MagicPtr);
	a.mov(x86::rax, x86::qword_ptr(x86::rax, 0x40));
	a.cmp(x86::qword_ptr(MagicInt64, 0x18), x86::rax);
	a.jne(MenuValCodeAOB + 6);
	a.bind(MagicCode);
	a.mov(x86::rax, reinterpret_cast<uint64_t>(&fp_fov_base));
	a.movzx(x86::eax, x86::byte_ptr(x86::rax));
	a.shl(x86::edx, 1);
	a.add(x86::edx, x86::eax);
	a.sub(x86::edx, 0xA);
	a.jmp(MenuValCodeAOB + 6);
	a.int3();
	a.align(AlignMode::kZero, 0x8);
	a.bind(MagicInt64);
	a.embedDataArray(TypeId::kUInt64, MagicData, 6);
	InitAsm(code, MenuValCodeAOB, CodeMemFree, 1);

	code.init(env);
	code.attach(&a);
	Label StringPatchState = a.newLabel();
	Label SPS_retjmp = a.newLabel();
	PUSH;
	a.mov(x86::al, x86::ptr(StringPatchState));
	a.test(x86::al, x86::al);
	a.jne(SPS_retjmp);
	a.inc(x86::byte_ptr(StringPatchState));
	a.mov(x86::rax, reinterpret_cast<uint64_t>(&pPatchMsgRepo));
	a.call(x86::qword_ptr(x86::rax));
	a.bind(SPS_retjmp);
	POP;
	a.mov(x86::rbx, x86::byte_ptr(x86::rcx, 0x8));
	a.mov(x86::rdi, x86::rcx);
	a.jmp(StartScreenHookAOB + 7);
	a.int3();
	a.align(AlignMode::kZero, 0x10);
	a.bind(StringPatchState);
	a.embedInt8(0x0);
	InitAsm(code, StartScreenHookAOB, CodeMemFree, 2);

	code.init(env);
	code.attach(&a);
	Label Idle3 = a.newLabel();
	a.mov(x86::rcx, x86::rdi);
	a.movzx(x86::edx, x86::ax);
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	a.jne(HkRunCodeAOB + 6);
	a.cmp(x86::edx, 3);
	a.je(Idle3);
	a.mov(x86::rax, pToggleRunFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	a.jne(HkRunCodeAOB + 6);
	a.cmp(x86::dl, 2);
	a.mov(x86::eax, 1);
	a.cmove(x86::edx, x86::eax);
	a.jmp(HkRunCodeAOB + 6);
	a.bind(Idle3);
	a.mov(x86::rdx, WorldChrMan);
	a.mov(x86::rdx, x86::ptr(x86::rdx));
	a.mov(x86::rdx, x86::ptr(x86::rdx, 0x80));
	a.mov(x86::rdx, x86::ptr(x86::rdx, 0x1F90));
	a.mov(x86::rdx, x86::ptr(x86::rdx, 0x80));
	a.mov(x86::edx, x86::dword_ptr(x86::rdx, 0x12C));
	a.test(x86::edx, 4);
	a.mov(x86::edx, 3);
	a.jne(HkRunCodeAOB + 6);
	a.mov(x86::rax, x86::ptr(x86::rsi, 0x60));
	a.mov(x86::rax, x86::ptr(x86::rax, 0x28));
	a.mov(x86::rax, x86::ptr(x86::rax, 0x80));
	a.mov(x86::rax, x86::ptr(x86::rax, 0x110));
	a.mov(x86::rax, x86::ptr(x86::rax, 0x10));
	a.cmp(x86::dword_ptr(x86::rax, 0x140), 2);
	a.jb(HkRunCodeAOB + 6);
	a.mov(x86::dword_ptr(x86::rax, 0x140), 1);
	a.jmp(HkRunCodeAOB + 6);
	a.int3();
	InitAsm(code, HkRunCodeAOB, CodeMemFree, 1);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pToggleRunFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(RunStateCodeAOB + 7);
	a.or_(dword_ptr(x86::rdi, 0x12C), 4);
	a.jmp(RunStateCodeAOB + 7);
	a.int3();
	InitAsm(code, RunStateCodeAOB, CodeMemFree, 2);

	code.init(env);
	code.attach(&a);
	a.cmovbe(x86::rax, x86::rcx);
	a.mov(x86::eax, x86::dword_ptr(x86::rax));
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.jne(AimFadeCodeAOB + 6);
	PUSHR;
	a.mov(x86::rax, WorldChrMan);
	a.mov(x86::rax, x86::ptr(x86::rax));
	a.mov(x86::rax, x86::ptr(x86::rax, 0x80));
	a.cmp(x86::rdi, x86::rax);
	POPR;
	a.jne(AimFadeCodeAOB + 6);
	a.mov(x86::eax, 0x3F800000);
	a.jmp(AimFadeCodeAOB + 6);
	a.int3();
	InitAsm(code, AimFadeCodeAOB, CodeMemFree, 1);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB01 + 5);
	a.movdqa(x86::ptr(x86::rdi, 0x30), x86::xmm1);
	a.jmp(CamCodeAOB01 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB01, CodeMemFree);

	code.init(env);
	code.attach(&a);
	a.addps(x86::xmm0, x86::xmm6);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB02 + 7);
	a.movaps(x86::ptr(x86::rsi, 0x40), x86::xmm0);
	a.jmp(CamCodeAOB02 + 7);
	a.int3();
	InitAsm(code, CamCodeAOB02, CodeMemFree, 2);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB03 + 6);
	a.movdqa(x86::ptr(x86::r14, 0x30), x86::xmm6);
	a.jmp(CamCodeAOB03 + 6);
	a.int3();
	InitAsm(code, CamCodeAOB03, CodeMemFree, 1);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB04 + 5);
	a.movaps(x86::ptr(x86::r14, 0x30), x86::xmm2);
	a.jmp(CamCodeAOB04 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB04, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB05 + 5);
	a.movdqa(x86::ptr(x86::rsi, 0x40), x86::xmm7);
	a.jmp(CamCodeAOB05 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB05, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB06 + 5);
	a.movdqa(x86::ptr(x86::rsi, 0x40), x86::xmm0);
	a.jmp(CamCodeAOB06 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB06, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB07 + 5);
	a.movdqa(x86::ptr(x86::rcx, 0x30), x86::xmm1);
	a.jmp(CamCodeAOB07 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB07, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB08 + 5);
	a.movdqa(x86::ptr(x86::rbx, 0x10), x86::xmm1);
	a.jmp(CamCodeAOB08 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB08, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB09 + 5);
	a.movdqa(x86::ptr(x86::rsi, 0x20), x86::xmm4);
	a.jmp(CamCodeAOB09 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB09, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB10 + 5);
	a.movdqa(x86::ptr(x86::rcx, 0x10), x86::xmm1);
	a.jmp(CamCodeAOB10 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB10, CodeMemFree);

	code.init(env);
	code.attach(&a);
	a.movaps(x86::xmm1, x86::ptr(x86::rax, 0x10));
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB11 + 8);
	a.movdqa(x86::ptr(x86::rbx), x86::xmm0);
	a.jmp(CamCodeAOB11 + 8);
	a.int3();
	InitAsm(code, CamCodeAOB11, CodeMemFree, 3);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB12 + 7);
	a.movdqa(x86::ptr(x86::rcx), x86::xmm0);
	a.jmp(CamCodeAOB12 + 7);
	a.int3();
	InitAsm(code, CamCodeAOB12, CodeMemFree, 2);

	code.init(env);
	code.attach(&a);
	a.lea(x86::rdi, x86::ptr(x86::rsi, 0x10));
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB13 + 8);
	a.movdqa(x86::ptr(x86::rdi), x86::xmm5);
	a.jmp(CamCodeAOB13 + 8);
	a.int3();
	InitAsm(code, CamCodeAOB13, CodeMemFree, 3);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB14 + 5);
	a.movdqa(x86::ptr(x86::rbx, 0x20), x86::xmm0);
	a.jmp(CamCodeAOB14 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB14, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB15 + 5);
	a.movdqa(x86::ptr(x86::rsi, 0x30), x86::xmm0);
	a.jmp(CamCodeAOB15 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB15, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB16 + 5);
	a.movdqa(x86::ptr(x86::rcx, 0x20), x86::xmm0);
	a.jmp(CamCodeAOB16 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB16, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB17 + 5);
	a.movdqa(x86::ptr(x86::rbx, 0x30), x86::xmm1);
	a.jmp(CamCodeAOB17 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB17, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB18 + 5);
	a.movdqa(x86::ptr(x86::rsi, 0x40), x86::xmm0);
	a.jmp(CamCodeAOB18 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB18, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB19 + 5);
	a.movss(x86::ptr(x86::rsi, 0x50), x86::xmm1);
	a.jmp(CamCodeAOB19 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB19, CodeMemFree);

	code.init(env);
	code.attach(&a);
	Label CC20_retjmp = a.newLabel();
	a.mov(x86::ptr(x86::rsp, 0x20), x86::r15b);
	PUSH;
	a.mov(x86::rax, reinterpret_cast<uint64_t>(&pPerspectiveSwitch));
	a.call(x86::qword_ptr(x86::rax));
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	a.jne(CC20_retjmp);
	a.mov(x86::rax, reinterpret_cast<uint64_t>(&pAttachToHead));
	a.call(x86::qword_ptr(x86::rax));
	a.bind(CC20_retjmp);
	POP;
	a.jmp(CamCodeAOB20 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB20, CodeMemFree);

	code.init(env);
	code.attach(&a);
	a.lea(x86::rdx, x86::ptr(x86::rbx, 0x10));
	a.mov(x86::rcx, x86::rdi);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.jne(CamCodeAOB21 + 7);
	a.mov(x86::rdx, FieldArea);
	a.mov(x86::rdx, x86::ptr(x86::rdx));
	a.mov(x86::rdx, x86::ptr(x86::rdx, 0x18));
	a.mov(x86::rdx, x86::ptr(x86::rdx, 0xE8));
	a.lea(x86::rdx, x86::ptr(x86::rdx, 0x10));
	a.jmp(CamCodeAOB21 + 7);
	a.int3();
	InitAsm(code, CamCodeAOB21, CodeMemFree, 2);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB22 + 5);
	a.movdqa(x86::ptr(x86::rbx, 0x10), x86::xmm1);
	a.jmp(CamCodeAOB22 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB22, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB23 + 5);
	a.movdqa(x86::ptr(x86::rbx, 0x20), x86::xmm0);
	a.jmp(CamCodeAOB23 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB23, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB24 + 5);
	a.movdqa(x86::ptr(x86::rbx, 0x30), x86::xmm1);
	a.jmp(CamCodeAOB24 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB24, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB25 + 5);
	a.movdqa(x86::ptr(x86::rbx, 0x40), x86::xmm0);
	a.jmp(CamCodeAOB25 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB25, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB26 + 5);
	a.movaps(x86::ptr(x86::rbx, 0x40), x86::xmm8);
	a.jmp(CamCodeAOB26 + 5);
	a.int3();
	InitAsm(code, CamCodeAOB26, CodeMemFree);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB27 + 8);
	a.movaps(x86::ptr(x86::rbx, 0x90), x86::xmm1);
	a.jmp(CamCodeAOB27 + 8);
	a.int3();
	InitAsm(code, CamCodeAOB27, CodeMemFree, 3);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB28 + 8);
	a.movaps(x86::ptr(x86::rbx, 0x90), x86::xmm1);
	a.jmp(CamCodeAOB28 + 8);
	a.int3();
	InitAsm(code, CamCodeAOB28, CodeMemFree, 3);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB29 + 8);
	a.movaps(x86::ptr(x86::rbx, 0x90), x86::xmm1);
	a.jmp(CamCodeAOB29 + 8);
	a.int3();
	InitAsm(code, CamCodeAOB29, CodeMemFree, 3);

	code.init(env);
	code.attach(&a);
	PUSHR;
	a.mov(x86::rax, pIsFPS);
	a.cmp(x86::byte_ptr(x86::rax), 1);
	POPR;
	a.je(CamCodeAOB30 + 8);
	a.movaps(x86::ptr(x86::rbx, 0x90), x86::xmm0);
	a.jmp(CamCodeAOB30 + 8);
	a.int3();
	InitAsm(code, CamCodeAOB30, CodeMemFree, 3);
}