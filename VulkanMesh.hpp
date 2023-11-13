#pragma once

#include "VulkanHeaderLoadOrder.hpp"

namespace vkMesh {
	static vk::VertexInputBindingDescription getPosColorBindingDescription() {


		vk::VertexInputBindingDescription bindingDesc;
		bindingDesc.binding = 0;
		bindingDesc.stride = 5 * sizeof(float);
		bindingDesc.inputRate = vk::VertexInputRate::eVertex;
		
		return bindingDesc;
	}

	static std::array<vk::VertexInputAttributeDescription, 2> getPosColorAttributeDescription() {
		std::array<vk::VertexInputAttributeDescription, 2> attributes;

		attributes[0].binding = 0;
		attributes[0].location = 0;
		attributes[0].format = vk::Format::eR32G32Sfloat;
		attributes[0].offset = 0;

		attributes[1].binding = 0;
		attributes[1].location = 1;
		attributes[1].format = vk::Format::eR32G32B32Sfloat;
		attributes[1].offset = 2 * sizeof(float);

		return attributes;
	}
}