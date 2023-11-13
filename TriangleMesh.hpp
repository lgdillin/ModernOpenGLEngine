#pragma once
#include "VulkanHeaderLoadOrder.hpp"
#include "VulkanMemory.hpp"

class TriangleMesh {
public:
	TriangleMesh();
	TriangleMesh(vk::Device _logicalDevice, vk::PhysicalDevice _physicalDevice);
	~TriangleMesh();

	vkMem::Buffer m_vertexBuffer;
private:
	vk::Device m_logicalDevice;
};