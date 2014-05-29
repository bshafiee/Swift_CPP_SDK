/*
 * ErrorNo.h
 *
 *  Created on: 2014-05-26
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef ERRORNO_H_
#define ERRORNO_H_

#include <iostream>
#include <cstring>

namespace Swift {

struct SwiftError {
  int code;
  std::string msg;
  SwiftError(int _code,std::string _msg) {
    code = _code;
    msg = _msg;
  }

  std::string toString() {
    int len = strlen(msg.c_str())+20;
    char *temp = new char[len];
    snprintf(temp,len,"Error %d: %s",code,msg.c_str());
    return std::string(temp);
  }

  /** List of Errors **/
  static const int SWIFT_OK = 0; //Successful
  static const int SWIFT_FAIL = -1; //Unsuccessful
};

}
#endif /* ERRORNO_H_ */
