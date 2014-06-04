/*
 * SwiftAPI.h
 *
 *  Created on: 2014-06-02
 *      Author: Behrooz Shafiee Sarjaz
 */

#ifndef SWIFTAPI_H_
#define SWIFTAPI_H_

#include <iostream>
#include "SwiftResult.h"
#include "../model/Account.h"
#include "../io/HTTPIO.h"

namespace Swift {

/** SWIFT API Interfaces **/
//Accounts Operations
/*SwiftResult<Account*>* swiftAccountAuthenticate(
    const AuthenticationInfo &_authInfo, bool _allowReauthenticate = true);
SwiftResult<std::istream*>* swiftAccountDetails(Account &_account,
    HTTPIO::HTTPParamMap *_reqParamMap = nullptr);
SwiftResult<std::istream*>* swiftAccountCreateMetadata(Account &_account,
    const std::string &metadataName, const std::string &metadataValue,
    HTTPIO::HTTPParamMap *_reqParamMap = nullptr);
SwiftResult<std::istream*>* swiftAccountUpdateMetadata(Account &_account,
    const std::string &metadataName, const std::string &metadataValue,
    HTTPIO::HTTPParamMap *_reqParamMap = nullptr);
SwiftResult<std::istream*>* swiftAccountRemoveMetadata(Account &_account,
    const std::string &metadataName, HTTPIO::HTTPParamMap *_reqParamMap =
        nullptr);*/

//Container Operations
//Object Operations
}
/* namespace Swift */
#endif /* SWIFTAPI_H_ */
