Swift_CPP_SDK Version 0.1 (Beta)
=============

Build:

Usage:
Swift is a distributed object storages service which is part of openstack project. This SDK still lacks many functionalities and is under active development. There are three important entities in this SDK corresponding to Swift architecture elements. These three elements represent Swift hierarchy. The first element which is the root level in Swift hierarchy is Account which contains authentication information and is a namespace for containers. The second entity is Container, which is a namespace for objects. Finally, there is Object entity which correspond to a single object or file in our storage. In order to use Swift, you need to have an account first, then you can have different containers whithin that account; finally, you can have multiple objects within a container. In other words, we can correspond an account to a partition or drive, a container to a folder and an object to a file in terms of traditional file storage systems.

Swift provieds a REST api to create,access, and modify object storage elements. This SDK has implemented all the necessary communications with the Swift server and you won't deal with them. In order to use this api you need to first create an account entity correspond to your account on the swift server. Swift provides different authentication mechanism including: BASIC, TEMPAUTH, and KEYSTONE. However, in this SDK only KEYSTONE has been implemented. To create an account we need to fill requred information in an instance of AuthenticationInfo struct:

AuthenticationInfo info;
info.username = "username";
info.password = "****";
info.authUrl = "url to swift authentication server";//e.g: http://10.42.0.83:5000/v2.0/tokens
info.tenantName = "tenant name";
info.method = AuthenticationMethod::KEYSTONE;


Then you can use this information to create an Account instance: 

SwiftResult<Account*>* authenticateResult = Account::authenticate(info);

In this SDK the results of all functions are returned as a pointer to SwiftResult class. SwiftResult class is a template class which holds the information and payload of transaction to Swift server. SwiftResult class has following structure:

template <class T>
class SwiftResult {
  Poco::Net::HTTPResponse *response;
  Poco::Net::HTTPClientSession *session;
  SwiftError error;
  /** Real Data **/
  T payload;
}

response and session, are used to make transaction to the Swift server. They have useful information about your connection to the server, and in case something goes wrong you can use these to find out more about the error. For instance, you can dump your HTTPResponse to a stream (cout) as follows:

response->write(cout);

SwiftError is a class which contains information about the error which happend during performing this transaction. You can see different values of error in the ErrorNo.h file. If the transaction is successful it should have code equal to zer and msg equal to “SWIFT_OK”.  Therefore, the first thing to do after each transaction is to check status of error. 

Payload contains the actual result of a transaction. For instance int our transaction we should expect a pointer to the Account class as payload, and we can access it using getPayload() method of SwiftResult class:

SwiftResult<Account*>* authenticateResult = Account::authenticate(info);
if(authenticateResult->getError() == SWIFT_OK) {
 //Do something with payload
 Account* account = authenticateResult->getPayload();
 ...
} else {
 //Ooops! Something went wrong. 
 cerr <<AuthenticateResult->getError().msg<<endl;
} 


After creating an account, the next step is to create or access to a container. Please note that Swift does not accept white space in the containers or objecst name.


Container container(authenticateResult->getPayload(), "MyContainer");

Notice the use of authentication result (Account) in our container! Now we have a container named “MyContainer”; remember, so far this is only a local instance and nothing has happened on the server. If you want to actually create this container on the server, you should use create function:

//Create container
SwiftResult<int*>* containerResult = container.swiftCreateContainer();

if this operation completes successfully, “MyContainer” will be created on server. In this case the payload does not conatain any data and is just a nullptr. On the other hand, if “MyContainer” already exist and you want to do something with it (or you just created it). You can use its different functions; for example, let's get the list of objects in this container:

SwiftResult<std::vector<Object>*>* objects = container.swiftGetObjects();

Having an account and container ready, we can proceed to create an object:

//Create Object 
Object object(&container, "MyObject");
string data = "Hello Swift :)";
SwiftResult<int*> *objResult = object.swiftCreateReplaceObject(data.c_str(), data.length(), true);

Similar to creating a container, if this operations completes successfully Swift server will create/replace “MyObject”. 

One important point to remember is that because all the results of transactions are allocated dynamically and returned as pointers it's responsibility of user to call delete on them once they are not needed anymore. Otherwise, there will be memory leaks in your program. Putting all this together, the following code snippet would be a very simple and basic usage of this SDK with the swift object storage service:


AuthenticationInfo info;
info.username = "username";
info.password = "****";
info.authUrl = "url to swift authentication server";//e.g: http://10.42.0.83:5000/v2.0/tokens
info.tenantName = "tenant name";
info.method = AuthenticationMethod::KEYSTONE;

//Do authentication
SwiftResult<Account*>* authenticateResult = Account::authenticate(info);
if(authenticateResult->getError() != SWIFT_OK){
 //Ooops! Something went wrong. 
 cerr <<AuthenticateResult->getError().msg<<endl;
 delete authenticateResult;
 return -1;
} 

//Create Container
Container container(authenticateResult->getPayload(), "MyContainer");
SwiftResult<int*>* containerResult = container.swiftCreateContainer();
if(containerResult->getError() != SWIFT_OK){
 //Ooops! Something went wrong. 
 cerr <<containerResult->getError().msg<<endl;
 delete containerResult;
 return -1;
} 
delete containerResult;


//Create Object 
Object object(&container, "MyObject");
string data = "Hello Swift :)";
SwiftResult<int*> *objResult = object.swiftCreateReplaceObject(data.c_str(), data.length(), true);
if(containerResult->getError() != SWIFT_OK){
 //Ooops! Something went wrong. 
 cerr <<containerResult->getError().msg<<endl;
 delete containerResult;
 return -1;
}
delete containerResult; 

//Done with authenticate result as well
delete authenticateResult;
return 0;


In the following, there is a list of important functions of each class and a brief explanation about them.

Class Account:
Class Container:
Class Object:


