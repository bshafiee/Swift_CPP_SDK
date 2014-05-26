#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/Path.h>
#include <Poco/URI.h>
#include <Poco/Exception.h>
#include <Poco/HashMap.h>
#include <iostream>
#include <string>
#include "interface/ConfigManager.h"


using namespace Poco::Net;
using namespace Poco;
using namespace std;


int main(int argc, char **argv)
{
  /*const string* res = Swift::ConfigManager::getProperty("fuck");
  if(res == "NULL")
    cout<<"NULLLL";
  else
    cout<<"Before:"<<res<<"\tINJAAAAAAA";;*/

  if (argc != 2)
  {
    cout << "Usage: " << argv[0] << " <uri>" << endl;
    cout << "       fetches the resource identified by <uri> and print it" << endl;
    return -1;
  }

  try
  {
    // prepare session
    URI uri(argv[1]);
    HTTPClientSession session(uri.getHost(), uri.getPort());

    // prepare path
    string path(uri.getPathAndQuery());
    if (path.empty()) path = "/";

    // send request
    HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
    session.sendRequest(req);

    // get response
    HTTPResponse res;
    cout << res.getStatus() << " " << res.getReason() << endl;

    // print response
    istream &is = session.receiveResponse(res);
    StreamCopier::copyStream(is, cout);
  }
  catch (Exception &ex)
  {
    cerr << ex.displayText() << endl;
    return -1;
  }

  return 0;
}
