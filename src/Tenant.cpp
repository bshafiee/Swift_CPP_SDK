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

#include "Tenant.h"

namespace Swift {

Tenant::Tenant() :
    id("null"), name("null"), description("null"), enabled(false) {
  // TODO Auto-generated constructor stub

}

Tenant::~Tenant() {
  // TODO Auto-generated destructor stub
}

Tenant::Tenant(const std::string& _id, const std::string& _name,
    const std::string& _description, bool _enabled) {
  this->id = _id;
  this->name = _name;
  this->description = _description;
  this->enabled = _enabled;
}

Tenant* Tenant::fromJSON(const Json::Value &val) {
  Tenant *instance = new Tenant();
  instance->setId(val.get("id", Json::Value::null).asString());
  instance->setName(val.get("name", Json::Value::null).asString());
  instance->setDescription(
      val.get("description", Json::Value::null).asString());
  instance->setEnabled(val.get("enabled", false).asBool());
  return instance;
}

Json::Value* Tenant::toJSON(const Tenant &instance) {
  Json::Value* json = new Json::Value();

  (*json)["id"] = instance.getId();
  (*json)["name"] = instance.getName();
  (*json)["description"] = instance.getDescription();
  (*json)["enabled"] = instance.isEnabled();

  return json;
}
const std::string& Tenant::getDescription() const {
  return description;
}

void Tenant::setDescription(const std::string& description) {
  this->description = description;
}

bool Tenant::isEnabled() const {
  return enabled;
}

void Tenant::setEnabled(bool enabled) {
  this->enabled = enabled;
}

const std::string& Tenant::getId() const {
  return id;
}

void Tenant::setId(const std::string& id) {
  this->id = id;
}

const std::string& Tenant::getName() const {
  return name;
}

void Tenant::setName(const std::string& name) {
  this->name = name;
}

} /* namespace Swift */
