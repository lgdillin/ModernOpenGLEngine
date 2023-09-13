#pragma once

// indices (locations) of queue families if they exist at all

struct QueueFamilyIndices {
	// Location of graphics queue family
	int graphicsFamily = -1;

	// Location of Presentation Queue Family
	int presentationFamily = -1;

	// Check if queue families are valid
	bool isValid() {
		return graphicsFamily >= 0 && presentationFamily >= 0;
	}
};

struct SwapChainDetails {
	// Surface properties, e.g. image size/extent
	VkSurfaceCapabilitiesKHR surfaceCapabilities;

	// Surface image formats, e.g. RGBA and size of each colour
	std::vector<VkSurfaceFormatKHR> formats;

	// How images should be presented to screen
	std::vector<VkPresentModeKHR> presentationModes;
};