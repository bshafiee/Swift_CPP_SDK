/*
 * HTTPIO.cpp
 *
 *  Created on: 2014-05-27
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "HTTPIO.h"
#include <sstream>      // ostringstream
#include <mutex>

namespace Swift {

using namespace std;
using namespace Poco::Net;
using namespace Poco;

static HTTPClientSession *httpSession = nullptr;
static istream* resultStream = nullptr;
mutex transactionMutex;

Poco::Net::HTTPClientSession* doHTTPIO(const Poco::URI& uri,
    const std::string& type, std::vector<HTTPHeader>* params) {
  Poco::Net::HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  Poco::Net::HTTPRequest request(type, uri.getPathAndQuery());

  //Add params
  if (params != nullptr && params->size() > 0) {
    vector<HTTPHeader>::iterator it = params->begin();
    while (it != params->end()) {
      request.add(it->getKey(), it->getValue());
      it++;
    }
  }
  //cout<<endl<<"Request:"<<endl;request.write(cout);cout<<endl;
  session->sendRequest(request);
  return session;
}

Poco::Net::HTTPClientSession* doHTTPIO(const Poco::URI& uri,
    const std::string& type, std::vector<HTTPHeader>* params,
    const std::string& reqBody, const std::string& contentType) {
  HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  HTTPRequest request(type, uri.getPath());
  //Set Content Type
  request.setContentLength(reqBody.size());
  request.setContentType(contentType);

  //Add params
  if (params != nullptr && params->size() > 0) {
    vector<HTTPHeader>::iterator it = params->begin();
    while (it != params->end()) {
      request.add(it->getKey(), it->getValue());
      it++;
    }
  }

  //write request body
  ostream &ostream = session->sendRequest(request);
  if (ostream == nullptr)
    return nullptr;
  ostream << reqBody;
  return session;
}

Poco::Net::HTTPClientSession* doHTTPIO(const Poco::URI& uri,
    const std::string& type, std::vector<HTTPHeader>* params,
    const char* reqBody, ulong size, const std::string& contentType) {
  HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  HTTPRequest request(type, uri.getPath());
  //Set Content size
  request.setContentLength(size);
  //Content Type
  if(contentType.length() != 0)
    request.setContentType(contentType);

  //Add params
  if (params != nullptr && params->size() > 0) {
    vector<HTTPHeader>::iterator it = params->begin();
    while (it != params->end()) {
      request.add(it->getKey(), it->getValue());
      it++;
    }
  }

  //write request body
  ostream &ostream = session->sendRequest(request);
  if (ostream == nullptr)
    return nullptr;
  ostream << reqBody;
  return session;
}

Poco::Net::HTTPClientSession* doHTTPIO(const Poco::URI& uri,
    const std::string& type, std::vector<HTTPHeader>* params,
    std::ostream* &outputStream) {
  HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  HTTPRequest request(type, uri.getPath());

  //Add params
  if (params != nullptr && params->size() > 0) {
    vector<HTTPHeader>::iterator it = params->begin();
    while (it != params->end()) {
      request.add(it->getKey(), it->getValue());
      it++;
    }
  }

  //Ouput stream
  outputStream = &session->sendRequest(request);
  //Copy input stream to the output stream
  //StreamCopier::copyStream(inputStream, ostream);
  //ostream << inputStream;

  return session;
}

/** Template instantiation for common used types **/
template SwiftResult<void*>* returnNullError<void*>(const string &whatsNull);
template SwiftResult<istream*>* returnNullError<istream*>(const string &whatsNull);
template SwiftResult<HTTPClientSession*>* returnNullError<HTTPClientSession*>(const string &whatsNull);

template<class T>
inline SwiftResult<T>* returnNullError(const string &whatsNull) {
  SwiftResult<T> *result = new SwiftResult<T>();
  SwiftError error(SwiftError::SWIFT_FAIL, whatsNull + " is NULL");
  result->setError(error);
  result->setResponse(nullptr);
  result->setPayload(nullptr);
  return result;
}

/** Template instantiation for common used types **/
template
SwiftResult<istream*>* doSwiftTransaction<istream*>(Account *_account,
    std::string &_uriPath, const std::string &_method,
    std::vector<HTTPHeader>* _uriParams, std::vector<HTTPHeader>* _reqMap,
    std::vector<int> *_httpValidCodes, const char *bodyReqBuffer, ulong size,
    std::string *contentType);

template
SwiftResult<void*>* doSwiftTransaction<void*>(Account *_account,
    std::string &_uriPath, const std::string &_method,
    std::vector<HTTPHeader>* _uriParams, std::vector<HTTPHeader>* _reqMap,
    std::vector<int> *_httpValidCodes, const char *bodyReqBuffer, ulong size,
    std::string *contentType);

template<class T>
SwiftResult<T>* doSwiftTransaction(Account *_account, std::string &_uriPath,
    const std::string &_method, std::vector<HTTPHeader>* _uriParams,
    std::vector<HTTPHeader>* _reqMap, std::vector<int> *_httpValidCodes,
    const char *bodyReqBuffer, ulong size, std::string *contentType) {
  //Start locking
  lock_guard<mutex> guard(transactionMutex);
  //Start of function
  if (_account == nullptr)
    return returnNullError<T>("account");
  Endpoint* swiftEndpoint = _account->getSwiftService()->getFirstEndpoint();
  if (swiftEndpoint == nullptr)
    return returnNullError<T>("SWIFT Endpoint");

  //Create parameter map
  vector<HTTPHeader> reqParamMap;
  //Add authentication token
  string tokenID = _account->getToken()->getId();
  HTTPHeader authHeader("X-Auth-Token", tokenID);
  reqParamMap.push_back(authHeader);
  //Add rest of request Parameters
  if (_reqMap != nullptr && _reqMap->size() > 0) {
    for (uint i = 0; i < _reqMap->size(); i++) {
      reqParamMap.push_back(_reqMap->at(i));
      //cout<<_reqMap->at(i).getQueryValue()<<endl;
    }
  }

  URI uri(swiftEndpoint->getPublicUrl());
  if (uri.getPath().size() > 0)
    uri.setPath(uri.getPath() + "/" + _uriPath);
  else
    uri.setPath(_uriPath);

  if (_uriParams != nullptr && _uriParams->size() > 0) {
    //Create appropriate URI
    ostringstream queryStream;
    //queryStream << "?";
    for (uint i = 0; i < _uriParams->size(); i++) {
      if (i > 0)
        queryStream << ",";
      queryStream << _uriParams->at(i).getQueryValue();
    }
    uri.setQuery(queryStream.str());
    //cout<<uri.toString()<<endl;;
  }

  //Creating HTTP Session
  HTTPResponse *httpResponse = new HTTPResponse();
  //istream* resultStream;
  //HTTPClientSession *httpSession = nullptr;
  if(httpSession != nullptr) {
    httpSession->reset();
    delete httpSession;
    httpSession = nullptr;
    resultStream = nullptr;
  }

  try {
    /** This operation does not accept a request body. **/
    if (bodyReqBuffer == nullptr)
      httpSession = doHTTPIO(uri, _method, &reqParamMap);
    else {
      if (contentType != nullptr)
        httpSession = doHTTPIO(uri, _method, &reqParamMap, bodyReqBuffer, size,
            *contentType);
      else
        httpSession = doHTTPIO(uri, _method, &reqParamMap, bodyReqBuffer, size,"");
    }

    //Now we should increase number of calls to SWIFT API
    _account->increaseCallCounter();

    resultStream = &httpSession->receiveResponse(*httpResponse);
  } catch (Exception &e) {
    SwiftResult<T> *result = new SwiftResult<T>();
    SwiftError error(SwiftError::SWIFT_EXCEPTION, e.displayText());
    result->setError(error);
    //Try to set HTTP Response as the payload
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }

  /**
   * Check HTTP return code
   */
  bool valid = false;
  for (uint i = 0; i < _httpValidCodes->size(); i++)
    if (_httpValidCodes->at(i) == httpResponse->getStatus()) {
      valid = true;
      break;
    }

  if (!valid) {
    SwiftResult<T> *result = new SwiftResult<T>();
    SwiftError error(SwiftError::SWIFT_HTTP_ERROR, httpResponse->getReason());
    result->setError(error);
    result->setResponse(httpResponse);
    result->setPayload(nullptr);
    return result;
  }

  //Everything seems fine
  SwiftResult<T> *result = new SwiftResult<T>();
  result->setError(SWIFT_OK);
  result->setResponse(httpResponse);
  result->setPayload(resultStream);
  //Cleanup
  return result;
}

} /* namespace Swift */
