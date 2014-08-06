/*
 * Container.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Container.h"
#include <sstream>      // ostringstream
#include "../io/HTTPIO.h"
#include "Object.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Swift {

Container::Container(Account* _account, std::string _name) :
    account(_account), name(_name) {
  // TODO Auto-generated destructor stub
}

Container::~Container() {
  // TODO Auto-generated destructor stub
}

SwiftResult<std::istream*>* Container::swiftListObjects(
    HTTPHeader& _formatHeader, std::vector<HTTPHeader>* _uriParam,
    bool _newest) {
  //Check Container
  if (account == nullptr)
    return returnNullError<istream*>("account");
  //Path
  string path = this->name;
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
  std::vector<HTTPHeader> _reqMap;
  if (_newest){
    HTTPHeader header("X-Newest", "True");
    _reqMap.push_back(header);
  }
  //Push Header Format
  bool shouldDelete = false;
  if (_uriParam == nullptr) {
    _uriParam = new vector<HTTPHeader>();
    shouldDelete = true;
  }
  _uriParam->push_back(_formatHeader);

  //Do swift transaction
  SwiftResult<istream*> *result = doSwiftTransaction<istream*>(this->account, path,
      HTTPRequest::HTTP_GET, _uriParam, &_reqMap, &validHTTPCodes, nullptr, 0,
      nullptr);
  if(!shouldDelete)
    return result;
  else {
    delete _uriParam;
    return result;
  }
}

SwiftResult<void*>* Container::swiftCreateContainer(
    std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   * 201:
   *  Success. container was created successfully.
   *
   * 204:
   *   Success??
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_CREATED);
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //Do swift transaction
  return doSwiftTransaction<void*>(this->account, path, HTTPRequest::HTTP_PUT,
      nullptr, _reqMap, &validHTTPCodes, nullptr, 0, nullptr);
}

SwiftResult<void*>* Container::swiftDeleteContainer() {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   *
   * 204:
   *  Success. container was deleted successfully.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //Do swift transaction
  return doSwiftTransaction<void*>(this->account, path,
      HTTPRequest::HTTP_DELETE, nullptr, nullptr, &validHTTPCodes, nullptr, 0,
      nullptr);
}

SwiftResult<void*>* Container::swiftCreateMetadata(
    std::vector<std::pair<std::string, std::string> >& _metaData,
    std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   *
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
    for (uint i = 0; i < _metaData.size(); i++) {
      HTTPHeader header("X-Container-Meta-" + _metaData[i].first,
          _metaData[i].second);
      _reqMap->push_back(header);
    }
  }

  //Do swift transaction
  SwiftResult<void*>* result = doSwiftTransaction<void*>(this->account, path, HTTPRequest::HTTP_POST,
      nullptr, _reqMap, &validHTTPCodes, nullptr, 0, nullptr);
  if(!shouldDelete)
      return result;
    else {
      delete _reqMap;
      return result;
    }
}

SwiftResult<void*>* Container::swiftUpdateMetadata(
    std::vector<std::pair<std::string, std::string> >& _metaData,
    std::vector<HTTPHeader>* _reqMap) {
  return swiftCreateMetadata(_metaData, _reqMap);
}

SwiftResult<void*>* Container::swiftDeleteMetadata(
    std::vector<std::string>& _metaDataKeys, std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   *
   * 204:
   *  Success. Metadata was deleted successfully.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //Add metadata keys to be deleted
  bool shouldDelete = false;
  if (_metaDataKeys.size() > 0) {
    if (_reqMap == nullptr) {
      _reqMap = new vector<HTTPHeader>();
      shouldDelete = true;
    }
    for (uint i = 0; i < _metaDataKeys.size(); i++) {
      HTTPHeader header("X-Remove-Container-Meta-" + _metaDataKeys[i], "x");
      _reqMap->push_back(header);
    }
  }

  //Do swift transaction
  SwiftResult<void*>* result = doSwiftTransaction<void*>(this->account, path, HTTPRequest::HTTP_POST,
      nullptr, _reqMap, &validHTTPCodes, nullptr, 0, nullptr);
  if(!shouldDelete)
      return result;
    else {
      delete _reqMap;
      return result;
    }
}

std::string& Container::getName() {
  return name;
}

SwiftResult<vector<Object>*>* Container::swiftGetObjects(bool _newest) {
  SwiftResult<istream*> *objectList = this->swiftListObjects(HEADER_FORMAT_APPLICATION_JSON,nullptr,_newest);
  SwiftResult<vector<Object>*> *result = new SwiftResult<vector<Object>*>();
  result->setError(objectList->getError());
  result->setResponse(objectList->getResponse());
  result->setSession(objectList->getSession());

  //Check error
  if(objectList->getError().code != SWIFT_OK.code) {
    result->setPayload(nullptr);
    return result;
  }

  //Parse JSON
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(*objectList->getPayload(), root, false);
  if (!parsingSuccessful) {
    SwiftError error(SwiftError::SWIFT_JSON_PARSE_ERROR,
        reader.getFormattedErrorMessages());
    result->setError(error);
    result->setPayload(nullptr);
    return result;
  }

  //Allocate containers
  vector<Object>*objects = new vector<Object>();
  //Successful parse
  for(int i=0;i<root.size();i++) {
    string name = root[i].get("name","").asString();
    long length = root[i].get("bytes",-1).asInt64();
    string content_type = root[i].get("content_type","").asString();
    string hash = root[i].get("hash","").asString();
    string last_modified = root[i].get("last_modified","").asString();
    //Create new object
    Object object(this,name,length,content_type,hash,last_modified);
    objects->push_back(object);
  }

  //Set payload
  result->setPayload(objects);
  return result;
}

void Container::setName(const std::string& name) {
  this->name = name;
}

Account* Container::getAccount() {
  return account;
}

SwiftResult<void*>* Container::swiftShowMetadata(bool _newest) {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   *
   * 204:
   *  Success. Response body is empty.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  vector<HTTPHeader> _reqMap;
  if (_newest) {
    _reqMap.push_back(*new HTTPHeader("X-Newest", "True"));
  }

  //Do swift transaction
  return doSwiftTransaction<void*>(this->account, path, HTTPRequest::HTTP_HEAD,
      nullptr, &_reqMap, &validHTTPCodes, nullptr, 0, nullptr);
}

} /* namespace Swift */
