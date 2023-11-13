#pragma once

#include "VulkanHeaderLoadOrder.hpp"

namespace vkMem {
	struct BufferInput {
		size_t size;
		vk::BufferUsageFlags usage;
		vk::Device logicalDevice;
		vk::PhysicalDevice physicalDevice;
	};
	
	struct Buffer {
		vk::Buffer buffer;
		vk::DeviceMemory bufferMemory;
	};

	static uint32_t findMemoryTypeIndex(
		vk::PhysicalDevice _physicalDevice,
		uint32_t _supportedMemoryIndices,
		vk::MemoryPropertyFlags _requestedProperties
	) {
		vk::PhysicalDeviceMemoryProperties memoryProps 
			= _physicalDevice.getMemoryProperties();

		for (uint32_t i = 0; i < memoryProps.memoryTypeCount; ++i) {
			// position i will be set to 1 if it is supported
			bool supported = static_cast<bool>(_supportedMemoryIndices & (1 << i));
			bool sufficient = (memoryProps.memoryTypes[i].propertyFlags & _requestedProperties) == _requestedProperties;
			if (supported && sufficient) {
				return i;
			}
		}
	}

	static void allocateBufferMemory(
		Buffer &_buffer,
		const BufferInput &_input
	) {
		vk::MemoryRequirements memReqs = _input.logicalDevice
			.getBufferMemoryRequirements(_buffer.buffer);

		vk::MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize = memReqs.size;
		allocInfo.memoryTypeIndex = findMemoryTypeIndex(
			_input.physicalDevice,
			memReqs.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eHostVisible
				| vk::MemoryPropertyFlagBits::eHostCoherent
			);

		_buffer.bufferMemory = _input.logicalDevice.allocateMemory(allocInfo);
		_input.logicalDevice.bindBufferMemory(
			_buffer.buffer,
			_buffer.bufferMemory,
			0
		);

	}

	static Buffer createBuffer(BufferInput _input) {
		vk::BufferCreateInfo bufferInfo;
		bufferInfo.flags = vk::BufferCreateFlags();
		bufferInfo.size = _input.size;
		bufferInfo.usage = _input.usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		Buffer buffer;
		buffer.buffer = _input.logicalDevice.createBuffer(bufferInfo);

		allocateBufferMemory(buffer, _input);
		return buffer;
	}

}