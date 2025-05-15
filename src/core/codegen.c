#include "codegen.h"

#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/DebugInfo.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Transforms/PassBuilder.h>

extern Context global_context; // from compiler_internal.h

typedef struct _CodegenContext
{
	LLVMContextRef llvm_context;
	LLVMModuleRef llvm_module;
	LLVMBuilderRef llvm_builder;

	LLVMTypeRef bool_type;
	LLVMTypeRef char_type;
	LLVMTypeRef short_type;
	LLVMTypeRef int_type;
	LLVMTypeRef long_type;
	LLVMTypeRef float_type;
	LLVMTypeRef double_type;

	LLVMValueRef current_function;
} CodegenContext;

LLVMValueRef codegen_emit_expression(CodegenContext* context, Expression* expression);
void codegen_emit_statement(CodegenContext* context, Statement* statement);

LLVMValueRef codegen_emit_constant_expression(CodegenContext* context, Expression* expression)
{
	switch (expression->type->kind)
	{
	case TYPE_KIND_BOOL:
		return LLVMConstInt(context->bool_type, expression->constant.bool_value, false);
	case TYPE_KIND_CHAR:
		return LLVMConstInt(context->char_type, expression->constant.int_value, false);
	case TYPE_KIND_UCHAR:
		NOT_IMPLEMENTED;
		break;
	case TYPE_KIND_SHORT:
	case TYPE_KIND_USHORT:
		return LLVMConstInt(context->short_type, expression->constant.int_value, false);
	case TYPE_KIND_INT:
	case TYPE_KIND_UINT:
		return LLVMConstInt(context->int_type, expression->constant.int_value, false);
	case TYPE_KIND_LONG:
	case TYPE_KIND_ULONG:
		return LLVMConstInt(context->long_type, expression->constant.int_value, false);
	case TYPE_KIND_FLOAT:
		return LLVMConstReal(context->float_type, expression->constant.float_value);
	case TYPE_KIND_DOUBLE:
		return LLVMConstReal(context->double_type, expression->constant.double_value);
	default:
		break;
	}

	UNREACHABLE;
}

LLVMTypeRef codegen_emit_type(CodegenContext* context, Type* type)
{
	switch (type->kind)
	{
	case TYPE_KIND_BOOL:
		return context->bool_type;
	case TYPE_KIND_CHAR:
	case TYPE_KIND_UCHAR:
		return context->char_type;
	case TYPE_KIND_SHORT:
	case TYPE_KIND_USHORT:
		return context->short_type;
	case TYPE_KIND_INT:
	case TYPE_KIND_UINT:
		return context->int_type;
	case TYPE_KIND_LONG:
	case TYPE_KIND_ULONG:
		return context->long_type;
	case TYPE_KIND_FLOAT:
		return context->float_type;
	case TYPE_KIND_DOUBLE:
		return context->double_type;
	default:
		break;
	}

	UNREACHABLE;
}

LLVMValueRef codegen_emit_cast_bool_to_int(CodegenContext* context, LLVMValueRef value)
{
	LLVMValueRef casted = LLVMBuildZExt(context->llvm_builder, value, context->int_type, "bool.to.int");

	return casted;
}

LLVMValueRef codegen_emit_cast_int_to_bool(CodegenContext* context, LLVMValueRef value)
{
	// 0 - false, anything else - true
	LLVMValueRef casted = LLVMBuildICmp(context->llvm_builder, LLVMIntNE, value,
	                                    LLVMConstInt(context->int_type, 0, false), "int.to.bool");

	return casted;
}

LLVMValueRef codegen_emit_char_to_uchar(CodegenContext* context, LLVMValueRef value)
{
	LLVMValueRef casted = LLVMBuildZExt(context->llvm_builder, value, context->char_type, "char.to.uchar");
	return casted;
}

LLVMValueRef codegen_emit_short_to_ushort(CodegenContext* context, LLVMValueRef value)
{
	LLVMValueRef casted = LLVMBuildZExt(context->llvm_builder, value, context->short_type, "short.to.ushort");
	return casted;
}

LLVMValueRef codegen_emit_cast_int_to_uint(CodegenContext* context, LLVMValueRef value)
{
	LLVMValueRef casted = LLVMBuildZExt(context->llvm_builder, value, context->int_type, "int.to.uint");
	return casted;
}

LLVMValueRef codegen_emit_long_to_ulong(CodegenContext* context, LLVMValueRef value)
{
	LLVMValueRef casted = LLVMBuildZExt(context->llvm_builder, value, context->long_type, "long.to.ulong");
	return casted;
}

LLVMValueRef codegen_emit_int_to_short(CodegenContext* context, LLVMValueRef value)
{
	LLVMValueRef casted = LLVMBuildTrunc(context->llvm_builder, value, context->short_type, "int.to.short");
	return casted;
}

LLVMValueRef codegen_emit_int_to_ushort(CodegenContext* context, LLVMValueRef value)
{
	LLVMValueRef casted = LLVMBuildTrunc(context->llvm_builder, value, context->short_type, "int.to.ushort");
	return casted;
}

LLVMValueRef codegen_emit_int_to_ulong(CodegenContext* context, LLVMValueRef value)
{
	LLVMValueRef casted = LLVMBuildZExt(context->llvm_builder, value, context->long_type, "int.to.ulong");
	return casted;
}

LLVMValueRef codegen_emit_int_to_long(CodegenContext* context, LLVMValueRef value)
{
	LLVMValueRef casted = LLVMBuildSExt(context->llvm_builder, value, context->long_type, "int.to.long");
	return casted;
}

LLVMValueRef codegen_emit_binary_expression(CodegenContext* context, Expression* expression)
{
	switch (expression->binary.operator)
	{
	case BINARY_OPERATOR_ADD: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildAdd(context->llvm_builder, lhs, rhs, "add");
	}
	case BINARY_OPERATOR_SUBTRACT: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildSub(context->llvm_builder, lhs, rhs, "sub");
	}
	case BINARY_OPERATOR_MULTIPLY: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildMul(context->llvm_builder, lhs, rhs, "mul");
	}
	case BINARY_OPERATOR_DIVIDE: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildSDiv(context->llvm_builder, lhs, rhs, "div");
	}
	case BINARY_OPERATOR_EQUAL: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildICmp(context->llvm_builder, LLVMIntEQ, lhs, rhs, "equal");
	}
	case BINARY_OPERATOR_NOT_EQUAL: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildICmp(context->llvm_builder, LLVMIntNE, lhs, rhs, "not.equal");
	}
	case BINARY_OPERATOR_GREATER: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildICmp(context->llvm_builder, LLVMIntSGT, lhs, rhs, "greater");
	}
	case BINARY_OPERATOR_LESS: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildICmp(context->llvm_builder, LLVMIntSLT, lhs, rhs, "less");
	}
	case BINARY_OPERATOR_GREATER_OR_EQUAL: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildICmp(context->llvm_builder, LLVMIntSGE, lhs, rhs, "greater.or.equal");
	}
	case BINARY_OPERATOR_LESS_OR_EQUAL: {
		LLVMValueRef lhs = codegen_emit_expression(context, expression->binary.left);
		LLVMValueRef rhs = codegen_emit_expression(context, expression->binary.right);

		return LLVMBuildICmp(context->llvm_builder, LLVMIntSLE, lhs, rhs, "less.or.equal");
	}
	case BINARY_OPERATOR_ASSIGN: {
		LLVMValueRef variable = expression->binary.left->identifier.refered->handle;
		LLVMValueRef rhs      = codegen_emit_expression(context, expression->binary.right);

		LLVMBuildStore(context->llvm_builder, rhs, variable);

		return rhs;
	}

	default:
		break;
	}

	ASSERT(false, "Invalid binary operator: %d\n", expression->binary.operator);
}

LLVMValueRef codegen_emit_cast_expression(CodegenContext* context, Expression* expression)
{
	LLVMValueRef value = codegen_emit_expression(context, expression->cast.expression);

	switch (expression->cast.cast_kind)
	{
	case CAST_INT_TO_BOOL:
		return codegen_emit_cast_int_to_bool(context, value);
	case CAST_BOOL_TO_INT:
		return codegen_emit_cast_bool_to_int(context, value);
	case CAST_CHAR_TO_UCHAR:
		return codegen_emit_char_to_uchar(context, value);
	case CAST_SHORT_TO_USHORT:
		return codegen_emit_short_to_ushort(context, value);
	case CAST_INT_TO_UINT:
		return codegen_emit_cast_int_to_uint(context, value);
	case CAST_LONG_TO_ULONG:
		return codegen_emit_long_to_ulong(context, value);
	case CAST_INT_TO_SHORT:
		return codegen_emit_int_to_short(context, value);
	case CAST_INT_TO_LONG:
		return codegen_emit_int_to_long(context, value);
	case CAST_INT_TO_ULONG:
		return codegen_emit_int_to_ulong(context, value);
	case CAST_INT_TO_USHORT:
		return codegen_emit_int_to_ushort(context, value);
	default:
		break;
	}

	NOT_IMPLEMENTED;
}

LLVMValueRef codegen_emit_expression(CodegenContext* context, Expression* expression)
{
	switch (expression->kind)
	{
	case EXPRESSION_CONSTANT:
		return codegen_emit_constant_expression(context, expression);
	case EXPRESSION_GROUP:
		return codegen_emit_expression(context, expression->group.expression);
	case EXPRESSION_BINARY:
		return codegen_emit_binary_expression(context, expression);
	case EXPRESSION_IDENTIFIER: {
		LLVMTypeRef type = codegen_emit_type(context, expression->type);

		return LLVMBuildLoad2(context->llvm_builder, type, expression->identifier.refered->handle,
		                      expression->identifier.name);
	}
	case EXPRESSION_CAST:
		return codegen_emit_cast_expression(context, expression);
	default:
		break;
	}

	ASSERT(false, "Invalid expression kind: %d\n", expression->kind);
}

void codegen_emit_return_statement(CodegenContext* context, Statement* statement)
{
	if (statement->return_.expression == nullptr)
	{
		LLVMBuildRetVoid(context->llvm_builder);
		return;
	}

	LLVMValueRef value = codegen_emit_expression(context, statement->return_.expression);
	LLVMBuildRet(context->llvm_builder, value);
}

void codegen_emit_variable_declaration(CodegenContext* context, Statement* statement)
{
	LLVMTypeRef type         = codegen_emit_type(context, statement->declaration.declaration->variable.type);
	Declaration* declaration = statement->declaration.declaration;
	LLVMValueRef variable    = LLVMBuildAlloca(context->llvm_builder, type, declaration->variable.name);
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

void codegen_emit_if_statement(CodegenContext* context, Statement* statement)
{
	LLVMValueRef condition = codegen_emit_expression(context, statement->if_.condition);

	LLVMBasicBlockRef then_block =
	    LLVMAppendBasicBlockInContext(context->llvm_context, context->current_function, "if.then");
	LLVMBasicBlockRef else_block =
	    LLVMAppendBasicBlockInContext(context->llvm_context, context->current_function, "if.else");
	LLVMBasicBlockRef merge_block =
	    LLVMAppendBasicBlockInContext(context->llvm_context, context->current_function, "if.merge");

	LLVMBuildCondBr(context->llvm_builder, condition, then_block, else_block);

	LLVMPositionBuilderAtEnd(context->llvm_builder, then_block);

	codegen_emit_compound_statement(context, statement->if_.then_branch);

	LLVMBuildBr(context->llvm_builder, merge_block);

	LLVMPositionBuilderAtEnd(context->llvm_builder, else_block);

	if (statement->if_.else_branch != nullptr)
		codegen_emit_compound_statement(context, statement->if_.else_branch);

	LLVMBuildBr(context->llvm_builder, merge_block);

	LLVMPositionBuilderAtEnd(context->llvm_builder, merge_block);
}

void codegen_emit_while_statement(CodegenContext* context, Statement* statement)
{
	LLVMBasicBlockRef cond_block =
	    LLVMAppendBasicBlockInContext(context->llvm_context, context->current_function, "while.cond");

	LLVMBasicBlockRef loop_block =
	    LLVMAppendBasicBlockInContext(context->llvm_context, context->current_function, "while.loop");

	LLVMBasicBlockRef merge_block =
	    LLVMAppendBasicBlockInContext(context->llvm_context, context->current_function, "while.merge");

	LLVMBuildBr(context->llvm_builder, cond_block);

	LLVMPositionBuilderAtEnd(context->llvm_builder, cond_block);
	LLVMValueRef condition = codegen_emit_expression(context, statement->while_.condition);
	LLVMBuildCondBr(context->llvm_builder, condition, loop_block, merge_block);

	LLVMPositionBuilderAtEnd(context->llvm_builder, loop_block);
	codegen_emit_compound_statement(context, statement->while_.body);

	LLVMBuildBr(context->llvm_builder, cond_block);

	LLVMPositionBuilderAtEnd(context->llvm_builder, merge_block);
}

void codegen_emit_statement(CodegenContext* context, Statement* statement)
{
	switch (statement->kind)
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

	case STATEMENT_EXPRESSION:
		codegen_emit_expression(context, statement->expression.expression);
		break;

	case STATEMENT_COMPOUND:
		codegen_emit_compound_statement(context, statement->compound.first);
		break;

	case STATEMENT_RETURN:
		codegen_emit_return_statement(context, statement);
		break;

	case STATEMENT_IF:
		codegen_emit_if_statement(context, statement);
		break;

	case STATEMENT_WHILE:
		codegen_emit_while_statement(context, statement);
		break;

	default:
		UNREACHABLE;
		break;
	}
}

void codegen_emit_function(CodegenContext* context, Declaration* declaration)
{
	FunctionSignature fn_signature = declaration->function.signature;

	LLVMTypeRef type      = codegen_emit_type(context, declaration->function.signature.return_type);
	LLVMTypeRef func_type = LLVMFunctionType(type, nullptr, 0, false);

	LLVMValueRef function = LLVMAddFunction(context->llvm_module, fn_signature.name, func_type);
	declaration->handle   = function;

	LLVMSetFunctionCallConv(function, LLVMCCallConv);
	// LLVMSetLinkage(function, LLVMInternalLinkage);
	LLVMSetVisibility(function, LLVMDefaultVisibility);

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context->llvm_context, declaration->handle, "entry");
	LLVMPositionBuilderAtEnd(context->llvm_builder, entry);

	context->current_function = function;

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

	/*
	    LLVMInitializeAllAsmParsers();
	    LLVMInitializeAllAsmPrinters();
	    LLVMInitializeAllTargetInfos();
	    LLVMInitializeAllTargets();
	    LLVMInitializeAllDisassemblers();
	    LLVMInitializeAllTargetMCs();

	    LLVMTargetRef Target = LLVMGetFirstTarget();
	    while (Target)
	    {
	        const char* Name        = LLVMGetTargetName(Target);
	        const char* Description = LLVMGetTargetDescription(Target);
	        printf("Target: %s (%s)\n", Name, Description);
	        Target = LLVMGetNextTarget(Target);
	    }
	    return;
	*/

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

	context.bool_type   = LLVMInt1TypeInContext(context.llvm_context);
	context.char_type   = LLVMInt8TypeInContext(context.llvm_context);
	context.short_type  = LLVMInt16TypeInContext(context.llvm_context);
	context.int_type    = LLVMInt32TypeInContext(context.llvm_context);
	context.long_type   = LLVMInt64TypeInContext(context.llvm_context);
	context.float_type  = LLVMFloatTypeInContext(context.llvm_context);
	context.double_type = LLVMDoubleTypeInContext(context.llvm_context);

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

	LLVMPassBuilderOptionsRef pass_options = LLVMCreatePassBuilderOptions();
	LLVMPassBuilderOptionsSetVerifyEach(pass_options, true);    // Verify after each pass
	LLVMPassBuilderOptionsSetDebugLogging(pass_options, false); // Disable debug logging for now

	/*
	    constprop
	    Propagates constant values through the IR.

	    mem2reg
	    Promotes alloca-based variables to SSA registers, eliminating loads and stores.

	    dce
	    Dead code elimination.

	    instcombine
	    Combines instructions to form fewer, more complex instructions.

	    simplifycfg
	    Simplifies the control flow graph of the function.

	    loop-unroll,loop-vectorize
	    Unrolls and vectorizes loops.

	    early-cse
	    Performs early common subexpression elimination.

	    adce
	    Aggressive dead code elimination.

	    default<O0>, default<O1>, default<O2>, default<O3>, default<Os>, default<Oz>
	    0 - 3 are optimization levels, Os and Oz are size optimizations.
	*/

	// const char* passes      = "mem2reg,instcombine,simplifycfg,loop-unroll,loop-vectorize,early-cse,adce";
	const char* passes      = "default<O0>";
	LLVMErrorRef pass_error = LLVMRunPasses(context.llvm_module, passes, platform_target.machine_ref, pass_options);
	if (pass_error != LLVMErrorSuccess)
	{
		char* error_msg = LLVMGetErrorMessage(pass_error);
		ERROR("Failed to run passes: %s\n", error_msg);
		LLVMDisposeErrorMessage(error_msg);
		LLVMDisposePassBuilderOptions(pass_options);
		return;
	}
	LLVMDisposePassBuilderOptions(pass_options);

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

	const char* linker = "lld-link main.obj /subsystem:console /entry:mainCRTStartup /out:main.exe kernel32.lib "
	                     "libcmt.lib /incremental /threads:2 /opt:noref";

	if (!run_linker(linker))
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
