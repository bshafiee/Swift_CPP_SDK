/*
 * Container.h
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "Account.h"

namespace Swift {

class Container {
  Account* account;
public:
  Container(Account *_account);
  virtual ~Container();

  /** API Functions **/

  SwiftResult<std::istream*>* swiftGetObjects(const std::string &_containerName,HTTPHeader &_formatHeader = HEADER_FORMAT_APPLICATION_JSON, bool _newest = false);
  SwiftResult<std::istream*>* swiftGetObjects(const std::string &_containerName,HTTPHeader &_formatHeader, std::vector<HTTPHeader> &_queryMap, bool _newest);

  SwiftResult<void*>* swiftCreateContainer(const std::string &_containerName);
  SwiftResult<void*>* swiftCreateContainer(const std::string &_containerName, std::vector<HTTPHeader> &_queryMap);

  SwiftResult<void*>* swiftDeleteContainer(const std::string &_containerName);

  SwiftResult<void*>* swiftCreateMetadata(std::vector<std::pair<std::string,std::string>> &_metaData);
  SwiftResult<void*>* swiftCreateMetadata(std::vector<std::pair<std::string,std::string>> &_metaData,std::vector<HTTPHeader> &_reqMap);

  SwiftResult<void*>* swiftUpdateMetadata(std::vector<std::pair<std::string,std::string>> &_metaData);
  SwiftResult<void*>* swiftUpdateMetadata(std::vector<std::pair<std::string,std::string>> &_metaData,std::vector<HTTPHeader> &_reqMap);

  SwiftResult<void*>* swiftDeleteMetadata(std::vector<std::string> &_metaDataKeys);
  SwiftResult<void*>* swiftDeleteMetadata(std::vector<std::string> &_metaDataKeys,std::vector<HTTPHeader> &_reqMap);
};

} /* namespace Swift */
#endif /* CONTAINER_H_ */
