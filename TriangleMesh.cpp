#include "TriangleMesh.hpp"

TriangleMesh::TriangleMesh() {
}

TriangleMesh::TriangleMesh(
	vk::Device _logicalDevice, 
	vk::PhysicalDevice _physicalDevice
) {
	m_logicalDevice = _logicalDevice;

	std::vector<float> vertices = { {
		0.0f, -0.05f,
		0.0f, 1.0f, 0.0f,

		0.05f, 0.05f,
		0.0f, 1.0f, 0.0f,

		-0.05f, 0.05f,
		0.0f, 1.0f, 0.0f
	} };

	vkMem::BufferInput inputChunk;
	inputChunk.logicalDevice = _logicalDevice;
	inputChunk.physicalDevice = _physicalDevice;
	inputChunk.size = sizeof(float) * vertices.size();
	inputChunk.usage = vk::BufferUsageFlagBits::eVertexBuffer;

	m_vertexBuffer = vkMem::createBuffer(inputChunk);

	void *memoryLocation = _logicalDevice.mapMemory(
		m_vertexBuffer.bufferMemory,
		0,
		inputChunk.size
	);

	memcpy(memoryLocation, vertices.data(), inputChunk.size);
	_logicalDevice.unmapMemory(m_vertexBuffer.bufferMemory);
}

TriangleMesh::~TriangleMesh() {
	m_logicalDevice.destroyBuffer(m_vertexBuffer.buffer);
	m_logicalDevice.freeMemory(m_vertexBuffer.bufferMemory);
}