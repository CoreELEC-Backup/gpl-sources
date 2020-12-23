/* Copyright (c) 2017-2018 Hans-Kristian Arntzen
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "cookie.hpp"
#include "descriptor_set.hpp"
#include "hash.hpp"
#include "intrusive.hpp"
#include "limits.hpp"
#include "vulkan.hpp"
#include "enum_cast.hpp"

namespace Vulkan
{
class Device;

enum class ShaderStage
{
	Vertex = 0,
	TessControl = 1,
	TessEvaluation = 2,
	Geometry = 3,
	Fragment = 4,
	Compute = 5,
	Count
};

struct ResourceLayout
{
	uint32_t input_mask = 0;
	uint32_t output_mask = 0;
	uint32_t push_constant_size = 0;
	uint32_t spec_constant_mask = 0;
	DescriptorSetLayout sets[VULKAN_NUM_DESCRIPTOR_SETS];
};

struct CombinedResourceLayout
{
	uint32_t attribute_mask = 0;
	uint32_t render_target_mask = 0;
	DescriptorSetLayout sets[VULKAN_NUM_DESCRIPTOR_SETS] = {};
	uint32_t stages_for_bindings[VULKAN_NUM_DESCRIPTOR_SETS][VULKAN_NUM_BINDINGS] = {};
	uint32_t stages_for_sets[VULKAN_NUM_DESCRIPTOR_SETS] = {};
	VkPushConstantRange push_constant_range = {};
	uint32_t descriptor_set_mask = 0;
	uint32_t spec_constant_mask[Util::ecast(ShaderStage::Count)] = {};
	uint32_t combined_spec_constant_mask = 0;
	Util::Hash push_constant_layout_hash = 0;
};

class PipelineLayout : public HashedObject<PipelineLayout>
{
public:
	PipelineLayout(Util::Hash hash, Device *device, const CombinedResourceLayout &layout);
	~PipelineLayout();

	const CombinedResourceLayout &get_resource_layout() const
	{
		return layout;
	}

	VkPipelineLayout get_layout() const
	{
		return pipe_layout;
	}

	DescriptorSetAllocator *get_allocator(unsigned set) const
	{
		return set_allocators[set];
	}

private:
	Device *device;
	VkPipelineLayout pipe_layout = VK_NULL_HANDLE;
	CombinedResourceLayout layout;
	DescriptorSetAllocator *set_allocators[VULKAN_NUM_DESCRIPTOR_SETS] = {};
};

class Shader : public HashedObject<Shader>
{
public:
	Shader(Util::Hash hash, Device *device, const uint32_t *data, size_t size);
	~Shader();

	const ResourceLayout &get_layout() const
	{
		return layout;
	}

	VkShaderModule get_module() const
	{
		return module;
	}

	static const char *stage_to_name(ShaderStage stage);

private:
	Device *device;
	VkShaderModule module;
	ResourceLayout layout;
};

class Program : public HashedObject<Program>, public InternalSyncEnabled
{
public:
	Program(Device *device, Shader *vertex, Shader *fragment);
	Program(Device *device, Shader *compute);
	~Program();

	inline const Shader *get_shader(ShaderStage stage) const
	{
		return shaders[Util::ecast(stage)];
	}

	void set_pipeline_layout(PipelineLayout *new_layout)
	{
		layout = new_layout;
	}

	PipelineLayout *get_pipeline_layout() const
	{
		return layout;
	}

	VkPipeline get_pipeline(Util::Hash hash) const;
	VkPipeline add_pipeline(Util::Hash hash, VkPipeline pipeline);

private:
	void set_shader(ShaderStage stage, Shader *handle);
	Device *device;
	Shader *shaders[Util::ecast(ShaderStage::Count)] = {};
	PipelineLayout *layout = nullptr;
	VulkanCache<Util::IntrusivePODWrapper<VkPipeline>> pipelines;
};
}
