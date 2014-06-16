/*
 * Object.h
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "Container.h"
#include <Poco/HashMap.h>

namespace Swift {

class Object {
  Container* container;
  std::string name;
public:
  Object(Container* _container, std::string _name = "");
  virtual ~Object();

  std::vector<std::pair<std::string, std::string> >* getExistingMetaData();
  Container* getContainer();
  void setContainer(Container* container);
  std::string getName();
  void setName(const std::string& name);

  /** API Functions **/
  SwiftResult<std::istream*>* swiftGetObjectContent(
      std::vector<HTTPHeader> *_uriParams = nullptr,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<void*>* swiftCreateReplaceObject(const char* _data, ulong _size,
      bool _calculateETag = true, std::vector<HTTPHeader> *_uriParams = nullptr,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  /**
   * should not be bigger than 5GB
   */
  SwiftResult<void*>* swiftCreateReplaceObject(std::istream &inputStream,
      std::vector<HTTPHeader> *_uriParams = nullptr,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<void*>* swiftCopyObject(const std::string &_dstObjectName,
      Container &_dstContainer, std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<std::istream*>* swiftDeleteObject(
      bool _multipartManifest = false);

  SwiftResult<std::istream*>* swiftCreateMetadata(
      std::map<std::string, std::string> &_metaData,
      std::vector<HTTPHeader> *_reqMap = nullptr, bool _keepExistingMetadata =
          true);

  SwiftResult<std::istream*>* swiftUpdateMetadata(
      std::map<std::string, std::string> &_metaData,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<std::istream*>* swiftDeleteMetadata(
      std::vector<std::string> &_metaDataKeys);

  SwiftResult<void*>* swiftShowMetadata(std::vector<HTTPHeader>* _uriParams =
      nullptr, bool _newest = false);
};

} /* namespace Swift */
#endif /* OBJECT_H_ */
