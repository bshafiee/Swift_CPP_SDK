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
    delimiter('/'), authenticationMethod(AuthenticationMethod::KEYSTONE),
    numOfCalls(0) {
  // TODO Auto-generated constructor stub

}

SwiftResult<Account*> Account::authenticate(std::string username,
    std::string password, std::string authUrl, bool allowReauthenticate,
    std::string tenant) {
  Account* instance = new Account();


}

long Account::getBytesUsed() {
}

int Account::getObjectCount() {
}

long Account::getServerTime() {
}

void Account::setAllowReauthenticate(bool allowReauthenticate) {
}

void Account::setPublicHost(const std::string& host) {
}

bool Account::isAllowReauthenticate() {
}

void Account::increaseCallCounter() {
}

int Account::getNumberOfCalls() {
}

std::vector<Tenant> Account::getTenants() {
}

} /* namespace Swift */
