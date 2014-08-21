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

#include "Service.h"

namespace Swift {

Service::Service() :
    name("null"), type("null") {
  // TODO Auto-generated constructor stub
}

Service::~Service() {
  //Delete endpoints
  for(Endpoint* endpoint:endpoints) {
    delete endpoint;
    endpoint = nullptr;
  }

}

Service* Service::fromJSON(const Json::Value &val) {
  Service *instance = new Service();
  //Getting name and type
  instance->setName(val.get("name", Json::Value::null).asString());
  instance->setType(val.get("type", Json::Value::null).asString());
  //Parsing endpoint array information
  Json::Value endpoint = val.get("endpoints",Json::nullValue);
  for(uint i=0;i<endpoint.size();i++)
    instance->endpoints.push_back(Endpoint::fromJSON(endpoint[i]));

  return instance;
}

Json::Value* Service::toJSON(const Service &instance) {
  Json::Value* json = new Json::Value();

  (*json)["type"] = instance.getType();
  (*json)["name"] = instance.getName();
  //Create endpoints Arrayindex
  (*json)["endpoints"] = Json::Value(Json::arrayValue);
  for(uint i=0;i<instance.getEndpoints().size();i++) {
    Json::Value info;

    std::string tmp = instance.getEndpoints()[i]->getId();
    (info)["id"] = instance.getEndpoints()[i]->getId();
    (info)["adminURL"] = instance.getEndpoints()[i]->getAdminUrl();
    (info)["internalURL"] = instance.getEndpoints()[i]->getInternalUrl();
    (info)["publicURL"] = instance.getEndpoints()[i]->getPublicUrl();
    (info)["region"] = instance.getEndpoints()[i]->getRegion();

    (*json)["endpoints"].append(info);
  }

  return json;
}

const std::string& Service::getName() const {
  return name;
}

void Service::setName(const std::string& name) {
  this->name = name;
}

const std::string& Service::getType() const {
  return type;
}

const std::vector<Endpoint*>& Service::getEndpoints() const {
  return endpoints;
}

void Service::setEndpoints(const std::vector<Endpoint*>& endpoints) {
  this->endpoints = endpoints;
}

void Service::setType(const std::string& type) {
  this->type = type;
}

Endpoint* Service::getFirstEndpoint() {
  if(endpoints.size() > 0)
    return endpoints[0];
  else
    return nullptr;
}

} /* namespace Swift */
