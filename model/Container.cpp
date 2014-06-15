/*
 * Container.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Container.h"
#include <sstream>      // ostringstream
#include "../io/HTTPIO.h"

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Swift {

/** Forward Declarations **/
template<class T>
extern SwiftResult<T>* returnNullError(const string &whatsNull);

Container::Container(Account* _account, std::string _name) :
    account(_account), name(_name) {
  // TODO Auto-generated destructor stub
}

Container::~Container() {
  // TODO Auto-generated destructor stub
}

SwiftResult<std::istream*>* Container::swiftGetObjects(
    HTTPHeader& _formatHeader, bool _newest) {
  vector<HTTPHeader> _queryMap;
  return swiftGetObjects(_formatHeader, _queryMap, _newest);
}

SwiftResult<std::istream*>* Container::swiftGetObjects(
    HTTPHeader& _formatHeader, std::vector<HTTPHeader>& _queryMap,
    bool _newest) {
  //Check Container
  if (account == nullptr)
    return returnNullError<istream*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   * 200:
   *  Success. The response body lists the containers.
   *
   * 204:
   *  Success. The response body shows no containers.
   *  Either the account has no containers or you are paging
   *  through a long list of names by using the marker, limit,
   *  or end_marker query parameters, and you have reached
   *  the end of the list.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_OK);
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //add X-Newest
  std::vector<HTTPHeader> _reqMap;
  if (_newest)
    _reqMap.push_back(*new HTTPHeader("X-Newest", "True"));
  //Push Header Format
  _queryMap.push_back(_formatHeader);

  //Do swift transaction
  return doSwiftTransaction<istream*>(this->account, path,
      HTTPRequest::HTTP_GET, &_queryMap, &_reqMap, &validHTTPCodes, nullptr, 0,
      nullptr, nullptr);
}

SwiftResult<void*>* Container::swiftCreateContainer(
    std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   * 201:
   *  Success. container was created successfully.
   *
   * 204:
   *   Success??
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_CREATED);
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //Do swift transaction
  return doSwiftTransaction<void*>(this->account, path, HTTPRequest::HTTP_PUT,
      nullptr, _reqMap, &validHTTPCodes, nullptr, 0, nullptr, nullptr);
}

SwiftResult<void*>* Container::swiftDeleteContainer() {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   *
   * 204:
   *  Success. container was deleted successfully.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //Do swift transaction
  return doSwiftTransaction<void*>(this->account, path,
      HTTPRequest::HTTP_DELETE, nullptr, nullptr, &validHTTPCodes, nullptr, 0,
      nullptr, nullptr);
}

SwiftResult<void*>* Container::swiftCreateMetadata(
    std::vector<std::pair<std::string, std::string> >& _metaData,
    std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   *
   * 204:
   *  Success. The response body is empty.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //Add Actual metadata
  if (_metaData.size() > 0) {
    if (_reqMap == nullptr)
      _reqMap = new vector<HTTPHeader>();
    for (uint i = 0; i < _metaData.size(); i++)
      _reqMap->push_back(
          *new HTTPHeader("X-Container-Meta-" + _metaData[i].first,
              _metaData[i].second));
  }

  //Do swift transaction
  return doSwiftTransaction<void*>(this->account, path, HTTPRequest::HTTP_POST,
      nullptr, _reqMap, &validHTTPCodes, nullptr, 0, nullptr, nullptr);
}

SwiftResult<void*>* Container::swiftUpdateMetadata(
    std::vector<std::pair<std::string, std::string> >& _metaData,
    std::vector<HTTPHeader>* _reqMap) {
  return swiftCreateMetadata(_metaData, _reqMap);
}

SwiftResult<void*>* Container::swiftDeleteMetadata(
    std::vector<std::string>& _metaDataKeys, std::vector<HTTPHeader>* _reqMap) {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   *
   * 204:
   *  Success. Metadata was deleted successfully.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  //Add metadata keys to be deleted
  if (_metaDataKeys.size() > 0) {
    if (_reqMap == nullptr)
      _reqMap = new vector<HTTPHeader>();
    for (uint i = 0; i < _metaDataKeys.size(); i++)
      _reqMap->push_back(
          *new HTTPHeader("X-Remove-Container-Meta-" + _metaDataKeys[i], "x"));
  }

  //Do swift transaction
  return doSwiftTransaction<void*>(this->account, path, HTTPRequest::HTTP_POST,
      nullptr, _reqMap, &validHTTPCodes, nullptr, 0, nullptr, nullptr);
}

std::string& Container::getName() {
  return name;
}

void Container::setName(const std::string& name) {
  this->name = name;
}

Account* Container::getAccount() {
  return account;
}

SwiftResult<void*>* Container::swiftShowMetadata(bool _newest) {
  //Check Container
  if (account == nullptr)
    return returnNullError<void*>("account");
  //Path
  string path = this->name;
  /**
   * Check HTTP return code
   *
   * 204:
   *  Success. Response body is empty.
   */
  vector<int> validHTTPCodes;
  validHTTPCodes.push_back(HTTPResponse::HTTP_NO_CONTENT);

  vector<HTTPHeader> _reqMap;
  if (_newest) {
    _reqMap.push_back(*new HTTPHeader("X-Newest", "True"));
  }

  //Do swift transaction
  return doSwiftTransaction<void*>(this->account, path, HTTPRequest::HTTP_HEAD,
      nullptr, &_reqMap, &validHTTPCodes, nullptr, 0, nullptr, nullptr);
}

} /* namespace Swift */
