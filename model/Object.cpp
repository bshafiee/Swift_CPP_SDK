/*
 * Object.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Object.h"
#include <sstream>      // ostringstream
#include "../io/HTTPIO.h"
#include <Poco/MD5Engine.h>

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Swift {

/** Forward Declarations **/
template<class T>
extern SwiftResult<T>* returnNullError(const string &whatsNull);

Object::Object(Container* _container) :
    container(_container) {
  // TODO Auto-generated constructor stub

}

Object::~Object() {
  // TODO Auto-generated destructor stub
}

SwiftResult<istream*>* Object::swiftGetObjectContent(
    const std::string &_objectName, std::vector<HTTPHeader>* _uriParams,
    std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (container == nullptr)
    return returnNullError<istream*>("container");
  //Path
  string path = container->getName() + "/" + _objectName;
  /**
   * Valid HTTP return codes for this operation: 200
   * Success. The response body shows object content
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_OK);

  //Do swift transaction
  return doSwiftTransaction<istream*>(container->getAccount(), path,
      HTTPRequest::HTTP_GET, _uriParams, _reqMap, &validHTTPCodes, nullptr, 0,
      nullptr, nullptr);
}

SwiftResult<void*>* Object::swiftCreateReplaceObject(
    const std::string& _objectName, const char* _data, ulong _size,
    bool _calculateETag, std::vector<HTTPHeader>* _uriParams,
    std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (container == nullptr)
    return returnNullError<void*>("container");
  //Path
  string path = container->getName() + "/" + _objectName;
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
  if (_calculateETag) {
    MD5Engine md5;
    md5.reset();
    md5.update(_data, _size);
    string digestString(DigestEngine::digestToHex(md5.digest()));
    //Set Header
    if (_reqMap == nullptr)
      _reqMap = new vector<HTTPHeader>();
    _reqMap->push_back(*new HTTPHeader("ETag", digestString));
    cout << "inja:\t" << digestString << endl;
  }

  //Do swift transaction
  return doSwiftTransaction<void*>(container->getAccount(), path,
      HTTPRequest::HTTP_PUT, _uriParams, _reqMap, &validHTTPCodes, _data, _size,
      nullptr, nullptr);
}

SwiftResult<void*>* Object::swiftCopyObject(const std::string& _srcObjectName,
    const std::string& _dstObjectName, Container& _dstContainer,
    std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (container == nullptr)
    return returnNullError<void*>("container");
  //Path
  string path = container->getName() + "/" + _srcObjectName;
  /**
   * Check HTTP return code
   * 201:
   *  Normal response code
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_OK);

  //Add Destination token
  if (_reqMap == nullptr)
    _reqMap = new vector<HTTPHeader>();
  _reqMap->push_back(
      *new HTTPHeader("Destination",
          _dstContainer.getName() + "/" + _dstObjectName));

  //Do swift transaction
  return doSwiftTransaction<void*>(container->getAccount(), path, "COPY",
      nullptr, _reqMap, &validHTTPCodes, nullptr, 0, nullptr, nullptr);
}

SwiftResult<std::istream*>* Object::swiftDeleteObject(
    const std::string& _objectName, bool _multipartManifest) {
  //Check Container
  if (container == nullptr)
    return returnNullError<istream*>("container");
  //Path
  string path = container->getName() + "/" + _objectName;
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
      0, nullptr, nullptr);
}

SwiftResult<istream*>* Object::swiftCreateMetadata(
    const std::string& _objectName, map<std::string, std::string> &_metaData,
    std::vector<HTTPHeader>* _reqMap, bool _keepExistingMetadata) {

  //Check Container
  if (container == nullptr)
    return returnNullError<istream*>("container");
  //Path
  string path = container->getName() + "/" + _objectName;
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
        getExistingMetaData(_objectName);
    if (existingMeta != nullptr && existingMeta->size() > 0)
      for (uint i = 0; i < existingMeta->size(); i++)
        if (_metaData.find(existingMeta->at(i).first) == _metaData.end())
          _metaData.insert(
              map<string, string>::value_type(existingMeta->at(i).first,
                  existingMeta->at(i).second));
  }

  //Add Actual metadata
  if (_metaData.size() > 0) {
    if (_reqMap == nullptr)
      _reqMap = new vector<HTTPHeader>();
    for (map<string, string>::iterator it = _metaData.begin();
        it != _metaData.end(); it++)
      _reqMap->push_back(
          *new HTTPHeader("X-Object-Meta-" + it->first, it->second));
  }

  //Do swift transaction
  return doSwiftTransaction<istream*>(container->getAccount(), path,
      HTTPRequest::HTTP_POST, nullptr, _reqMap, &validHTTPCodes, nullptr, 0,
      nullptr, nullptr);
}

SwiftResult<std::istream*>* Object::swiftUpdateMetadata(
    const std::string& _objectName,
    std::map<std::string, std::string> &_metaData,
    std::vector<HTTPHeader>* _reqMap) {
  return swiftCreateMetadata(_objectName, _metaData, _reqMap);
}

SwiftResult<istream*>* Object::swiftDeleteMetadata(
    const std::string& _objectName, std::vector<std::string>& _metaDataKeys) {
  //Existing MetaData
  map<string, string> metaData;
  //Keep Existing MetaData
  std::vector<std::pair<std::string, std::string> > *existingMeta =
      getExistingMetaData(_objectName);
  if (existingMeta != nullptr && existingMeta->size() > 0)
    for (uint i = 0; i < existingMeta->size(); i++)
      metaData.insert(
          make_pair(existingMeta->at(i).first, existingMeta->at(i).second));

  for (uint i = 0; i < _metaDataKeys.size(); i++) {
    map<string, string>::iterator it = metaData.find(_metaDataKeys[i]);
    if (it != metaData.end())
      metaData.erase(it);
  }

  return swiftCreateMetadata(_objectName, metaData, nullptr, false);
}

SwiftResult<void*>* Object::swiftCreateReplaceObject(
    const std::string& _objectName, std::istream &inputStream,
    std::vector<HTTPHeader>* _uriParams, std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (container == nullptr)
    return returnNullError<void*>("container");
  //Path
  string path = container->getName() + "/" + _objectName;
  /**
   * Check HTTP return code
   * 201:
   *  Created
   *
   *  Error response codes:
   *  timeout (408),
   *  lengthRequired (411),
   *  unprocessableEntity (422)
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_CREATED);

  //Set Transfer-Encoding: chunked
  if (_reqMap == nullptr)
    _reqMap = new vector<HTTPHeader>();
  _reqMap->push_back(*new HTTPHeader("Transfer-Encoding", "chunked"));

  //Do swift transaction
  return doSwiftTransaction<void*>(container->getAccount(), path,
      HTTPRequest::HTTP_PUT, _uriParams, _reqMap, &validHTTPCodes, nullptr, 0,
      nullptr, &inputStream);
}

SwiftResult<void*>* Object::swiftShowMetadata(const std::string& _objectName,
    std::vector<HTTPHeader>* _uriParams, bool _newest) {
  //Check Container
  if (container == nullptr)
    return returnNullError<void*>("container");
  //Path
  string path = container->getName() + "/" + _objectName;
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
  return doSwiftTransaction<void*>(container->getAccount(), path,
      HTTPRequest::HTTP_HEAD, _uriParams, &reqHeaders, &validHTTPCodes, nullptr,
      0, nullptr, nullptr);
}

std::vector<std::pair<std::string, std::string> >* Object::getExistingMetaData(
    const std::string& _objectName) {
  SwiftResult<void*>* metadata = this->swiftShowMetadata(_objectName, nullptr,
      false);
  if (metadata == nullptr)
    return nullptr;
  if (metadata->getResponse() == nullptr)
    return nullptr;
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

  return result;
}

} /* namespace Swift */
