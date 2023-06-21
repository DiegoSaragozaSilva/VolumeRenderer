#ifndef _TRANSFER_FUNCTION_HPP_
#define _TRANSFER_FUNCTION_HPP_

#include <glm/glm.hpp>
#include <vector>

#include "Utils.hpp"

class TransferFunction {
public:
  TransferFunction();
  ~TransferFunction();

  uint32_t numSamples;
  glm::vec3 redLimits;
  glm::vec3 greenLimits;
  glm::vec3 blueLimits;
  std::vector<glm::vec3> alphaLimitPoints;

  ImageData getRGBImageData();
  ImageData getAlphaImageData();
};

#endif
