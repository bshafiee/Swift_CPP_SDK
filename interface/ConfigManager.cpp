/**************************************************************************
    This is a general SDK for OpenStack Swift API written in C++
    Copyright (C) <2014>  <Behrooz Shafiee Sarjaz>
    This program comes with ABSOLUTELY NO WARRANTY;

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

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
