/*
 * Header.cpp
 *
 *  Created on: 2014-06-04
 *      Author: Behrooz Shafiee Sarjaz
 */

#include <iostream>
#include "Header.h"

namespace Swift {

HTTPHeader::HTTPHeader(std::string key,std::string value) {
  pair = std::make_pair(key,value);
}

Swift::HTTPHeader::~HTTPHeader() {
}

std::string HTTPHeader::getKey() {
  return pair.first;
}

std::string HTTPHeader::getValue() {
  return pair.second;
}

std::string HTTPHeader::getQueryValue() {
  return pair.first + "=" + pair.second;
}

/** Common HTTP Headers **/
HTTPHeader HEADER_FORMAT_APPLICATION_JSON("format", "json");
HTTPHeader HEADER_FORMAT_APPLICATION_XML("format", "xml");
HTTPHeader HEADER_FORMAT_TEXT_XML("format", "text/xml");

} /* namespace Swift */

