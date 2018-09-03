#include "task2_recognition_client_alg.h"

ErlTask2Algorithm::ErlTask2Algorithm(void)
{
  pthread_mutex_init(&this->access_,NULL);
}

ErlTask2Algorithm::~ErlTask2Algorithm(void)
{
  pthread_mutex_destroy(&this->access_);
}

void ErlTask2Algorithm::config_update(Config& config, uint32_t level)
{
  this->lock();

  // save the current configuration
  this->config_=config;
  
  this->unlock();
}

// ErlTask2Algorithm Public API
