#pragma once

#include "VulkanHeaderLoadOrder.hpp"

namespace vkShader {
	static std::vector<char> readFile(
		std::string _filename,
		bool _debug
	) {
		std::ifstream file(_filename, std::ios::ate | std::ios::binary);

		if (_debug && !file.is_open()) {
			std::cout << "failed to load " << _filename << std::endl;
		}

		// tellg() queries location of pointer
		size_t filesize{ static_cast<size_t>(file.tellg()) };

		std::vector<char> buffer(filesize);
		file.seekg(0);
		file.read(buffer.data(), filesize);

		file.close();
		return buffer;
	}

	static vk::ShaderModule createModule(
		std::string _filename,
		vk::Device _logicalDevice,
		bool _debug
	) {
		std::vector<char> sourceCode = readFile(_filename, _debug);

		vk::ShaderModuleCreateInfo info = {};
		info.flags = vk::ShaderModuleCreateFlags(); // sets default values like sType, pNext
		info.codeSize = sourceCode.size();
		info.pCode = reinterpret_cast<const uint32_t *>(sourceCode.data());

		return _logicalDevice.createShaderModule(info);
	}
}