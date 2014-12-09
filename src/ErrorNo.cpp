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
#include <cstring>
#include "ErrorNo.h"

namespace Swift {

SwiftError::SwiftError(int _code, std::string _msg) {
  code = _code;
  msg = _msg;
}

const std::string SwiftError::toString() {
  int len = strlen(msg.c_str()) + 20;
  char *temp = new char[len];
  snprintf(temp, len, "Error %d: %s", code, msg.c_str());
  return std::string(temp);
}

//Always the same message
SwiftError SWIFT_OK(SwiftError::SWIFT_OK, "SWIFT_OK");

}
