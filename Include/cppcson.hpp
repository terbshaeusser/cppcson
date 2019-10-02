#pragma once
#include <cinttypes>
#include <functional>
#include <istream>
#include <map>

namespace cppcson {

class Location {
private:
  uint32_t startLine;
  uint32_t startColumn;
  uint32_t endLine;
  uint32_t endColumn;

public:
  explicit Location(uint32_t startLine, uint32_t startColumn, uint32_t endLine,
                    uint32_t endColumn);

  static Location unknown();

  uint32_t getStartLine() const;

  uint32_t getStartColumn() const;

  uint32_t getEndLine() const;

  uint32_t getEndColumn() const;

  bool operator==(const Location &other) const;

  bool operator!=(const Location &other) const;

  friend std::ostream &operator<<(std::ostream &os, const Location &location);
};

enum class ErrorKind { InvalidType, OutOfRange, MissingKey };

#ifndef CPPCSON_DISABLE_EXCEPTIONS
class ParseError : public std::runtime_error {
private:
  ErrorKind errorKind;
  Location location;

public:
  explicit ParseError(const std::string &message, ErrorKind errorKind,
                      const Location &location);

  ErrorKind getErrorKind() const;

  const Location &getLocation() const;
};
#endif

class Value {
private:
  enum class Kind : int8_t { Bool, Int, Float, String, Null, Array, Object };

  union NonStrValue {
    bool boolValue;
    int64_t intValue;
    double floatValue;
    const Value *arrayValue;
    const std::map<std::string, Value> *objectValue;
  };

  Kind kind;
  bool faulty;
  uint32_t itemCount;
  Location location;
  Value *parent;
  std::string key;
  std::string strValue;
  NonStrValue nonStrValue;

  static const char *toString(Kind kind);

  explicit Value(Kind kind, bool faulty, uint32_t itemCount,
                 const Location &location, Value *parent,
                 const std::string &key, const std::string &strValue,
                 const NonStrValue &nonStrValue);

  void ensureKind(Kind expected) const;

public:
  Value(Value &&other) noexcept;

  Value(const Value &) = delete;

  ~Value();

  bool isFaulty() const;

  uint32_t getItemCount() const;

  const Location &getLocation() const;

  bool isBool() const;

  bool isInt() const;

  bool isFloat() const;

  bool isString() const;

  bool isNull() const;

  bool isArray() const;

  bool isObject() const;

  bool asBool() const;

  int64_t asInt() const;

  double asFloat() const;

  const std::string &asString() const;

  const Value &asNull() const;

  const Value &asArray() const;

  const Value &asObject() const;

  const Value &item(uint32_t index) const;

  const Value &item(const std::string &key) const;

  bool contains(const std::string &key) const;

  std::string getPath() const;

  Value &operator=(Value &&other) noexcept;

  bool operator==(const Value &other) const;

  bool operator!=(const Value &other) const;

  friend std::ostream &operator<<(std::ostream &os, const Value &value);
};

struct Options {
  uint32_t maxDepth;
  std::function<void(const std::string &message, ErrorKind errorKind,
                     const Location &location)>
      onParseError;
};

extern const Options DEFAULT_OPTIONS;

Value parse(std::istream &stream, const Options &options = DEFAULT_OPTIONS);

} // namespace cppcson
