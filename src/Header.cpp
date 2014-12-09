/**************************************************************************
    This is a general SDK for OpenStack Swift API written in C++
    Copyright (C) <2014>  <Behrooz Shafiee Sarjaz>
    This program comes with ABSOLUTELY NO WARRANTY;

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

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

