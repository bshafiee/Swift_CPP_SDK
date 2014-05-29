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


namespace Swift {

class HTTPIO {
public:
  //Hashmap
  typedef Poco::HashMap<std::string, std::string> HTTPParamMap;
  static Poco::Net::HTTPClientSession* doGet (const std::string &url,uint port,const HTTPParamMap *params);
  static Poco::Net::HTTPClientSession* doPost(const std::string &url,uint port,const HTTPParamMap *params);
  static Poco::Net::HTTPClientSession* doPost(const std::string &url,uint port,const std::string &reqBody, const std::string &contentType);
};

} /* namespace Swift */
#endif /* HTTPIO_H_ */
