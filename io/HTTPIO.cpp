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
  //request.write(cout);
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
    const char* reqBody, ulong size) {
  HTTPClientSession *session = new HTTPClientSession(uri.getHost(),
      uri.getPort());
  HTTPRequest request(type, uri.getPath());
  //Set Content size
  request.setContentLength(size);

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
    std::istream& inputStream) {
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

  //write request body
  ostream &ostream = session->sendRequest(request);
  if (ostream == nullptr)
    return nullptr;
  //Copy input stream to the output stream
  StreamCopier::copyStream(inputStream,ostream);
  //ostream << inputStream;

  return session;
}

} /* namespace Swift */
