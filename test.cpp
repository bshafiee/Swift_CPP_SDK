#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <json/json.h>
#include <iostream>
#include "io/HTTPIO.h"
#include "model/Account.h"
#include "model/Container.h"
#include "model/Object.h"
#include <sstream>      // ostringstream
#include <cstring>

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
  info.authUrl = "http://192.168.249.109:5000/v2.0/tokens";
  info.tenantName = "BehroozProject";
  info.method = AuthenticationMethod::KEYSTONE;

  //Account Test cases
  SwiftResult<Account*>* authenticateResult = Account::authenticate(info);
  //cout<<"Account Test case:\n"<<authenticateResult->getPayload()->toString()<<endl<<endl;

  //Get account details
  SwiftResult<istream*>* accountDetails = authenticateResult->getPayload()->swiftAccountDetails(HEADER_FORMAT_APPLICATION_JSON);
  //StreamCopier::copyStream(*accountDetails->getPayload(),cout);cout<<endl<<endl;

  SwiftResult<vector<Container*>*>* conatiners = authenticateResult->getPayload()->swiftGetContainers(true);
  for(int i=0;i<conatiners->getPayload()->size();i++)
    cout<<conatiners->getPayload()->at(i)->getName()<<endl;


/*
  //Create account metadata
  vector<pair<string, string> > accountMetaData;
  accountMetaData.push_back(make_pair("Key 1","Value 1"));
  accountMetaData.push_back(make_pair("Key 2","Value 2"));
  SwiftResult<void*>* accountMetaDataRes = authenticateResult->getPayload()->swiftCreateMetadata(accountMetaData);
  accountMetaDataRes->getResponse()->write(cout);cout<<endl<<endl;

  //delete account metadata
  vector<string> accountMetaDataDeleteKeys;
  accountMetaDataDeleteKeys.push_back("Key 2");
  accountMetaDataRes = authenticateResult->getPayload()->swiftDeleteMetadata(accountMetaDataDeleteKeys);
  accountMetaDataRes->getResponse()->write(cout);cout<<endl<<endl;

  //Update account metadata
  accountMetaData.clear();
  accountMetaData.push_back(make_pair("Key 1","Value 1 updated"));
  accountMetaDataRes = authenticateResult->getPayload()->swiftUpdateMetadata(accountMetaData);
  accountMetaDataRes->getResponse()->write(cout);cout<<endl<<endl;

  //Show account metadata
  accountMetaDataRes = authenticateResult->getPayload()->swiftShowMetadata();
  accountMetaDataRes->getResponse()->write(cout);cout<<endl<<endl;


  //Container Test Cases
  Container container(authenticateResult->getPayload(),"Container 3");

  //Create container
  SwiftResult<void*>* containerRes = container.swiftCreateContainer();
  containerRes->getResponse()->write(cout);cout<<endl<<endl;

  //Delete container
  containerRes = container.swiftDeleteContainer();
  containerRes->getResponse()->write(cout);cout<<endl<<endl;

  //Get objects of an existing container
  container.setName("Container2");
  SwiftResult<istream*>* containerResult = container.swiftGetObjects();
  StreamCopier::copyStream(*containerResult->getPayload(),cout);cout<<endl<<endl;

  //Create Container metadata
  vector<pair<string, string> > containerMetaData;
  containerMetaData.push_back(make_pair("Key 1","Container Value 1"));
  containerMetaData.push_back(make_pair("Key 2","container Value 2"));
  containerRes = container.swiftCreateMetadata(containerMetaData);
  containerRes->getResponse()->write(cout);cout<<endl<<endl;

  //Delete Container metadata
  vector<string> containerMetaDataDeleteKeys;
  containerMetaDataDeleteKeys.push_back("Key 2");
  containerRes = container.swiftDeleteMetadata(containerMetaDataDeleteKeys);
  containerRes->getResponse()->write(cout);cout<<endl<<endl;

  //Update Container metadata
  containerMetaData.clear();
  containerMetaData.push_back(make_pair("Key 1","Container Value 1 updated"));
  containerRes = container.swiftUpdateMetadata(containerMetaData);
  containerRes->getResponse()->write(cout);cout<<endl<<endl;

  //Show Container metadata
  containerRes = container.swiftShowMetadata();
  containerRes->getResponse()->write(cout);cout<<endl<<endl;


  //Object Test case
  Object object(&container,"Object2");
  string data = "Hello crappy World :)";
  SwiftResult<void*> *objResult = object.swiftCreateReplaceObject(data.c_str(),data.length(),true);
  objResult->getResponse()->write(cout);cout<<endl<<endl;

  //Object get content
  SwiftResult<istream*>* objReadResult = object.swiftGetObjectContent();
  StreamCopier::copyStream(*objReadResult->getPayload(),cout);cout<<endl<<endl;

  //Chunked Object
  char buf[] = "hello fucking world";
  membuf sbuf(buf, buf + sizeof(buf));
  std::istream in(&sbuf);
  Object objectStream(&container,"ChunkedObj");
  SwiftResult<void*>* createResult = objectStream.swiftCreateReplaceObject(in,nullptr,nullptr);
  createResult->getResponse()->write(cout);cout<<endl<<endl;

  //Object get content
  objReadResult = objectStream.swiftGetObjectContent();
  StreamCopier::copyStream(*objReadResult->getPayload(),cout);cout<<endl<<endl;

  //Copy Object
  SwiftResult<void*>* copyResult = objectStream.swiftCopyObject("CopyStreamObject",container);
  copyResult->getResponse()->write(cout);cout<<endl<<endl;

  //Delete Object
  Object copyStreamObject(&container,"CopyStreamObject");
  SwiftResult<istream*>* deleteResult = copyStreamObject.swiftDeleteObject();
  StreamCopier::copyStream(*deleteResult->getPayload(),cout);cout<<endl<<endl;

  //Object create Metadata
  map<string, string> metaDataMap;
  metaDataMap.insert(make_pair("Key1","metaData stream object Value 1"));
  SwiftResult<istream*>* createMetaResult = objectStream.swiftCreateMetadata(metaDataMap);
  createMetaResult->getResponse()->write(cout);
  if(createMetaResult->getPayload()!=nullptr)
  StreamCopier::copyStream(*createMetaResult->getPayload(),std::cout);cout<<endl<<endl;

  //Object show Metadata
  cout<<endl<<endl<<"MetaDataResult:"<<endl;
  SwiftResult<void*>* metaDataShowResult = objectStream.swiftShowMetadata();
  metaDataShowResult->getResponse()->write(cout);cout<<endl<<endl;

  //Add More Metadata
  metaDataMap.clear();
  metaDataMap.insert(make_pair("Key2","metaData Value 2"));
  metaDataMap.insert(make_pair("Key 3","metaData Value 3"));
  objectStream.swiftCreateMetadata(metaDataMap);

  //Update Metadata
  metaDataMap.clear();
  metaDataMap.insert(make_pair("Key2","Updated metaData Gholi Value 2"));
  objectStream.swiftUpdateMetadata(metaDataMap);

  //Delete metadata
  vector<string> keysToDelete;
  keysToDelete.push_back("Key 3");
  objectStream.swiftDeleteMetadata(keysToDelete);

  //Object show Metadata
  cout<<endl<<endl<<"MetaDataResult:"<<endl;
  metaDataShowResult = objectStream.swiftShowMetadata();
  metaDataShowResult->getResponse()->write(cout);cout<<endl<<endl;

  //Total number of calls to the api
  cout<<"Total Number of Calls to the api:"<<authenticateResult->getPayload()->getNumberOfCalls()<<endl;
*/
}
