/*
 * Account.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Account.h"

namespace Swift {

struct Role {
  std::string name = "null";
  Role() {
  }
  static Role* fromJSON(Json::Value value) {
    Role* instance = new Role();
    instance->name = value.get("name", Json::Value::null).asString();
    return instance;
  }

  static Json::Value* toJSON(Role *instance) {
    Json::Value* root = new Json::Value();
    (*root)["name"] = instance->name;
    return root;
  }
};

Account::~Account() {
  // TODO Auto-generated destructor stub
}

Account::Account() :
    tenant(nullptr), username(""), password(""), authUrl(""),
    allowReauthenticate(false),publicHost(""), preferredRegion(""),
    delimiter('/'), authenticationMethod(AuthenticationMethod::KEYSTONE) {
  // TODO Auto-generated constructor stub

}

} /* namespace Swift */
