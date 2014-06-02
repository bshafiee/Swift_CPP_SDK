/*
 * Account.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Account.h"
#include "../io/HTTPIO.h"
#include <Poco/Exception.h>

using namespace std;
using namespace Poco;
using namespace Poco::Net;

namespace Swift {

/** Initialize Static members **/
ulong Account::numOfCalls = 0;

struct Role {
	string name = "null";
	Role() {
	}
	static Role* fromJSON(Json::Value value) {
		Role* instance = new Role();
		instance->name = value.get("name", Json::Value::null).asString();
		return instance;
	}

	static Json::Value* toJSON(Role *instance) {
		Json::Value* root = new Json::Value();
		(*root)["name"] = instance->name;
		return root;
	}
};

Account::~Account() {
	// TODO Auto-generated destructor stub
}

Account::Account() :
		userID(""), name(""), token(nullptr), username(""), password(""), authUrl(""),
		allowReauthenticate(false), publicHost(""), preferredRegion(""), delimiter('/'),
		authenticationMethod(AuthenticationMethod::KEYSTONE) {
	// TODO Auto-generated constructor stub

}

SwiftResult<Account*>* Account::authenticate(const string & _username,
		const string & _password, const string &_authUrl,
		bool _allowReauthenticate, const string &_tenantName) {
	// Create Json Request
	Json::Value jReq;
	Json::Value auth;
	if (_tenantName != "")
		auth["tenantName"] = _tenantName;
	auth["passwordCredentials"]["username"] = _username;
	auth["passwordCredentials"]["password"] = _password;
	jReq["auth"] = auth;
	Json::FastWriter writer;

	//Make Network Request
	string req = writer.write(jReq);
	string contentType = "application/json";
	HTTPClientSession *httpSession = nullptr;
	HTTPResponse *httpResponse = new HTTPResponse();
	istream* inputStream = nullptr;
	try {
		httpSession = Swift::HTTPIO::doPost(_authUrl, req, contentType);
		//Receive Data
		inputStream = &httpSession->receiveResponse(*httpResponse);
	} catch (Exception &e) {
		SwiftResult<Account*> *result = new SwiftResult<Account*>();
		SwiftError error(SwiftError::SWIFT_EXCEPTION, e.displayText());
		result->setError(error);
		//Try to set HTTP Response as the payload
		//httpSession->receiveResponse(*httpResponse);
		result->setResponse(httpResponse);
		result->setPayload(nullptr);
		return result;
	}

	//Check HTTP status
	if (httpResponse->getStatus() != HTTPResponse::HTTPStatus::HTTP_OK) {
		SwiftResult<Account*> *result = new SwiftResult<Account*>();
		SwiftError error(SwiftError::SWIFT_HTTP_ERROR,
				httpResponse->getReason());
		result->setError(error);
		result->setResponse(httpResponse);
		result->setPayload(nullptr);
		return result;
	}
	//Everything seems fine
	Json::Value root;   // will contains the root value after parsing.
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(*inputStream, root, true);
	if (!parsingSuccessful) {
		SwiftResult<Account*> *result = new SwiftResult<Account*>();
		SwiftError error(SwiftError::SWIFT_JSON_PARSE_ERROR,
				reader.getFormattedErrorMessages());
		result->setError(error);
		result->setResponse(httpResponse);
		result->setPayload(nullptr);
		return result;
	}

	Json::StyledWriter writer2;
	cout << writer2.write(root) << endl;
	;

	//Parsing JSON Successful
	//Everything is inside "access"
	root = root["access"];
	Account* instance = new Account();
	SwiftResult<Account*> *result = new SwiftResult<Account*>();
	//Parse User Info
	Json::Value userRoot = root.get("user", Json::nullValue);
	instance->userID = userRoot.get("id","").asString();
	instance->name = userRoot.get("name","").asString();
	instance->username = userRoot.get("username","").asString();
	//Roles
	Json::Value roles = root.get("user", Json::nullValue);
	if(roles != Json::nullValue) {
		for(uint i=0;i<roles.size();i++) {
			Role *role = Role::fromJSON(roles[i]);
			instance->roles.push_back(*role);
		}
	}
	//Parse Token
	Json::Value	tokenRoot = root.get("token", Json::nullValue);
	//instance->token = Token::fromJSON(tokenRoot);
	Token::fromJSON(tokenRoot);
	//Parse Endpoints
	Json::Value endpointsRoot = root.get("serviceCatalog", Json::nullValue);
	if(endpointsRoot != Json::nullValue) {
			for(uint i=0;i<endpointsRoot.size();i++) {
				Endpoint *endpoint = Endpoint::fromJSON(endpointsRoot[i]);
				instance->endpoints.push_back(*endpoint);
			}
	}

	return result;
}

long Account::getBytesUsed() {
}

int Account::getObjectCount() {
}

long Account::getServerTime() {
}

void Account::setAllowReauthenticate(bool allowReauthenticate) {
}

void Account::setPublicHost(const string& host) {
}

bool Account::isAllowReauthenticate() {
}

ulong Account::increaseCallCounter() {
	return ++numOfCalls;
}

ulong Account::getNumberOfCalls() {
	return Account::numOfCalls;
}

vector<Tenant> Account::getTenants() {
}

} /* namespace Swift */
