/*
 * Pagination.h
 *
 *  Created on: 2014-06-04
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef PAGINATION_H_
#define PAGINATION_H_

#include <iostream>

namespace Swift {

class Pagination {
  int limit;
  std::string marker;
  std::string end_marker;
public:
  Pagination();
  virtual ~Pagination();
};

} /* namespace Swift */
#endif /* PAGINATION_H_ */
