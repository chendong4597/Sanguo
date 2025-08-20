#pragma once
#ifndef GLSL_OPTIMIZER_H
#define GLSL_OPTIMIZER_H

/*
 Main GLSL optimizer interface.
 See ../../README.md for more instructions.

 General usage:

 ctx = glslopt_initialize();
 for (lots of shaders) {
   shader = glslopt_optimize (ctx, shaderType, shaderSource, options);
   if (glslopt_get_status (shader)) {
     newSource = glslopt_get_output (shader);
   } else {
     errorLog = glslopt_get_log (shader);
   }
   glslopt_shader_delete (shader);
 }
 glslopt_cleanup (ctx);
*/

struct glslopt_shader;
struct glslopt_ctx;

enum glslopt_shader_type {
	kGlslOptShaderVertex = 0,
	kGlslOptShaderFragment,
};

// Options flags for glsl_optimize
enum glslopt_options {
	kGlslOptionSkipPreprocessor = (1<<0), // Skip preprocessing shader source. Saves some time if you know you don't need it.
	kGlslOptionNotFullShader = (1<<1), // Passed shader is not the full shader source. This makes some optimizations weaker.
	kGlslOptionPackUniforms = (1<<2),
};

// Optimizer target language
enum glslopt_target {
	kGlslTargetOpenGL = 0,
	kGlslTargetOpenGLES20 = 1,
	kGlslTargetOpenGLES30 = 2,
	kGlslTargetMetal = 3,
};

// Type info
enum glslopt_basic_type {
	kGlslTypeFloat = 0,
	kGlslTypeInt,
	kGlslTypeBool,
	kGlslTypeTex2D,
	kGlslTypeTex3D,
	kGlslTypeTexCube,
	kGlslTypeTex2DShadow,
	kGlslTypeTex2DArray,
	kGlslTypeOther,
	kGlslTypeCount
};
enum glslopt_precision {
	kGlslPrecHigh = 0,
	kGlslPrecMedium,
	kGlslPrecLow,
	kGlslPrecCount
};

enum glslopt_uniform_packed_typename
{
	kGlslPackedTypeNameHighP = 'h',
	kGlslPackedTypeNameMediumP = 'm',
	kGlslPackedTypeNameLowP = 'l',
	kGlslPackedTypeNameInt = 'i',
	kGlslPackedTypeNameUint = 'u',

	kGlslPackedTypeNameInvalid = ' ',
};

enum glslopt_uniform_packed_typeindex
{
	kGlslPackedTypeIndexHighP = 0,
	kGlslPackedTypeIndexMediumP,
	kGlslPackedTypeIndexLowP,
	kGlslPackedTypeIndexInt,
	kGlslPackedTypeIndexUint,

	kGlslPackedTypeIndexCount,
	kGlslPackedTypeIndexInvalid = -1,
};

glslopt_ctx* glslopt_initialize (glslopt_target target);
void glslopt_cleanup (glslopt_ctx* ctx);

void glslopt_set_max_unroll_iterations (glslopt_ctx* ctx, unsigned iterations);

glslopt_shader* glslopt_optimize (glslopt_ctx* ctx, glslopt_shader_type type, const char* shaderSource, unsigned options);
glslopt_shader* glslopt_optimize_legacy (glslopt_ctx* ctx, glslopt_shader_type type, const char* shaderSource, unsigned options);
bool glslopt_get_status (glslopt_shader* shader);
const char* glslopt_get_output (glslopt_shader* shader);
const char* glslopt_get_raw_output (glslopt_shader* shader);
const char* glslopt_get_preprocessed_output(glslopt_shader* shader);
const char* glslopt_get_log (glslopt_shader* shader);
void glslopt_shader_delete (glslopt_shader* shader);

int glslopt_shader_get_output_count(glslopt_shader* shader);
void glslopt_shader_get_output_desc(glslopt_shader* shader, int index, const char** outName, glslopt_basic_type* outType, glslopt_precision* outPrec, int* outVecSize, int* outMatSize, int* outArraySize, int* outLocation);
int glslopt_shader_get_input_count (glslopt_shader* shader);
void glslopt_shader_get_input_desc (glslopt_shader* shader, int index, const char** outName, glslopt_basic_type* outType, glslopt_precision* outPrec, int* outVecSize, int* outMatSize, int* outArraySize, int* outLocation);
int glslopt_shader_get_uniform_count (glslopt_shader* shader);
int glslopt_shader_get_uniform_total_size (glslopt_shader* shader);
void glslopt_shader_get_uniform_desc (glslopt_shader* shader, int index, const char** outName, glslopt_basic_type* outType, glslopt_precision* outPrec, int* outVecSize, int* outMatSize, int* outArraySize, int* outLocation);
int glslopt_shader_get_texture_count (glslopt_shader* shader);
void glslopt_shader_get_texture_desc (glslopt_shader* shader, int index, const char** outName, glslopt_basic_type* outType, glslopt_precision* outPrec, int* outVecSize, int* outMatSize, int* outArraySize, int* outLocation);

int glslopt_shader_uniform_get_packed_global_count(glslopt_shader* shader);
void glslopt_shader_uniform_get_packed_global_desc(glslopt_shader* shader, int index, const char** out_name, char* out_packedtype, int* out_offset, int* out_count);
int glslopt_shader_uniform_get_packed_buffer_count(glslopt_shader* shader);
void glslopt_shader_uniform_get_packed_buffer_desc(glslopt_shader* shader, int index, const char** out_name, const char** out_type, int* out_field_count);
void glslopt_shader_uniform_get_packed_buffer_field_desc(glslopt_shader* shader, int index, int fieldIndex, const char** out_name, int* out_offset, int* out_count);
int glslopt_shader_uniform_get_packed_buffer_copy_count(glslopt_shader* shader);
void glslopt_shader_uniform_get_packed_buffer_copy_desc(glslopt_shader* shader, int index,
	int* out_sourceBufferIndex, int* out_sourceOffset, int* out_destBufferIndex, char* out_destPackedType, int* out_destOffset, int* out_count);
int glslopt_shader_uniform_get_packed_sampler_count(glslopt_shader* shader);
void glslopt_shader_uniform_get_packed_sampler_desc(glslopt_shader* shader, int index, const char** out_name, int* out_offset, int* out_count);

// Get *very* approximate shader stats:
// Number of math, texture and flow control instructions.
void glslopt_shader_get_stats (glslopt_shader* shader, int* approxMath, int* approxTex, int* approxFlow);


#endif /* GLSL_OPTIMIZER_H */
