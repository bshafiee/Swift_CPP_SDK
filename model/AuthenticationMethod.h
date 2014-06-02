/*
 * AuthenticationMethod.h
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef AUTHENTICATIONMETHOD_H_
#define AUTHENTICATIONMETHOD_H_

#include <cstdio>

namespace Swift {

enum class AuthenticationMethod {
  BASIC , TEMPAUTH , KEYSTONE
};

static std::string authenticationMethodToString(AuthenticationMethod method) {
  switch (method) {
  case AuthenticationMethod::BASIC:
    return "BASIC";
  case AuthenticationMethod::TEMPAUTH:
    return "TEMPAUTH";
  case AuthenticationMethod::KEYSTONE:
    return "KEYSTONE";
  default:
    return "UNKNOWN";
  }
}

}


#endif /* AUTHENTICATIONMETHOD_H_ */
