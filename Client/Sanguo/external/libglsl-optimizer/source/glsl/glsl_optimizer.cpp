#include "glsl_optimizer.h"
#include "ast.h"
#include "glsl_parser_extras.h"
#include "glsl_parser.h"
#include "ir_optimization.h"
#include "ir_print_metal_visitor.h"
#include "ir_print_glsl_visitor.h"
#include "ir_print_visitor.h"
#include "ir_stats.h"
#include "loop_analysis.h"
#include "program.h"
#include "linker.h"
#include "standalone_scaffolding.h"
#include "ir_rvalue_visitor.h"
#include <algorithm>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <set>
#include <sstream>
#include <cstring>

extern "C" struct gl_shader *
_mesa_new_shader(struct gl_context *ctx, GLuint name, GLenum type);

static void DeleteShader(struct gl_context *ctx, struct gl_shader *shader)
{
	ralloc_free(shader);
}


static void
initialize_mesa_context(struct gl_context *ctx, glslopt_target api)
{
	gl_api mesaAPI;
	switch(api)
	{
		default:
		case kGlslTargetOpenGL:
			mesaAPI = API_OPENGL_COMPAT;
			break;
		case kGlslTargetOpenGLES20:
			mesaAPI = API_OPENGLES2;
			break;
		case kGlslTargetOpenGLES30:
			mesaAPI = API_OPENGL_CORE;
			break;
		case kGlslTargetMetal:
			mesaAPI = API_OPENGL_CORE;
			break;
	}
	initialize_context_to_defaults (ctx, mesaAPI);

	switch(api)
	{
	default:
	case kGlslTargetOpenGL:
		ctx->Const.GLSLVersion = 150;
		break;
	case kGlslTargetOpenGLES20:
		ctx->Extensions.OES_standard_derivatives = true;
		ctx->Extensions.EXT_shadow_samplers = true;
		ctx->Extensions.EXT_frag_depth = true;
		ctx->Extensions.EXT_shader_framebuffer_fetch = true;
		break;
	case kGlslTargetOpenGLES30:
		ctx->Extensions.ARB_ES3_compatibility = true;
		ctx->Extensions.EXT_shader_framebuffer_fetch = true;
		break;
	case kGlslTargetMetal:
		ctx->Extensions.ARB_ES3_compatibility = true;
		ctx->Extensions.EXT_shader_framebuffer_fetch = true;
		break;
	}


   // allow high amount of texcoords
   ctx->Const.MaxTextureCoordUnits = 16;

   ctx->Const.Program[MESA_SHADER_VERTEX].MaxTextureImageUnits = 16;
   ctx->Const.Program[MESA_SHADER_FRAGMENT].MaxTextureImageUnits = 16;
   ctx->Const.Program[MESA_SHADER_GEOMETRY].MaxTextureImageUnits = 16;

   // For GLES2.0 this would be 1, but we do support GL_EXT_draw_buffers
   ctx->Const.MaxDrawBuffers = 4;

   ctx->Driver.NewShader = _mesa_new_shader;
   ctx->Driver.DeleteShader = DeleteShader;
}


struct glslopt_ctx {
	glslopt_ctx (glslopt_target target) {
		this->target = target;
		mem_ctx = ralloc_context (NULL);
		initialize_mesa_context (&mesa_ctx, target);
	}
	~glslopt_ctx() {
		ralloc_free (mem_ctx);
	}
	struct gl_context mesa_ctx;
	void* mem_ctx;
	glslopt_target target;
};

glslopt_ctx* glslopt_initialize (glslopt_target target)
{
	return new glslopt_ctx(target);
}

void glslopt_cleanup (glslopt_ctx* ctx)
{
	delete ctx;
	_mesa_destroy_shader_compiler();
}

void glslopt_set_max_unroll_iterations (glslopt_ctx* ctx, unsigned iterations)
{
	for (int i = 0; i < MESA_SHADER_STAGES; ++i)
		ctx->mesa_ctx.Const.ShaderCompilerOptions[i].MaxUnrollIterations = iterations;
}

struct glslopt_shader_var
{
	const char* name;
	glslopt_basic_type type;
	glslopt_precision prec;
	int vectorSize;
	int matrixSize;
	int arraySize;
	int location;
};

struct glslopt_shader_packed_cb_field
{
	const char* name;
	int offset;
	int count;
};

struct glslopt_shader_packed_cbuffer
{
	const char* name;
	const char* type;
	unsigned index;

	unsigned length;
	glslopt_shader_packed_cb_field* fields;
};

struct glslopt_shader_packed_global
{
	const char* name;
	char packedType;
	int offset;
	int count;
};

struct glslopt_shader_packed_sampler
{
	const char* name;
	int offset;
	int count;
};

struct glslopt_shader_packed_cb_copy
{
	int sourceCB;
	int sourceOffset;
	int destCB;
	int destPackedType;
	int destOffset;
	int count;
};

struct glslopt_shader
{
	static void* operator new(size_t size, void *ctx)
	{
		void *node;
		node = ralloc_size(ctx, size);
		assert(node != NULL);
		return node;
	}
	static void operator delete(void *node)
	{
		ralloc_free(node);
	}

	glslopt_shader ()
		: rawOutput(0)
		, optimizedOutput(0)
		, status(false)
		, uniformCount(0)
		, uniformsSize(0)
		, inputCount(0)
		, outputCount(0)
		, textureCount(0)
		, packedUBsCount(0)
		, packedUBsGlobalCopiesCount(0)
		, packedGlobalsCount(0)
		, packedSamplersCount(0)
		, statsMath(0)
		, statsTex(0)
		, statsFlow(0)
	{
		infoLog = "Shader not compiled yet";
		
		whole_program = rzalloc (NULL, struct gl_shader_program);
		assert(whole_program != NULL);
		whole_program->InfoLog = ralloc_strdup(whole_program, "");
		
		whole_program->Shaders = reralloc(whole_program, whole_program->Shaders, struct gl_shader *, whole_program->NumShaders + 1);
		assert(whole_program->Shaders != NULL);
		
		shader = rzalloc(whole_program, gl_shader);
		whole_program->Shaders[whole_program->NumShaders] = shader;
		whole_program->NumShaders++;
		
		whole_program->LinkStatus = true;		
	}
	
	~glslopt_shader()
	{
		for (unsigned i = 0; i < MESA_SHADER_STAGES; i++)
			ralloc_free(whole_program->_LinkedShaders[i]);
		ralloc_free(whole_program);
		ralloc_free(preprocessedOutput);
		ralloc_free(rawOutput);
		ralloc_free(optimizedOutput);
	}
	
	struct gl_shader_program* whole_program;
	struct gl_shader* shader;

	static const int kMaxShaderUniforms = 1024;
	static const int kMaxShaderInputs = 128;
	static const int kMaxShaderTextures = 128;
	static const int kMaxShaderOutputs = 128;
	static const int kMaxShaderPackedUBs = kMaxShaderUniforms;
	static const int kMaxShaderPackedGlobals = kMaxShaderUniforms;
	static const int kMaxShaderPackedSamplers = kMaxShaderTextures;
	glslopt_shader_var uniforms[kMaxShaderUniforms];
	glslopt_shader_var inputs[kMaxShaderInputs];
	glslopt_shader_var textures[kMaxShaderInputs];
	glslopt_shader_var outputs[kMaxShaderOutputs];
	glslopt_shader_packed_cbuffer packedUBs[kMaxShaderPackedUBs];
	glslopt_shader_packed_cb_copy packedUBsGlobalCopies[kMaxShaderPackedGlobals];
	glslopt_shader_packed_global packedGlobals[kMaxShaderPackedGlobals];
	glslopt_shader_packed_sampler packedSamplers[kMaxShaderPackedSamplers];
	int uniformCount, uniformsSize;
	int textureCount;
	int inputCount;
	int outputCount;
	int packedUBsCount;
	int packedUBsGlobalCopiesCount;
	int packedGlobalsCount;
	int packedSamplersCount;
	int statsMath, statsTex, statsFlow;

	char*	preprocessedOutput;
	char*	rawOutput;
	char*	optimizedOutput;
	const char*	infoLog;
	bool	status;
};

static inline void debug_print_ir (const char* name, exec_list* ir, _mesa_glsl_parse_state* state, void* memctx)
{
	#if 0
	printf("**** %s:\n", name);
//	_mesa_print_ir (ir, state);
	char* foobar = _mesa_print_ir_glsl(ir, state, ralloc_strdup(memctx, ""), kPrintGlslFragment);
	printf("%s\n", foobar);
	validate_ir_tree(ir);
	#endif
}


struct precision_ctx
{
	exec_list* root_ir;
	bool res;
};


static void propagate_precision_deref(ir_instruction *ir, void *data)
{
	// variable deref with undefined precision: take from variable itself
	ir_dereference_variable* der = ir->as_dereference_variable();
	if (der && der->get_precision() == glsl_precision_undefined && der->var->data.precision != glsl_precision_undefined)
	{
		der->set_precision ((glsl_precision)der->var->data.precision);
		((precision_ctx*)data)->res = true;
	}
	
	// array deref with undefined precision: take from array itself
	ir_dereference_array* der_arr = ir->as_dereference_array();
	if (der_arr && der_arr->get_precision() == glsl_precision_undefined && der_arr->array->get_precision() != glsl_precision_undefined)
	{
		der_arr->set_precision (der_arr->array->get_precision());
		((precision_ctx*)data)->res = true;
	}
	
	// swizzle with undefined precision: take from swizzle argument
	ir_swizzle* swz = ir->as_swizzle();
	if (swz && swz->get_precision() == glsl_precision_undefined && swz->val->get_precision() != glsl_precision_undefined)
	{
		swz->set_precision (swz->val->get_precision());
		((precision_ctx*)data)->res = true;
	}
	
}

static void propagate_precision_expr(ir_instruction *ir, void *data)
{
	ir_expression* expr = ir->as_expression();
	if (!expr)
		return;
	if (expr->get_precision() != glsl_precision_undefined)
		return;
	
	glsl_precision prec_params_max = glsl_precision_undefined;
	for (int i = 0; i < (int)expr->get_num_operands(); ++i)
	{
		ir_rvalue* op = expr->operands[i];
		if (op && op->get_precision() != glsl_precision_undefined)
			prec_params_max = higher_precision (prec_params_max, op->get_precision());
	}
	if (expr->get_precision() != prec_params_max)
	{
		expr->set_precision (prec_params_max);
		((precision_ctx*)data)->res = true;
	}
	
}

static void propagate_precision_texture(ir_instruction *ir, void *data)
{
	ir_texture* tex = ir->as_texture();
	if (!tex)
		return;

	glsl_precision sampler_prec = tex->sampler->get_precision();
	if (tex->get_precision() == sampler_prec || sampler_prec == glsl_precision_undefined)
		return;

	// set precision of ir_texture node to that of the sampler itself
	tex->set_precision(sampler_prec);
	((precision_ctx*)data)->res = true;
}

struct undefined_ass_ctx
{
	ir_variable* var;
	bool res;
};

static void has_only_undefined_precision_assignments(ir_instruction *ir, void *data)
{
	ir_assignment* ass = ir->as_assignment();
	if (!ass)
		return;
	undefined_ass_ctx* ctx = (undefined_ass_ctx*)data;
	if (ass->whole_variable_written() != ctx->var)
		return;
	glsl_precision prec = ass->rhs->get_precision();
	if (prec == glsl_precision_undefined)
		return;
	ctx->res = false;
}


static void propagate_precision_assign(ir_instruction *ir, void *data)
{
	ir_assignment* ass = ir->as_assignment();
	if (!ass || !ass->lhs || !ass->rhs)
		return;

	glsl_precision lp = ass->lhs->get_precision();
	glsl_precision rp = ass->rhs->get_precision();

	// for assignments with LHS having undefined precision, take it from RHS
	if (rp != glsl_precision_undefined)
	{
		ir_variable* lhs_var = ass->lhs->variable_referenced();
		if (lp == glsl_precision_undefined)
		{		
			if (lhs_var)
				lhs_var->data.precision = rp;
			ass->lhs->set_precision (rp);
			((precision_ctx*)data)->res = true;
		}
		return;
	}
	
	// for assignments where LHS has precision, but RHS is a temporary variable
	// with undefined precision that's only assigned from other undefined precision
	// sources -> make the RHS variable take LHS precision
	if (lp != glsl_precision_undefined && rp == glsl_precision_undefined)
	{
		ir_dereference* deref = ass->rhs->as_dereference();
		if (deref)
		{
			ir_variable* rhs_var = deref->variable_referenced();
			if (rhs_var && rhs_var->data.mode == ir_var_temporary && rhs_var->data.precision == glsl_precision_undefined)
			{
				undefined_ass_ctx ctx;
				ctx.var = rhs_var;
				// find if we only assign to it from undefined precision sources
				ctx.res = true;
				exec_list* root_ir = ((precision_ctx*)data)->root_ir;
				foreach_in_list(ir_instruction, inst, root_ir)
				{
					visit_tree (ir, has_only_undefined_precision_assignments, &ctx);
				}
				if (ctx.res)
				{
					rhs_var->data.precision = lp;
					ass->rhs->set_precision(lp);
					((precision_ctx*)data)->res = true;
				}
			}
		}
		return;
	}
}


static void propagate_precision_call(ir_instruction *ir, void *data)
{
	ir_call* call = ir->as_call();
	if (!call)
		return;
	if (!call->return_deref)
		return;
	if (call->return_deref->get_precision() == glsl_precision_undefined /*&& call->callee->precision == glsl_precision_undefined*/)
	{
		glsl_precision prec_params_max = glsl_precision_undefined;
		foreach_two_lists(formal_node, &call->callee->parameters,
						  actual_node, &call->actual_parameters) {
			ir_variable* sig_param = (ir_variable*)formal_node;
			ir_rvalue* param = (ir_rvalue*)actual_node;
			
			glsl_precision p = (glsl_precision)sig_param->data.precision;
			if (p == glsl_precision_undefined)
				p = param->get_precision();
			
			prec_params_max = higher_precision (prec_params_max, p);
		}
		if (call->return_deref->get_precision() != prec_params_max)
		{
			call->return_deref->set_precision (prec_params_max);
			((precision_ctx*)data)->res = true;
		}
	}
}

static bool propagate_precision(exec_list* list, bool assign_high_to_undefined)
{
	bool anyProgress = false;
	precision_ctx ctx;
	
	do {
		ctx.res = false;
		ctx.root_ir = list;
		foreach_in_list(ir_instruction, ir, list)
		{
			visit_tree (ir, propagate_precision_texture, &ctx);
			visit_tree (ir, propagate_precision_deref, &ctx);
			bool hadProgress = ctx.res;
			ctx.res = false;
			visit_tree (ir, propagate_precision_assign, &ctx);
			if (ctx.res)
			{
				// assignment precision propagation might have added precision
				// to some variables; need to propagate dereference precision right
				// after that too.
				visit_tree (ir, propagate_precision_deref, &ctx);
			}
			ctx.res |= hadProgress;
			visit_tree (ir, propagate_precision_call, &ctx);
			visit_tree (ir, propagate_precision_expr, &ctx);
		}
		anyProgress |= ctx.res;
	} while (ctx.res);
	anyProgress |= ctx.res;
	
	// for globals that have undefined precision, set it to highp
	if (assign_high_to_undefined)
	{
		foreach_in_list(ir_instruction, ir, list)
		{
			ir_variable* var = ir->as_variable();
			if (var)
			{
				if (var->data.precision == glsl_precision_undefined)
				{
					var->data.precision = glsl_precision_high;
					anyProgress = true;
				}
			}
		}
	}
	
	return anyProgress;
}

static void do_optimization_passes(exec_list* ir, bool linked, _mesa_glsl_parse_state* state, void* mem_ctx)
{
	bool progress;
	// FIXME: Shouldn't need to bound the number of passes
	int passes = 0,
		kMaximumPasses = 1000;
	do {
		progress = false;
		++passes;
		bool progress2;
		debug_print_ir ("Initial", ir, state, mem_ctx);
		if (linked) {
			progress2 = do_function_inlining(ir); progress |= progress2; if (progress2) debug_print_ir ("After inlining", ir, state, mem_ctx);
			progress2 = do_dead_functions(ir); progress |= progress2; if (progress2) debug_print_ir ("After dead functions", ir, state, mem_ctx);
			progress2 = do_structure_splitting(ir); progress |= progress2; if (progress2) debug_print_ir ("After struct splitting", ir, state, mem_ctx);
		}
		progress2 = do_if_simplification(ir); progress |= progress2; if (progress2) debug_print_ir ("After if simpl", ir, state, mem_ctx);
		progress2 = opt_flatten_nested_if_blocks(ir); progress |= progress2; if (progress2) debug_print_ir ("After if flatten", ir, state, mem_ctx);
		progress2 = propagate_precision (ir, state->metal_target); progress |= progress2; if (progress2) debug_print_ir ("After prec propagation", ir, state, mem_ctx);
		progress2 = do_copy_propagation(ir); progress |= progress2; if (progress2) debug_print_ir ("After copy propagation", ir, state, mem_ctx);
		progress2 = do_copy_propagation_elements(ir); progress |= progress2; if (progress2) debug_print_ir ("After copy propagation elems", ir, state, mem_ctx);

		if (linked)
		{
			progress2 = do_vectorize(ir); progress |= progress2; if (progress2) debug_print_ir ("After vectorize", ir, state, mem_ctx);
		}
		if (linked) {
			progress2 = do_dead_code(ir,false); progress |= progress2; if (progress2) debug_print_ir ("After dead code", ir, state, mem_ctx);
		} else {
			progress2 = do_dead_code_unlinked(ir); progress |= progress2; if (progress2) debug_print_ir ("After dead code unlinked", ir, state, mem_ctx);
		}
		progress2 = do_dead_code_local(ir); progress |= progress2; if (progress2) debug_print_ir ("After dead code local", ir, state, mem_ctx);
		progress2 = propagate_precision (ir, state->metal_target); progress |= progress2; if (progress2) debug_print_ir ("After prec propagation", ir, state, mem_ctx);
		progress2 = do_tree_grafting(ir); progress |= progress2; if (progress2) debug_print_ir ("After tree grafting", ir, state, mem_ctx);
		progress2 = do_constant_propagation(ir); progress |= progress2; if (progress2) debug_print_ir ("After const propagation", ir, state, mem_ctx);
		if (linked) {
			progress2 = do_constant_variable(ir); progress |= progress2; if (progress2) debug_print_ir ("After const variable", ir, state, mem_ctx);
		} else {
			progress2 = do_constant_variable_unlinked(ir); progress |= progress2; if (progress2) debug_print_ir ("After const variable unlinked", ir, state, mem_ctx);
		}
		progress2 = do_constant_folding(ir); progress |= progress2; if (progress2) debug_print_ir ("After const folding", ir, state, mem_ctx);
		progress2 = do_minmax_prune(ir); progress |= progress2; if (progress2) debug_print_ir ("After minmax prune", ir, state, mem_ctx);
		progress2 = do_cse(ir); progress |= progress2; if (progress2) debug_print_ir ("After CSE", ir, state, mem_ctx);
		progress2 = do_rebalance_tree(ir); progress |= progress2; if (progress2) debug_print_ir ("After rebalance tree", ir, state, mem_ctx);
		progress2 = do_algebraic(ir, state->ctx->Const.NativeIntegers, &state->ctx->Const.ShaderCompilerOptions[state->stage]); progress |= progress2; if (progress2) debug_print_ir ("After algebraic", ir, state, mem_ctx);
		progress2 = do_lower_jumps(ir); progress |= progress2; if (progress2) debug_print_ir ("After lower jumps", ir, state, mem_ctx);
		progress2 = do_vec_index_to_swizzle(ir); progress |= progress2; if (progress2) debug_print_ir ("After vec index to swizzle", ir, state, mem_ctx);
		progress2 = lower_vector_insert(ir, false); progress |= progress2; if (progress2) debug_print_ir ("After lower vector insert", ir, state, mem_ctx);
		progress2 = do_swizzle_swizzle(ir); progress |= progress2; if (progress2) debug_print_ir ("After swizzle swizzle", ir, state, mem_ctx);
		progress2 = do_noop_swizzle(ir); progress |= progress2; if (progress2) debug_print_ir ("After noop swizzle", ir, state, mem_ctx);
		progress2 = optimize_split_arrays(ir, linked, state->metal_target && state->stage == MESA_SHADER_FRAGMENT); progress |= progress2; if (progress2) debug_print_ir ("After split arrays", ir, state, mem_ctx);
		progress2 = optimize_redundant_jumps(ir); progress |= progress2; if (progress2) debug_print_ir ("After redundant jumps", ir, state, mem_ctx);

		// do loop stuff only when linked; otherwise causes duplicate loop induction variable
		// problems (ast-in.txt test)
		if (linked)
		{
			loop_state *ls = analyze_loop_variables(ir);
			if (ls->loop_found) {
				progress2 = set_loop_controls(ir, ls); progress |= progress2; if (progress2) debug_print_ir ("After set loop", ir, state, mem_ctx);
				progress2 = unroll_loops(ir, ls, &state->ctx->Const.ShaderCompilerOptions[state->stage]); progress |= progress2; if (progress2) debug_print_ir ("After unroll", ir, state, mem_ctx);
			}
			delete ls;
		}
	} while (progress && passes < kMaximumPasses);

	if (!state->metal_target)
	{
		// GLSL/ES does not have saturate, so lower it
		lower_instructions(ir, SAT_TO_CLAMP);
	}
}

static bool move_global_instructions_to_main(exec_list* ir, _mesa_glsl_parse_state* state)
{
	ir_function_signature* mainsig = nullptr;
	unsigned int numfuncs = 0;

	foreach_in_list(ir_instruction, node, ir)
	{
		ir_function* const func = node->as_function();
		if (!func) continue;

		foreach_in_list(ir_function_signature, sig, &func->signatures)
		{
			if (std::strcmp(sig->function_name(), "main") == 0)
			{
				mainsig = sig;
			}
			if (sig->is_defined && !sig->is_builtin())
			{
				numfuncs++;
			}
		}
	}

	if (mainsig)
	{
		exec_list global_ir;
		const bool bCanMove = (numfuncs == 1);

		foreach_in_list(ir_instruction, node, ir)
		{
			if (node->ir_type == ir_type_variable)
			{
				ir_variable* var = (ir_variable*)node;
				const bool bBuiltin = var->name && std::strncmp(var->name, "gl_", 3) == 0;
				const bool bTemp =
					(var->data.mode == ir_var_temporary) ||
					(var->data.mode == ir_var_auto && bCanMove);

				if (!bBuiltin && bTemp)
				{
					node = (ir_instruction*)node->get_prev();
					var->remove();
					global_ir.push_tail(var);
				}
			}
			else if (node->ir_type == ir_type_assignment)
			{
				ir_assignment* ass = (ir_assignment*)node;

				node = (ir_instruction*)node->get_prev();
				ass->remove();
				global_ir.push_tail(ass);
			}
		}
	
#if defined(__ANDROID__)
		// NOTE: Insert this redundant code to avoid optimization error?
		unsigned int move_count = 0;
		foreach_in_list(ir_instruction, node, &global_ir)
		{
			move_count++;
		}

		if (move_count)
#else
		if (!global_ir.is_empty())
#endif
		{
			mainsig->body.prepend_list(&global_ir);
			return true;
		}
	}
	return false;
}
/*
static void optimize_ir(exec_list* ir, _mesa_glsl_parse_state* state, bool linked, void* mem_ctx)
{
	move_global_instructions_to_main(ir);
	do_optimization_passes(ir, linked, state, mem_ctx);
}
*/
static void glsl_type_to_optimizer_desc(const glsl_type* type, glsl_precision prec, glslopt_shader_var* out)
{
	out->arraySize = type->array_size();

	// type; use element type when in array
	if (type->is_array())
		type = type->element_type();

	if (type->is_float())
		out->type = kGlslTypeFloat;
	else if (type->is_integer())
		out->type = kGlslTypeInt;
	else if (type->is_boolean())
		out->type = kGlslTypeBool;
	else if (type->is_sampler())
	{
		if (type->sampler_dimensionality == GLSL_SAMPLER_DIM_2D)
		{
			if (type->sampler_shadow)
				out->type = kGlslTypeTex2DShadow;
			else if (type->sampler_array)
				out->type = kGlslTypeTex2DArray;
			else
				out->type = kGlslTypeTex2D;
		}
		else if (type->sampler_dimensionality == GLSL_SAMPLER_DIM_3D)
			out->type = kGlslTypeTex3D;
		else if (type->sampler_dimensionality == GLSL_SAMPLER_DIM_CUBE)
			out->type = kGlslTypeTexCube;
		else
			out->type = kGlslTypeOther;
	}
	else
		out->type = kGlslTypeOther;
	
	// sizes
	out->vectorSize = type->vector_elements;
	out->matrixSize = type->matrix_columns;
	
	// precision
	switch (prec)
	{
		case glsl_precision_high: out->prec = kGlslPrecHigh; break;
		case glsl_precision_medium: out->prec = kGlslPrecMedium; break;
		case glsl_precision_low: out->prec = kGlslPrecLow; break;
		default: out->prec = kGlslPrecHigh; break;
	}
}

static void find_shader_variables(glslopt_shader* sh, exec_list* ir)
{
	foreach_in_list(ir_instruction, node, ir)
	{
		ir_variable* const var = node->as_variable();
		if (var == NULL)
			continue;
		if (var->data.mode == ir_var_shader_out)
		{
			if (sh->outputCount >= glslopt_shader::kMaxShaderOutputs)
				continue;

			glslopt_shader_var& v = sh->outputs[sh->outputCount];
			v.name = ralloc_strdup(sh, var->name);
			glsl_type_to_optimizer_desc(var->type, (glsl_precision)var->data.precision, &v);
			v.location = var->data.explicit_location ? var->data.location : -1;
			++sh->outputCount;
		}
		if (var->data.mode == ir_var_shader_in)
		{
			if (sh->inputCount >= glslopt_shader::kMaxShaderInputs)
				continue;

			glslopt_shader_var& v = sh->inputs[sh->inputCount];
			v.name = ralloc_strdup(sh, var->name);
			glsl_type_to_optimizer_desc(var->type, (glsl_precision)var->data.precision, &v);
			v.location = var->data.explicit_location ? var->data.location : -1;
			++sh->inputCount;
		}
		if (var->data.mode == ir_var_uniform && !var->type->is_sampler())
		{
			if (sh->uniformCount >= glslopt_shader::kMaxShaderUniforms)
				continue;
			
			glslopt_shader_var& v = sh->uniforms[sh->uniformCount];
			v.name = ralloc_strdup(sh, var->name);
			glsl_type_to_optimizer_desc(var->type, (glsl_precision)var->data.precision, &v);
			v.location = var->data.explicit_location ? var->data.location : -1;
			++sh->uniformCount;
		}
		if (var->data.mode == ir_var_uniform && var->type->is_sampler())
		{
			if (sh->textureCount >= glslopt_shader::kMaxShaderTextures)
				continue;
			
			glslopt_shader_var& v = sh->textures[sh->textureCount];
			v.name = ralloc_strdup(sh, var->name);
			glsl_type_to_optimizer_desc(var->type, (glsl_precision)var->data.precision, &v);
			v.location = var->data.explicit_location ? var->data.location : -1;
			++sh->textureCount;
		}
	}
}

//// BEGIN MODIFY: bzbee.igg
static void rename_shader_inputs(glslopt_shader* sh, exec_list* ir)
{
	char unify_name[] = "in_ATTRIBUTEXX";	
	ir_variable* input_vars[16] = {};

	foreach_in_list(ir_instruction, node, ir)
	{	
		ir_variable* const var = node->as_variable();
		if (var && var->data.mode == ir_var_shader_in)
		{
			if (std::strncmp(var->name, "gl_", 3) == 0)
			{
				continue;
			}

			if (std::strncmp(var->name, "in_ATTRIBUTE", 12) == 0)
			{
				const char* str_digit = var->name + 12;

				int index = 0;
				while (*str_digit && *str_digit >= '0' && *str_digit <= '9')
				{
					index = (index * 10) + (*str_digit++ - '0');
				}

				if (index < 16)
				{
					input_vars[index] = var;
					continue;
				}
			}

			for (int index = 0; index < 16; ++index)
			{
				if (!input_vars[index])
				{
					if (index < 10)
					{
						unify_name[12] = '0';
						unify_name[13] = '0' + (char)(index);
					}
					else
					{
						unify_name[12] = '1';
						unify_name[13] = '0' + (char)(index % 10);
					}
					unify_name[14] = 0;

					ralloc_free((void*)var->name);
					var->name = ralloc_strdup(var, unify_name);

					input_vars[index] = var;
					break;
				}
			}
		}
	}
}

enum glslopt_uniform_array_type
{
	UAT_FLOAT_high = glslopt_uniform_packed_typename::kGlslPackedTypeNameHighP,
	UAT_FLOAT_medium = glslopt_uniform_packed_typename::kGlslPackedTypeNameMediumP,
	UAT_FLOAT_low = glslopt_uniform_packed_typename::kGlslPackedTypeNameLowP,
	UAT_INT = glslopt_uniform_packed_typename::kGlslPackedTypeNameInt,
	UAT_UINT = glslopt_uniform_packed_typename::kGlslPackedTypeNameUint,
	UAT_SAMPLER = 's',
	UAT_IMAGE = 'g',

	// Match with kGlslPackedTypeIndex*!
	UAT_INDEX_FLOAT_high = glslopt_uniform_packed_typeindex::kGlslPackedTypeIndexHighP,
	UAT_INDEX_FLOAT_medium = glslopt_uniform_packed_typeindex::kGlslPackedTypeIndexMediumP,
	UAT_INDEX_FLOAT_low = glslopt_uniform_packed_typeindex::kGlslPackedTypeIndexLowP,
	UAT_INDEX_INT = glslopt_uniform_packed_typeindex::kGlslPackedTypeIndexInt,
	UAT_INDEX_UINT = glslopt_uniform_packed_typeindex::kGlslPackedTypeIndexUint,
	UAT_INDEX_SAMPLER = glslopt_uniform_packed_typeindex::kGlslPackedTypeIndexCount + 0,
	UAT_INDEX_IMAGE = glslopt_uniform_packed_typeindex::kGlslPackedTypeIndexCount + 1,
};

static char get_array_char_by_precision_type(glsl_base_type type, glsl_precision prec)
{
	switch (type)
	{
	case GLSL_TYPE_FLOAT:
	{
		switch (prec)
		{
		case glsl_precision_high: return UAT_FLOAT_high;
		case glsl_precision_medium: return UAT_FLOAT_medium;
		case glsl_precision_low: return UAT_FLOAT_low;
		default: break;
		}
		return UAT_FLOAT_high;
	}

	case GLSL_TYPE_INT:
	case GLSL_TYPE_BOOL:
		return UAT_INT;

	case GLSL_TYPE_UINT:
//	case GLSL_TYPE_BOOL:
		return UAT_UINT;

	case GLSL_TYPE_SAMPLER:
		return UAT_SAMPLER;

	case GLSL_TYPE_IMAGE:
		return UAT_IMAGE;

	default:
		break;
	}
	return 0;
}

struct glslopt_uniform_entry
{
	ir_variable* uarrvar;
	int	vec4start;
	int	components;
	int	rows;
};

typedef std::unordered_map<ir_variable*, glslopt_uniform_entry> glslopt_uniform_var_map;
typedef std::vector<ir_variable*> glslopt_ir_var_array;
typedef std::list<ir_variable*> glslopt_ir_var_list;
typedef std::set<std::pair<ir_variable*, const char*>> glslopt_ir_var_set;

struct glsl_uniform_struct_field
{
	const char* name;
	unsigned offsetInFloats;
	unsigned sizeInFloats;
	unsigned num_columns;
	unsigned num_rows;
	unsigned num_array_elements;
};

struct glsl_uniform_struct
{
	const glsl_type* type;
	glsl_uniform_struct_field* fields;

	static void calculate_field_info(unsigned& float_offset, unsigned& float_size, const glsl_type* field_type)
	{
		bool bNewRow = false;
		unsigned last_row_elements = 0;

		if (field_type->is_array())
		{
			unsigned inner_float_offset = 0, inner_float_size = 0;
			calculate_field_info(inner_float_offset, inner_float_size, field_type->fields.array);
			assert(inner_float_size > 0);
			float_size = (field_type->length - 1) * inner_float_size;
			if (field_type->fields.array->is_matrix())
			{
				last_row_elements = 0;
				float_size += inner_float_size;
			}
			bNewRow = (float_offset % 4) != 0;
		}
		else if (field_type->is_matrix())
		{
			float_size = (field_type->matrix_columns - 1) * 4;
			last_row_elements = field_type->column_type()->vector_elements;
			bNewRow = (float_offset % 4) != 0;
		}
		else if (field_type->is_record())
		{
			const unsigned origin_float_offset = float_offset;
			for (unsigned i = 0; i < field_type->length; ++i)
			{
				unsigned inner_float_size = 0;
				calculate_field_info(float_offset, inner_float_size, field_type->fields.structure[i].type);
				float_offset += inner_float_size;
			}
			float_size = float_offset - origin_float_offset;
		}
		else if (field_type->vector_elements > 0)
		{
			last_row_elements = field_type->vector_elements;
		}
		else
		{
			return;
		}

		float_size += last_row_elements;
		if (bNewRow || ((float_offset % 4) + last_row_elements) > 4)
		{
			float_offset = (float_offset + 4) & ~3u;
		}
	}
};

struct glslopt_uniform_cbuffer
{
	ir_variable* struct_var;	// original declared uniform struct variable
	const glsl_type* struct_type;

	unsigned num_fields;
	ir_variable** field_vars;	// flattened uniform struct field vairable
	glsl_uniform_struct_field* fields;
};

typedef std::unordered_map<const glsl_type*, glsl_uniform_struct> glsl_uniform_struct_layouts;
typedef std::vector<glslopt_uniform_cbuffer> glslopt_uniform_cbuffer_array;

struct glslopt_uniform_array
{
	int		uniformCount{};
	int		sizeInFloats{};
	glslopt_ir_var_list	variables;
};

struct glslopt_packed_uniforms
{
	std::unordered_map<char, glslopt_uniform_array>		uniformArrays;

	void add(ir_variable* var, _mesa_glsl_parse_state* state)
	{
		const glsl_type* type = var->type->is_array() ? var->type->fields.array : var->type;
		char arrchar = get_array_char_by_precision_type(type->base_type, (glsl_precision)var->data.precision);

		glslopt_uniform_array& uarr = uniformArrays[arrchar];
		uarr.uniformCount++;

		int stride = (type->vector_elements > 2 || var->type->is_array()) ? 4 : std::max(type->vector_elements, 1u);
		int rows = var->type->is_array() ? var->type->length : 1;
		rows = rows * std::max(type->matrix_columns, 1u);

		uarr.sizeInFloats += stride * rows;
		uarr.variables.emplace_back(var);
	}
};

static void find_main_and_calc_uniforms_array_size(exec_list* ir, _mesa_glsl_parse_state* state,
	ir_function_signature** out_entry, glslopt_ir_var_array* out_uniforms, glslopt_packed_uniforms* out_packed)
{
	foreach_in_list(ir_instruction, node, ir)
	{
		if (node->ir_type == ir_type_variable)
		{
			ir_variable* const var = (ir_variable*)node;
			if (var->data.mode == ir_var_uniform)
			{
				if (std::strncmp(var->name, "gl_", 3) == 0) continue;

				const glsl_type* var_type =
					var->type->is_array() ? var->type->fields.array : var->type;
				if (var_type->is_array())
				{
					_mesa_glsl_error(nullptr, state,
						"'%s' uniform variables cannot be multi-dimensional arrays", var->name);
					return;
				}

				out_uniforms->emplace_back(var);
				out_packed->add(var, state);
			}
		}
		else if (node->ir_type == ir_type_function && *out_entry == nullptr)
		{
			ir_function* const func = (ir_function*)node;
			foreach_in_list(ir_function_signature, sig, &func->signatures)
			{
				if (std::strcmp(sig->function_name(), "main") == 0)
				{
					*out_entry = sig;
					break;
				}
			}
		}
	}
}

/** Compare two uniform variables for the purpose of packing them into arrays. */
struct glslopt_sort_uniforms_predicate
{
	bool operator()(ir_variable* v1, ir_variable* v2)
	{
		const glsl_type* type1 = v1->type;
		const glsl_type* type2 = v2->type;

		const bool bType1Array = type1->is_array();
		const bool bType2Array = type2->is_array();

		// Sort by base type.
		const glsl_base_type BaseType1 = bType1Array ? type1->fields.array->base_type : type1->base_type;
		const glsl_base_type BaseType2 = bType2Array ? type2->fields.array->base_type : type2->base_type;

		if (BaseType1 != BaseType2)
		{
			static const unsigned BaseTypeOrder[] =
			{
				0, // GLSL_TYPE_UINT,
				2, // GLSL_TYPE_INT,
				3, // GLSL_TYPE_FLOAT,
				1, // GLSL_TYPE_BOOL,
				5, // GLSL_TYPE_SAMPLER,
				11, // GLSL_TYPE_IMAGE,
				4, // GLSL_TYPE_ATOMIC_UINT
				6, // GLSL_TYPE_STRUCT,
				7, // GLSL_TYPE_INTERFACE
				8, // GLSL_TYPE_ARRAY,
				9, // GLSL_TYPE_VOID,
				10, // GLSL_TYPE_ERROR,
			};

			return BaseTypeOrder[BaseType1] < BaseTypeOrder[BaseType2];
		}

		// Sort by array first
		// arrays must be aligned on a vec4 boundary, placing them first ensures this
		if (bType1Array != bType2Array)
		{
			return (int)bType1Array > (int)bType2Array;
		}

		// Then number of vector elements.
		if (type1->vector_elements != type2->vector_elements)
		{
			return type1->vector_elements > type2->vector_elements;
		}

		// Then matrix columns.
		if (type1->matrix_columns != type2->matrix_columns)
		{
			return type1->matrix_columns > type2->matrix_columns;
		}

		// If the types match, sort on the uniform name.
		return (std::strcmp(v1->name, v2->name) < 0);
	}
};

static const char* glsl_variable_tag_from_shader_target(gl_shader_stage target)
{
	switch (target)
	{
	case gl_shader_stage::MESA_SHADER_VERTEX: return "v";
	case gl_shader_stage::MESA_SHADER_GEOMETRY: return "g";
	case gl_shader_stage::MESA_SHADER_FRAGMENT: return "p";
	case gl_shader_stage::MESA_SHADER_COMPUTE: return "c";
	default: return "invalid";
	}
}

static std::string get_uniform_array_name(gl_shader_stage target, glsl_base_type type, glsl_precision prec, int cbindex)
{
	std::basic_stringstream<char, std::char_traits<char>, std::allocator<char>> name("");
	name << glsl_variable_tag_from_shader_target(target);

	if (cbindex == -1)
	{
		name << "u_";
	}
	else
	{
		name << "c" << cbindex << "_";
	}

	name << get_array_char_by_precision_type(type, prec);
	name.flush();

	return name.str();
}

// Naming convention::
//	_vu_h: Global Vertex Highp Float
//	_vu_m: Global Vertex Mediump Float
//	_vu_l: Global Vertex Lowp Float
//	_vu_i: Global Vertex Int
//	_vu_u: Global Vertex Unit & Bool
//	_vs0: Global Sampler 0
//	_vs1: Global Sampler 1
struct glsl_packed_uniform
{
	const char* name;
	unsigned offset;
	unsigned num_components;

	// Extra information for Uniforms coming from CBuffers
	const char* cb_packed_sampler;	// CB name or the name of the packed sampler
	unsigned offsetIntoCBufferInFloats;
	unsigned sizeInFloats;

	glsl_packed_uniform()
		: name("<INVALID>")
		, offset(0)
		, num_components(0)
		, cb_packed_sampler("")
		, offsetIntoCBufferInFloats(0)
		, sizeInFloats(0)
	{ }

	glsl_packed_uniform(const char* in_name, unsigned in_offset, unsigned in_components)
		: name(in_name)
		, offset(in_offset)
		, num_components(in_components)
		, cb_packed_sampler("")
		, offsetIntoCBufferInFloats(0)
		, sizeInFloats(0)
	{ }

	glsl_packed_uniform(
		const char* in_name, unsigned in_offset, unsigned in_components,
		const char* in_cb_sampler, unsigned in_offset_infloat, unsigned in_size_infloat)
		: name(in_name)
		, offset(in_offset)
		, num_components(in_components)
		, cb_packed_sampler(in_cb_sampler)
		, offsetIntoCBufferInFloats(in_offset_infloat)
		, sizeInFloats(in_size_infloat)
	{ }
};
typedef std::list<glsl_packed_uniform> glslopt_uniform_list;
typedef std::unordered_map<char, glslopt_uniform_list> glslopt_packed_arrays_map;
typedef std::unordered_map<const char*, glslopt_packed_arrays_map> glslopt_cb_packed_arrays_map;

class find_struct_fields_visitor : public ir_rvalue_visitor
{
public:
	find_struct_fields_visitor(glslopt_ir_var_set& inFoundVars)
		: found_vars(inFoundVars)
	{ }

	virtual void handle_rvalue(ir_rvalue** prvalue) override
	{
		if (!prvalue || !*prvalue) return;
		
		ir_rvalue* rvalue = *prvalue;
		if (rvalue->ir_type == ir_type_dereference_record)
		{
			handle_dereference_record((ir_dereference_record*)rvalue);
		}
		else if (rvalue->as_texture())
		{
			ir_texture* tex = (ir_texture*)rvalue;
			if (tex->sampler && tex->sampler->ir_type == ir_type_dereference_record)
			{
				handle_dereference_record((ir_dereference_record*)tex->sampler);
			}
		}
	}

private:
	void handle_dereference_record(ir_dereference_record* rvalue)
	{
		ir_variable* var = rvalue->variable_referenced();
		if (var->data.mode == ir_var_uniform)
		{
			assert(var->type->is_record());
			assert(var->semantic && *var->semantic);
			found_vars.emplace(var, rvalue->field);
		}
	}

private:
	glslopt_ir_var_set& found_vars;
};

class convert_struct_field_to_uniform_visitor : public ir_rvalue_visitor
{
public:
	convert_struct_field_to_uniform_visitor(_mesa_glsl_parse_state*	in_state,
		std::unordered_map<const char*, std::unordered_map<std::string, ir_variable*>>& in_uniforms)
		: state(in_state)
		, uniforms(in_uniforms)
	{ }

	virtual void handle_rvalue(ir_rvalue** prvalue) override
	{
		if (!prvalue || !*prvalue) return;

		ir_rvalue* rvalue = *prvalue;
		if (rvalue->ir_type == ir_type_dereference_record)
		{
			*prvalue = handle_dereference_record((ir_dereference_record*)rvalue);
		}
		else if (rvalue->as_texture())
		{
			ir_texture* tex = (ir_texture*)rvalue;
			if (tex->sampler && tex->sampler->ir_type == ir_type_dereference_record)
			{
				tex->sampler = handle_dereference_record((ir_dereference_record*)tex->sampler);
			}
		}
	}

private:
	ir_dereference* handle_dereference_record(ir_dereference_record* rvalue)
	{
		ir_variable* var = rvalue->variable_referenced();
		if (var && var->name)
		{
			const auto found = uniforms.find(var->name);
			if (found != uniforms.end())
			{
				const auto found_field = found->second.find(rvalue->field);
				if (found_field != found->second.end())
				{
					return new(state) ir_dereference_variable(found_field->second);
				}
				else
				{
					_mesa_glsl_error(nullptr, state,
						"dereference unexpected struct '%s' field '%s'", var->name, rvalue->field);
				}
			}
		}
		return rvalue;
	}

private:
	_mesa_glsl_parse_state*	state;
	std::unordered_map<const char*, std::unordered_map<std::string, ir_variable*>>& uniforms;
};

/**
 * Flattens structures inside a uniform buffer into uniform variables;
 *	from:
 *		struct Constants
 *		{
 *			vec4 Value0;
 *			struct 
 *			{
 *				vec4 Member0;
 *				vec3 Member1;
 *			} S;
 *			vec2 Value1;
 *		};
 *		uniform Constants	u_constants;
 *
 *	to:
 *		struct Constants
 *		{
 *			vec4 Value0;
 *			vec4 S_Member0;
 *			vec3 S_Member1;
 *			vec2 Value1;
 *		};
 *		uniform Constants	u_constants;
 * 
 */
static void flatten_uniform_buffer_structures(exec_list* ir,
	_mesa_glsl_parse_state* state, glslopt_uniform_cbuffer_array* out_cbuffers, glsl_uniform_struct_layouts* out_layouts)
{
	// Stored all used struct uniform variable
	glslopt_ir_var_set structvars;

	foreach_in_list(ir_instruction, node, ir)
	{
		ir_function* const func = node->as_function();
		if (func)
		{
			foreach_in_list(ir_function_signature, sig, &func->signatures)
			{
				if (!sig->is_builtin() && sig->is_defined)
				{
					find_struct_fields_visitor visitor(structvars);
					visitor.run(&sig->body);
				}
			}
		}
		else if (node->ir_type == ir_type_variable)
		{
			ir_variable* const cb_var = (ir_variable*)node;
			if (cb_var->data.mode != ir_var_uniform) continue;
			if (std::strncmp(cb_var->name, "gl_", 3) == 0) continue;
			
			const glsl_type* cb_type =
				cb_var->type->is_array() ? cb_var->type->fields.array : cb_var->type;
			if (!cb_type->is_record()) continue;

			glsl_uniform_struct* ustruct;
			const auto found = out_layouts->find(cb_type);
			if (found == out_layouts->end())
			{
				ustruct = &(*out_layouts)[cb_type];
				ustruct->type = cb_type;
				ustruct->fields = rzalloc_array(state, glsl_uniform_struct_field, cb_type->length);
				for (unsigned i = 0; i < cb_type->length; ++i)
				{
					const glsl_type* field_type = cb_type->fields.structure[i].type;

					unsigned float_offset = 0;
					if (i > 0)
					{
						float_offset = ustruct->fields[i-1].offsetInFloats + ustruct->fields[i - 1].sizeInFloats;
					}

					unsigned float_size = 0;
					glsl_uniform_struct::calculate_field_info(float_offset, float_size, field_type);

					glsl_uniform_struct_field& cbfield = ustruct->fields[i];
					cbfield.name = cb_type->fields.structure[i].name;
					cbfield.offsetInFloats = float_offset;
					cbfield.sizeInFloats = float_size;
					cbfield.num_columns = field_type->vector_elements;
					cbfield.num_rows = field_type->matrix_columns;
					cbfield.num_array_elements = field_type->is_array() ? field_type->array_size() : 0;
				}
			}
			else
			{
				ustruct = &found->second;
			}
			
			out_cbuffers->emplace_back();
			glslopt_uniform_cbuffer& cbuffer = out_cbuffers->back();
			cbuffer.struct_var = cb_var;
			cbuffer.struct_type = cb_type;
			cbuffer.num_fields = cb_type->length;
			cbuffer.fields = ustruct->fields;
			cbuffer.field_vars = rzalloc_array(state, ir_variable*, cbuffer.num_fields);
		}
	}

	// Now store field info (uniform struct var name -> (field name -> flattened field uniform var))
	std::unordered_map<const char*, std::unordered_map<std::string, ir_variable*>> flattendfields;
	for (const auto& deref : structvars)
	{
		ir_variable* var = deref.first;

		int UBIndex = -1;
		for (unsigned index = 0; index < out_cbuffers->size(); ++index)
		{
			if (var == (*out_cbuffers)[index].struct_var)
			{
				UBIndex = (int)index;
				break;
			}
		}

		assert(UBIndex >= 0);
		const glslopt_uniform_cbuffer& cbuffer = (*out_cbuffers)[UBIndex];

		// Find dereference field and add a new entry on the uniform buffer
		auto& ufields = flattendfields[var->name];
		for (unsigned index = 0; index < cbuffer.num_fields; ++index)
		{
			const glsl_struct_field& struct_field = cbuffer.struct_type->fields.structure[index];
			if (std::strcmp(struct_field.name, deref.second) != 0) continue;

			const char* field_name = ralloc_asprintf(state, "%s_%s", var->name, struct_field.name);

			ir_variable* flattened_var = new(state) ir_variable(
				struct_field.type, field_name, ir_var_uniform, struct_field.precision);
			flattened_var->semantic = cbuffer.struct_type->name;
			flattened_var->data.read_only = true;

			var->insert_before(flattened_var);

			cbuffer.field_vars[index] = flattened_var;
			ufields[struct_field.name] = flattened_var;
			break;
		}
	}

	for (unsigned index = 0; index < out_cbuffers->size(); ++index)
	{
		ir_variable* var = (*out_cbuffers)[index].struct_var;

		var->semantic = (*out_cbuffers)[index].struct_type->name;

		// Downgrade the structure variable to a local
		var->data.mode = ir_var_temporary;
		var->remove();
	}

	// Finally replace the struct field accesses into regular uniform access
	foreach_in_list(ir_instruction, node, ir)
	{
		ir_function* const func = node->as_function();
		if (func)
		{
			foreach_in_list(ir_function_signature, sig, &func->signatures)
			{
				if (!sig->is_builtin() && sig->is_defined)
				{
					convert_struct_field_to_uniform_visitor visitor(state, flattendfields);
					visitor.run(&sig->body);
				}
			}
		}
	}
}

static void find_offset_into_cbuffer_in_floats(
	const glslopt_uniform_cbuffer_array* cbuffers,
	const char* cbname, const char* fieldname, unsigned& out_offset, unsigned& out_size)
{
	for (unsigned i = 0; i < cbuffers->size(); ++i)
	{
		if ((*cbuffers)[i].struct_type->name != cbname) continue;
		
		const glslopt_uniform_cbuffer& cbuffer = (*cbuffers)[i];
		for (unsigned f = 0; f < cbuffer.num_fields; ++f)
		{
			if (cbuffer.field_vars[f]
				&& cbuffer.field_vars[f]->name == fieldname)
			{
				out_offset = cbuffer.fields[f].offsetInFloats;
				out_size = cbuffer.fields[f].sizeInFloats;
				return;
			}
		}

		assert(0);
		break;
	}
}

static int process_packed_uniform_arrays(exec_list* ir,
	ir_function_signature* mainsig, _mesa_glsl_parse_state* state,
	const glslopt_uniform_cbuffer_array* cbuffers, const glslopt_ir_var_array* uvars,
	glslopt_packed_uniforms* packed, glslopt_uniform_var_map* out_uvarmap,
	glslopt_packed_arrays_map* out_puarrmap, glslopt_cb_packed_arrays_map* out_pcbarrmap)
{
	int uindex = 0;

	// First organize all uniforms by location (CB or Global) and Precision
	std::unordered_map<const char*, std::unordered_map<char, glslopt_ir_var_array>> orgvars;
	for (const int ucount = uvars->size(); uindex < ucount; ++uindex)
	{
		ir_variable* var = (*uvars)[uindex];

		const glsl_type* type = var->type->is_array() ? var->type->fields.array : var->type;
		assert(!type->is_record());

		if (type->is_sampler() || type->is_image()) break;

		const glsl_base_type array_base_type = (type->base_type == GLSL_TYPE_BOOL) ? GLSL_TYPE_INT : type->base_type;
		const char arrchar = get_array_char_by_precision_type(array_base_type, (glsl_precision)var->data.precision);
		if (!arrchar)
		{
			_mesa_glsl_error(nullptr, state, "uniform '%s' has invalid type '%s'", var->name, var->type->name);
			return -1;
		}

		orgvars[var->semantic ? var->semantic : nullptr][arrchar].emplace_back(var);
	}

#if 0
	// Now create the list of used cb's to get their index
	std::unordered_map<const char*, int> cb_indices;
	int cb_index = 0;
	cb_indices[nullptr] = -1;
	for (auto& iter : *flattened)
	{
		auto found = orgvars.find(iter.name);
		if (found != orgvars.end())
		{
			cb_indices[iter.name] = cb_index;
			cb_index++;
		}
	}
#endif

	// Make sure any cb's with big matrices get at the end
	std::vector<const char*> cb_order;
	{
		std::vector<const char*> endvars;
		for (auto& iter : orgvars)
		{
			bool bIsFound = false;
			for (auto& found : iter.second)
			{
				for (unsigned index = 0; index < found.second.size(); ++index)
				{
					if (!found.second[index]->type->is_array())
					{
						bIsFound = true;
						break;
					}
				}

				if (bIsFound) break;
			}

			if (bIsFound)
			{
				cb_order.emplace_back(iter.first);
			}
			else
			{
				endvars.emplace_back(iter.first);
			}
		}

		cb_order.insert(cb_order.end(), endvars.begin(), endvars.end());
	}

	// Do not push the declarations before struct decls or precision statements
	ir_instruction* after_node = nullptr;
	foreach_in_list(ir_instruction, node, ir)
	{
		if (node->ir_type == ir_type_precision || node->ir_type == ir_type_typedecl)
		{
			after_node = node;
			continue;
		}
		break;
	}

	// Now actually create the packed variables
	std::unordered_map<std::string, ir_variable*> uarrvarmap;
	std::unordered_map<char, unsigned> uarrsizemap;
	for (unsigned i = 0; i < cb_order.size(); ++i)
	{
		const char* cb_name = cb_order[i];
		assert(orgvars.find(cb_name) != orgvars.end());
		
		for (auto& iter : orgvars[cb_name])
		{
			ir_variable* uarrvar = nullptr;

			const char arrchar = iter.first;
			glslopt_ir_var_array& uvars = iter.second;

			for (ir_variable* var : uvars)
			{
				const glsl_type* type = var->type->is_array() ? var->type->fields.array : var->type;
				const glsl_base_type array_base_type = (type->base_type == GLSL_TYPE_BOOL) ? GLSL_TYPE_INT : type->base_type;

				const glsl_precision prec = (glsl_precision)var->data.precision;

				if (!uarrvar)
				{
					std::string uarrname =
						get_uniform_array_name(state->stage, type->base_type, prec, -1);

					auto found_iter = uarrvarmap.find(uarrname);
					if (found_iter == uarrvarmap.end())
					{
						const glsl_type* inner_type = glsl_type::get_instance(array_base_type, 4, 1);
						const int inner_aligned = (packed->uniformArrays[arrchar].sizeInFloats + 3) / 4;

						uarrvar = new(state) ir_variable(
							glsl_type::get_array_instance(inner_type, inner_aligned),
							ralloc_asprintf(state, "%s", uarrname.c_str()), ir_var_uniform, prec);
						uarrvar->semantic = ralloc_asprintf(state, "%c", arrchar);

						if (after_node)
						{
							after_node->insert_after(uarrvar);
						}
						else
						{
							ir->push_head(uarrvar);
						}

						if (uarrsizemap.find(arrchar) == uarrsizemap.end())
						{
							uarrsizemap.emplace(arrchar, 0);
						}

						uarrvarmap.emplace(uarrname, uarrvar);
					}
					else
					{
						uarrvar = found_iter->second;
					}
				}

				unsigned& uarrsize = uarrsizemap[arrchar];

				const bool bVarIsArray = var->type->is_array();
				const unsigned stride = (type->vector_elements > 2 || bVarIsArray) ? 4 : std::max(type->vector_elements, 1u);
				const unsigned rows = (bVarIsArray ? var->type->length : 1) * std::max(type->matrix_columns, 1u);

				assert(var->name);
				if (cb_name && cb_name[0])
				{
					unsigned offsetInFloats = 0, sizeInFloats = 0;
					find_offset_into_cbuffer_in_floats(cbuffers, cb_name, var->name, offsetInFloats, sizeInFloats);
					(*out_pcbarrmap)[cb_name][arrchar].emplace_back(
						var->name, uarrsize, stride * rows, cb_name, offsetInFloats, sizeInFloats);
				}
				else
				{
					(*out_puarrmap)[arrchar].emplace_back(var->name, uarrsize, stride * rows);
				}

				glslopt_uniform_entry& uentry = (*out_uvarmap)[var];
				uentry.uarrvar = uarrvar;
				uentry.vec4start = uarrsize / 4;
				uentry.components = uarrsize % 4;
				uentry.rows = rows;

				for (unsigned rowi = 0; rowi < rows; ++rowi)
				{
					const int srci = uarrsize / 4;
					const int srcc = uarrsize % 4;

					ir_rvalue* src = new(state) ir_dereference_array(
						new(state) ir_dereference_variable(uarrvar),
						new(state) ir_constant(srci));

					if (type->is_numeric() || type->is_boolean())
					{
						src = new(state) ir_swizzle(src,
							std::min(srcc + 0, 3),
							std::min(srcc + 1, 3),
							std::min(srcc + 2, 3),
							std::min(srcc + 3, 3),
							type->vector_elements);
					}

					if (type->is_boolean())
					{
						src = new(state) ir_expression(ir_unop_i2b, src);
						//src = new(state) ir_expression(ir_unop_u2b, src);
					}

					ir_dereference* dest = new(state) ir_dereference_variable(var);
					if (rows > 1 || bVarIsArray)
					{
						if (bVarIsArray && var->type->fields.array->matrix_columns > 1)
						{
							const int mat_num = rowi / (var->type->fields.array->matrix_columns);
							const int mat_row = rowi - (var->type->fields.array->matrix_columns * mat_num);

							dest = new(state) ir_dereference_array(dest, new(state) ir_constant(mat_num));
							dest = new(state) ir_dereference_array(dest, new(state) ir_constant(mat_row));
						}
						else
						{
							dest = new(state) ir_dereference_array(dest, new(state) ir_constant(rowi));
						}
					}

					var->insert_after(new(state) ir_assignment(dest, src));
					uarrsize += stride;
				}
				var->data.mode = ir_var_auto;

				// Update Uniform Array size to match actual usage
				uarrsize = (uarrsize + 3) & ~3;
				uarrvar->type = glsl_type::get_array_instance(uarrvar->type->fields.array, uarrsize / 4);
			}
		}
	}

	// cbuffer
	for (auto& iter : (*out_uvarmap))
	{
		ir_variable* var = iter.first;
	}
	return uindex;
}

static int process_packed_samplers(int uindex,
	_mesa_glsl_parse_state* state, const glslopt_ir_var_array* uvars, glslopt_packed_arrays_map* out_puarrmap)
{
	assert((*out_puarrmap)[UAT_SAMPLER].empty());
	int uarrsize = 0;
	for (const int ucount = uvars->size(); uindex < ucount; ++uindex)
	{
		ir_variable* var = (*uvars)[uindex];

		const glsl_type* type = var->type->is_array() ? var->type->fields.array : var->type;
		if (type->is_image()) break;

		if (!type->is_sampler())
		{
			_mesa_glsl_error(nullptr, state, "unexpected uniform '%s' "
				"of type '%s' when packing uniforms", var->name, var->type->name);
			return -1;
		}

		assert(var->name);
		glsl_packed_uniform packedsampler(var->name, uarrsize, var->type->is_array() ? var->type->length : 1);
		{
			var->name = ralloc_asprintf(var, "%ss%d",
				glsl_variable_tag_from_shader_target(state->stage), uarrsize);
		}
		packedsampler.cb_packed_sampler = var->name;
		(*out_puarrmap)[UAT_SAMPLER].emplace_back(packedsampler);

		uarrsize += packedsampler.num_components;
	}
	return uindex;
}

static int process_packed_images(int uindex,
	_mesa_glsl_parse_state* state, const glslopt_ir_var_array* uvars, glslopt_packed_arrays_map* out_puarrmap)
{
	assert((*out_puarrmap)[UAT_IMAGE].empty());
	int uarrsize = 0;
	for (const int ucount = uvars->size(); uindex < ucount; ++uindex)
	{
		ir_variable* var = (*uvars)[uindex];

		const glsl_type* type = var->type->is_array() ? var->type->fields.array : var->type;

		if (!type->is_image())
		{
			_mesa_glsl_error(nullptr, state, "unexpected uniform '%s' "
				"of type '%s' when packing uniforms", var->name, var->type->name);
			return -1;
		}

		// TODO: Not implemented yet...
		assert(false);
	}
	return uindex;
}

class fix_simple_array_dereferences_visitor : public ir_rvalue_visitor
{
public:
	explicit fix_simple_array_dereferences_visitor(
		_mesa_glsl_parse_state* in_state, exec_list* in_ir, glslopt_uniform_var_map* in_uvarmap)
		: state(in_state), ir(in_ir), uvarmap(in_uvarmap)
	{ }

	virtual void handle_rvalue(ir_rvalue** prvalue) override
	{
		static int temp_id = 0;
		if (!prvalue) return;

		ir_rvalue* rvalue = *prvalue;
		if (!rvalue) return;

		ir_dereference_array* derefarray = rvalue->as_dereference_array();
		if (!derefarray) return;

		ir_variable* arrvar = rvalue->variable_referenced();
		if (!arrvar->data.read_only) return;
		if (arrvar->data.mode != ir_var_auto) return;

		const glsl_type* arr_element_Type = arrvar->type->element_type();
		if (!arr_element_Type) return;

		auto found_iter = uvarmap->find(arrvar);
		if (found_iter == uvarmap->end()) return;

		glslopt_uniform_entry& uentry = found_iter->second;

		const glsl_precision prec = (glsl_precision)arrvar->data.precision;

		if (!arr_element_Type->is_matrix())
		{
			ir_variable* local = new(state) ir_variable(
				arr_element_Type, ralloc_asprintf(state, "ar%d", temp_id++), ir_var_auto, prec);
			*prvalue = new(state) ir_dereference_variable(local);

			ir_constant* baseOffset = (derefarray->array_index->type->base_type == GLSL_TYPE_UINT)
				? new(state) ir_constant((unsigned)uentry.vec4start)
				: new(state) ir_constant((int)uentry.vec4start);
			ir_expression* arrayIndex = new(state) ir_expression(ir_binop_add, baseOffset, derefarray->array_index);
			ir_dereference_array* newderefarr = new(state)
				ir_dereference_array(new(state) ir_dereference_variable(uentry.uarrvar), arrayIndex);

			ir_swizzle* swz = new(state) ir_swizzle(
				newderefarr,
				std::min(uentry.components + 0, 3),
				std::min(uentry.components + 1, 3),
				std::min(uentry.components + 2, 3),
				std::min(uentry.components + 3, 3),
				arr_element_Type->vector_elements);

			ir_assignment* ass = new(state) ir_assignment(new(state) ir_dereference_variable(local), swz);
			base_ir->insert_before(ass);
			ass->insert_before(local);
		}
		else
		{
			ir_variable* local = new(state) ir_variable(
				arr_element_Type, ralloc_asprintf(state, "ar%d", temp_id++), ir_var_auto, prec);
			*prvalue = new(state) ir_dereference_variable(local);

			exec_list extra_ir;
			extra_ir.push_tail(local);

			// Matrix construction goes column by column performing an assignment
			for (unsigned coli = 0; coli < arr_element_Type->matrix_columns; ++coli)
			{
				// Offset baking in matrix column
				ir_constant* baseOffset = (derefarray->array_index->type->base_type == GLSL_TYPE_UINT)
					? new(state) ir_constant((unsigned)(uentry.vec4start + coli))
					: new(state) ir_constant((int)(uentry.vec4start + coli));
				// Scale index by matrix columns
				ir_constant* baseScale = (derefarray->array_index->type->base_type == GLSL_TYPE_UINT)
					? new(state) ir_constant((unsigned)arr_element_Type->matrix_columns)
					: new(state) ir_constant((int)arr_element_Type->matrix_columns);
				ir_rvalue* baseIndex = derefarray->array_index->clone(state, nullptr);

				// Compute final matrix address
				ir_expression* arrayScale = new(state) ir_expression(ir_binop_mul, baseIndex, baseScale);
				ir_expression* arrayIndex = new(state) ir_expression(ir_binop_add, baseOffset, arrayScale);
				ir_dereference_array* newderefarr = new(state)
					ir_dereference_array(new(state) ir_dereference_variable(uentry.uarrvar), arrayIndex);

				ir_swizzle* swz = new(state) ir_swizzle(
					newderefarr,
					std::min(uentry.components + 0, 3),
					std::min(uentry.components + 1, 3),
					std::min(uentry.components + 2, 3),
					std::min(uentry.components + 3, 3),
					arr_element_Type->vector_elements);

				ir_assignment* ass = new(state) ir_assignment(
					new(state) ir_dereference_array(local, new(state) ir_constant(coli)), swz);
				extra_ir.push_tail(ass);
			}
			base_ir->insert_before(&extra_ir);
		}
	}

private:
	_mesa_glsl_parse_state* state;
	exec_list* ir;
	glslopt_uniform_var_map* uvarmap;
};

static void remove_packed_uniform_buffer_references(
	exec_list* ir, _mesa_glsl_parse_state* state, glslopt_uniform_var_map* uvarmap)
{
	foreach_in_list(ir_instruction, node, ir)
	{
		ir_function* const func = node->as_function();
		if (!func) continue;
		
		foreach_in_list(ir_function_signature, sig, &func->signatures)
		{
			if (!sig->is_builtin() && sig->is_defined)
			{
				fix_simple_array_dereferences_visitor(state, &sig->body, uvarmap).run(&sig->body);
			}
		}
	}
}

static void repack_uniforms(
	glslopt_shader* sh, exec_list* ir, _mesa_glsl_parse_state* state,
	const glslopt_uniform_cbuffer_array* cbuffers, const glsl_uniform_struct_layouts* flattened,
	glslopt_packed_arrays_map* out_puarrmap, glslopt_cb_packed_arrays_map* out_pcbarrmap)
{
	glslopt_uniform_var_map uvarmap;

	ir_function_signature* mainsig = nullptr;
	glslopt_ir_var_array uvars;

	glslopt_packed_uniforms packed;
	find_main_and_calc_uniforms_array_size(ir, state, &mainsig, &uvars, &packed);

	while (mainsig && uvars.size() > 0)
	{
		std::sort(uvars.begin(), uvars.end(), glslopt_sort_uniforms_predicate());
		int uindex;

		uindex = process_packed_uniform_arrays(ir, mainsig, state,
			cbuffers, &uvars, &packed, &uvarmap, out_puarrmap, out_pcbarrmap);
		if (uindex < 0) break;

		uindex = process_packed_samplers(uindex, state, &uvars, out_puarrmap);
		if (uindex < 0) break;

		uindex = process_packed_images(uindex, state, &uvars, out_puarrmap);
		if (uindex < 0) break;

		state->has_packed_uniforms = true;

		remove_packed_uniform_buffer_references(ir, state, &uvarmap);
		break;
	}
}

void find_shader_packed_uniforms(glslopt_shader* shader, const glslopt_packed_arrays_map* puarrmap)
{
	for (const auto& iter : (*puarrmap))
	{
		const char arrchar = iter.first;
		const glslopt_uniform_list& uniforms = iter.second;

		if (arrchar != UAT_IMAGE && arrchar != UAT_SAMPLER)
		{
			for (const glsl_packed_uniform& u : uniforms)
			{
				if (u.cb_packed_sampler[0]) continue;

				glslopt_shader_packed_global& v = shader->packedGlobals[shader->packedGlobalsCount];
				v.name = ralloc_strdup(shader, u.name);
				v.packedType = arrchar;
				v.offset = u.offset;
				v.count = u.num_components;
				shader->packedGlobalsCount++;
			}
		}
		else if (arrchar == UAT_SAMPLER)
		{
			for (const glsl_packed_uniform& u : uniforms)
			{
				if (!u.cb_packed_sampler[0]) continue;

				glslopt_shader_packed_sampler& v = shader->packedSamplers[shader->packedSamplersCount];
				v.name = ralloc_strdup(shader, u.name);
				v.offset = u.offset;
				v.count = u.num_components;
				shader->packedSamplersCount++;
			}
		}
		else
		{
			assert(false);
		}
	}
}

void insert_cb_packed_range(
	std::map<unsigned, std::list<glslopt_shader_packed_cb_copy>>& OutRangesMap,
	int inSrcIndex, int inSrcOffset, int inSize, int inDestIndex, int inDestPackedType, int inDestOffset)
{
	const unsigned SourceDestCBKey = (inSrcIndex << 20) | (inDestIndex << 8) | inDestPackedType;
	const glslopt_shader_packed_cb_copy CBRange
	{
		inSrcIndex, inSrcOffset, inDestIndex, inDestPackedType, inDestOffset, inSize
	};

	auto& ranges = OutRangesMap[SourceDestCBKey];
	if (ranges.size() == 0)
	{
		ranges.emplace_back(CBRange);
	}
	else
	{
		bool bIsAdded = false;
		auto prev_iter = ranges.end();
		for (auto iter = ranges.begin(); iter != ranges.end(); ++iter)
		{
			if (inSrcOffset + inSize <= iter->sourceOffset)
			{
				if (prev_iter == ranges.end())
				{
					ranges.emplace_front(CBRange);
				}
				else
				{
					ranges.emplace(iter, CBRange);
				}

				bIsAdded = true;
				break;
			}
			prev_iter = iter;
		}

		if (!bIsAdded)
		{
			ranges.emplace_back(CBRange);
		}

		// Try to merge ranges
		if (ranges.size() > 1)
		{
			while (true)
			{
				bool bIsDirty = false;

				std::list<glslopt_shader_packed_cb_copy> new_ranges;
				for (auto iter = ranges.begin(); iter != ranges.end(); ++iter)
				{
					if (iter == ranges.begin())
					{
						prev_iter = ranges.begin();
					}
					else
					{
						if ((prev_iter->sourceOffset + prev_iter->count) == iter->sourceOffset
							&& (prev_iter->destOffset + prev_iter->count) == iter->destOffset)
						{
							glslopt_shader_packed_cb_copy v = *prev_iter;
							v.count = prev_iter->count + iter->count;
							new_ranges.pop_back();
							new_ranges.emplace_back(v);
							++iter;
							new_ranges.insert(new_ranges.end(), iter, ranges.end());
							bIsDirty = true;
							break;
						}
					}

					new_ranges.emplace_back(*iter);
					prev_iter = iter;
				}

				ranges.swap(new_ranges);

				if (!bIsDirty) break;
			}
		}
	}
}

void find_shader_packed_uniform_buffers(glslopt_shader* shader,
	const glslopt_uniform_cbuffer_array* cbuffers, const glslopt_cb_packed_arrays_map* pcbarrmap)
{
	if (pcbarrmap->size() <= 0) return;

	// First find all used CBs (since we lost that info during flattening)
	std::set<const char*> usedCBuffers;
	for (const auto& cb_iter : (*pcbarrmap))
	{
		for (const auto& iter : cb_iter.second)
		{
			const glslopt_uniform_list& CBUniforms = iter.second;
			for (const glsl_packed_uniform& PackedUniform : CBUniforms)
			{
				if (PackedUniform.cb_packed_sampler[0])
				{
					assert(cb_iter.first == PackedUniform.cb_packed_sampler);
					usedCBuffers.emplace(PackedUniform.cb_packed_sampler);
				}
			}
		}
	}

	assert(usedCBuffers.size() == pcbarrmap->size());

	// NOTE: Use std::map to keep the value sorted by UBIndex(key)
	std::map<unsigned, std::list<glslopt_shader_packed_cb_copy>> packedRangeMap;

	// Now get the CB index
	for (unsigned index = 0; index < cbuffers->size(); ++index)
	{
		const glslopt_uniform_cbuffer& CBuffer = (*cbuffers)[index];
		if (usedCBuffers.find(CBuffer.struct_type->name) == usedCBuffers.end()) continue;

		glslopt_shader_packed_cbuffer& v = shader->packedUBs[shader->packedUBsCount];
		v.name = ralloc_strdup(shader, CBuffer.struct_var->name);
		v.type = ralloc_strdup(shader, CBuffer.struct_type->name);
		v.index = shader->packedUBsCount;
		v.length = 0;
		v.fields = nullptr;
		/*v.fields = rzalloc_array(shader, glslopt_shader_packed_cb_field, v.length);
		for (const glsl_packed_uniform& field : cbfields)
		{
			v.fields[i].name = ralloc_strdup(shader, field.name);
			v.fields[i].offset;
			v.fields[i].count;
		}*/

		const auto& PackedArrays = pcbarrmap->find(CBuffer.struct_type->name)->second;
		for (const auto& iter : PackedArrays)
		{
			const char PackedType = iter.first;
			const glslopt_uniform_list& CBUniforms = iter.second;
			for (const glsl_packed_uniform& PackedUniform : CBUniforms)
			{
				insert_cb_packed_range(packedRangeMap,
					v.index, PackedUniform.offsetIntoCBufferInFloats,
					PackedUniform.sizeInFloats, 0, PackedType, PackedUniform.offset);
			}
		}

		shader->packedUBsCount++;
	}

	// Store packed cbuffer copy info
	for (const auto& iter : packedRangeMap)
	{
		for (const glslopt_shader_packed_cb_copy& cbcopy : iter.second)
		{
			shader->packedUBsGlobalCopies[shader->packedUBsGlobalCopiesCount] = cbcopy;
			shader->packedUBsGlobalCopiesCount++;
		}
	}
}
//// END MODIFY: bzbee.igg

glslopt_shader* glslopt_optimize_legacy(glslopt_ctx* ctx, glslopt_shader_type type, const char* shaderSource, unsigned options)
{
	glslopt_shader* shader = new (ctx->mem_ctx) glslopt_shader();

	PrintGlslMode printMode = kPrintGlslVertex;
	switch (type) {
	case kGlslOptShaderVertex:
		shader->shader->Type = GL_VERTEX_SHADER;
		shader->shader->Stage = MESA_SHADER_VERTEX;
		printMode = kPrintGlslVertex;
		break;
	case kGlslOptShaderFragment:
		shader->shader->Type = GL_FRAGMENT_SHADER;
		shader->shader->Stage = MESA_SHADER_FRAGMENT;
		printMode = kPrintGlslFragment;
		break;
	}
	if (!shader->shader->Type)
	{
		shader->infoLog = ralloc_asprintf(shader, "Unknown shader type %d", (int)type);
		shader->status = false;
		return shader;
	}

	_mesa_glsl_parse_state* state = new (shader) _mesa_glsl_parse_state(&ctx->mesa_ctx, shader->shader->Stage, shader);
	if (ctx->target == kGlslTargetMetal)
		state->metal_target = true;
	state->error = 0;

	if (!(options & kGlslOptionSkipPreprocessor))
	{
		state->error = !!glcpp_preprocess(state, &shaderSource, &state->info_log, state->extensions, &ctx->mesa_ctx);
		if (state->error)
		{
			shader->status = !state->error;
			shader->infoLog = state->info_log;
			return shader;
		}
	}

	_mesa_glsl_lexer_ctor(state, shaderSource);
	_mesa_glsl_parse(state);
	_mesa_glsl_lexer_dtor(state);

	exec_list* ir = new (shader) exec_list();
	shader->shader->ir = ir;

	if (!state->error && !state->translation_unit.is_empty())
		_mesa_ast_to_hir(ir, state);

	// Un-optimized output
	if (!state->error) {
		validate_ir_tree(ir);
		if (ctx->target == kGlslTargetMetal)
			shader->rawOutput = _mesa_print_ir_metal(ir, state, ralloc_strdup(shader, ""), printMode, &shader->uniformsSize);
		else
			shader->rawOutput = _mesa_print_ir_glsl(ir, state, ralloc_strdup(shader, ""), printMode);
	}

	// Link built-in functions
	shader->shader->symbols = state->symbols;
	shader->shader->uses_builtin_functions = state->uses_builtin_functions;

	struct gl_shader* linked_shader = NULL;

	if (!state->error && !ir->is_empty() && !(options & kGlslOptionNotFullShader))
	{
		linked_shader = link_intrastage_shaders(shader,
			&ctx->mesa_ctx,
			shader->whole_program,
			shader->whole_program->Shaders,
			shader->whole_program->NumShaders);
		if (!linked_shader)
		{
			shader->status = false;
			shader->infoLog = shader->whole_program->InfoLog;
			return shader;
		}
		ir = linked_shader->ir;

		debug_print_ir("==== After link ====", ir, state, shader);
	}

	// Do optimization post-link
	if (!state->error && !ir->is_empty())
	{
		const bool linked = !(options & kGlslOptionNotFullShader);
		do_optimization_passes(ir, linked, state, shader);
		validate_ir_tree(ir);
	}

	// Final optimized output
	if (!state->error)
	{
		if (ctx->target == kGlslTargetMetal)
			shader->optimizedOutput = _mesa_print_ir_metal(ir, state, ralloc_strdup(shader, ""), printMode, &shader->uniformsSize);
		else
			shader->optimizedOutput = _mesa_print_ir_glsl(ir, state, ralloc_strdup(shader, ""), printMode);
	}

	shader->status = !state->error;
	shader->infoLog = state->info_log;

	find_shader_variables(shader, ir);
	if (!state->error)
		calculate_shader_stats(ir, &shader->statsMath, &shader->statsTex, &shader->statsFlow);

	ralloc_free(ir);
	ralloc_free(state);

	if (linked_shader)
		ralloc_free(linked_shader);

	return shader;
}

glslopt_shader* glslopt_optimize (glslopt_ctx* ctx, glslopt_shader_type type, const char* shaderSource, unsigned options)
{
	glslopt_shader* shader = new (ctx->mem_ctx) glslopt_shader ();

	PrintGlslMode printMode = kPrintGlslVertex;
	switch (type) {
	case kGlslOptShaderVertex:
			shader->shader->Type = GL_VERTEX_SHADER;
			shader->shader->Stage = MESA_SHADER_VERTEX;
			printMode = kPrintGlslVertex;
			break;
	case kGlslOptShaderFragment:
			shader->shader->Type = GL_FRAGMENT_SHADER;
			shader->shader->Stage = MESA_SHADER_FRAGMENT;
			printMode = kPrintGlslFragment;
			break;
	}
	if (!shader->shader->Type)
	{
		shader->infoLog = ralloc_asprintf (shader, "Unknown shader type %d", (int)type);
		shader->status = false;
		return shader;
	}

	_mesa_glsl_parse_state* state = new (shader) _mesa_glsl_parse_state (&ctx->mesa_ctx, shader->shader->Stage, shader);
	if (ctx->target == kGlslTargetMetal)
		state->metal_target = true;
	state->error = 0;

	if (!(options & kGlslOptionSkipPreprocessor))
	{
		state->error = !!glcpp_preprocess (state, &shaderSource, &state->info_log, state->extensions, &ctx->mesa_ctx);
		if (state->error)
		{
			shader->status = !state->error;
			shader->infoLog = state->info_log;
			return shader;
		}
	}

	if (!state->error)
	{
		shader->preprocessedOutput = ralloc_strdup(shader, shaderSource);
	}

	_mesa_glsl_lexer_ctor (state, shaderSource);
	_mesa_glsl_parse (state);
	_mesa_glsl_lexer_dtor (state);

	exec_list* ir = new (shader) exec_list();
	shader->shader->ir = ir;

	if (!state->error && !state->translation_unit.is_empty())
		_mesa_ast_to_hir (ir, state);

	// Un-optimized output
	if (!state->error) {
		validate_ir_tree(ir);
		if (ctx->target == kGlslTargetMetal)
			shader->rawOutput = _mesa_print_ir_metal(ir, state, ralloc_strdup(shader, ""), printMode, &shader->uniformsSize);
		else
			shader->rawOutput = _mesa_print_ir_glsl(ir, state, ralloc_strdup(shader, ""), printMode);
	}
	
	// Link built-in functions
	shader->shader->symbols = state->symbols;
	shader->shader->uses_builtin_functions = state->uses_builtin_functions;
	
	struct gl_shader* linked_shader = NULL;

	if (!state->error && !ir->is_empty() && !(options & kGlslOptionNotFullShader))
	{
		linked_shader = link_intrastage_shaders(shader,
												&ctx->mesa_ctx,
												shader->whole_program,
												shader->whole_program->Shaders,
												shader->whole_program->NumShaders);
		if (!linked_shader)
		{
			shader->status = false;
			shader->infoLog = shader->whole_program->InfoLog;
			return shader;
		}
		ir = linked_shader->ir;
		
		debug_print_ir ("==== After link ====", ir, state, shader);
	}

	glslopt_uniform_cbuffer_array cbufferStrutcs;
	glslopt_packed_arrays_map globalPackedArraysMap;
	glslopt_cb_packed_arrays_map cbufferPackedArraysMap;

	// Do optimization post-link
	if (!state->error && !ir->is_empty())
	{
		const bool bPackUniforms = !!(options & kGlslOptionPackUniforms);
		const bool bLinked = !(options & kGlslOptionNotFullShader);

		if (bPackUniforms)
		{
			if (bLinked)
			{
				do_function_inlining(ir);
				do_dead_functions(ir);
				do_structure_splitting(ir);

				do_if_simplification(ir);
				opt_flatten_nested_if_blocks(ir);
				propagate_precision(ir, state->metal_target);
			}

			if (type == kGlslOptShaderVertex)
			{
				rename_shader_inputs(shader, ir);
			}
			
			glsl_uniform_struct_layouts flattened;
			flatten_uniform_buffer_structures(ir, state, &cbufferStrutcs, &flattened);

			repack_uniforms(shader, ir, state,
				&cbufferStrutcs, &flattened, &globalPackedArraysMap, &cbufferPackedArraysMap);

			if (bLinked)
			{
				move_global_instructions_to_main(ir, state);
			}
		}

		//do_optimization_passes(ir, bLinked, state, shader);
		validate_ir_tree(ir);
	}

	// Final optimized output
	if (!state->error)
	{
		if (ctx->target == kGlslTargetMetal)
			shader->optimizedOutput = _mesa_print_ir_metal(ir, state, ralloc_strdup(shader, ""), printMode, &shader->uniformsSize);
		else
			shader->optimizedOutput = _mesa_print_ir_glsl(ir, state, ralloc_strdup(shader, ""), printMode);
	}

	shader->status = !state->error;
	shader->infoLog = state->info_log;

	find_shader_variables (shader, ir);
	find_shader_packed_uniforms(shader, &globalPackedArraysMap);
	find_shader_packed_uniform_buffers(shader, &cbufferStrutcs, &cbufferPackedArraysMap);

	if (!state->error)
		calculate_shader_stats (ir, &shader->statsMath, &shader->statsTex, &shader->statsFlow);

	ralloc_free (ir);
	ralloc_free (state);

	if (linked_shader)
		ralloc_free(linked_shader);

	return shader;
}

void glslopt_shader_delete (glslopt_shader* shader)
{
	delete shader;
}

bool glslopt_get_status (glslopt_shader* shader)
{
	return shader->status;
}

const char* glslopt_get_output (glslopt_shader* shader)
{
	return shader->optimizedOutput;
}

const char* glslopt_get_raw_output (glslopt_shader* shader)
{
	return shader->rawOutput;
}

const char* glslopt_get_preprocessed_output(glslopt_shader* shader)
{
	return shader->preprocessedOutput;
}

const char* glslopt_get_log (glslopt_shader* shader)
{
	return shader->infoLog;
}

int glslopt_shader_get_output_count(glslopt_shader* shader)
{
	return shader->outputCount;
}

int glslopt_shader_get_input_count (glslopt_shader* shader)
{
	return shader->inputCount;
}

int glslopt_shader_get_uniform_count (glslopt_shader* shader)
{
	return shader->uniformCount;
}

int glslopt_shader_get_uniform_total_size (glslopt_shader* shader)
{
	return shader->uniformsSize;
}

int glslopt_shader_get_texture_count (glslopt_shader* shader)
{
	return shader->textureCount;
}

void glslopt_shader_get_output_desc(glslopt_shader* shader, int index, const char** outName, glslopt_basic_type* outType, glslopt_precision* outPrec, int* outVecSize, int* outMatSize, int* outArraySize, int* outLocation)
{
	const glslopt_shader_var& v = shader->outputs[index];
	*outName = v.name;
	*outType = v.type;
	*outPrec = v.prec;
	*outVecSize = v.vectorSize;
	*outMatSize = v.matrixSize;
	*outArraySize = v.arraySize;
	*outLocation = v.location;
}

void glslopt_shader_get_input_desc (glslopt_shader* shader, int index, const char** outName, glslopt_basic_type* outType, glslopt_precision* outPrec, int* outVecSize, int* outMatSize, int* outArraySize, int* outLocation)
{
	const glslopt_shader_var& v = shader->inputs[index];
	*outName = v.name;
	*outType = v.type;
	*outPrec = v.prec;
	*outVecSize = v.vectorSize;
	*outMatSize = v.matrixSize;
	*outArraySize = v.arraySize;
	*outLocation = v.location;
}

void glslopt_shader_get_uniform_desc (glslopt_shader* shader, int index, const char** outName, glslopt_basic_type* outType, glslopt_precision* outPrec, int* outVecSize, int* outMatSize, int* outArraySize, int* outLocation)
{
	const glslopt_shader_var& v = shader->uniforms[index];
	*outName = v.name;
	*outType = v.type;
	*outPrec = v.prec;
	*outVecSize = v.vectorSize;
	*outMatSize = v.matrixSize;
	*outArraySize = v.arraySize;
	*outLocation = v.location;
}

void glslopt_shader_get_texture_desc (glslopt_shader* shader, int index, const char** outName, glslopt_basic_type* outType, glslopt_precision* outPrec, int* outVecSize, int* outMatSize, int* outArraySize, int* outLocation)
{
	const glslopt_shader_var& v = shader->textures[index];
	*outName = v.name;
	*outType = v.type;
	*outPrec = v.prec;
	*outVecSize = v.vectorSize;
	*outMatSize = v.matrixSize;
	*outArraySize = v.arraySize;
	*outLocation = v.location;
}

int glslopt_shader_uniform_get_packed_global_count(glslopt_shader* shader)
{
	return shader->packedGlobalsCount;
}

int glslopt_shader_uniform_get_packed_buffer_count(glslopt_shader* shader)
{
	return shader->packedUBsCount;
}

int glslopt_shader_uniform_get_packed_buffer_copy_count(glslopt_shader* shader)
{
	return shader->packedUBsGlobalCopiesCount;
}

int glslopt_shader_uniform_get_packed_sampler_count(glslopt_shader* shader)
{
	return shader->packedSamplersCount;
}

void glslopt_shader_uniform_get_packed_global_desc(glslopt_shader* shader, int index, const char** out_name, char* out_packedtype, int* out_offset, int* out_count)
{
	const glslopt_shader_packed_global& v = shader->packedGlobals[index];
	*out_name = v.name;
	*out_packedtype = v.packedType;
	*out_offset = v.offset;
	*out_count = v.count;
}

void glslopt_shader_uniform_get_packed_buffer_desc(glslopt_shader* shader, int index, const char** out_name, const char** out_type, int* out_field_count)
{
	const glslopt_shader_packed_cbuffer& v = shader->packedUBs[index];
	*out_name = v.name;
	*out_type = v.type;
	*out_field_count = v.length;
}

void glslopt_shader_uniform_get_packed_buffer_field_desc(glslopt_shader* shader, int index, int fieldIndex, const char** out_name, int* out_offset, int* out_count)
{
	const glslopt_shader_packed_cb_field& v = shader->packedUBs[index].fields[fieldIndex];
	*out_name = v.name;
	*out_offset = v.offset;
	*out_count = v.count;
}

void glslopt_shader_uniform_get_packed_buffer_copy_desc(glslopt_shader* shader, int index,
	int* out_sourceBufferIndex, int* out_sourceOffset, int* out_destBufferIndex, char* out_destPackedType, int* out_destOffset, int* out_count)
{
	const glslopt_shader_packed_cb_copy& v = shader->packedUBsGlobalCopies[index];
	*out_sourceBufferIndex = v.sourceCB;
	*out_sourceOffset = v.sourceOffset;
	*out_destBufferIndex = v.destCB;
	*out_destPackedType = (char)v.destPackedType;
	*out_destOffset = v.destOffset;
	*out_count = v.count;
}

void glslopt_shader_uniform_get_packed_sampler_desc(glslopt_shader* shader, int index, const char** out_name, int* out_offset, int* out_count)
{
	const glslopt_shader_packed_sampler& v = shader->packedSamplers[index];
	*out_name = v.name;
	*out_offset = v.offset;
	*out_count = v.count;
}

void glslopt_shader_get_stats (glslopt_shader* shader, int* approxMath, int* approxTex, int* approxFlow)
{
	*approxMath = shader->statsMath;
	*approxTex = shader->statsTex;
	*approxFlow = shader->statsFlow;
}
