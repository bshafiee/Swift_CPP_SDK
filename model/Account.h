/*
 * Account.h
 *
 *  Created on: 2014-05-28
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include "Token.h"
#include "Service.h"
#include "AuthenticationMethod.h"
#include "../interface/SwiftResult.h"
#include <iostream>
#include <vector>

namespace Swift {

/** Associated roles with this account **/
struct Role;

class Account {
private:

	/**
	 * Account user ID
	 */
	std::string userID;

	/**
	 * name
	 */
	std::string name;

	/**
	 * ObjectStore token
	 */
	Token *token;

	/**
	 * The ObjectStore username
	 */
	std::string username;

	/**
	 * The ObjectStore password
	 */
	std::string password;

	/**
	 * The ObjectStore authentication URL (Keystone)
	 */
	std::string authUrl;

	/**
	 * Account Roles
	 */
	std::vector<Role> roles;

	/**
	 * Openstack Services
	 */
	std::vector<Service> services;

	/**
	 * ObjectStore tokens expire after 24 hours. If reauthentication is allowed (default behaviour),
	 * after expiration, Account will reauthenticate itself and get a new authentication token. If
	 * reauthentication is turned off, you will have to manually arrange reauthentication. This mode
	 * is recommended for web servers or otherwise long-living processes.
	 */
	bool allowReauthenticate = true;

	/**
	 * The preferred region will determine what Swift end-point will be chosen. If no preferred region is
	 * set (default), the first applicable end-point will be selected.
	 */
	std::string preferredRegion;

	/**
	 * The delimiter is used to check for directory boundaries. The default will be a '/'.
	 */
	char delimiter = '/';

	/**
	 * The method of authentication. Various options:
	 * <ul>
	 *     <li>
	 *         <b>BASIC</b>; authenticate against Swift itself. Authentication URL, username and password
	 *         must be passed.
	 *     </li>
	 *     <li>
	 *         <b>TEMPAUTH</b>; authenticate against Swift itself. Authentication URL, username and password
	 *         must be passed.
	 *     </li>
	 *     <li>
	 *         <b>KEYSTONE</b> (default); makes use of OpenStack Compute. Authentication URL, username and
	 *         password must be passed. Ideally, tenant ID and/or name are passed as well. API can auto-
	 *         discover the tenant if none is passed and if it can be resolved (one tenant for user).
	 *     </li>
	 * </ul>
	 */
	AuthenticationMethod authenticationMethod = AuthenticationMethod::KEYSTONE;

	/**
	 * Number of calls made to Object Store
	 */
	static ulong numOfCalls;

	/** Helper Methods **/
	void parseEndpoints(Json::Value root);

public:
	virtual ~Account();
	Account();

	/**
	 * Trigger the authentication against Object Store. There are two use cases for this method. The first is
	 * triggered pro-actively by the user by calling authenticate on the client. The second is when the token
	 * has expired and AbstractSecureCommand triggers a re-authentication.
	 * @return the access element including a new token
	 */
	static SwiftResult<Account*>* authenticate(const std::string &_username,
			const std::string &_password, const std::string &_authUrl,
			bool _allowReauthenticate = true, const std::string &_tenantName =
					"");

	/**
	 * The number of bytes stored by the StoredObjects in all Containers in the Account.
	 * @return number of bytes
	 */
	long getBytesUsed();

	/**
	 * The number of StoredObjects in all Containers in the Account.
	 * @return number of StoredObjects
	 */
	int getObjectCount();

	/**
	 * Returns the server time in milliseconds since 1970
	 * @return server time
	 */
	long getServerTime();

	void setAllowReauthenticate(bool _allowReauthenticate);

	/**
	 * Whether reauthentication on expiration of the authentication token is allowed, or this is done
	 * manually by the client
	 * @return whether reauthentication is allowed
	 */
	bool isAllowReauthenticate();

	/**
	 * Increase the call counter which tracks how many calls are made to the ObjectStore
	 */
	static ulong increaseCallCounter();

	/**
	 * Returns the number of HTTP calls that have been made to the ObjectStore server. This could be
	 * useful to check the efficiency of the methods and configuration in use
	 * @return number of calls made to the ObjectStore server
	 */
	static ulong getNumberOfCalls();

	/**
	 * Returns the tenant of the account.
	 * @return the tenant belonging to the Account
	 */
	Tenant* getTenants();

	/**
	 * String representation of this class
	 * @return string containing all the objects of this account
	 */
	std::string toString();

	/**
	 * Get Object Storage service
	 * @return swift service
	 */
	Service* getSwiftService();
};

} /* namespace Swift */
#endif /* ACCOUNT_H_ */
