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


using namespace Poco::Net;
using namespace Poco;
using namespace std;

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
  Json::Value jReq;
  Json::Value auth;
  auth["tenantName"] = "BehroozProject";
  auth["passwordCredentials"]["username"] = "behrooz";
  auth["passwordCredentials"]["password"] = "behrooz";
  jReq ["auth"] = auth;
  Json::FastWriter writer;

  string url = "http://192.168.249.109/v2.0/tokens";
  int port = 5000;
  string req = writer.write(jReq);
  string contentType = "application/json";
  HTTPClientSession *session = Swift::HTTPIO::doPost(url,port,req,contentType);
  HTTPResponse response;
  istream& rs = session->receiveResponse(response);
  //StreamCopier::copyStream(rs,std::cout);
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  bool parsingSuccessful = reader.parse(rs,root,true);
  if ( !parsingSuccessful )
  {
      // report to the user the failure and their locations in the document.
      std::cout  << "Failed to parse configuration\n"
                 << reader.getFormattedErrorMessages()<<"\nInput:"<<response.getStatus()<<"\t"<<response.getReason();
      StreamCopier::copyStream(rs,std::cout);
  }
  cout<<root.toStyledString()<<endl;
}
