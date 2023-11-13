#pragma once

#include "VulkanHeaderLoadOrder.hpp"
#include "VulkanShader.hpp"

#include "VulkanRenderStructs.hpp"
#include "VulkanMesh.hpp"

namespace vkPipeline {
	struct GraphicsPipelineInBundle {
		vk::Device logicalDevice;
		std::string vertexFilepath;
		std::string fragmentFilepath;
		vk::Format swapchainImageFormat; // know which pixels we're binding in our color buffer
		//vk::Extent2D swapchainExtent; // screen size,but i dislike this name
		vk::Extent2D screenSize;
	};

	struct GraphicsPipelineOutBundle {
		vk::PipelineLayout layout;
		vk::RenderPass renderPass;
		vk::Pipeline pipeline;
	};

	////////////////////////////////////////////////////////
	static vk::PipelineLayout createPipelineLayout(
		vk::Device _logicalDevice,
		bool _debug
	) {
		vk::PipelineLayoutCreateInfo layoutInfo;
		layoutInfo.flags = vk::PipelineLayoutCreateFlags();
		layoutInfo.setLayoutCount = 0; // descriptor set for large data
		
		// push constants for small data
		layoutInfo.pushConstantRangeCount = 1; 
		vk::PushConstantRange pushConstantInfo;
		pushConstantInfo.offset = 0;
		pushConstantInfo.size = sizeof(vkRenderStructs::ObjectData);
		pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;
		layoutInfo.pPushConstantRanges = &pushConstantInfo;

		try {
			return _logicalDevice.createPipelineLayout(layoutInfo);
		} catch (vk::SystemError err) {
			if (_debug) { std::cout << "failed to create pipeline layout\n"; }
		}
	}

	////////////////////////////////////////////////////////
	static vk::RenderPass createRenderPass(
		vk::Device _logicalDevice,
		vk::Format _swapchainImageFormat,
		bool _debug
	) {
		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.flags = vk::AttachmentDescriptionFlags();
		colorAttachment.format = _swapchainImageFormat;
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;
		
		vk::SubpassDescription subpass = {};
		subpass.flags = vk::SubpassDescriptionFlags();
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
	
		// render pass describes underlying attachments
		vk::RenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.flags = vk::RenderPassCreateFlags();
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		try {
			return _logicalDevice.createRenderPass(renderPassInfo);
		} catch (vk::SystemError err) {
			if (_debug) { std::cout << "failed to create renderpass\n"; }
		}
	}

	static GraphicsPipelineOutBundle createGraphicsPipeline(
		GraphicsPipelineInBundle _specs,
		bool _debug
	) {
		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.flags = vk::PipelineCreateFlags();

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

		// vertex input (what the buffers/attributes are coming in)
		vk::VertexInputBindingDescription bindingDesc 
			= vkMesh::getPosColorBindingDescription();
		std::array<vk::VertexInputAttributeDescription, 2> attrDesc 
			= vkMesh::getPosColorAttributeDescription();
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
		vertexInputInfo.vertexAttributeDescriptionCount = 2;
		vertexInputInfo.pVertexAttributeDescriptions = attrDesc.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;

		// input assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
		inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
		inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

		// vertex shader
		if (_debug) { std::cout << "Create vertex shader module\n"; }
		vk::ShaderModule vertexShader = vkShader::createModule(
			_specs.vertexFilepath, _specs.logicalDevice, _debug);
		vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
		vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
		vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertexShaderInfo.module = vertexShader;
		vertexShaderInfo.pName = "main";
		shaderStages.push_back(vertexShaderInfo);

		// viewport and scissor (screen visibility tests)
		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)_specs.screenSize.width;
		viewport.height = (float)_specs.screenSize.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vk::Rect2D scissor = {};
		scissor.offset.x = 0.0f;
		scissor.offset.y = 0.0f;
		scissor.extent = _specs.screenSize;
		vk::PipelineViewportStateCreateInfo viewportState = {};
		viewportState.flags = vk::PipelineViewportStateCreateFlags();
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;
		pipelineInfo.pViewportState = &viewportState;

		// rasterizer
		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE; // throw its output away and don't produce colors
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise; // is this vertex winding?
		rasterizer.depthBiasEnable = VK_FALSE; // depth bias can be useful in shadow maps
		pipelineInfo.pRasterizationState = &rasterizer;

		// fragment shader
		if (_debug) { std::cout << "Create fragment shader module\n"; }
		vk::ShaderModule fragmentShader = vkShader::createModule(
			_specs.fragmentFilepath, _specs.logicalDevice, _debug);
		vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
		fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
		fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragmentShaderInfo.module = fragmentShader;
		fragmentShaderInfo.pName = "main";
		shaderStages.push_back(fragmentShaderInfo);
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();

		// Multisampling
		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		pipelineInfo.pMultisampleState = &multisampling;

		// Color blend (alpha transparency)
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask
			= vk::ColorComponentFlagBits::eR
			| vk::ColorComponentFlagBits::eG
			| vk::ColorComponentFlagBits::eB
			| vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;
		vk::PipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
		pipelineInfo.pColorBlendState = &colorBlending;

		// pipeline layout
		if (_debug) { std::cout << "create pipeline layout\n"; }
		vk::PipelineLayout layout = createPipelineLayout(
			_specs.logicalDevice, _debug);
		pipelineInfo.layout = layout;

		// Render pass
		if (_debug) { std::cout << "Create renderpass\n"; }
		vk::RenderPass renderPass = createRenderPass(_specs.logicalDevice,
			_specs.swapchainImageFormat, _debug); 
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;

		// Extra stuff
		// base pipeline to work from
		pipelineInfo.basePipelineHandle = nullptr;

		if (_debug) { std::cout << "create pipeline\n"; }
		vk::Pipeline graphicsPipeline;
		try {
			graphicsPipeline = _specs.logicalDevice
				.createGraphicsPipeline(nullptr, pipelineInfo).value;
		} catch (vk::SystemError err) {
			if (_debug) { std::cout << "failed to create graphics pipeline\n"; }
		}

		GraphicsPipelineOutBundle output = {};
		output.layout = layout;
		output.renderPass = renderPass;
		output.pipeline = graphicsPipeline;

		_specs.logicalDevice.destroyShaderModule(vertexShader);
		_specs.logicalDevice.destroyShaderModule(fragmentShader);
		return output;
	}
}