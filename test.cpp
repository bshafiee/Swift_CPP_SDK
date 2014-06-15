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


struct membuf : std::streambuf
{
    membuf(char* begin, char* end) {
        this->setg(begin, begin, end);
    }
};

int main(int argc, char** argv)
{
  AuthenticationInfo info;
  info.username = "behrooz";
  info.password = "behrooz";
  info.authUrl = "http://192.168.249.109:5000/v2.0/tokens";
  info.tenantName= "BehroozProject";
  info.method = AuthenticationMethod::KEYSTONE;
  SwiftResult<Account*>* result = Account::authenticate(info);

  /*cout<<result->getPayload()->toString();
  SwiftResult<istream*>* accountDetails = result->getPayload()->swiftAccountDetails(HEADER_FORMAT_APPLICATION_JSON);
  StreamCopier::copyStream(*accountDetails->getPayload(),cout);
  cout<<endl<<"X-Account-Object-Count: "<< accountDetails->getResponse()->get("X-Account-Object-Count")<<endl;
  cout<<"X-Account-Bytes-Used: "<< accountDetails->getResponse()->get("X-Account-Bytes-Used")<<endl;
  cout<<"Date: "<< accountDetails->getResponse()->get("Date")<<endl;
  cout<<"X-Trans-Id: "<< accountDetails->getResponse()->get("X-Trans-Id")<<endl;
  vector<pair<string, string> > metaData;
  metaData.push_back(make_pair("metaData1key","metaData2value"));
  SwiftResult<void*>* metaDataRes = result->getPayload()->swiftCreateMetadata(metaData);
  cout<<endl<<endl;
  metaDataRes->getResponse()->write(cout);

  vector<string> deleteKey;
  deleteKey.push_back("metaData1key");
  metaDataRes = result->getPayload()->swiftDeleteMetadata(deleteKey);
  cout<<endl<<endl;
  metaDataRes->getResponse()->write(cout);
  cout<<metaDataRes->getResponse()->get("X-Trans-Id");


  SwiftResult<void*>* metaDataRes = result->getPayload()->swiftShowMetadata();
  cout<<endl<<endl;
  metaDataRes->getResponse()->write(cout);

  Container container(result->getPayload());
  SwiftResult<istream*>* containerDetails = container.swiftGetObjects("bcontainer");
  StreamCopier::copyStream(*containerDetails->getPayload(),cout);

  Container container(result->getPayload());
  SwiftResult<void*>* containerDetails = container.swiftDeleteContainer("Container2");
  containerDetails->getResponse()->write(cout);

  Container container(result->getPayload());
  vector<pair<string, string> > metaData;
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
  containerDetails->getResponse()->write(cout);*/

  //Create a container
  Container container(result->getPayload());
  container.setName("Container2");

  //Create an object
  Object newObject(&container);
  /*string data = "Hello Sexy World :)";
  SwiftResult<void*>* createResult = newObject.swiftCreateReplaceObject("gholiOBJ",data.c_str(),data.size(),true);
  createResult->getResponse()->write(cout);

  SwiftResult<void*>* copyResult = newObject.swiftCopyObject("gholiOBJ","CopyGholi",container);
  copyResult->getResponse()->write(cout);

  cout<<"DeleteResult:"<<endl;
  SwiftResult<istream*>* deleteResult = newObject.swiftDeleteObject("CopyGholi",false);
  deleteResult->getResponse()->write(cout);

  vector<HTTPHeader> _uriParams;
  SwiftResult<istream*>* objResult = newObject.swiftGetObjectContent("gholiOBJ",&_uriParams,nullptr);
  objResult->getResponse()->write(cout);
  StreamCopier::copyStream(*objResult->getPayload(),std::cout);

  map<string, string> metaData;
  metaData.insert(make_pair("Key1","metaData Gholi Value 1"));
  SwiftResult<istream*>* createMetaResult = newObject.swiftCreateMetadata("gholiOBJ",metaData);
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
  newObject.swiftCreateMetadata("gholiOBJ",metaData);

  vector<string> deleteVec;
  deleteVec.push_back("Key3");
  deleteVec.push_back("Key1");
  deleteVec.push_back("Key2");
  deleteVec.push_back("Key4");
  newObject.swiftDeleteMetadata("gholiOBJ",deleteVec);

  cout<<endl<<endl<<"MetaDataResult:"<<endl;
  metaDataShowResult = newObject.swiftShowMetadata("gholiOBJ");
  metaDataShowResult->getResponse()->write(cout);
  cout<<endl<<endl;*/


  char data[] = "hello fucking world";
  membuf sbuf(data, data + sizeof(data));
  std::istream in(&sbuf);
  //stream.rdbuf()->pubsetbuf(data,strlen(data));
  //_objectName, inputStream, _uriParams,_reqMap)
  SwiftResult<void*>* createResult = newObject.swiftCreateReplaceObject("gholiOBJ",in,nullptr,nullptr);
  createResult->getResponse()->write(cout);

  cout<<endl<<endl;
  SwiftResult<void*>* copyResult = newObject.swiftCopyObject("gholiOBJ","COpyGholi",container,nullptr);
  copyResult->getResponse()->write(cout);
  cout<<endl<<endl;

  SwiftResult<istream*>* objResult = newObject.swiftGetObjectContent("gholiOBJ",nullptr,nullptr);
  objResult->getResponse()->write(cout);
  StreamCopier::copyStream(*objResult->getPayload(),std::cout);


  //doSwiftTransaction<void*>(nullptr,nullptr,"",nullptr,nullptr,nullptr);

}
