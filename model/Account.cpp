/*
 * Account.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Account.h"
#include "../io/HTTPIO.h"
#include <Poco/Exception.h>

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Swift {

/** Initialize Static members **/
ulong Account::numOfCalls = 0;

struct Role {
  string name = "null";
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
    tenant(nullptr), username(""), password(""), authUrl(""), allowReauthenticate(
        false), publicHost(""), preferredRegion(""), delimiter('/'), authenticationMethod(
        AuthenticationMethod::KEYSTONE) {
  // TODO Auto-generated constructor stub

}

SwiftResult<Account*>* Account::authenticate(const string & _username,
    const string & _password, const string &_authUrl, bool _allowReauthenticate,
    const string &_tenantName) {
  // Create Json Request
  Json::Value jReq;
  Json::Value auth;
  if (_tenantName != "")
    auth["tenantName"] = _tenantName;
  auth["passwordCredentials"]["username"] = _username;
  auth["passwordCredentials"]["password"] = _password;
  jReq["auth"] = auth;
  Json::FastWriter writer;

  //Make Network Request
  string req = writer.write(jReq);
  string contentType = "application/json";
  HTTPClientSession *httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();

  try {
    httpSession = Swift::HTTPIO::doPost(_authUrl, req, contentType);
  } catch (Exception &e) {
    SwiftResult<Account*> *result = new SwiftResult<Account*>();
    SwiftError error(SwiftError::SWIFT_EXCEPTION, e.displayText());
    result->setError(error);
    //Try to set HTTP Response as the payload
    httpSession->receiveResponse(*httpResponse);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }
  //Receive Data
  istream& rs = httpSession->receiveResponse(*httpResponse);
  //Check HTTP status
  if (httpResponse->getStatus() != HTTPResponse::HTTPStatus::HTTP_OK) {
    SwiftResult<Account*> *result = new SwiftResult<Account*>();
    SwiftError error(SwiftError::SWIFT_HTTP_ERROR, httpResponse->getReason());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }
  //Everythign seems fine
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(rs, root, true);
  if (!parsingSuccessful) {
    SwiftResult<Account*> *result = new SwiftResult<Account*>();
    SwiftError error(SwiftError::SWIFT_JSON_PARSE_ERROR, reader.getFormattedErrorMessages());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }
  //Parsing JSON Successful
  Account* instance = new Account();
  SwiftResult<Account*> *result = new SwiftResult<Account*>();
  return result;
}

long Account::getBytesUsed() {
}

int Account::getObjectCount() {
}

long Account::getServerTime() {
}

void Account::setAllowReauthenticate(bool allowReauthenticate) {
}

void Account::setPublicHost(const string& host) {
}

bool Account::isAllowReauthenticate() {
}

ulong Account::increaseCallCounter() {
  return ++numOfCalls;
}

ulong Account::getNumberOfCalls() {
  return Account::numOfCalls;
}

vector<Tenant> Account::getTenants() {
}

} /* namespace Swift */
