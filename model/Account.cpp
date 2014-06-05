/*
 * Account.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Account.h"
#include "../io/HTTPIO.h"
#include <Poco/Exception.h>
#include <sstream>      // ostringstream
using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Swift {

/** Initialize Static members **/
ulong Account::numOfCalls = 0;

/** Forward Declarations **/
template<class T>
inline SwiftResult<T>* returnNullError(const string &whatsNull);

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
  //Create HTTPSession
  HTTPClientSession* httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();
  istream* inputStream = nullptr;
  try {
    URI uri(_authInfo.authUrl);
    httpSession = doHTTPIO(uri, HTTPRequest::HTTP_POST, nullptr, req,
        contentType);
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
   cout<<styledWriter.write(root)<<endl;
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

string Account::toString() {
  ostringstream output;
  ostringstream roleStream;
  ostringstream serviceStream;
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

SwiftResult<istream*>* Account::swiftAccountDetails(HTTPHeader& _formatHeader,
    bool _newest) {
  vector<HTTPHeader> _queryMap;
  return swiftAccountDetails(_formatHeader, _queryMap, _newest);
}

SwiftResult<istream*>* Account::swiftAccountDetails(HTTPHeader &_formatHeader,
    vector<HTTPHeader> &_queryMap, bool _newest) {
  //Swift Endpoint
  Endpoint* swiftEndpoint = this->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<istream*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));
  //add X-Newest
  if (_newest)
    reqParamMap->push_back(*new HTTPHeader("X-Newest", "True"));
  //Accept encoding
  reqParamMap->push_back(*new HTTPHeader("Accept", _formatHeader.getValue()));
  //Create appropriate URI
  ostringstream queryStream;
  queryStream << "?";
  if (_queryMap.size() > 0) {
    for (uint i = 0; i < _queryMap.size(); i++) {
      if (i > 0)
        queryStream << ",";
      queryStream << _queryMap[i].getQueryValue();
    }
  }

  URI uri(swiftEndpoint->getPublicUrl());
  uri.setQuery(queryStream.str());

  HTTPClientSession *httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();
  istream* inputStream = nullptr;
  try {
    /** This operation does not accept a request body. **/
    httpSession = doHTTPIO(uri, HTTPRequest::HTTP_GET, reqParamMap);
    //Receive Data
    inputStream = &httpSession->receiveResponse(*httpResponse);
  } catch (Exception &e) {
    SwiftResult<istream*> *result = new SwiftResult<istream*>();
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
  if (httpResponse->getStatus() != HTTPResponse::HTTP_OK
      && httpResponse->getStatus() != HTTPResponse::HTTP_NO_CONTENT) {
    SwiftResult<istream*> *result = new SwiftResult<istream*>();
    SwiftError error(SwiftError::SWIFT_HTTP_ERROR, httpResponse->getReason());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }
  //Everything seems fine
  SwiftResult<istream*> *result = new SwiftResult<istream*>();
  result->setError(SWIFT_OK);
  result->setResponse(httpResponse);
  result->setPayload(inputStream);
  return result;
}

template<class T>
inline SwiftResult<T>* returnNullError(const string &whatsNull) {
  SwiftResult<T> *result = new SwiftResult<T>();
  SwiftError error(SwiftError::SWIFT_FAIL, whatsNull + " is NULL");
  result->setError(error);
  result->setResponse(nullptr);
  result->setPayload(nullptr);
  return result;
}

SwiftResult<void*>* Account::swiftCreateMetadata(
    vector<pair<string, string> >& _metaData) {
  vector<HTTPHeader> _reqMap;  //zero sized requestParameter map
  return swiftCreateMetadata(_metaData, _reqMap);
}

SwiftResult<void*>* Account::swiftCreateMetadata(
    vector<pair<string, string> >& _metaData, vector<HTTPHeader>& _reqMap) {
  //Swift Endpoint
  Endpoint* swiftEndpoint = this->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));
  //Add rest of request Parameters
  if (_reqMap.size() > 0) {
    for (uint i = 0; i < _reqMap.size(); i++) {
      reqParamMap->push_back(_reqMap[i]);
    }
  }

  //Add Actual metadata
  if (_metaData.size() > 0)
    for (uint i = 0; i < _metaData.size(); i++)
      reqParamMap->push_back(
          *new HTTPHeader("X-Account-Meta-" + _metaData[i].first,
              _metaData[i].second));

  //Set URI
  URI uri(swiftEndpoint->getPublicUrl());
  //Creating HTTP Session
  HTTPResponse *httpResponse = new HTTPResponse();
  try {
    /** This operation does not accept a request body. **/
    HTTPClientSession *httpSession = doHTTPIO(uri, HTTPRequest::HTTP_POST,
        reqParamMap);
    httpSession->receiveResponse(*httpResponse);
  } catch (Exception &e) {
    SwiftResult<void*> *result = new SwiftResult<void*>();
    SwiftError error(SwiftError::SWIFT_EXCEPTION, e.displayText());
    result->setError(error);
    //Try to set HTTP Response as the payload
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }

  /**
   * Check HTTP return code
   * 204:
   *  Success. The response body is empty.
   */
  if (httpResponse->getStatus() != HTTPResponse::HTTP_NO_CONTENT) {
    SwiftResult<void*> *result = new SwiftResult<void*>();
    SwiftError error(SwiftError::SWIFT_HTTP_ERROR, httpResponse->getReason());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }
  //Everything seems fine
  SwiftResult<void*> *result = new SwiftResult<void*>();
  result->setError(SWIFT_OK);
  result->setResponse(httpResponse);
  result->setPayload(nullptr);
  return result;
}

SwiftResult<void*>* Account::swiftUpdateMetadata(
    vector<pair<string, string> >& _metaData) {
  return swiftCreateMetadata(_metaData);
}

SwiftResult<void*>* Account::swiftUpdateMetadata(
    vector<pair<string, string> >& _metaData, vector<HTTPHeader>& _reqMap) {
  return swiftCreateMetadata(_metaData, _reqMap);
}

SwiftResult<void*>* Account::swiftDeleteMetadata(
    vector<string> &_metaDataKeys) {
  vector<HTTPHeader> _reqMap;  //zero sized requestParameter map
  return swiftDeleteMetadata(_metaDataKeys, _reqMap);
}

SwiftResult<void*>* Account::swiftDeleteMetadata(
    std::vector<string> &_metaDataKeys, vector<HTTPHeader>& _reqMap) {
  //Swift Endpoint
  Endpoint* swiftEndpoint = this->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));
  //Add rest of request Parameters
  if (_reqMap.size() > 0) {
    for (uint i = 0; i < _reqMap.size(); i++) {
      reqParamMap->push_back(_reqMap[i]);
    }
  }

  //Add Actual metadata
  if (_metaDataKeys.size() > 0)
    for (uint i = 0; i < _metaDataKeys.size(); i++)
      reqParamMap->push_back(
          *new HTTPHeader("X-Remove-Account-Meta-" + _metaDataKeys[i], "x"));

  //Set URI
  URI uri(swiftEndpoint->getPublicUrl());
  //Creating HTTP Session
  HTTPResponse *httpResponse = new HTTPResponse();
  try {
    /** This operation does not accept a request body. **/
    HTTPClientSession *httpSession = doHTTPIO(uri, HTTPRequest::HTTP_POST,
        reqParamMap);
    httpSession->receiveResponse(*httpResponse);
  } catch (Exception &e) {
    SwiftResult<void*> *result = new SwiftResult<void*>();
    SwiftError error(SwiftError::SWIFT_EXCEPTION, e.displayText());
    result->setError(error);
    //Try to set HTTP Response as the payload
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }

  /**
   * Check HTTP return code
   * 204:
   *  Success. The response body is empty.
   */
  if (httpResponse->getStatus() != HTTPResponse::HTTP_NO_CONTENT) {
    SwiftResult<void*> *result = new SwiftResult<void*>();
    SwiftError error(SwiftError::SWIFT_HTTP_ERROR, httpResponse->getReason());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }
  //Everything seems fine
  SwiftResult<void*> *result = new SwiftResult<void*>();
  result->setError(SWIFT_OK);
  result->setResponse(httpResponse);
  result->setPayload(nullptr);
  return result;
}

SwiftResult<void*>* Account::swiftShowMetadata(bool _newest) {
  //Swift Endpoint
  Endpoint* swiftEndpoint = this->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));
  if(_newest)
    reqParamMap->push_back(*new HTTPHeader("X-Newest", "True"));

  //Set URI
  URI uri(swiftEndpoint->getPublicUrl());
  //Creating HTTP Session
  HTTPResponse *httpResponse = new HTTPResponse();
  try {
    /** This operation does not accept a request body. **/
    HTTPClientSession *httpSession = doHTTPIO(uri, HTTPRequest::HTTP_HEAD,
        reqParamMap);
    httpSession->receiveResponse(*httpResponse);
  } catch (Exception &e) {
    SwiftResult<void*> *result = new SwiftResult<void*>();
    SwiftError error(SwiftError::SWIFT_EXCEPTION, e.displayText());
    result->setError(error);
    //Try to set HTTP Response as the payload
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }

  /**
   * Check HTTP return code
   * 204:
   *  Success. The response body is empty.
   */
  if (httpResponse->getStatus() != HTTPResponse::HTTP_NO_CONTENT) {
    SwiftResult<void*> *result = new SwiftResult<void*>();
    SwiftError error(SwiftError::SWIFT_HTTP_ERROR, httpResponse->getReason());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }
  //Everything seems fine
  SwiftResult<void*> *result = new SwiftResult<void*>();
  result->setError(SWIFT_OK);
  result->setResponse(httpResponse);
  result->setPayload(nullptr);
  return result;
}

} /* namespace Swift */
