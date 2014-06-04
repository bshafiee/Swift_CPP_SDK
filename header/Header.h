/*
 * Header.h
 *
 *  Created on: 2014-06-04
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef HEADER_H_
#define HEADER_H_

#include <iostream>

namespace Swift {

struct HTTPHeader {
private:
  std::pair<std::string,std::string> pair;
public:
  HTTPHeader(std::string key,std::string value) {
    pair = std::make_pair(key,value);
  }

  std::string getKey() {
    return pair.first;
  }

  std::string getValue() {
      return pair.second;
  }

  std::string getQueryValue() {
    return pair.first+"="+pair.second;
  }
};


/** Common HTTP Headers **/
static HTTPHeader HEADER_FORMAT_JSON("format","json");
static HTTPHeader HEADER_FORMAT_XML("format","xml");


} /* namespace Swift */
#endif /* HEADER_H_ */
