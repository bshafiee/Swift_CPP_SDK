/*
 * Account.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Account.h"
#include "../io/HTTPIO.h"
#include <Poco/Exception.h>
#include <sstream>      // std::ostringstream
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
  static Role* fromJSON(const Json::Value &value) {
    Role* instance = new Role();
    instance->name = value.get("name", Json::Value::null).asString();
    return instance;
  }

  static Json::Value* toJSON(const Role &instance) {
    Json::Value* root = new Json::Value();
    (*root)["name"] = instance.name;
    return root;
  }
};

Account::~Account() {
  // TODO Auto-generated destructor stub
}

Account::Account() :
    userID(""), name(""), token(nullptr), username(""), password(""), authUrl(
        ""), allowReauthenticate(false), preferredRegion(""), delimiter('/'), authenticationMethod(
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
  istream* inputStream = nullptr;
  try {
    httpSession = Swift::HTTPIO::doPost(_authUrl, req, contentType);
    //Receive Data
    inputStream = &httpSession->receiveResponse(*httpResponse);
  } catch (Exception &e) {
    SwiftResult<Account*> *result = new SwiftResult<Account*>();
    SwiftError error(SwiftError::SWIFT_EXCEPTION, e.displayText());
    result->setError(error);
    //Try to set HTTP Response as the payload
    //httpSession->receiveResponse(*httpResponse);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }

  //Check HTTP status
  if (httpResponse->getStatus() != HTTPResponse::HTTPStatus::HTTP_OK) {
    SwiftResult<Account*> *result = new SwiftResult<Account*>();
    SwiftError error(SwiftError::SWIFT_HTTP_ERROR, httpResponse->getReason());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }

  //Everything seems fine
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(*inputStream, root, true);
  if (!parsingSuccessful) {
    SwiftResult<Account*> *result = new SwiftResult<Account*>();
    SwiftError error(SwiftError::SWIFT_JSON_PARSE_ERROR,
        reader.getFormattedErrorMessages());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }
  /*Printing result for debugging purposes
  Json::StyledWriter styledWriter;
  std::cout<<styledWriter.write(root)<<std::endl;
  */
  //Parsing JSON Successful
  //Everything is inside "access"
  root = root["access"];
  Account* instance = new Account();
  //Parse User Info
  Json::Value userRoot = root.get("user", Json::nullValue);
  instance->userID = userRoot.get("id", "").asString();
  instance->name = userRoot.get("name", "").asString();
  instance->username = userRoot.get("username", "").asString();
  //Roles
  Json::Value roles = userRoot.get("roles", Json::nullValue);
  if (roles != Json::nullValue) {
    for (uint i = 0; i < roles.size(); i++) {
      Role *role = Role::fromJSON(roles[i]);
      instance->roles.push_back(*role);
    }
  }
  //Parse Token
  Json::Value tokenRoot = root.get("token", Json::nullValue);
  instance->token = Token::fromJSON(tokenRoot);
  //Parse Service Information
  Json::Value serviceRoot = root.get("serviceCatalog", Json::nullValue);
  if (serviceRoot != Json::nullValue)
    for (uint i = 0; i < serviceRoot.size(); i++)
      instance->services.push_back(*Service::fromJSON(serviceRoot[i]));

  //Return result
  instance->password = _password;
  instance->authUrl = _authUrl;
  instance->allowReauthenticate = _allowReauthenticate;

  SwiftResult<Account*> *result = new SwiftResult<Account*>();
  SwiftError error(SwiftError::SWIFT_OK, "SWIFT_OK");
  result->setError(error);
  result->setResponse(httpResponse);
  result->setPayload(instance);

  return result;
}

long Account::getBytesUsed() {
  //TODO implement
  return -1;
}

int Account::getObjectCount() {
  //TODO implement
  return -1;
}

long Account::getServerTime() {
  //TODO implement
  return -1;
}

void Account::setAllowReauthenticate(bool _allowReauthenticate) {
  this->allowReauthenticate = _allowReauthenticate;
}

bool Account::isAllowReauthenticate() {
  return this->allowReauthenticate;
}

ulong Account::increaseCallCounter() {
  return ++numOfCalls;
}

ulong Account::getNumberOfCalls() {
  return Account::numOfCalls;
}

Tenant* Account::getTenants() {
  return this->token->getTenant();
}

std::string Account::toString() {
  std::ostringstream output;
  std::ostringstream roleStream;
  std::ostringstream serviceStream;
  Json::StyledWriter jsonWriter;

  //Build Roles
  roleStream << "Roles: {";
  for (uint i = 0; i < roles.size(); i++)
    roleStream << jsonWriter.write(*Role::toJSON(roles[i])) << ",";
  roleStream << "}";
  //Build Services
  serviceStream << "Services: {";
  for (uint i = 0; i < services.size(); i++)
    serviceStream << jsonWriter.write(*Service::toJSON(services[i])) << ",";
  serviceStream << "}";

  output << "userID:" << userID << ",\n" << "name:" << name << ",\n"
      << "username:" << username << ",\n" << "password:" << password << ",\n"
      << "authUrl:" << authUrl << ",\n" << "allowReauthenticate:"
      << allowReauthenticate << ",\n" << "preferredRegion:" << preferredRegion
      << ",\n" << "delimiter:" << delimiter << ",\n" << "authenticationmethod:"
      << authenticationMethodToString(authenticationMethod) << ",\n" << "token:"
      << jsonWriter.write(*Token::toJSON(*this->token)) << ",\n"
      << roleStream.str() << ",\n" << serviceStream.str();

  return output.str();
}

} /* namespace Swift */
