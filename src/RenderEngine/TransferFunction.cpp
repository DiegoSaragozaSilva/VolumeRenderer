#include "TransferFunction.hpp"

TransferFunction::TransferFunction() {
  numSamples  = 10000;
  redLimits   = glm::vec3(0.0f, 1.0f, 1.0f);
  greenLimits = glm::vec3(0.0f, 1.0f, 1.0f);
  blueLimits  = glm::vec3(0.0f, 1.0f, 1.0f);
  alphaLimitPoints.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
}

TransferFunction::~TransferFunction() {}

ImageData TransferFunction::getRGBImageData() {
  ImageData rgbImageData;
  rgbImageData.width    = numSamples;
  rgbImageData.height   = 1;
  rgbImageData.depth    = 1;
  rgbImageData.channels = 4;

  float redMidPoint   = (redLimits.z - redLimits.x) / 2.0f; 
  float greenMidPoint = (greenLimits.z - greenLimits.x) / 2.0f;
  float blueMidPoint  = (blueLimits.z - blueLimits.x) / 2.0f;

  for (uint32_t i = 0; i < numSamples; i++) {
    float samplingValue = (float)i / (float)numSamples;

    uint8_t redSample = 0;
    if (samplingValue >= redLimits.x && samplingValue <= redLimits.z)
      redSample = samplingValue <= redMidPoint ? (uint8_t)((redLimits.x + 2.0f * (redLimits.y - redLimits.x) * samplingValue) * 255.0f)
                                              : (uint8_t)((redLimits.y - 2.0f * redLimits.y * (samplingValue - 0.5f)) * 255.0f);
    uint8_t greenSample = 0;
    if (samplingValue >= greenLimits.x && samplingValue <= greenLimits.z)
      greenSample = samplingValue <= greenMidPoint ? (uint8_t)((greenLimits.x + 2.0f * (greenLimits.y - greenLimits.x) * samplingValue) * 255.0f) 
                                                  : (uint8_t)((greenLimits.y - 2.0f * greenLimits.y * (samplingValue - 0.5f)) * 255.0f);
    uint8_t blueSample = 0;
    if (samplingValue >= blueLimits.x && samplingValue <= blueLimits.z)
      blueSample = samplingValue <= blueMidPoint ? (uint8_t)((blueLimits.x + 2.0f * (blueLimits.y - blueLimits.x) * samplingValue) * 255.0f) 
                                                : (uint8_t)((blueLimits.y - 2.0f * blueLimits.y * (samplingValue - 0.5f)) * 255.0f);
  
    rgbImageData.data.push_back(redSample);
    rgbImageData.data.push_back(greenSample);
    rgbImageData.data.push_back(blueSample);
    rgbImageData.data.push_back(0);
  }
  return rgbImageData;
}

ImageData TransferFunction::getAlphaImageData() {
  ImageData a;
  return a;
}
