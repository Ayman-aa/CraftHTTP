/* -- httpError.hpp -- */

# ifndef HTTP_ERROR_HPP
#define HTTP_ERROR_HPP

#include <string>
#include <stdexcept>

enum ErrorCode {
    BadRequest = 400,
    NotFound = 404,
    MethodNotAllowed = 405,
	HTTPVersionNotSupported = 505
};

class HttpError : public std::runtime_error {
private:
    ErrorCode errorCode;

public:
    HttpError(ErrorCode code, const std::string& message);
    ErrorCode getErrorCode() const;
};

#endif
