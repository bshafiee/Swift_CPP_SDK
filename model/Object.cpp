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

#include "Object.h"
#include <sstream>
#include "../io/HTTPIO.h"
#include <Poco/MD5Engine.h>

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Swift {

/** Forward Declarations **/
template<class T>
extern SwiftResult<T>* returnNullError(const string &whatsNull);

Object::Object(Container* _container, std::string _name, long _length,
    std::string _content_type, std::string _hash, std::string _last_modified) :
    container(_container), name(_name), length(_length), content_type(
        _content_type), hash(_hash), last_modified(_last_modified) {
}

Object::~Object() {
  // TODO Auto-generated destructor stub
}

SwiftResult<istream*>* Object::swiftGetObjectContent(
    std::vector<HTTPHeader>* _uriParams, std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (container == nullptr)
    return returnNullError<istream*>("container");
  //Path
  string path = container->getName() + "/" + name;
  /**
   * Valid HTTP return codes for this operation: 200
   * Success. The response body shows object content
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_OK);

  //Do swift transaction
  return doSwiftTransaction<istream*>(container->getAccount(), path,
      HTTPRequest::HTTP_GET, _uriParams, _reqMap, &validHTTPCodes, nullptr, 0,
      nullptr);
}

SwiftResult<int*>* Object::swiftCreateReplaceObject(const char* _data,
    ulong _size, bool _calculateETag, std::vector<HTTPHeader>* _uriParams,
    std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (container == nullptr)
    return returnNullError<int*>("container");
  //Path
  string path = container->getName() + "/" + name;
  /**
   * Check HTTP return code
   * 201:
   *  Normal response code
   *
   * Error response codes:
   *  timeout (408),
   *  lengthRequired (411),
   *  unprocessableEntity (422)
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_CREATED);

  //Calculate ETag if requested
  bool shouldDelete = false;
  if (_calculateETag) {
    MD5Engine md5;
    md5.reset();
    md5.update(_data, _size);
    string digestString(DigestEngine::digestToHex(md5.digest()));
    //Set Header
    if (_reqMap == nullptr) {
      _reqMap = new vector<HTTPHeader>();
      shouldDelete = true;
    }
    HTTPHeader etagHeader("ETag", digestString);
    _reqMap->push_back(etagHeader);
  }

  //Do swift transaction
  SwiftResult<int*>* result = doSwiftTransaction<int*>(
      container->getAccount(), path, HTTPRequest::HTTP_PUT, _uriParams, _reqMap,
      &validHTTPCodes, _data, _size, nullptr);
  if (!shouldDelete)
    return result;
  else {
    delete _reqMap;
    return result;
  }
}

SwiftResult<int*>* Object::swiftCopyObject(const std::string& _dstObjectName,
    Container& _dstContainer, std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (container == nullptr)
    return returnNullError<int*>("container");
  //Path
  string path = container->getName() + "/" + name;
  /**
   * Check HTTP return code
   * 201:
   *  Normal response code
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_CREATED);

  //Add Destination token
  bool shouldDelete = false;
  if (_reqMap == nullptr) {
    _reqMap = new vector<HTTPHeader>();
    shouldDelete = true;
  }
  HTTPHeader destHeader("Destination", _dstContainer.getName() + "/" + _dstObjectName);
  _reqMap->push_back(destHeader);

  //Do swift transaction
  SwiftResult<int*>* result = doSwiftTransaction<int*>(
      container->getAccount(), path, "COPY", nullptr, _reqMap, &validHTTPCodes,
      nullptr, 0, nullptr);
  if (!shouldDelete)
    return result;
  else {
    delete _reqMap;
    return result;
  }
}

SwiftResult<std::istream*>* Object::swiftDeleteObject(bool _multipartManifest) {
  //Check Container
  if (container == nullptr)
    return returnNullError<istream*>("container");
  //Path
  string path = container->getName() + "/" + name;
  /**
   * Check HTTP return code
   * 204:
   *  No Content Found.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);
  validHTTPCodes.push_back(HTTPResponse::HTTP_OK);

  //Create appropriate URI
  std::vector<HTTPHeader> _uriParams;
  if (_multipartManifest) {
    _uriParams.push_back(*new HTTPHeader("multipart-manifest", "delete"));
  }

  //Do swift transaction
  return doSwiftTransaction<istream*>(container->getAccount(), path,
      HTTPRequest::HTTP_DELETE, &_uriParams, nullptr, &validHTTPCodes, nullptr,
      0, nullptr);
}

SwiftResult<istream*>* Object::swiftCreateMetadata(
    map<std::string, std::string> &_metaData, std::vector<HTTPHeader>* _reqMap,
    bool _keepExistingMetadata) {

  //Check Container
  if (container == nullptr)
    return returnNullError<istream*>("container");
  //Path
  string path = container->getName() + "/" + name;
  /**
   * Check HTTP return code
   * 202:
   *  Success. HTTP_ACCEPTED
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_ACCEPTED);

  //Keep Existing MetaData
  if (_keepExistingMetadata) {
    std::vector<std::pair<std::string, std::string> > *existingMeta =
        getExistingMetaData();
    if (existingMeta != nullptr && existingMeta->size() > 0)
      for (uint i = 0; i < existingMeta->size(); i++)
        if (_metaData.find(existingMeta->at(i).first) == _metaData.end())
          _metaData.insert(
              map<string, string>::value_type(existingMeta->at(i).first,
                  existingMeta->at(i).second));
    if(existingMeta!= nullptr){
      existingMeta->clear();
      delete existingMeta;
      existingMeta = nullptr;
    }
  }

  //Add Actual metadata
  bool shouldDelete = false;
  if (_metaData.size() > 0) {
    if (_reqMap == nullptr) {
      _reqMap = new vector<HTTPHeader>();
      shouldDelete = true;
    }
    for (map<string, string>::iterator it = _metaData.begin();
        it != _metaData.end(); it++){
      HTTPHeader metaHeader("X-Object-Meta-" + it->first, it->second);
      _reqMap->push_back(metaHeader);
    }
  }

  //Do swift transaction
  SwiftResult<istream*>* result = doSwiftTransaction<istream*>(
      container->getAccount(), path, HTTPRequest::HTTP_POST, nullptr, _reqMap,
      &validHTTPCodes, nullptr, 0, nullptr);
  if (!shouldDelete)
    return result;
  else {
    delete _reqMap;
    return result;
  }
}

SwiftResult<std::istream*>* Object::swiftUpdateMetadata(
    std::map<std::string, std::string> &_metaData,
    std::vector<HTTPHeader>* _reqMap) {
  return swiftCreateMetadata(_metaData, _reqMap);
}

SwiftResult<istream*>* Object::swiftDeleteMetadata(
    std::vector<std::string>& _metaDataKeys) {
  //Existing MetaData
  map<string, string> metaData;
  //Keep Existing MetaData
  std::vector<std::pair<std::string, std::string> > *existingMeta =
      getExistingMetaData();
  if (existingMeta != nullptr && existingMeta->size() > 0)
    for (uint i = 0; i < existingMeta->size(); i++)
      metaData.insert(
          make_pair(existingMeta->at(i).first, existingMeta->at(i).second));

  for (uint i = 0; i < _metaDataKeys.size(); i++) {
    map<string, string>::iterator it = metaData.find(_metaDataKeys[i]);
    if (it != metaData.end())
      metaData.erase(it);
  }

  if(existingMeta!=nullptr){
    delete existingMeta;
    existingMeta = nullptr;
  }

  return swiftCreateMetadata(metaData, nullptr, false);
}

SwiftResult<HTTPClientSession*>* Object::swiftCreateReplaceObject(std::ostream* &outputStream,
    std::vector<HTTPHeader>* _uriParams, std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (container == nullptr)
    return returnNullError<HTTPClientSession*>("container");

  if (container->getAccount() == nullptr)
    return returnNullError<HTTPClientSession*>("account");
  Endpoint* swiftEndpoint =
      container->getAccount()->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<HTTPClientSession*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> reqParamMap;
  //Add authentication token
  string tokenID = container->getAccount()->getToken()->getId();
  HTTPHeader authHeader("X-Auth-Token", tokenID);
  reqParamMap.push_back(authHeader);
  //Push Chuncked Encoding
  HTTPHeader encHeader("Transfer-Encoding", "chunked");
  reqParamMap.push_back(encHeader);
  //Add rest of request Parameters
  if (_reqMap != nullptr && _reqMap->size() > 0) {
    for (uint i = 0; i < _reqMap->size(); i++) {
      reqParamMap.push_back(_reqMap->at(i));
    }
  }

  //Path
  string path = container->getName() + "/" + name;
  URI uri(swiftEndpoint->getPublicUrl());
  string encoded;
  URI::encode(path,"",encoded);
  path = encoded;
  uri.setPath(uri.getPath() + "/" + path);

  if (_uriParams != nullptr && _uriParams->size() > 0) {
    //Create appropriate URI
    ostringstream queryStream;
    for (uint i = 0; i < _uriParams->size(); i++) {
      if (i > 0)
        queryStream << ",";
      queryStream << _uriParams->at(i).getQueryValue();
    }
    uri.setQuery(queryStream.str());
    //cout << uri.toString() << endl;
  }

  //Creating HTTP Session
  HTTPClientSession *httpSession = nullptr;
  try {
    /** This operation does not accept a request body. **/
    httpSession = doHTTPIO(uri, HTTPRequest::HTTP_PUT, &reqParamMap, outputStream);
    //Now we should increase number of calls to SWIFT API
    container->getAccount()->increaseCallCounter();
  } catch (Exception &e) {
    SwiftResult<HTTPClientSession*> *result = new SwiftResult<HTTPClientSession*>();
    SwiftError error(SwiftError::SWIFT_EXCEPTION, e.displayText());
    result->setError(error);
    //Try to set HTTP Response as the payload
    result->setSession(httpSession);
    result->setResponse(nullptr);
    result->setPayload(nullptr);
    return result;
  }

  //Everything seems fine
  SwiftResult<HTTPClientSession*> *result = new SwiftResult<HTTPClientSession*>();
  result->setError(SWIFT_OK);
  result->setSession(httpSession);
  result->setResponse(nullptr);
  result->setPayload(httpSession);
  return result;
}

SwiftResult<int*>* Object::swiftShowMetadata(
    std::vector<HTTPHeader>* _uriParams, bool _newest) {
  //Check Container
  if (container == nullptr)
    return returnNullError<int*>("container");
  //Path
  string path = container->getName() + "/" + name;
  /**
   * Check HTTP return code
   * 200 through 299 indicates success.
   * 204:
   *  Success. The response body is empty.
   *
   HTTP_OK                              = 200,
   HTTP_CREATED                         = 201,
   HTTP_ACCEPTED                        = 202,
   HTTP_NONAUTHORITATIVE                = 203,
   HTTP_NO_CONTENT                      = 204,
   HTTP_RESET_CONTENT                   = 205,
   HTTP_PARTIAL_CONTENT                 = 206,
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_OK);
  validHTTPCodes.push_back(HTTPResponse::HTTP_CREATED);
  validHTTPCodes.push_back(HTTPResponse::HTTP_ACCEPTED);
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);
  validHTTPCodes.push_back(HTTPResponse::HTTP_RESET_CONTENT);
  validHTTPCodes.push_back(HTTPResponse::HTTP_PARTIAL_CONTENT);
  validHTTPCodes.push_back(HTTPResponse::HTTP_NONAUTHORITATIVE);

  vector<HTTPHeader> reqHeaders;
  if (_newest)
    reqHeaders.push_back(*new HTTPHeader("X-Newest", "True"));

  //Do swift transaction
  return doSwiftTransaction<int*>(container->getAccount(), path,
      HTTPRequest::HTTP_HEAD, _uriParams, &reqHeaders, &validHTTPCodes, nullptr,
      0, nullptr);
}

std::vector<std::pair<std::string, std::string> >* Object::getExistingMetaData() {
  SwiftResult<int*>* metadata = this->swiftShowMetadata(nullptr, false);
  if (metadata == nullptr){
    delete metadata;
    metadata = nullptr;
    return nullptr;
  }
  if (metadata->getResponse() == nullptr){
    delete metadata;
    metadata = nullptr;
    return nullptr;
  }

  std::vector<std::pair<std::string, std::string> >* result = new std::vector<
      std::pair<std::string, std::string> >();
  NameValueCollection::ConstIterator it = metadata->getResponse()->begin();

  while (it != metadata->getResponse()->end()) {
    string key = it->first;
    if (it->first.find("X-Object-Meta-") != string::npos) {
      key = it->first.substr(strlen("X-Object-Meta-"));
      result->push_back(make_pair(key, it->second));
    }
    ++it;
  }

  delete metadata;
  metadata = nullptr;

  return result;
}

Container* Object::getContainer() {
  return container;
}

void Object::setContainer(Container* container) {
  this->container = container;
}

std::string Object::getName() {
  return name;
}

void Object::setName(const std::string& name) {
  this->name = name;
}

std::string Object::getContentType() {
  return content_type;
}

void Object::setContentType(const std::string& contentType) {
  content_type = contentType;
}

std::string Object::getHash() {
  return hash;
}

void Object::setHash(const std::string& hash) {
  this->hash = hash;
}

std::string Object::getLastModified() {
  return last_modified;
}

void Object::setLastModified(const std::string& lastModified) {
  last_modified = lastModified;
}

long Object::getLength() {
  return length;
}

void Object::setLength(long length) {
  this->length = length;
}

} /* namespace Swift */
