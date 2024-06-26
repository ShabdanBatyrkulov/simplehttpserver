#ifndef HTTP_MESSAGE_H_
#define HTTP_MESSAGE_H_

#include <map>
#include <string>

namespace http_server {

enum class HttpMethod {
    GET,
    HEAD
};

enum class HttpVersion {
    HTTP_1_1 = 11,
};

enum class HttpStatusCode {
    Continue = 100,
    SwitchingProtocols = 101,
    EarlyHints = 103,
    Ok = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    NotModified = 304,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    RequestTimeout = 408,
    ImATeapot = 418,
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnvailable = 503,
    GatewayTimeout = 504,
    HttpVersionNotSupported = 505
};

// Utility functions
std::string to_string(HttpMethod method);
std::string to_string(HttpVersion version);
std::string to_string(HttpStatusCode status_code);
HttpMethod string_to_method(const std::string& method_string);
HttpVersion string_to_version(const std::string& version_string);

class HttpMessageInterface {
 public:
  HttpMessageInterface() : version_(HttpVersion::HTTP_1_1) {}
  virtual ~HttpMessageInterface() = default;

  void SetHeader(const std::string& key, const std::string& value) {
    headers_[key] = std::move(value);
  }
  void RemoveHeader(const std::string& key) { headers_.erase(key); }
  void ClearHeader() { headers_.clear(); }
  void SetContent(const std::string& content) {
    content_ = std::move(content);
    SetContentLength();
  }
  void ClearContent(const std::string& content) {
    content_.clear();
    SetContentLength();
  }

  HttpVersion version() const { return version_; }
  std::string header(const std::string& key) const {
    if (headers_.count(key) > 0) return headers_.at(key);
    return std::string();
  }
  std::map<std::string, std::string> headers() const { return headers_; }
  std::string content() const { return content_; }
  size_t content_length() const { return content_.length(); }

 protected:
  HttpVersion version_;
  std::map<std::string, std::string> headers_;
  std::string content_;

  void SetContentLength() {
    SetHeader("Content-Length", std::to_string(content_.length()));
  }
};

class HttpRequest : public HttpMessageInterface {
 public:
  HttpRequest() : method_(HttpMethod::GET) {}
  ~HttpRequest() = default;

  void SetMethod(HttpMethod method) { method_ = method; }
  void SetUri(const std::string& uri) { uri_ = std::move(uri); }

  HttpMethod method() const { return method_; }
  std::string uri() const { return uri_; }

  friend std::string to_string(const HttpRequest& request);
  friend HttpRequest string_to_request(const std::string& request_string);

 private:
  HttpMethod method_;
  std::string uri_;
};

class HttpResponse : public HttpMessageInterface {
 public:
  HttpResponse() : status_code_(HttpStatusCode::Ok) {}
  HttpResponse(HttpStatusCode status_code) : status_code_(status_code) {}
  ~HttpResponse() = default;

  void SetStatusCode(HttpStatusCode status_code) { status_code_ = status_code; }

  HttpStatusCode status_code() const { return status_code_; }

  friend std::string to_string(const HttpResponse& request, bool send_content);
  friend HttpResponse string_to_response(const std::string& response_string);

 private:
  HttpStatusCode status_code_;
};

// Utility functions
std::string to_string(const HttpRequest& request);
std::string to_string(const HttpResponse& response, bool send_content = true);
HttpRequest string_to_request(const std::string& request_string);
HttpResponse string_to_response(const std::string& response_string);

} 

#endif // HTTP_MESSAGE_H_