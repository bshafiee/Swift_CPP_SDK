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

#ifndef OBJECT_H_
#define OBJECT_H_

#include <Container.h>
#include <Poco/HashMap.h>
#include <Poco/Net/HTTPClientSession.h>

namespace Swift {

class Object {
  Container* container;
  std::string name;
  long length;
  std::string content_type;
  std::string hash;
  std::string last_modified;
public:
  Object(Container* _container, std::string _name = "", long _length = -1,
      std::string _content_type = "", std::string _hash = "",
      std::string _last_modified = "");
  virtual ~Object();

  std::vector<std::pair<std::string, std::string> >* getExistingMetaData();
  Container* getContainer();
  void setContainer(Container* container);
  std::string getName();
  void setName(const std::string& name);
  std::string getContentType();
  void setContentType(const std::string& _contentType);
  std::string getHash();
  void setHash(const std::string& _hash);
  std::string getLastModified();
  void setLastModified(const std::string& _lastModified);
  long getLength();
  void setLength(long _length);

  /** API Functions **/
  SwiftResult<std::istream*>* swiftGetObjectContent(
      std::vector<HTTPHeader> *_uriParams = nullptr,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<int*>* swiftCreateReplaceObject(const char* _data, ulong _size,
      bool _calculateETag = true, std::vector<HTTPHeader> *_uriParams = nullptr,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  /**
   * should not be bigger than 5GB
   * This is an especial function:
   * you should pass a outputStream pointer so you will have access to an output stream to write your data
   * this pointer is valid as long as you call httpclientsession->recveiveRespone() to receive actual response
   * the valid http response code is HTTP_CREATED(201).
   */
  SwiftResult<Poco::Net::HTTPClientSession*>* swiftCreateReplaceObject(
      std::ostream* &ouputStream, std::vector<HTTPHeader> *_uriParams = nullptr,
      std::vector<HTTPHeader> *_reqMap = nullptr);

  SwiftResult<int*>* swiftCopyObject(const std::string &_dstObjectName,
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

  SwiftResult<int*>* swiftShowMetadata(std::vector<HTTPHeader>* _uriParams =
      nullptr, bool _newest = false);
};

} /* namespace Swift */
#endif /* OBJECT_H_ */
