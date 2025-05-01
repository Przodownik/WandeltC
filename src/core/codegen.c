#include "codegen.h"

#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>

extern Context global_context; // from compiler_internal.h

typedef struct _CodegenContext
{
	LLVMContextRef llvm_context;
	LLVMModuleRef llvm_module;
	LLVMBuilderRef llvm_builder;
} CodegenContext;

LLVMValueRef codegen_emit_expression(CodegenContext* context, Expression* expression)
{
	switch (expression->kind)
	{
	case EXPRESSION_LITERAL:
		return LLVMConstInt(LLVMInt32TypeInContext(context->llvm_context), expression->literal.int_value, false);
	default:
		break;
	}
}

void codegen_emit_return_statement(CodegenContext* context, Statement* statement)
{
	LLVMValueRef value = codegen_emit_expression(context, statement->return_.expression);
	LLVMBuildRet(context->llvm_builder, value);
}

void codegen_emit_statement(CodegenContext* context, Statement* statement)
{
	switch (statement->type)
	{
	case STATEMENT_RETURN:
		codegen_emit_return_statement(context, statement);
		break;
	default:
		break;
	}
}

void codegen_emit_function(CodegenContext* context, Declaration* declaration)
{
	FunctionSignature fn_signature = declaration->function.signature;

	LLVMTypeRef int32_type = LLVMIntTypeInContext(context->llvm_context, 32);

	LLVMTypeRef func_type = LLVMFunctionType(int32_type, NULL, 0, false);

	LLVMValueRef function = LLVMAddFunction(context->llvm_module, fn_signature.name, func_type);
	declaration->handle   = function;

	LLVMSetFunctionCallConv(function, LLVMCCallConv);
	// LLVMSetLinkage(function, LLVMInternalLinkage);
	LLVMSetVisibility(function, LLVMDefaultVisibility);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context->llvm_context, declaration->handle, "entry");
	LLVMPositionBuilderAtEnd(context->llvm_builder, entry);

	const Statement* body_statement = declaration->function.body->compound.first;

	while (body_statement != nullptr)
	{
		codegen_emit_statement(context, body_statement);
		body_statement = body_statement->next;
	}
}

typedef struct _PlatformTarget
{
	const char* target_triple;
	int alloca_address_space;
	LLVMTargetRef target_ref;
	LLVMTargetMachineRef machine_ref;
	LLVMTargetDataRef target_data_ref;
} PlatformTarget;

PlatformTarget codegen_initialize_target()
{
	LLVMInitializeX86AsmParser();
	LLVMInitializeX86AsmPrinter();
	LLVMInitializeX86TargetInfo();
	LLVMInitializeX86Target();
	LLVMInitializeX86Disassembler();
	LLVMInitializeX86TargetMC();

	PlatformTarget platform_target;
	platform_target.target_triple        = LLVM_DEFAULT_TARGET_TRIPLE;
	platform_target.alloca_address_space = 0;

	char* err = nullptr;
	VERIFY(LLVMGetTargetFromTriple(platform_target.target_triple, &platform_target.target_ref, &err) == 0,
	       "Could not create target from triple %s", platform_target.target_triple);

	platform_target.machine_ref = LLVMCreateTargetMachine(platform_target.target_ref, platform_target.target_triple, "",
	                                                      "", LLVMCodeGenLevelNone, LLVMRelocPIC, LLVMCodeModelDefault);

	platform_target.target_data_ref = LLVMCreateTargetDataLayout(platform_target.machine_ref);

	return platform_target;
}

void codegen_linker_link(const char* output_file)
{
}

void codegen_generate(CompilerBuildOptions* build_options)
{
	TRACE(ANSI_COLOR_CYAN "Codegen started...\n" ANSI_COLOR_RESET);

	CodegenContext context;
	context.llvm_context = LLVMContextCreate();
	context.llvm_module  = LLVMModuleCreateWithNameInContext("module:core", context.llvm_context);
	context.llvm_builder = LLVMCreateBuilder();

	PlatformTarget platform_target = codegen_initialize_target();

	LLVMSetSourceFileName(context.llvm_module, "main.wdt", strlen("main.wdt"));

	char* layout = LLVMCopyStringRepOfTargetData(platform_target.target_data_ref);
	LLVMSetDataLayout(context.llvm_module, layout);
	LLVMDisposeMessage(layout);

	LLVMSetTarget(context.llvm_module, platform_target.target_triple);

	Clock clock = clock_create();

	for (uint64 i = 0; i < vector_get_length(global_context.functions_declarations); ++i)
	{
		Declaration* function_declaration = global_context.functions_declarations[i];

		codegen_emit_function(&context, function_declaration);
	}

	char* error = "";
	if (LLVMVerifyModule(context.llvm_module, LLVMPrintMessageAction, &error))
	{
		if (*error)
			ERROR("Could not verify IR: %s\n", error);

		ERROR("Could not verify module IR.\n");

		return;
	}

	char* err = "";
	if (LLVMTargetMachineEmitToFile(platform_target.machine_ref, context.llvm_module, "main.obj", LLVMObjectFile,
	                                &error))
	{
		ERROR("Could not emit object file: %s\n", err);
		return;
	}

	TRACE(ANSI_COLOR_CYAN "Codegen took %f ms\n" ANSI_COLOR_RESET, clock_get_elapsed_time(&clock) * 1000.0f);

	char* ir_string = LLVMPrintModuleToString(context.llvm_module);
	printf("%s\n", ir_string);
	LLVMDisposeMessage(ir_string);

	clock_restart_from_now(&clock);

	TRACE(ANSI_COLOR_CYAN "Linking started...\n" ANSI_COLOR_RESET);

	// codegen_linker_link("main.exe");
	const char* linker = "clang -fuse-ld=lld -Wl,/subsystem:console -o main.exe .\\main.obj";

	if (system(linker) != 0)
	{
		ERROR("Failed to link executable 'main.exe' using command '%s'.\n", linker);
		return;
	}

	TRACE(ANSI_COLOR_CYAN "Linking took %f ms\n" ANSI_COLOR_RESET, clock_get_elapsed_time(&clock) * 1000.0f);
	TRACE(ANSI_COLOR_CYAN "Program linked to executable 'main.exe'.\n" ANSI_COLOR_RESET);

	LLVMDisposeBuilder(context.llvm_builder);
	LLVMDisposeModule(context.llvm_module);
	LLVMContextDispose(context.llvm_context);
}
