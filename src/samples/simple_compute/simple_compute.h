#ifndef SIMPLE_COMPUTE_H
#define SIMPLE_COMPUTE_H

#include <utility>
#include <vector>
#define VK_NO_PROTOTYPES
#include "../../render/compute_common.h"
#include "../resources/shaders/common.h"
#include <vk_descriptor_sets.h>
#include <vk_copy.h>

#include <string>
#include <iostream>
#include <memory>
#include <chrono>
#include <type_traits>

using TimeT = std::chrono::duration<double>;

class SimpleCompute : public ICompute
{
public:
  SimpleCompute(uint32_t a_length);
  ~SimpleCompute()  { Cleanup(); };

  inline VkInstance   GetVkInstance() const override { return m_instance; }
  void InitVulkan(const char** a_instanceExtensions, uint32_t a_instanceExtensionsCount, uint32_t a_deviceId) override;

  void Execute() override;

  TimeT GetComputationTime() const {
    return computation_time;
  }

  template<typename T>
  void SetValues(T&& new_values) {
    static_assert(std::is_same_v<typename std::remove_reference<T>::type, std::vector<float>>, "Argument must be a std::vector<float>");
    assert(new_values.size() == m_length);
    in_values = std::forward<T>(new_values);
  }

  const auto& GetOutValues() const {
    return out_values;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // debugging utils
  //
  static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFn(
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
  {
    (void)flags;
    (void)objectType;
    (void)object;
    (void)location;
    (void)messageCode;
    (void)pUserData;
    std::cout << pLayerPrefix << ": " << pMessage << std::endl;
    return VK_FALSE;
  }

  VkDebugReportCallbackEXT m_debugReportCallback = nullptr;
private:

  VkInstance       m_instance       = VK_NULL_HANDLE;
  VkCommandPool    m_commandPool    = VK_NULL_HANDLE;
  VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
  VkDevice         m_device         = VK_NULL_HANDLE;
  VkQueue          m_computeQueue   = VK_NULL_HANDLE;
  VkQueue          m_transferQueue  = VK_NULL_HANDLE;

  vk_utils::QueueFID_T m_queueFamilyIDXs {UINT32_MAX, UINT32_MAX, UINT32_MAX};

  VkCommandBuffer m_cmdBufferCompute;
  VkFence m_fence;

  std::shared_ptr<vk_utils::DescriptorMaker> m_pBindings = nullptr;

  uint32_t m_length  = 16u;
  
  VkPhysicalDeviceFeatures m_enabledDeviceFeatures = {};
  std::vector<const char*> m_deviceExtensions      = {};
  std::vector<const char*> m_instanceExtensions    = {};

  bool m_enableValidation;
  std::vector<const char*> m_validationLayers;
  std::shared_ptr<vk_utils::ICopyEngine> m_pCopyHelper;

  VkDescriptorSet       m_avergeDS; 
  VkDescriptorSetLayout m_averageDSLayout = nullptr;
  
  VkPipeline m_pipeline;
  VkPipelineLayout m_layout;

  TimeT computation_time = {};

  VkBuffer in, out;

  std::vector<float> in_values, out_values;
 
  void CreateInstance();
  void CreateDevice(uint32_t a_deviceId);

  void BuildCommandBufferSimple(VkCommandBuffer a_cmdBuff, VkPipeline a_pipeline);

  void SetupSimplePipeline();
  void CreateComputePipeline();
  void CleanupPipeline();

  void Cleanup();

  void SetupValidationLayers();
};


#endif //SIMPLE_COMPUTE_H
