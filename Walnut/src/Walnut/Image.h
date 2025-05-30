#pragma once

#include <string>

#include "vulkan/vulkan.h"

#include <vector>

namespace Walnut {

	enum class ImageFormat
	{
		None = 0,
		RGBA,
		RGBA32F
	};

	class Image
	{
	public:
		Image(std::string_view path);
		Image(uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr);
		void create(uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr);
		~Image();
		
		static std::vector<char>& readSpvShader(std::string nameFile);
		void createShaderModule(const std::vector<char>& Vertexcode, const std::vector<char>& Fragcode);

		void SetData(const void* data);
		void Resize(uint32_t width, uint32_t height);
		void AllocateMemory(uint64_t size);
		void Release();
	public:
		uint32_t m_Width = 0, m_Height = 0;

		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VkDeviceMemory m_Memory = nullptr;
		VkSampler m_Sampler = nullptr;


		ImageFormat m_Format = ImageFormat::None;

		VkBuffer m_StagingBuffer = nullptr;
		VkDeviceMemory m_StagingBufferMemory = nullptr;

		size_t m_AlignedSize = 0;
		VkDescriptorSet m_DescriptorSet = nullptr;

		std::string m_Filepath;
	};

}



