/*
 * HTTPIO.cpp
 *
 *  Created on: 2014-05-27
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "HTTPIO.h"

namespace Swift {

using namespace std;
using namespace Poco::Net;
using namespace Poco;

HTTPClientSession* HTTPIO::doGet(const string &url, uint port,
    vector<HTTPHeader> *params) {
  URI uri(url);
  uri.setPort(port);
  Poco::Net::HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  Poco::Net::HTTPRequest request(HTTPRequest::HTTP_GET, uri.getPath());
  //Add params
  if (params != nullptr && params->size() > 0) {
    vector<HTTPHeader>::iterator it = params->begin();
    while (it != params->end()) {
      request.add(it->getKey(), it->getValue());
      it++;
    }
  }

  session->sendRequest(request);
  return session;
}

HTTPClientSession* HTTPIO::doGet(const string &url, vector<HTTPHeader> *params) {
  URI uri(url);
  Poco::Net::HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  Poco::Net::HTTPRequest request(HTTPRequest::HTTP_GET, uri.getPath());
  //Add params
  if (params != nullptr && params->size() > 0) {
    vector<HTTPHeader>::iterator it = params->begin();
    while (it != params->end()) {
      request.add(it->getKey(), it->getValue());
      it++;
    }
  }

  session->sendRequest(request);
  return session;
}

HTTPClientSession* HTTPIO::doGet(const Poco::URI &uri, vector<HTTPHeader> *params) {
  Poco::Net::HTTPClientSession *session = new HTTPClientSession(uri.getHost(),uri.getPort());
  Poco::Net::HTTPRequest request(HTTPRequest::HTTP_GET, uri.getPathAndQuery());
  //Add params
  if (params != nullptr && params->size() > 0) {
    vector<HTTPHeader>::iterator it = params->begin();
    while (it != params->end()) {
      request.add(it->getKey(), it->getValue());
      it++;
    }
  }

  session->sendRequest(request);
  return session;
}

HTTPClientSession* HTTPIO::doGet(const string &url, const string &reqBody,
    const string &contentType) {
  URI uri(url);
  //uri.setPort(port);
  HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_GET, uri.getPath());
  //Set Content Type
  request.setContentLength(reqBody.size());
  request.setContentType(contentType);
  //session->
  //write request body
  ostream &ostream = session->sendRequest(request);
  if(ostream == nullptr)
    return nullptr;
  ostream << reqBody;
  return session;
}

Poco::Net::HTTPClientSession* HTTPIO::doPost(const string &_uri, uint port,
    vector<HTTPHeader> *params) {
  URI uri(_uri);
  uri.setPort(port);
  HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_POST, uri.getPath());
  //Add params
  if (params != nullptr && params->size() > 0) {
  vector<HTTPHeader>::iterator it = params->begin();
    while (it != params->end()) {
      request.add(it->getKey(), it->getValue());
      it++;
    }
  }
  session->sendRequest(request);
  return session;
}

HTTPClientSession* HTTPIO::doPost(const string &url, const string &reqBody,
    const string &contentType) {
  URI uri(url);
  //uri.setPort(port);
  HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  HTTPRequest request(HTTPRequest::HTTP_POST, uri.getPath());
  //Set Content Type
  request.setContentLength(reqBody.size());
  request.setContentType(contentType);
  //session->
  //write request body
  ostream &ostream = session->sendRequest(request);
  if(ostream == nullptr)
	  return nullptr;
  ostream << reqBody;
  return session;
}

} /* namespace Swift */
