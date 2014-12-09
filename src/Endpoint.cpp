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

#include "Endpoint.h"

namespace Swift {

Endpoint::Endpoint() :
    adminURL("null"), id("null"), internalURL(
        "null"), publicURL("null"), region("null") {
  // TODO Auto-generated constructor stub
}

Endpoint::~Endpoint() {
  // TODO Auto-generated destructor stub
}

Endpoint* Endpoint::fromJSON(const Json::Value &val) {
  Endpoint *instance = new Endpoint();
  //Parsing endpoint array information
  instance->setId(val.get("id", Json::Value::null).asString());
  instance->setAdminUrl(val.get("adminURL", Json::Value::null).asString());
  instance->setInternalUrl(val.get("internalURL", Json::Value::null).asString());
  instance->setPublicUrl(val.get("publicURL", Json::Value::null).asString());
  instance->setRegion(val.get("region", Json::Value::null).asString());

  return instance;
}

Json::Value* Endpoint::toJSON(const Endpoint &instance) {
  Json::Value* json = new Json::Value();

  (*json)["id"] = instance.getId();
  (*json)["adminURL"] = instance.getAdminUrl();
  (*json)["internalURL"] = instance.getInternalUrl();
  (*json)["publicURL"] = instance.getPublicUrl();
  (*json)["region"] = instance.getRegion();


  return json;
}

const std::string& Endpoint::getAdminUrl() const {
  return adminURL;
}

void Endpoint::setAdminUrl(const std::string& adminUrl) {
  adminURL = adminUrl;
}

const std::string& Endpoint::getId() const {
  return id;
}

void Endpoint::setId(const std::string& id) {
  this->id = id;
}

const std::string& Endpoint::getInternalUrl() const {
  return internalURL;
}

void Endpoint::setInternalUrl(const std::string& internalUrl) {
  internalURL = internalUrl;
}

const std::string& Endpoint::getPublicUrl() const {
  return publicURL;
}

void Endpoint::setPublicUrl(const std::string& publicUrl) {
  publicURL = publicUrl;
}

const std::string& Endpoint::getRegion() const {
  return region;
}

void Endpoint::setRegion(const std::string& region) {
  this->region = region;
}
} /* namespace Swift */
