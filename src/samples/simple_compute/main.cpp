#include "simple_compute.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

std::unique_ptr<SimpleCompute> CreateAndInitSimpleCompute(int length) {
  constexpr int VULKAN_DEVICE_ID = 0;
  auto app = std::make_unique<SimpleCompute>(length);
  if (app == nullptr) {
    std::cout << "Can't create render of specified type" << std::endl;
    exit(1);
  }

  app->InitVulkan(nullptr, 0, VULKAN_DEVICE_ID);
  return app;
}

std::mt19937 gen{}; // Standard mersenne_twister_engine seeded with rd()

std::vector<float> PrepareRandomData(int length) {
  std::vector<float> res(length);
  std::uniform_real_distribution<float> dis(-10, 10);
  for (auto &it : res) {
    it = dis(gen);
  }
  return res;
}

const int LENGTH = 1e6;

void ComputeOnCPU(const std::vector<float> &in, std::vector<float> &out) {
  const int HALF_WINDOW_SIZE = 3;
  float win_sum = 0;
  for (int i = 0; i <= HALF_WINDOW_SIZE; ++i) {
    {
      int ind = LENGTH - i - 1;
      win_sum = 0;
      int start = std::max(0, ind - HALF_WINDOW_SIZE);
      int end = std::min(ind + HALF_WINDOW_SIZE + 1, LENGTH);
      for (int j = start; j < end; ++j) {
        win_sum += in[j];
      }
      out[ind] = in[ind] - win_sum / 7;
    }
    {
      int ind = i;
      win_sum = 0;
      int start = std::max(0, ind - HALF_WINDOW_SIZE);
      int end = std::min(ind + HALF_WINDOW_SIZE + 1, LENGTH);
      for (int j = start; j < end; ++j) {
        win_sum += in[j];
      }
      out[ind] = in[ind] - win_sum / 7;
    }
  }
  for (int i = HALF_WINDOW_SIZE + 1; i < LENGTH - HALF_WINDOW_SIZE; ++i) {
    win_sum -= in[i - HALF_WINDOW_SIZE - 1];
    win_sum += in[i + HALF_WINDOW_SIZE];
    out[i] = in[i] - win_sum / 7;
  }
}

void LogInfo(const std::vector<float> &res, std::string mode, TimeT duration) {
  auto sum = std::accumulate(res.begin(), res.end(), 0.f);
  std::cout << mode << "  sum: " << sum << "\n";
  std::cout << mode << " time: " << duration.count() << "sec"
            << "\n";
  std::cout << std::endl;
}

int main() {

  auto app = CreateAndInitSimpleCompute(LENGTH);

  const int TIMES = 6;

  for (int i = 0; i < TIMES; ++i) {
    auto data = PrepareRandomData(LENGTH);
    {
      std::vector<float> out(LENGTH);
      auto start = std::chrono::steady_clock::now();
      ComputeOnCPU(data, out);
      LogInfo(out, "CPU", std::chrono::steady_clock::now() - start);

      app->SetValues(data);
      app->Execute();
      auto gpu_out = app->GetOutValues();
      LogInfo(gpu_out, "GPU", app->GetComputationTime());

      const float EPS = 1e-4;
      for (int j = 0; j < LENGTH; ++j) {
        auto diff = out[j] - gpu_out[j];
        if (fabs(diff) > EPS) {
          std::cout << "fiasco: " << j << "-th element differs by " << diff
                    << "\n\n";
          break;
        }
      }

      std::cout
          << "if fiasco did not happen then each elements in resulted arrays "
             "differs less than "
          << EPS
          << " due to errors sum up => absolute error of resulted sums can be "
          << EPS * LENGTH << "\n\n";
    }
  }

  return 0;
}
