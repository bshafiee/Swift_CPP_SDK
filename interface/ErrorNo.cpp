/*
 *  ErrorNo.cpp
 *  Created on: 2014-05-26
 *      Author: Behrooz Shafiee Sarjaz
 */
#include <iostream>
#include <cstring>
#include "ErrorNo.h"

namespace Swift {

SwiftError::SwiftError(int _code, std::string _msg) {
  code = _code;
  msg = _msg;
}

std::string SwiftError::toString() {
  int len = strlen(msg.c_str()) + 20;
  char *temp = new char[len];
  snprintf(temp, len, "Error %d: %s", code, msg.c_str());
  return std::string(temp);
}

//Always the same message
SwiftError SWIFT_OK(SwiftError::SWIFT_OK, "SWIFT_OK");

}
