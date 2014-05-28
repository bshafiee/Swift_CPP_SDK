/*
 * Tenant.h
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef TENANT_H_
#define TENANT_H_

#include <iostream>

namespace Swift {

class Tenant {
public:
  Tenant();
  virtual ~Tenant();

  std::string id;

  std::string name;

  bool enabled;

};

} /* namespace Swift */
#endif /* TENANT_H_ */
