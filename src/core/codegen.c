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

LLVMValueRef codegen_emit_expression(CodegenContext* context, Expression* expression);
void codegen_emit_statement(CodegenContext* context, Statement* statement);

LLVMValueRef codegen_emit_literal_expression(CodegenContext* context, Expression* expression)
{
	switch (expression->literal.type->kind)
	{
	case TYPE_KIND_INT_32:
		return LLVMConstInt(LLVMInt32TypeInContext(context->llvm_context), expression->literal.int_value, false);
	default:
		break;
	}

	ASSERT(false, "Invalid literal type: %d\n", expression->literal.type->kind);
}

LLVMValueRef codegen_emit_binary_expression(CodegenContext* context, Expression* expression)
{
	LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
	LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

	switch (expression->binary.operator)
	{
	case BINARY_OPERATOR_ADD:
		return LLVMBuildAdd(context->llvm_builder, lhs, rhs, "add");
	case BINARY_OPERATOR_SUBTRACT:
		return LLVMBuildSub(context->llvm_builder, lhs, rhs, "sub");
	case BINARY_OPERATOR_MULTIPLY:
		return LLVMBuildMul(context->llvm_builder, lhs, rhs, "mul");
	case BINARY_OPERATOR_DIVIDE:
		return LLVMBuildSDiv(context->llvm_builder, lhs, rhs, "div");
	default:
		break;
	}

	ASSERT(false, "Invalid binary operator: %d\n", expression->binary.operator);
}

LLVMValueRef codegen_emit_expression(CodegenContext* context, Expression* expression)
{
	switch (expression->kind)
	{
	case EXPRESSION_LITERAL:
		return codegen_emit_literal_expression(context, expression);
	case EXPRESSION_GROUP:
		return codegen_emit_expression(context, expression->group.expression);
	case EXPRESSION_BINARY:
		return codegen_emit_binary_expression(context, expression);
	case EXPRESSION_IDENTIFIER: {
		return LLVMBuildLoad2(context->llvm_builder, LLVMInt32TypeInContext(context->llvm_context),
		                      expression->identifier.refered->handle, expression->identifier.name);
	}
	default:
		break;
	}

	ASSERT(false, "Invalid expression kind: %d\n", expression->kind);
}

void codegen_emit_return_statement(CodegenContext* context, Statement* statement)
{
	LLVMValueRef value = codegen_emit_expression(context, statement->return_.expression);
	LLVMBuildRet(context->llvm_builder, value);
}

void codegen_emit_variable_declaration(CodegenContext* context, Statement* statement)
{
	Declaration* declaration = statement->declaration.declaration;
	LLVMTypeRef int32_type   = LLVMIntTypeInContext(context->llvm_context, 32);
	LLVMValueRef variable    = LLVMBuildAlloca(context->llvm_builder, int32_type, declaration->variable.name);
	LLVMValueRef value       = codegen_emit_expression(context, declaration->variable.initializer);
	LLVMBuildStore(context->llvm_builder, value, variable);

	declaration->handle = variable;
}

void codegen_emit_compound_statement(CodegenContext* context, Statement* statement)
{
	Statement* current = statement;

	while (current != nullptr)
	{
		codegen_emit_statement(context, current);
		current = current->next;
	}
}

void codegen_emit_statement(CodegenContext* context, Statement* statement)
{
	switch (statement->type)
	{
	case STATEMENT_DECLARATION: {
		Declaration* declaration = statement->declaration.declaration;
		switch (declaration->kind)
		{
		case DECLARATION_VARIABLE:
			codegen_emit_variable_declaration(context, statement);
			break;
		case DECLARATION_FUNCTION:
			ASSERT(false, "Not top level function declarations are not supported.\n");
			break;
		default:
			break;
		}
		break;
	}

	case STATEMENT_COMPOUND:
		codegen_emit_compound_statement(context, statement->compound.first);
		break;

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

	codegen_emit_compound_statement(context, declaration->function.body->compound.first);
}

typedef struct _PlatformTarget
{
	const char* target_triple;
	int alloca_address_space;
	LLVMTargetRef target_ref;
	LLVMTargetMachineRef machine_ref;
	LLVMTargetDataRef target_data_ref;
} PlatformTarget;

PlatformTarget codegen_initialize_target(void)
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

	if (LLVMTargetMachineEmitToFile(platform_target.machine_ref, context.llvm_module, "main.asm", LLVMAssemblyFile,
	                                &error))
	{
		ERROR("Could not emit asm file: %s\n", err);
		return;
	}

	TRACE(ANSI_COLOR_CYAN "Codegen took %f ms\n" ANSI_COLOR_RESET, clock_get_elapsed_time(&clock) * 1000.0f);

	char* ir_string = LLVMPrintModuleToString(context.llvm_module);
	printf("%s\n", ir_string);

	// output to file output.ll (temporary)
	FILE* ir_file = fopen("output.ll", "w+");
	if (ir_file == nullptr)
	{
		ERROR("Could not open file output.ll for writing.\n");
		return;
	}
	fwrite(ir_string, sizeof(char), strlen(ir_string), ir_file);
	fclose(ir_file);

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
