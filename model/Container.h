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
  std::string name;
public:
  Container(Account *_account,std::string _name="");
  virtual ~Container();

  /** API Functions **/
  SwiftResult<std::istream*>* swiftGetObjects(HTTPHeader &_formatHeader = HEADER_FORMAT_APPLICATION_JSON, bool _newest = false);
  SwiftResult<std::istream*>* swiftGetObjects(HTTPHeader &_formatHeader, std::vector<HTTPHeader> &_queryMap, bool _newest);

  SwiftResult<void*>* swiftCreateContainer();
  SwiftResult<void*>* swiftCreateContainer(std::vector<HTTPHeader> &_queryMap);

  SwiftResult<void*>* swiftDeleteContainer();

  SwiftResult<void*>* swiftCreateMetadata(std::vector<std::pair<std::string,std::string>> &_metaData);
  SwiftResult<void*>* swiftCreateMetadata(std::vector<std::pair<std::string,std::string>> &_metaData,std::vector<HTTPHeader> &_reqMap);

  SwiftResult<void*>* swiftUpdateMetadata(std::vector<std::pair<std::string,std::string>> &_metaData);
  SwiftResult<void*>* swiftUpdateMetadata(std::vector<std::pair<std::string,std::string>> &_metaData,std::vector<HTTPHeader> &_reqMap);

  SwiftResult<void*>* swiftDeleteMetadata(std::vector<std::string> &_metaDataKeys);
  SwiftResult<void*>* swiftDeleteMetadata(std::vector<std::string> &_metaDataKeys,std::vector<HTTPHeader> &_reqMap);

  SwiftResult<void*>* swiftShowMetadata( bool _newest = false);

  Account* getAccount();
  std::string& getName();
  void setName(const std::string& name);
};

} /* namespace Swift */
#endif /* CONTAINER_H_ */
