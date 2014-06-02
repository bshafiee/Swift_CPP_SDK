/*
 * Endpoint.cpp
 *
 *  Created on: 2014-05-29
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Endpoint.h"

namespace Swift {

Endpoint::Endpoint() :
    name("null"), type("null"), adminURL("null"), id("null"), internalURL(
        "null"), publicURL("null"), region("null") {
  // TODO Auto-generated constructor stub
}

Endpoint::~Endpoint() {
  // TODO Auto-generated destructor stub
}

Endpoint* Endpoint::fromJSON(const Json::Value &val) {
  Endpoint *instance = new Endpoint();

  instance->setId(val.get("id", Json::Value::null).asString());
  instance->setName(val.get("name", Json::Value::null).asString());
  instance->setType(val.get("type", Json::Value::null).asString());
  instance->setAdminUrl(val.get("adminURL", Json::Value::null).asString());
  instance->setInternalUrl(val.get("internalURL", Json::Value::null).asString());
  instance->setPublicUrl(val.get("publicURL", Json::Value::null).asString());
  instance->setRegion(val.get("region", Json::Value::null).asString());

  return instance;
}

Json::Value* Endpoint::toJSON(const Endpoint &instance) {
  Json::Value* json = new Json::Value();

  (*json)["id"] = instance.getId();
  (*json)["name"] = instance.getName();
  (*json)["type"] = instance.getType();
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

const std::string& Endpoint::getName() const {
  return name;
}

void Endpoint::setName(const std::string& name) {
  this->name = name;
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

const std::string& Endpoint::getType() const {
  return type;
}

void Endpoint::setType(const std::string& type) {
  this->type = type;
}

} /* namespace Swift */
