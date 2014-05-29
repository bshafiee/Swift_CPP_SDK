/*
 * ConfigManager.cpp
 *
 *  Created on: 2014-05-26
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "ConfigManager.h"

namespace Swift {

static ConfigManager::ConfigMap config;

const string* ConfigManager::getProperty(ConfigKey key) {
  ConfigManager::ConfigMap::Iterator it = config.find(key);
  if(it == config.end())
    return nullptr;
  else
    return &it->second;
}

int ConfigManager::putProperty(ConfigKey key, const string &value) {
  config.insert(ConfigMap::ValueType(key, value));
  return SwiftError::SWIFT_OK;
}

int ConfigManager::removeProperty(ConfigKey key) {
  config.erase(key);
  return SwiftError::SWIFT_OK;
}

ConfigManager::ConfigMap::Iterator beginIterator() {
  return config.begin();
}

} /* namespace Swift */
