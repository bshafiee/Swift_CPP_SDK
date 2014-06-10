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


using namespace Poco::Net;
using namespace Poco;
using namespace std;
using namespace Swift;

int main(int argc, char** argv)
{
  /*Poco::Net::SocketAddress sa("www.appinf.com", 80);
  Poco::Net::StreamSocket socket(sa);
  Poco::Net::SocketStream str(socket);*/

  /*HTTPClientSession session("www.google.com");
  HTTPRequest request(HTTPRequest::HTTP_GET);
  session.sendRequest(request);

  HTTPResponse response;
  std::istream& rs = session.receiveResponse(response);
  StreamCopier::copyStream(rs,std::cout);


  Json::Value root;
  Json::Reader reader;
  const char *temp = "{\"firstName\": \"John\",\"lastName\": \"Smith\"}";
  reader.parse(temp,root,false);
  std::string encoding = root.get("firstName", "Not Found" ).asString();
  std::cout<<std::endl<<encoding<<std::endl;

  Json::Value mydoc;
  Json::Value mydoc2;
  mydoc2["key1"] = "sdfsdf";
  //key1 {"sdfds",{"ssf"}}

  mydoc["key1"] = {};
  mydoc["key1"]["name"] = "jafar";
  mydoc["key2"] = "value 2";
  mydoc["key3"]["key3_1"] = "value 1_1";
  mydoc["key3"]["key31"] = "value 1_1";
  mydoc["key1"]["sdf"] = "sdfsdf";
  Json::StyledWriter writer;
  // Make a new JSON document for the configuration. Preserve original comments.
  std::string outputConfig = writer.write( mydoc );

  // And you can write to a stream, using the StyledWriter automatically.
  std::cout << mydoc;
  */

  /*
    {
      "auth":
        {
          "tenantName": "BehroozProject",
          "passwordCredentials":
            {
              "username": "behrooz",
              "password": "behrooz"
            }
        }
    }
   */
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
  string data = "Hello Sexy World :)";
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

}
