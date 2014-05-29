/*
 * Endpoint.h
 *
 *  Created on: 2014-05-29
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef ENDPOINT_H_
#define ENDPOINT_H_

#include <iostream>
#include <json/json.h>

namespace Swift {
/**
 *  {
            "endpoints" : [
               {
                  "adminURL" : "http://192.168.249.109:35357/v2.0",
                  "id" : "173b6f35020040e7b4db0119d3449be0",
                  "internalURL" : "http://192.168.249.109:5000/v2.0",
                  "publicURL" : "http://192.168.249.109:5000/v2.0",
                  "region" : "RegionOne"
               }
            ],
            "endpoints_links" : [],
            "name" : "keystone",
            "type" : "identity"
         }
 */
class Endpoint {
  std::string name;
  std::string type;
  std::string adminURL;
  std::string id;
  std::string internalURL;
  std::string publicURL;
  std::string region;

public:
  Endpoint();
  virtual ~Endpoint();
  static Endpoint* fromJSON(Json::Value *val);
  static Json::Value* toJSON(Endpoint* instance);
  //Getter Setters
  const std::string& getAdminUrl() const;
  void setAdminUrl(const std::string& adminUrl);
  const std::string& getId() const;
  void setId(const std::string& id);
  const std::string& getInternalUrl() const;
  void setInternalUrl(const std::string& internalUrl);
  const std::string& getName() const;
  void setName(const std::string& name);
  const std::string& getPublicUrl() const;
  void setPublicUrl(const std::string& publicUrl);
  const std::string& getRegion() const;
  void setRegion(const std::string& region);
  const std::string& getType() const;
  void setType(const std::string& type);
};

} /* namespace Swift */
#endif /* ENDPOINT_H_ */
