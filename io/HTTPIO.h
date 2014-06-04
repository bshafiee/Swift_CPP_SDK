/*
 * HTTPIO.h
 *
 *  Created on: 2014-05-27
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef HTTPIO_H_
#define HTTPIO_H_

#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/HashMap.h>
#include <Poco/URI.h>
#include <iostream>
#include <vector>
#include "../header/Header.h"


namespace Swift {

class HTTPIO {
public:
  static Poco::Net::HTTPClientSession* doGet(const std::string &url, uint port, std::vector<HTTPHeader> *params);
  static Poco::Net::HTTPClientSession* doGet(const std::string &url, std::vector<HTTPHeader> *params);
  static Poco::Net::HTTPClientSession* doGet(const Poco::URI &uri, std::vector<HTTPHeader> *params);
  static Poco::Net::HTTPClientSession* doGet(const std::string &url, const std::string &reqBody, const std::string &contentType);
  static Poco::Net::HTTPClientSession* doPost(const std::string &url,uint port,std::vector<HTTPHeader> *params);
  static Poco::Net::HTTPClientSession* doPost(const std::string &uri,const std::string &reqBody, const std::string &contentType);
};

} /* namespace Swift */
#endif /* HTTPIO_H_ */
