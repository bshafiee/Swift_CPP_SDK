/*
 * SwiftResult.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "SwiftResult.h"

namespace Swift {

template<class T>
SwiftResult<T>::SwiftResult() :
    error(SwiftError::SWIFT_FAIL, "Unknown error"),
    payload(nullptr) {
}

template<class T>
SwiftResult<T>::~SwiftResult() {
  // TODO Auto-generated destructor stub
}

} /* namespace Swift */
