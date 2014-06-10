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
  //Swift Endpoint
  if (container == nullptr)
    return returnNullError<istream*>("container");
  Account* account = container->getAccount();
  if (account == nullptr)
    return returnNullError<istream*>("account");
  Endpoint* swiftEndpoint = account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<istream*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));
  //add request params
  if (_reqMap != nullptr)
    for (uint i = 0; i < _reqMap->size(); i++)
      reqParamMap->push_back(_reqMap->at(i));
  //Create appropriate URI
  ostringstream queryStream;
  queryStream << "?";
  if (_uriParams != nullptr && _uriParams->size() > 0) {
    for (uint i = 0; i < _uriParams->size(); i++) {
      if (i > 0)
        queryStream << ",";
      queryStream << _uriParams->at(i).getQueryValue();
    }
  }

  URI uri(
      swiftEndpoint->getPublicUrl() + "/" + container->getName() + "/"
          + _objectName);
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
   *  Success. The response body shows object content
   */
  if (httpResponse->getStatus() != HTTPResponse::HTTP_OK) {
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

SwiftResult<void*>* Object::swiftCreateReplaceObject(
    const std::string& _objectName, const char* _data, ulong _size,
    bool _calculateETag, std::vector<HTTPHeader>* _uriParams,
    std::vector<HTTPHeader>* _reqMap) {
  //Swift Endpoint
  if (container == nullptr)
    return returnNullError<void*>("container");
  Account* account = container->getAccount();
  if (account == nullptr)
    return returnNullError<void*>("account");
  Endpoint* swiftEndpoint = account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));

  //Calculate ETag if requested
  if (_calculateETag) {
    MD5Engine md5;
    md5.reset();
    md5.update(_data, _size);
    string digestString(DigestEngine::digestToHex(md5.digest()));
    //Set Header
    reqParamMap->push_back(*new HTTPHeader("ETag", digestString));
    cout << "inja:\t" << digestString << endl;
  }

  //add request params
  if (_reqMap != nullptr)
    for (uint i = 0; i < _reqMap->size(); i++)
      reqParamMap->push_back(_reqMap->at(i));
  //Create appropriate URI
  ostringstream queryStream;
  queryStream << "?";
  if (_uriParams != nullptr && _uriParams->size() > 0) {
    for (uint i = 0; i < _uriParams->size(); i++) {
      if (i > 0)
        queryStream << ",";
      queryStream << _uriParams->at(i).getQueryValue();
    }
  }

  URI uri(
      swiftEndpoint->getPublicUrl() + "/" + container->getName() + "/"
          + _objectName);
  uri.setQuery(queryStream.str());

  HTTPClientSession *httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();
  try {
    /** This operation does not accept a request body. **/
    httpSession = doHTTPIO(uri, HTTPRequest::HTTP_PUT, reqParamMap, _data,
        _size);
    //Receive Response
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
   * 201:
   *  Normal response code
   *
   * Error response codes:
   *  timeout (408),
   *  lengthRequired (411),
   *  unprocessableEntity (422)
   */
  if (httpResponse->getStatus() != HTTPResponse::HTTP_OK) {
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

SwiftResult<void*>* Object::swiftCopyObject(const std::string& _srcObjectName,
    const std::string& _dstObjectName, Container& _dstContainer,
    std::vector<HTTPHeader>* _reqMap) {
  //Swift Endpoint
  if (container == nullptr)
    return returnNullError<void*>("container");
  Account* account = container->getAccount();
  if (account == nullptr)
    return returnNullError<void*>("account");
  Endpoint* swiftEndpoint = account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));

  //Add Destination token
  reqParamMap->push_back(
      *new HTTPHeader("Destination",
          _dstContainer.getName() + "/" + _dstObjectName));

  //add request params
  if (_reqMap != nullptr)
    for (uint i = 0; i < _reqMap->size(); i++)
      reqParamMap->push_back(_reqMap->at(i));

  URI uri(
      swiftEndpoint->getPublicUrl() + "/" + container->getName() + "/"
          + _srcObjectName);

  HTTPClientSession *httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();
  try {
    /** This operation does not accept a request body. **/
    httpSession = doHTTPIO(uri, "COPY", reqParamMap);
    //Receive Response
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
   * 201:
   *  Normal response code
   *
   */
  if (httpResponse->getStatus() != HTTPResponse::HTTP_OK) {
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

SwiftResult<std::istream*>* Object::swiftDeleteObject(
    const std::string& _objectName, bool _multipartManifest) {
  //Swift Endpoint
  if (container == nullptr)
    return returnNullError<istream*>("container");
  Account* account = container->getAccount();
  if (account == nullptr)
    return returnNullError<istream*>("account");
  Endpoint* swiftEndpoint = account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<istream*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));

  //Create appropriate URI
  ostringstream queryStream;
  queryStream << "?";
  if (_multipartManifest)
    queryStream << "multipart-manifest=delete";

  URI uri(
      swiftEndpoint->getPublicUrl() + "/" + container->getName() + "/"
          + _objectName);
  uri.setQuery(queryStream.str());

  HTTPClientSession *httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();
  istream* streamResult = nullptr;
  try {
    /** This operation does not accept a request body. **/
    httpSession = doHTTPIO(uri, HTTPRequest::HTTP_DELETE, reqParamMap);
    //Receive Response
    streamResult = &httpSession->receiveResponse(*httpResponse);
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
   * 204:
   *  no Content found.
   */
  if (httpResponse->getStatus() != HTTPResponse::HTTP_OK) {
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
  result->setPayload(streamResult);
  return result;
}

} /* namespace Swift */
