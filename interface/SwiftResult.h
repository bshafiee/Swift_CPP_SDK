/*
 * SwiftResult.h
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef SWIFTRESULT_H_
#define SWIFTRESULT_H_

//#include <Poco/Net/HTTPClientSession.h>
//#include <Poco/Net/HTTPResponse.h>
#include <iostream>
#include "ErrorNo.h"

namespace Swift {

template <class T>
class SwiftResult {
  //Poco::Net::HTTPClientSession *session;
  //Poco::Net::HTTPResponse *response;
  SwiftError error;
  /** Real Data **/
  T payload;

public:
  SwiftResult();
  virtual ~SwiftResult();
};

} /* namespace Swift */
#endif /* SWIFTRESULT_H_ */
