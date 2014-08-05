/*
 * Token.cpp
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#include "Token.h"

namespace Swift {

Token::Token() :
		expires(""), id(""), issued_at(""), tenant(nullptr) {
	// TODO Auto-generated constructor stub

}

Token::Token(const std::string& _expires, const std::string& _id,
		const std::string& _issued_at, Tenant* _tenant) {
	this->expires = _expires;
	this->id = _id;
	this->issued_at = _issued_at;
	this->tenant = _tenant;
}

Token::~Token() {
	delete tenant;
	tenant = nullptr;
}

Token* Token::fromJSON(const Json::Value &val) {
	Token *instance = new Token();
	instance->setExpires(val.get("expires", Json::Value::null).asString());
	instance->setId(val.get("id", Json::Value::null).asString());
	instance->setIssuedAt(val.get("issued_at", Json::Value::null).asString());
	Json::Value tenant = val.get("tenant", Json::Value::null);
	instance->setTenant(Tenant::fromJSON(tenant));

	return instance;
}

Json::Value* Token::toJSON(const Token &instance) {
	Json::Value* json = new Json::Value();

	Json::Value* tenantJSON = Tenant::toJSON(*instance.getTenant());

	(*json)["id"] = instance.getId();
	(*json)["expires"] = instance.getExpires();
	(*json)["issued_at"] = instance.getIssuedAt();
	(*json)["tenant"] = *tenantJSON;

	delete tenantJSON;
	tenantJSON = nullptr;
	return json;
}

const std::string& Token::getExpires() const {
	return expires;
}

void Token::setExpires(const std::string& expires) {
	this->expires = expires;
}

const std::string& Token::getId() const {
	return id;
}

void Token::setId(const std::string& id) {
	this->id = id;
}

const std::string& Token::getIssuedAt() const {
	return issued_at;
}

void Token::setIssuedAt(const std::string& issuedAt) {
	issued_at = issuedAt;
}

Tenant* Token::getTenant() const {
	return tenant;
}

void Token::setTenant(Tenant* tenant) {
	this->tenant = tenant;
}

} /* namespace Swift */
