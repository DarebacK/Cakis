#pragma once

#include <exception>
#include <string>

namespace De
{
  class Exception : public std::exception
  {
  public:
    explicit Exception(std::string message);
    explicit Exception(const char* message);
    Exception(const Exception& other) = default;
    Exception(Exception&& other) = default;
    Exception& operator=(const Exception& rhs) = default;
    Exception& operator=(Exception&& rhs) = default;
    virtual ~Exception() = default;

    const char* what() const noexcept override;

    std::string message;
  };
}

#define DECLARE_AND_DEFINE_SIMPLE_EXCEPTION(name) \
class name : public De::Exception \
{ \
public: \
  explicit name(const std::string& message) \
    :De::Exception(#name": " + message) \
  {} \
  explicit name(const char* message) \
    :De::Exception(#name": " + std::string(message)) \
  {} \
};