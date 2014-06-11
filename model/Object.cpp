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

SwiftResult<istream*>* Object::swiftCreateMetadata(
    const std::string& _objectName,
    map<std::string,std::string> &_metaData,
    std::vector<HTTPHeader>* _reqMap,bool _keepExistingMetadata) {

  //Keep Existing MetaData
  if(_keepExistingMetadata) {
    std::vector<std::pair<std::string, std::string> > *existingMeta = getExistingMetaData(_objectName);
    if(existingMeta != nullptr && existingMeta->size() > 0)
      for(uint i =0; i<existingMeta->size();i++)
        if(_metaData.find(existingMeta->at(i).first) == _metaData.end())
          _metaData.insert(map<string,string>::value_type(existingMeta->at(i).first,existingMeta->at(i).second));
  }

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
  //Add rest of request Parameters
  if (_reqMap!=nullptr&& _reqMap->size() > 0) {
    for (uint i = 0; i < _reqMap->size(); i++) {
      reqParamMap->push_back(_reqMap->at(i));
    }
  }

  //Add Actual metadata
  if (_metaData.size() > 0)
    for (map<string,string>::iterator it = _metaData.begin(); it != _metaData.end(); it++)
      reqParamMap->push_back(
          *new HTTPHeader("X-Object-Meta-" + it->first, it->second));

  //Set URI
  URI uri(
        swiftEndpoint->getPublicUrl() + "/" + container->getName() + "/"
            + _objectName);
  //Creating HTTP Session
  HTTPResponse *httpResponse = new HTTPResponse();
  istream* resultStream = nullptr;
  try {
    /** This operation does not accept a request body. **/
    HTTPClientSession *httpSession = doHTTPIO(uri, HTTPRequest::HTTP_POST,reqParamMap);
    resultStream = &httpSession->receiveResponse(*httpResponse);
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
   * 202:
   *  Success. HTTP_ACCEPTED
   */
  if (httpResponse->getStatus() != HTTPResponse::HTTP_NO_CONTENT) {
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
  result->setPayload(resultStream);
  return result;
}

SwiftResult<std::istream*>* Object::swiftUpdateMetadata(
    const std::string& _objectName,
    std::map<std::string,std::string> &_metaData,
    std::vector<HTTPHeader>* _reqMap) {
  return swiftCreateMetadata(_objectName,_metaData,_reqMap);
}

SwiftResult<istream*>* Object::swiftDeleteMetadata(const std::string& _objectName,
    std::vector<std::string>& _metaDataKeys) {
  //Existing MetaData
  map<string, string> metaData;
  //Keep Existing MetaData
  std::vector<std::pair<std::string, std::string> > *existingMeta = getExistingMetaData(_objectName);
  if(existingMeta != nullptr && existingMeta->size() > 0)
    for(uint i =0; i<existingMeta->size();i++)
      metaData.insert(make_pair(existingMeta->at(i).first,existingMeta->at(i).second));

  for(uint i =0; i<_metaDataKeys.size();i++) {
    map<string, string>::iterator it = metaData.find(_metaDataKeys[i]);
    if(it != metaData.end())
      metaData.erase(it);
  }

  return swiftCreateMetadata(_objectName,metaData,nullptr,false);
}

SwiftResult<void*>* Object::swiftCreateReplaceObject(
    const std::string& _objectName, std::istream &inputStream, std::vector<HTTPHeader>* _uriParams,
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

  //Set Transfer-Encoding: chunked
  reqParamMap->push_back(*new HTTPHeader("Transfer-Encoding", "chunked"));

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
    httpSession = doHTTPIO(uri, HTTPRequest::HTTP_PUT, reqParamMap, inputStream);
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

SwiftResult<void*>* Object::swiftShowMetadata(const std::string& _objectName,
    std::vector<HTTPHeader>* _uriParams, bool _newest) {
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
  if (_newest)
    reqParamMap->push_back(*new HTTPHeader("X-Newest", "True"));

  //Set URI
  URI uri(
        swiftEndpoint->getPublicUrl() + "/" + container->getName() + "/"
            + _objectName);
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
   * 200 through 299 indicates success.
   * 204:
   *  Success. The response body is empty.
   */
  if (httpResponse->getStatus() < 200 || httpResponse->getStatus() > 209) {
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

std::vector<std::pair<std::string, std::string> >* Object::getExistingMetaData(const std::string& _objectName) {
  SwiftResult<void*>* metadata = this->swiftShowMetadata(_objectName,nullptr, false);
  if(metadata == nullptr)
    return nullptr;
  if(metadata->getResponse() == nullptr)
      return nullptr;
  std::vector<std::pair<std::string, std::string> >* result = new std::vector<std::pair<std::string, std::string> >();
  NameValueCollection::ConstIterator it = metadata->getResponse()->begin();

  while(it != metadata->getResponse()->end()) {
    string key = it->first;
    if(it->first.find("X-Object-Meta-") != string::npos)
    {
      key = it->first.substr(strlen("X-Object-Meta-"));
      result->push_back(make_pair(key,it->second));
    }
    ++it;
  }

  return result;
}

} /* namespace Swift */
