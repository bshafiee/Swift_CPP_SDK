/*
 * Container.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Container.h"
#include <sstream>      // ostringstream
#include "../io/HTTPIO.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Swift {

/** Forward Declarations **/
template<class T>
extern SwiftResult<T>* returnNullError(const string &whatsNull);

Container::Container(Account* _account) :
    account(_account) {
  // TODO Auto-generated constructor stub

}

Container::~Container() {
  // TODO Auto-generated destructor stub
}

SwiftResult<std::istream*>* Container::swiftGetObjects(
    const std::string &_containerName, HTTPHeader& _formatHeader,
    bool _newest) {
  vector<HTTPHeader> _queryMap;
  return swiftGetObjects(_containerName, _formatHeader, _queryMap, _newest);
}

SwiftResult<std::istream*>* Container::swiftGetObjects(
    const std::string &_containerName, HTTPHeader& _formatHeader,
    std::vector<HTTPHeader>& _queryMap, bool _newest) {
  //Swift Endpoint
  if (account == nullptr)
    return returnNullError<istream*>("account");
  Endpoint* swiftEndpoint =
      this->account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<istream*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
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

  URI uri(swiftEndpoint->getPublicUrl() + "/" + _containerName);
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

SwiftResult<void*>* Container::swiftCreateContainer(
    const std::string& _containerName) {
  vector<HTTPHeader> _queryMap;
  return swiftCreateContainer(_containerName, _queryMap);
}

SwiftResult<void*>* Container::swiftCreateContainer(
    const std::string& _containerName, std::vector<HTTPHeader>& _queryMap) {
  //Swift Endpoint
  if (account == nullptr)
    return returnNullError<void*>("account");
  Endpoint* swiftEndpoint =
      this->account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));
  //Create appropriate URI
  ostringstream queryStream;
  if (_queryMap.size() > 0) {
    queryStream << "?";
    for (uint i = 0; i < _queryMap.size(); i++) {
      if (i > 0)
        queryStream << ",";
      queryStream << _queryMap[i].getQueryValue();
    }
  }

  URI uri(swiftEndpoint->getPublicUrl() + "/" + _containerName);
  uri.setQuery(queryStream.str());

  HTTPClientSession *httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();
  try {
    /** This operation does not accept a request body. **/
    httpSession = doHTTPIO(uri, HTTPRequest::HTTP_PUT, reqParamMap);
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
   *  Success. container was created successfully.
   *
   * 204:
   *   Success??
   */
  if (httpResponse->getStatus() != HTTPResponse::HTTP_CREATED
      && httpResponse->getStatus() != HTTPResponse::HTTP_NO_CONTENT) {
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

SwiftResult<void*>* Container::swiftDeleteContainer(
    const std::string& _containerName) {
  //Swift Endpoint
  if (account == nullptr)
    return returnNullError<void*>("account");
  Endpoint* swiftEndpoint =
      this->account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));

  //Set URI
  URI uri(swiftEndpoint->getPublicUrl() + "/" + _containerName);

  HTTPClientSession *httpSession = nullptr;
  HTTPResponse *httpResponse = new HTTPResponse();
  try {
    /** This operation does not accept a request body. **/
    httpSession = doHTTPIO(uri, HTTPRequest::HTTP_DELETE, reqParamMap);
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
   * 204:
   *  Success. container was deleted successfully.
   *
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

SwiftResult<void*>* Container::swiftCreateMetadata(const std::string &_containerName,
    std::vector<std::pair<std::string, std::string> >& _metaData,
    std::vector<HTTPHeader>& _reqMap) {
  //Swift Endpoint
  Endpoint* swiftEndpoint = account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
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
          *new HTTPHeader("X-Container-Meta-" + _metaData[i].first, _metaData[i].second));

  //Set URI
  URI uri(swiftEndpoint->getPublicUrl() + "/" + _containerName);
  //Creating HTTP Session
  HTTPResponse *httpResponse = new HTTPResponse();
  try {
    /** This operation does not accept a request body. **/
    HTTPClientSession *httpSession = doHTTPIO(uri, HTTPRequest::HTTP_POST,reqParamMap);
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

SwiftResult<void*>* Container::swiftCreateMetadata(const std::string &_containerName,
    std::vector<std::pair<std::string, std::string> >& _metaData) {
  std::vector<HTTPHeader> _reqMap;
  return swiftCreateMetadata(_containerName,_metaData,_reqMap);
}

SwiftResult<void*>* Container::swiftUpdateMetadata(const std::string &_containerName,
    std::vector<std::pair<std::string, std::string> >& _metaData) {
  std::vector<HTTPHeader> _reqMap;
  return swiftUpdateMetadata(_containerName,_metaData,_reqMap);
}

SwiftResult<void*>* Container::swiftUpdateMetadata(const std::string &_containerName,
    std::vector<std::pair<std::string, std::string> >& _metaData,
    std::vector<HTTPHeader>& _reqMap) {
  return swiftCreateMetadata(_containerName,_metaData,_reqMap);
}

SwiftResult<void*>* Container::swiftDeleteMetadata(const std::string &_containerName,
    std::vector<std::string>& _metaDataKeys) {
  std::vector<HTTPHeader> _reqMap;
  return swiftDeleteMetadata(_containerName,_metaDataKeys,_reqMap);
}

SwiftResult<void*>* Container::swiftDeleteMetadata(const std::string &_containerName,
    std::vector<std::string>& _metaDataKeys, std::vector<HTTPHeader>& _reqMap) {
  //Swift Endpoint
  Endpoint* swiftEndpoint = account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
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
          *new HTTPHeader("X-Remove-Container-Meta-" + _metaDataKeys[i], "x"));

  //Set URI
  URI uri(swiftEndpoint->getPublicUrl() + "/" + _containerName);
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

SwiftResult<void*>* Container::swiftShowMetadata(
    const std::string& _containerName, bool _newest) {
  //Swift Endpoint
  Endpoint* swiftEndpoint = account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<void*>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> *reqParamMap = new vector<HTTPHeader>();
  //Add authentication token
  string tokenID = account->getToken()->getId();
  reqParamMap->push_back(*new HTTPHeader("X-Auth-Token", tokenID));
  if(_newest)
    reqParamMap->push_back(*new HTTPHeader("X-Newest", "True"));

  //Set URI
  URI uri(swiftEndpoint->getPublicUrl() + "/" + _containerName);
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
