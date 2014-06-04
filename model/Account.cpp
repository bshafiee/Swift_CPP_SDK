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

/** Forward Declarations **/
inline SwiftResult<std::istream*>* returnNullError(const std::string &whatsNull);

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
    userID(""), name(""), token(nullptr), allowReauthenticate(false), preferredRegion(
        ""), delimiter('/') {
  // TODO Auto-generated constructor stub

}

SwiftResult<Account*>* Account::authenticate(
    const AuthenticationInfo &_authInfo, bool _allowReauthenticate) {
  // Create Json Request
  Json::Value jReq;
  Json::Value auth;
  if (_authInfo.tenantName != "")
    auth["tenantName"] = _authInfo.tenantName;
  auth["passwordCredentials"]["username"] = _authInfo.username;
  auth["passwordCredentials"]["password"] = _authInfo.password;
  jReq["auth"] = auth;
  Json::FastWriter writer;

  //Make Network Request
  string req = writer.write(jReq);
  string contentType = "application/json";
  HTTPClientSession *httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();
  istream* inputStream = nullptr;
  try {
    httpSession = Swift::HTTPIO::doPost(_authInfo.authUrl, req, contentType);
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
  instance->authInfo.username = userRoot.get("username", "").asString();
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
  instance->authInfo.password = _authInfo.password;
  instance->authInfo.authUrl = _authInfo.authUrl;
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

Tenant* Account::getTenant() {
  return this->token->getTenant();
}

Token* Account::getToken() {
  return this->token;
}

Service* Account::getSwiftService() {
  for (uint i = 0; i < services.size(); i++)
    if (services[i].getType() == "object-store")
      return &services[i];
  return nullptr;
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
      << "username:" << authInfo.username << ",\n" << "password:"
      << authInfo.password << ",\n" << "authUrl:" << authInfo.authUrl << ",\n"
      << "allowReauthenticate:" << allowReauthenticate << ",\n"
      << "preferredRegion:" << preferredRegion << ",\n" << "delimiter:"
      << delimiter << ",\n" << "authenticationmethod:"
      << authenticationMethodToString(authInfo.method) << ",\n" << "token:"
      << jsonWriter.write(*Token::toJSON(*this->token)) << ",\n"
      << roleStream.str() << ",\n" << serviceStream.str();

  return output.str();
}

SwiftResult<std::istream*>* Account::swiftAccountDetails(
    HTTPHeader& _formatHeader, bool _newest) {
  std::vector<HTTPHeader> _queryMap;
  return swiftAccountDetails(_formatHeader,_queryMap,_newest);
}

SwiftResult<std::istream*>* Account::swiftAccountDetails(HTTPHeader &_formatHeader, std::vector<HTTPHeader> &_queryMap, bool _newest) {
  //Swift Endpoint
  Endpoint* swiftEndpoint = this->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader>  *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));
  //add X-Newest
  if(_newest)
    reqParamMap->push_back(*new HTTPHeader("X-Newest", "True"));
  //Accept
  reqParamMap->push_back(*new HTTPHeader("Accept", "application/json"));
  //Create appropriate URI
  std::ostringstream queryStream;
  queryStream <<"?"<<_formatHeader.getQueryValue();
  if(_queryMap.size() > 0) {
    for(uint i=0;i<_queryMap.size();i++)
    {
      queryStream <<",";
      queryStream <<_queryMap[i].getQueryValue();
    }
  }

  URI uri(swiftEndpoint->getPublicUrl());
  uri.setQuery(queryStream.str());


  HTTPClientSession *httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();
  istream* inputStream = nullptr;
  try {
    /** This operation does not accept a request body. **/
    httpSession = Swift::HTTPIO::doGet(uri,reqParamMap);
    //Receive Data
    inputStream = &httpSession->receiveResponse(*httpResponse);
  } catch (Exception &e) {
    SwiftResult<std::istream*> *result = new SwiftResult<std::istream*>();
    SwiftError error(SwiftError::SWIFT_EXCEPTION, e.displayText());
    result->setError(error);
    //Try to set HTTP Response as the payload
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }

  /**
   * Check HTTP return code
   * 200:
   *  Success. The response body lists the containers.
   *
   * 204:
   *  Success. The response body shows no containers.
   *  Either the account has no containers or you are paging
   *  through a long list of names by using the marker, limit,
   *  or end_marker query parameters, and you have reached
   *  the end of the list.
   */
  if(
      httpResponse->getStatus() != HTTPResponse::HTTP_OK &&
      httpResponse->getStatus() != HTTPResponse::HTTP_NO_CONTENT
      ) {
    SwiftResult<std::istream*> *result = new SwiftResult<std::istream*>();
    SwiftError error(SwiftError::SWIFT_HTTP_ERROR, httpResponse->getReason());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }
  //Everything seems fine
  SwiftResult<std::istream*> *result = new SwiftResult<std::istream*>();
  result->setError(SWIFT_OK);
  result->setResponse(httpResponse);
  result->setPayload(inputStream);
  return result;
}

inline SwiftResult<std::istream*>* returnNullError(const std::string &whatsNull) {
  SwiftResult<std::istream*> *result = new SwiftResult<std::istream*>();
  SwiftError error(SwiftError::SWIFT_FAIL, whatsNull+" is NULL");
  result->setError(error);
  result->setResponse(nullptr);
  result->setPayload(nullptr);
  return result;
}

} /* namespace Swift */
