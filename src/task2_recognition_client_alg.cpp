#include "task2_recognition_client_alg.h"


Task2RecognitionAlgorithm::Task2RecognitionAlgorithm(void)
{
  pthread_mutex_init(&this->access_,NULL);
}

Task2RecognitionAlgorithm::~Task2RecognitionAlgorithm(void)
{
  pthread_mutex_destroy(&this->access_);
}

void Task2RecognitionAlgorithm::config_update(Config& config, uint32_t level)
{
  this->lock();

  // save the current configuration
  this->config_=config;

  this->unlock();
}

// ErlTask2Algorithm Public API
