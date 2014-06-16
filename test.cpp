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
  /*cout<<"Account Test case:\n"<<authenticateResult->getPayload()->toString()<<endl<<endl;

  //Get account details
  SwiftResult<istream*>* accountDetails = authenticateResult->getPayload()->swiftAccountDetails(HEADER_FORMAT_APPLICATION_JSON);
  StreamCopier::copyStream(*accountDetails->getPayload(),cout);cout<<endl<<endl;

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
  */


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



/*
  vector<pair<string, string> > metaData;
  metaData.push_back(make_pair("Key 1","Value 1"));
  SwiftResult<void*>* metaDataRes = result->getPayload()->swiftCreateMetadata(metaData);
  cout<<endl<<endl;
  metaDataRes->getResponse()->write(cout);

  vector<string> deleteKey;
  deleteKey.push_back("Metadata1key");
  deleteKey.push_back("book");
  metaDataRes = result->getPayload()->swiftDeleteMetadata(deleteKey);
  cout<<endl<<endl;
  metaDataRes->getResponse()->write(cout);
  cout<<metaDataRes->getResponse()->get("X-Trans-Id");

  metaDataRes = result->getPayload()->swiftShowMetadata();
  cout<<endl<<endl;
  metaDataRes->getResponse()->write(cout);

  Container mcontainer(result->getPayload());
  mcontainer.setName("Container2");
  metaData.clear();
  metaData.push_back(make_pair("metaData1key45767","metaData2value"));
  SwiftResult<void*>* createRes = mcontainer.swiftCreateMetadata(metaData);
  createRes->getResponse()->write(cout);

  metaDataRes = mcontainer.swiftShowMetadata();
  cout<<endl<<endl;
  metaDataRes->getResponse()->write(cout);

  vector<string> keys;
  keys.push_back("metaData1key45767");
  keys.push_back("metaData1key");
  mcontainer.swiftDeleteMetadata(keys);

  metaDataRes = mcontainer.swiftShowMetadata();
  cout<<endl<<endl;
  metaDataRes->getResponse()->write(cout);


  Container container(result->getPayload());
  SwiftResult<void*>* containerDetails = container.swiftDeleteContainer("Container2");
  containerDetails->getResponse()->write(cout);

  metaData.clear();
  metaData.push_back(make_pair("metaData1key","metaData2value"));
  SwiftResult<void*>* containerDetails = container.swiftCreateMetadata("bcontainer",metaData);
  containerDetails->getResponse()->write(cout);

  cout<<endl<<endl;
  containerDetails = container.swiftShowMetadata("bcontainer");
  containerDetails->getResponse()->write(cout);

  cout<<endl<<endl;
  vector<string> vec;
  vec.push_back("metaData1key");
  containerDetails = container.swiftDeleteMetadata("bcontainer",vec);
  containerDetails->getResponse()->write(cout);

  //Create a container
  Container container(result->getPayload(), "Container2");
  container.setName("Container2");
  SwiftResult<istream*>* objResult = container.swiftGetObjects(HEADER_FORMAT_APPLICATION_XML,nullptr,true);
  objResult->getResponse()->write(cout);
  StreamCopier::copyStream(*objResult->getPayload(),std::cout);

  //Create an object
  Object newObject(&container,"gholiOBJ");
  string data = "Hello Sexy World :)";
  SwiftResult<void*>* createResult = newObject.swiftCreateReplaceObject(data.c_str(),data.size(),true);
  createResult->getResponse()->write(cout);

  SwiftResult<void*>* copyResult = newObject.swiftCopyObject("CopyGholi",container);
  copyResult->getResponse()->write(cout);

  cout<<"DeleteResult:"<<endl;
  Object copyNew(&container,"CopyGholi");
  SwiftResult<istream*>* deleteResult = copyNew.swiftDeleteObject(false);
  deleteResult->getResponse()->write(cout);

  vector<HTTPHeader> _uriParams;
  SwiftResult<istream*>* objResult = newObject.swiftGetObjectContent(&_uriParams,nullptr);
  objResult->getResponse()->write(cout);
  StreamCopier::copyStream(*objResult->getPayload(),std::cout);

  map<string, string> metaDataMap;
  metaDataMap.insert(make_pair("Key1","metaData Gholi Value 1"));
  SwiftResult<istream*>* createMetaResult = newObject.swiftCreateMetadata("gholiOBJ",metaDataMap);
  createMetaResult->getResponse()->write(cout);
  if(createMetaResult->getPayload()!=nullptr)
  StreamCopier::copyStream(*createMetaResult->getPayload(),std::cout);

  cout<<endl<<endl<<"MetaDataResult:"<<endl;
  SwiftResult<void*>* metaDataShowResult = newObject.swiftShowMetadata("gholiOBJ");
  metaDataShowResult->getResponse()->write(cout);
  cout<<endl<<endl;

  metaData.clear();
  metaData.insert(make_pair("Key2","metaData Gholi Value 2"));
  newObject.swiftCreateMetadata("gholiOBJ",metaData);


  metaData.clear();
  metaData.insert(make_pair("Key3","Value 3"));
  newObject.swiftCreateMetadata(metaData);

  vector<string> deleteVec;
  deleteVec.push_back("Key3");
  deleteVec.push_back("Key1");
  deleteVec.push_back("Key2");
  deleteVec.push_back("Key4");
  //newObject.swiftDeleteMetadata("gholiOBJ",deleteVec);

  cout<<endl<<endl<<"MetaDataResult:"<<endl;
  metaDataShowResult = newObject.swiftShowMetadata();
  metaDataShowResult->getResponse()->write(cout);
  cout<<endl<<endl;


  char data[] = "hello fucking world";
  membuf sbuf(data, data + sizeof(data));
  std::istream in(&sbuf);
  //stream.rdbuf()->pubsetbuf(data,strlen(data));
  //_objectName, inputStream, _uriParams,_reqMap)
  SwiftResult<void*>* createResult = newObject.swiftCreateReplaceObject(in,nullptr,nullptr);
  createResult->getResponse()->write(cout);

  cout<<endl<<endl;
  SwiftResult<void*>* copyResult = newObject.swiftCopyObject("COpyGholi",container,nullptr);
  copyResult->getResponse()->write(cout);
  cout<<endl<<endl;

  cout<<endl<<endl;
  SwiftResult<istream*>* deleteResult = newObject.swiftDeleteObject(true);
  deleteResult->getResponse()->write(cout);
  cout<<endl<<endl;

  SwiftResult<istream*>* objResult = newObject.swiftGetObjectContent(nullptr,nullptr);
  objResult->getResponse()->write(cout);
  StreamCopier::copyStream(*objResult->getPayload(),std::cout);

  //doSwiftTransaction<void*>(nullptr,nullptr,"",nullptr,nullptr,nullptr);*/
  cout << endl << endl;

}
