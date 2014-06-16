/*
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Account.h"
#include "Container.h"
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

SwiftResult<istream*>* Account::swiftAccountDetails(HTTPHeader &_formatHeader,
    vector<HTTPHeader> *_reqMap, bool _newest)
{
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
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_OK);
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //add X-Newest
  bool shouldDelete = false;
  if (_newest) {
    if(_reqMap == nullptr) {
      _reqMap = new vector<HTTPHeader>();
      shouldDelete = true;
    }
    _reqMap->push_back(*new HTTPHeader("X-Newest", "True"));
  }

  vector<HTTPHeader> uriParams;
  uriParams.push_back(_formatHeader);

  //Do swift transaction
  string path = "";
  SwiftResult<istream*> *result = doSwiftTransaction<istream*>(this, path,
          HTTPRequest::HTTP_GET, &uriParams, _reqMap, &validHTTPCodes, nullptr, 0,
            nullptr, nullptr);
  if(!shouldDelete)
    return result;
  else {
    delete _reqMap;
    return result;
  }

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
    vector<pair<string, string> >& _metaData, vector<HTTPHeader>* _reqMap) {
  /**
   * 204:
   *  Success. The response body is empty.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //Add Actual metadata
  bool shouldDelete = false;
  if (_metaData.size() > 0) {
    if (_reqMap == nullptr) {
      _reqMap = new vector<HTTPHeader>();
      shouldDelete = true;
    }
    for (uint i = 0; i < _metaData.size(); i++)
      _reqMap->push_back(
          *new HTTPHeader("X-Account-Meta-" + _metaData[i].first,
              _metaData[i].second));
  }

  //Do swift transaction
  string path = "";
  SwiftResult<void*>* result = doSwiftTransaction<void*>(this, path,
      HTTPRequest::HTTP_POST, nullptr, _reqMap, &validHTTPCodes, nullptr, 0,
        nullptr, nullptr);
  if(!shouldDelete)
    return result;
  else {
    delete _reqMap;
    return result;
  }
}

SwiftResult<void*>* Account::swiftUpdateMetadata(
    vector<pair<string, string> >& _metaData, vector<HTTPHeader>* _reqMap) {
  return swiftCreateMetadata(_metaData, _reqMap);
}

SwiftResult<void*>* Account::swiftDeleteMetadata(
    std::vector<string> &_metaDataKeys, vector<HTTPHeader>* _reqMap) {
  /**
   * 204:
   *  Success. The response body is empty.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //Add Actual metadata
  bool shouldDelete = false;
  if (_metaDataKeys.size() > 0) {
    if (_reqMap == nullptr) {
      _reqMap = new vector<HTTPHeader>();
      shouldDelete = true;
    }
    for (uint i = 0; i < _metaDataKeys.size(); i++)
      _reqMap->push_back(*new HTTPHeader("X-Remove-Account-Meta-" + _metaDataKeys[i], "x"));
  }

  //Do swift transaction
  string path = "";
  SwiftResult<void*>* result = doSwiftTransaction<void*>(this, path,
      HTTPRequest::HTTP_POST, nullptr, _reqMap, &validHTTPCodes, nullptr, 0,
        nullptr, nullptr);
  if(!shouldDelete)
      return result;
    else {
      delete _reqMap;
      return result;
    }
}

SwiftResult<vector<Container*>*>* Account::swiftGetContainers(bool _newest) {
  SwiftResult<istream*> *accountDetail = this->swiftAccountDetails(HEADER_FORMAT_APPLICATION_JSON,nullptr,_newest);
  SwiftResult<vector<Container*>*> *result = new SwiftResult<vector<Container*>*>();
  result->setError(accountDetail->getError());
  result->setResponse(accountDetail->getResponse());

  //Check error
  if(accountDetail->getError().code != SWIFT_OK.code) {
    result->setPayload(nullptr);
    return result;
  }

  //Parse JSON
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(*accountDetail->getPayload(), root, false);
  if (!parsingSuccessful) {
    SwiftError error(SwiftError::SWIFT_JSON_PARSE_ERROR,
        reader.getFormattedErrorMessages());
    result->setError(error);
    result->setPayload(nullptr);
    return result;
  }

  //Allocate containers
  vector<Container*>*containers = new vector<Container*>();
  //Successful parse
  for(int i=0;i<root.size();i++) {
    string name = root[i].get("name","").asString();
    Container *container = new Container(this,name);
    containers->push_back(container);
  }

  //Set payload
  result->setPayload(containers);
  return result;
}

SwiftResult<void*>* Account::swiftShowMetadata(bool _newest) {
  /**
   * 204:
   *  Success. The response body is empty.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  vector<HTTPHeader> _reqMap;
  if (_newest)
    _reqMap.push_back(*new HTTPHeader("X-Newest", "True"));

  //Do swift transaction
  string path = "";
  return doSwiftTransaction<void*>(this, path,
      HTTPRequest::HTTP_HEAD, nullptr, &_reqMap, &validHTTPCodes, nullptr, 0,
        nullptr, nullptr);
}

} /* namespace Swift */
