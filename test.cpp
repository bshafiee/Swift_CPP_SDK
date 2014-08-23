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
#include "io/HTTPIO.h"
#include "model/Account.h"
#include "model/Container.h"
#include "model/Object.h"
#include <sstream>
#include <cstring>
#include <climits>

using namespace Poco::Net;
using namespace Poco;
using namespace std;
using namespace Swift;

struct membuf: std::streambuf {
  membuf(char* begin, char* end) {
    this->setg(begin, begin, end);
  }
};

int main(int argc, char** argv) {
  AuthenticationInfo info;
  info.username = "behrooz";
  info.password = "behrooz";
  info.authUrl = "http://10.42.0.83:5000/v2.0/tokens";
  info.tenantName = "kos";
  info.method = AuthenticationMethod::KEYSTONE;

  //Account Test cases
  SwiftResult<Account*>* authenticateResult = Account::authenticate(info);
  cout << "Account Test case:\n" << authenticateResult->getPayload()->toString()
      << endl << endl;

  //Get account details
  SwiftResult<istream*>* accountDetails =
      authenticateResult->getPayload()->swiftAccountDetails(
          HEADER_FORMAT_APPLICATION_JSON);
  StreamCopier::copyStream(*accountDetails->getPayload(), cout);
  cout << endl << endl;
  delete accountDetails;

  //Create account metadata
  vector<pair<string, string> > accountMetaData;
  accountMetaData.push_back(make_pair("Key 1", "Value 1"));
  accountMetaData.push_back(make_pair("Key 2", "Value 2"));
  SwiftResult<int*>* accountMetaDataRes =
      authenticateResult->getPayload()->swiftCreateMetadata(accountMetaData);
  accountMetaDataRes->getResponse()->write(cout);
  cout << endl << endl;
  delete accountMetaDataRes;

  //delete account metadata
  vector<string> accountMetaDataDeleteKeys;
  accountMetaDataDeleteKeys.push_back("Key 2");
  accountMetaDataRes = authenticateResult->getPayload()->swiftDeleteMetadata(
      accountMetaDataDeleteKeys);
  accountMetaDataRes->getResponse()->write(cout);
  cout << endl << endl;
  delete accountMetaDataRes;

  //Update account metadata
  accountMetaData.clear();
  accountMetaData.push_back(make_pair("Key 1", "Value 1 updated"));
  accountMetaDataRes = authenticateResult->getPayload()->swiftUpdateMetadata(
      accountMetaData);
  accountMetaDataRes->getResponse()->write(cout);
  cout << endl << endl;
  delete accountMetaDataRes;

  //Show account metadata
  accountMetaDataRes = authenticateResult->getPayload()->swiftShowMetadata();
  accountMetaDataRes->getResponse()->write(cout);
  cout << endl << endl;
  delete accountMetaDataRes;

  //List containers
  SwiftResult<vector<Container>*>* containerListRes = authenticateResult->getPayload()->swiftGetContainers();
  cout<<"Num of Container:"<<containerListRes->getPayload()->size()<<endl;;
  if(containerListRes->getError().code == SWIFT_OK.code)
    for(auto it=containerListRes->getPayload()->begin();it!=containerListRes->getPayload()->end();++it)
      cout<<"Container:"<<(*it).getName()<<endl;
  cout<<endl;
  delete containerListRes;

  //Container Test Cases
  Container container(authenticateResult->getPayload(), "Container2");

  //Create container
  SwiftResult<int*>* containerRes = container.swiftCreateContainer();
  containerRes->getResponse()->write(cout);
  cout << endl << endl;
  delete containerRes;

  //Container list objects test
  Container container2(authenticateResult->getPayload(), "Container2");
  SwiftResult<vector<Object>*>* objects = container2.swiftGetObjects(true);
  if(objects->getError().code == SwiftError::SWIFT_OK)
    for (uint i = 0; i < objects->getPayload()->size(); i++) {
      cout << objects->getPayload()->at(i).getName() << "\tLength:"
          << objects->getPayload()->at(i).getLength() << "\tType:"
          << objects->getPayload()->at(i).getContentType() << "\tHash:"
          << objects->getPayload()->at(i).getHash() << "\tLastModiefied:"
          << objects->getPayload()->at(i).getLastModified() << endl;
    }
  delete objects;

  //Delete container
  containerRes = container.swiftDeleteContainer();
  containerRes->getResponse()->write(cout);
  cout << endl << endl;
  delete containerRes;

  //Create Again
  SwiftResult<int*>*createRes = container.swiftCreateContainer();
  delete createRes;

  //Get objects of an existing container
  container.setName("Container2");
  SwiftResult<istream*>* containerResult = container.swiftListObjects();
  if(containerResult->getError().code == SwiftError::SWIFT_OK)
    StreamCopier::copyStream(*containerResult->getPayload(), cout);
  cout << endl << endl;
  delete containerResult;

  //Create Container metadata
  vector<pair<string, string> > containerMetaData;
  containerMetaData.push_back(make_pair("Key 1", "Container Value 1"));
  containerMetaData.push_back(make_pair("Key 2", "container Value 2"));
  containerRes = container.swiftCreateMetadata(containerMetaData);
  containerRes->getResponse()->write(cout);
  cout << endl << endl;
  delete containerRes;

  //Delete Container metadata
  vector<string> containerMetaDataDeleteKeys;
  containerMetaDataDeleteKeys.push_back("Key 2");
  containerRes = container.swiftDeleteMetadata(containerMetaDataDeleteKeys);
  containerRes->getResponse()->write(cout);
  cout << endl << endl;
  delete containerRes;

  //Update Container metadata
  containerMetaData.clear();
  containerMetaData.push_back(make_pair("Key 1", "Container Value 1 updated"));
  containerRes = container.swiftUpdateMetadata(containerMetaData);
  containerRes->getResponse()->write(cout);
  cout << endl << endl;
  delete containerRes;

  //Show Container metadata
  containerRes = container.swiftShowMetadata();
  containerRes->getResponse()->write(cout);
  cout << endl << endl;
  delete containerRes;

  //Object Test case
  Object object(&container, "Object2");
  string data = "Hello crappy World :)";
  SwiftResult<int*> *objResult = object.swiftCreateReplaceObject(data.c_str(),
      data.length(), true);
  objResult->getResponse()->write(cout);
  cout << endl << endl;
  delete objResult;

  //Object get content
  SwiftResult<istream*>* objReadResult = object.swiftGetObjectContent();
  if(objReadResult->getError().code == SwiftError::SWIFT_OK)
    StreamCopier::copyStream(*objReadResult->getPayload(), cout);
  cout << endl << endl;
  delete objReadResult;

  Object chucnkedObject(&container, "output-@#$%");
  ostream *outStream = nullptr;
  SwiftResult<HTTPClientSession*> *chunkedResult =
      chucnkedObject.swiftCreateReplaceObject(outStream);

  ulong max = INT_MAX / 100000;
  for (ulong i = 0; i < max; i++) {
    *outStream << i << "\n";
    if (i % 1000 == 0) {
      double id = i;
      cout << (id / (double) max) * 100 << " %" << endl;
    }
  }
  HTTPResponse response;
  chunkedResult->getPayload()->receiveResponse(response);
  response.write(cout);
  cout << endl << endl;
  delete chunkedResult;

  //Object get content
  SwiftResult<istream*> *readResult = chucnkedObject.swiftGetObjectContent();
  //StreamCopier::copyStream(*readResult->getPayload(),cout);cout<<endl<<endl;
  int bufSize = 1000000;
  char buf[bufSize];
  while (!readResult->getPayload()->eof())
    readResult->getPayload()->read(buf, bufSize);
  delete readResult;

  //Copy Object
  SwiftResult<int*>* copyResult = chucnkedObject.swiftCopyObject(
      "CopyStreamObject", container);
  copyResult->getResponse()->write(cout);
  cout << endl << endl;
  delete copyResult;

  //Delete Object
  Object copyStreamObject(&container, "CopyStreamObject");
  SwiftResult<istream*>* deleteResult = copyStreamObject.swiftDeleteObject();
  if(deleteResult->getError().code == SWIFT_OK.code)
    StreamCopier::copyStream(*deleteResult->getPayload(), cout);
  cout << endl << endl;
  delete deleteResult;

  //Object create Metadata
  map<string, string> metaDataMap;
  metaDataMap.insert(make_pair("Key1", "metaData stream object Value 1"));
  SwiftResult<istream*>* createMetaResult = chucnkedObject.swiftCreateMetadata(
      metaDataMap);
  createMetaResult->getResponse()->write(cout);
  if (createMetaResult->getPayload() != nullptr)
    StreamCopier::copyStream(*createMetaResult->getPayload(), std::cout);
  cout << endl << endl;
  delete createMetaResult;

  //Object show Metadata
  cout << endl << endl << "MetaDataResult:" << endl;
  SwiftResult<int*>* metaDataShowResult = chucnkedObject.swiftShowMetadata();
  metaDataShowResult->getResponse()->write(cout);
  cout << endl << endl;
  delete metaDataShowResult;

  //Add More Metadata
  metaDataMap.clear();
  metaDataMap.insert(make_pair("Key2", "metaData Value 2"));
  metaDataMap.insert(make_pair("Key 3", "metaData Value 3"));
  SwiftResult<std::istream*>* objMetaRes = chucnkedObject.swiftCreateMetadata(metaDataMap);
  delete objMetaRes;


  //Update Metadata
  metaDataMap.clear();
  metaDataMap.insert(make_pair("Key2", "Updated metaData Gholi Value 2"));
  objMetaRes = chucnkedObject.swiftUpdateMetadata(metaDataMap);
  delete objMetaRes;

  //Delete metadata
  vector<string> keysToDelete;
  keysToDelete.push_back("Key 3");
  objMetaRes = chucnkedObject.swiftDeleteMetadata(keysToDelete);
  delete objMetaRes;

  //Object show Metadata
  cout << endl << endl << "MetaDataResult:" << endl;
  metaDataShowResult = chucnkedObject.swiftShowMetadata();
  metaDataShowResult->getResponse()->write(cout);
  cout << endl << endl;
  delete metaDataShowResult;

  //Total number of calls to the api
  cout << "Total Number of Calls to the api:"
      << authenticateResult->getPayload()->getNumberOfCalls() << endl;

  delete authenticateResult;
}
