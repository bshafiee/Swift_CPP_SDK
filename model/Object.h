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
public:
  Object(Container* _container);
  virtual ~Object();

  std::vector<std::pair<std::string, std::string> >* getExistingMetaData(const std::string& _objectName);

  /** API Functions **/
  SwiftResult<std::istream*>* swiftGetObjectContent(
      const std::string &_objectName, std::vector<HTTPHeader> *_uriParams =
          nullptr, std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<void*>* swiftCreateReplaceObject(const std::string &_objectName,
      const char* _data, ulong _size, bool _calculateETag = true,
      std::vector<HTTPHeader> *_uriParams = nullptr,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  /**
   * should not be bigger than 5GB
   */
  SwiftResult<void*>* swiftCreateReplaceObject(const std::string &_objectName,
        std::istream &inputStream,
        std::vector<HTTPHeader> *_uriParams = nullptr,
        std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<void*>* swiftCopyObject(const std::string &_srcObjectName,
      const std::string &_dstObjectName, Container &_dstContainer,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<std::istream*>* swiftDeleteObject(const std::string &_objectName,
      bool _multipartManifest = false);

  SwiftResult<std::istream*>* swiftCreateMetadata(
      const std::string &_objectName,
      std::map<std::string,std::string> &_metaData,
      std::vector<HTTPHeader> *_reqMap = nullptr,bool _keepExistingMetadata=true);

  SwiftResult<std::istream*>* swiftUpdateMetadata(
      const std::string &_objectName,
      std::map<std::string,std::string> &_metaData,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<std::istream*>* swiftDeleteMetadata(const std::string &_objectName,
      std::vector<std::string> &_metaDataKeys);

  SwiftResult<void*>* swiftShowMetadata(const std::string &_objectName,
      std::vector<HTTPHeader>* _uriParams = nullptr, bool _newest = false);
};

} /* namespace Swift */
#endif /* OBJECT_H_ */
