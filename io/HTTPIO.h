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

using namespace Poco::Net;
using namespace Poco;
using namespace std;

namespace Swift {

class HTTPIO {
public:
  //Hashmap
  typedef HashMap<string, string> HTTPParamMap;
  static HTTPClientSession* doGet (const string &url,uint port,const HTTPParamMap *params);
  static HTTPClientSession* doPost(const string &url,uint port,const HTTPParamMap *params);
  static HTTPClientSession* doPost(const string &url,uint port,const string &reqBody, const string &contentType);
};

} /* namespace Swift */
#endif /* HTTPIO_H_ */
