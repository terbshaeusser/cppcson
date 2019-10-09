#pragma once
#include <cinttypes>
#include <functional>
#include <istream>
#include <map>
#include <vector>

namespace cppcson {

class Location {
private:
  uint32_t startLine;
  uint32_t startColumn;
  uint32_t endLine;
  uint32_t endColumn;

public:
  explicit Location(uint32_t line, uint32_t column);

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

class Error : public std::runtime_error {
private:
  Location location;

public:
  explicit Error(const std::string &message, const Location &location);

  const Location &getLocation() const;
};

class TypeError : public Error {
public:
  explicit TypeError(const std::string &expected, const std::string &actual,
                     const std::string &path, const Location &location);
};

class OutOfRangeError : public Error {
public:
  explicit OutOfRangeError(uint32_t index, const std::string &path,
                           const Location &location);
};

class MissingKeyError : public Error {
public:
  explicit MissingKeyError(const std::string &key, const std::string &path,
                           const Location &location);
};

class SyntaxError : public Error {
public:
  explicit SyntaxError(const std::string &message, const Location &location);
};

class NestingTooDeepError : public Error {
public:
  NestingTooDeepError();
};

class Value;

class Keys {
  friend Value;

private:
  const std::map<std::string, Value> &objectValue;

  explicit Keys(const std::map<std::string, Value> &objectValue);

public:
  struct iterator {
    friend Keys;

  private:
    std::map<std::string, Value>::const_iterator itr;

    explicit iterator(const std::map<std::string, Value>::const_iterator &itr);

  public:
    bool operator==(const iterator &other) const;

    bool operator!=(const iterator &other) const;

    const std::string &operator*() const;

    const std::string &operator->() const;

    iterator &operator++();

    iterator operator++(int);
  };

  iterator begin() const;

  iterator end() const;
};

class Value {
  friend class Parser;

public:
  enum class Kind { Bool, Int, Float, String, Null, Array, Object };

private:
  union NonStrValue {
    bool boolValue;
    int64_t intValue;
    double floatValue;
    const std::vector<Value> *arrayValue;
    const std::map<std::string, Value> *objectValue;
  };

  Kind kind;
  Location location;
  std::string path;
  std::string strValue;
  NonStrValue nonStrValue;

  static const char *toString(Kind kind);

  explicit Value(Kind kind, const Location &location, const std::string &path,
                 const std::string &strValue, const NonStrValue &nonStrValue);

  static Value fromBool(const Location &location, const std::string &path,
                        bool value);

  static Value fromInt(const Location &location, const std::string &path,
                       int64_t value);

  static Value fromFloat(const Location &location, const std::string &path,
                         double value);

  static Value fromString(const Location &location, const std::string &path,
                          const std::string &value);

  static Value fromNull(const Location &location, const std::string &path);

  static Value fromArray(const Location &location, const std::string &path,
                         const std::vector<Value> *arrayValue);

  static Value fromObject(const Location &location, const std::string &path,
                          const std::map<std::string, Value> *objectValue);

  void release();

  void ensureKind(Kind expected) const;

public:
  using iterator = std::vector<Value>::const_iterator;

  Value(Value &&other) noexcept;

  Value(const Value &) = delete;

  ~Value();

  uint32_t getItemCount() const;

  const Location &getLocation() const;

  const std::string &getPath() const;

  Kind getKind() const;

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

  Keys keys() const;

  iterator begin() const;

  iterator end() const;

  Value &operator=(Value &&other) noexcept;

  bool operator==(const Value &other) const;

  bool operator!=(const Value &other) const;

  friend std::ostream &operator<<(std::ostream &os, const Value &value);
};

struct Options {
  uint32_t maxDepth;
};

extern const Options DEFAULT_OPTIONS;

Value parse(std::istream &stream, const Options &options = DEFAULT_OPTIONS);

std::string escapeKey(const std::string &str);

std::string escape(const std::string &str);

} // namespace cppcson
