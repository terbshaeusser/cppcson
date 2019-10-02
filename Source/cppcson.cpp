#include "cppcson.hpp"
#include <cstring>
#ifdef CPPCSON_DISABLE_EXCEPTIONS
#include <iostream>
#endif

namespace cppcson {

static const std::map<std::string, Value> EMPTY_MAP;

static std::string escape(const std::string &str) {
  // TODO: escape
  return "";
}

static std::string escapeKey(const std::string &str) {
  // TODO: escapeKey
  return "";
}

Location::Location(uint32_t startLine, uint32_t startColumn, uint32_t endLine,
                   uint32_t endColumn)
    : startLine(startLine), startColumn(startColumn), endLine(endLine),
      endColumn(endColumn) {}

Location Location::unknown() { return Location(0, 0, 0, 0); }

uint32_t Location::getStartLine() const { return startLine; }

uint32_t Location::getStartColumn() const { return startColumn; }

uint32_t Location::getEndLine() const { return endLine; }

uint32_t Location::getEndColumn() const { return endColumn; }

bool Location::operator==(const Location &other) const {
  return startLine == other.startLine && startColumn == other.startColumn &&
         endLine == other.endLine && endColumn == other.endColumn;
}

bool Location::operator!=(const Location &other) const {
  return startLine != other.startLine || startColumn != other.startColumn ||
         endLine != other.endLine || endColumn != other.endColumn;
}

std::ostream &operator<<(std::ostream &os, const Location &location) {
  return os << "Location(startLine: " << location.startLine
            << ", startColumn: " << location.startColumn
            << ", endLine: " << location.endLine
            << ", startColumn: " << location.endColumn << ")";
}

#ifdef CPPCSON_DISABLE_EXCEPTIONS
[[noreturn]] static void throwError(const std::string &message,
                                    ErrorKind errorKind,
                                    const Location &location) {
  if (location != Location::unknown()) {
    std::cout << location << ": ";
  }

  std::cout << "Error: " << message << std::endl;
  abort();
}

[[noreturn]] static void unreachable() {
  std::cout << "Unreachable code reached" << std::endl;
  abort();
}
#else
ParseError::ParseError(const std::string &message, ErrorKind errorKind,
                       const Location &location)
    : std::runtime_error(message), errorKind(errorKind), location(location) {}

ErrorKind ParseError::getErrorKind() const { return errorKind; }

const Location &ParseError::getLocation() const { return location; }

[[noreturn]] static void throwError(const std::string &message,
                                    ErrorKind errorKind,
                                    const Location &location) {
  throw ParseError(message, errorKind, location);
}

[[noreturn]] static void unreachable() {
  throw std::runtime_error("Unreachable code reached");
}
#endif

const char *Value::toString(Value::Kind kind) {
  switch (kind) {
  case Kind::Bool:
    return "boolean";
  case Kind::Int:
    return "integer";
  case Kind::Float:
    return "float";
  case Kind::String:
    return "string";
  case Kind::Null:
    return "null";
  case Kind::Array:
    return "array";
  case Kind::Object:
    return "object";
  default:
    unreachable();
  }
}

Value::Value(Value::Kind kind, bool faulty, uint32_t itemCount,
             const Location &location, Value *parent, const std::string &key,
             const std::string &strValue, const Value::NonStrValue &nonStrValue)
    : kind(kind), faulty(faulty), itemCount(itemCount), location(location),
      parent(parent), key(key), strValue(strValue), nonStrValue(nonStrValue) {}

void Value::ensureKind(Value::Kind expected) const {
  if (kind != expected) {
    throwError(std::string("Expected ") + toString(kind) + " value but found " +
                   toString(expected) + " in " + getPath(),
               ErrorKind::InvalidType, location);
  }
}

Value::Value(Value &&other) noexcept
    : kind(other.kind), faulty(other.faulty), itemCount(other.itemCount),
      location(other.location), parent(other.parent), key(other.key),
      strValue(other.strValue), nonStrValue(other.nonStrValue) {
  switch (kind) {
  case Kind::Array: {
    other.itemCount = 0;
    other.nonStrValue.arrayValue = nullptr;
    break;
  }
  case Kind::Object: {
    other.itemCount = 0;
    other.nonStrValue.objectValue = &EMPTY_MAP;
    break;
  }
  default:
    unreachable();
  }
}

Value::~Value() {
  switch (kind) {
  case Kind::Array: {
    delete nonStrValue.arrayValue;
    break;
  }
  case Kind::Object: {
    if (nonStrValue.objectValue != &EMPTY_MAP) {
      delete nonStrValue.objectValue;
    }
    break;
  }
  default:
    break;
  }
}

bool Value::isFaulty() const { return faulty; }

uint32_t Value::getItemCount() const { return itemCount; }

const Location &Value::getLocation() const { return location; }

bool Value::isBool() const { return kind == Kind::Bool; }

bool Value::isInt() const { return kind == Kind::Int; }

bool Value::isFloat() const { return kind == Kind::Float; }

bool Value::isString() const { return kind == Kind::String; }

bool Value::isNull() const { return kind == Kind::Null; }

bool Value::isArray() const { return kind == Kind::Array; }

bool Value::isObject() const { return kind == Kind::Object; }

bool Value::asBool() const {
  ensureKind(Kind::Bool);

  return nonStrValue.boolValue;
}

int64_t Value::asInt() const {
  ensureKind(Kind::Int);

  return nonStrValue.intValue;
}

double Value::asFloat() const {
  ensureKind(Kind::Float);

  return nonStrValue.floatValue;
}

const std::string &Value::asString() const {
  ensureKind(Kind::String);

  return strValue;
}

const Value &Value::asNull() const {
  ensureKind(Kind::Null);

  return *this;
}

const Value &Value::asArray() const {
  ensureKind(Kind::Array);

  return *this;
}

const Value &Value::asObject() const {
  ensureKind(Kind::Object);

  return *this;
}

const Value &Value::item(uint32_t index) const {
  ensureKind(Kind::Array);

  if (index >= itemCount) {
    throwError("Index " + std::to_string(index) +
                   " exceeds number of available items in " + getPath(),
               ErrorKind::OutOfRange, location);
  }

  return nonStrValue.arrayValue[index];
}

const Value &Value::item(const std::string &key) const {
  ensureKind(Kind::Object);

  auto itr = nonStrValue.objectValue->find(key);
  if (itr == nonStrValue.objectValue->end()) {
    throwError("Key " + key + " does not exist under " + getPath(),
               ErrorKind::MissingKey, location);
  }

  return itr->second;
}

bool Value::contains(const std::string &key) const {
  ensureKind(Kind::Object);

  return nonStrValue.objectValue->find(key) != nonStrValue.objectValue->end();
}

std::string Value::getPath() const {
  return (parent != nullptr ? parent->getPath() : "") + key;
}

Value &Value::operator=(Value &&other) noexcept {
  switch (kind) {
  case Kind::Array: {
    delete nonStrValue.arrayValue;
    break;
  }
  case Kind::Object: {
    if (nonStrValue.objectValue != &EMPTY_MAP) {
      delete nonStrValue.objectValue;
    }
    break;
  }
  default:
    break;
  }

  kind = other.kind;
  faulty = other.faulty;
  itemCount = other.itemCount;
  location = other.location;
  parent = other.parent;
  key = other.key;
  strValue = other.strValue;
  nonStrValue = other.nonStrValue;

  switch (kind) {
  case Kind::Array: {
    other.itemCount = 0;
    other.nonStrValue.arrayValue = nullptr;
    break;
  }
  case Kind::Object: {
    other.itemCount = 0;
    other.nonStrValue.objectValue = &EMPTY_MAP;
    break;
  }
  default:
    break;
  }

  return *this;
}

bool Value::operator==(const Value &other) const {
  if (kind != other.kind) {
    return false;
  }

  switch (kind) {
  case Kind::Bool:
    return nonStrValue.boolValue == other.nonStrValue.boolValue;
  case Kind::Int:
    return nonStrValue.intValue == other.nonStrValue.intValue;
  case Kind::Float:
    return nonStrValue.floatValue == other.nonStrValue.floatValue;
  case Kind::String:
    return strValue == other.strValue;
  case Kind::Null:
    return true;
  case Kind::Array:
    return itemCount == other.itemCount &&
           (itemCount == 0 || std::equal(nonStrValue.arrayValue,
                                         nonStrValue.arrayValue + itemCount,
                                         other.nonStrValue.arrayValue));
  case Kind::Object:
    return itemCount == other.itemCount &&
           (itemCount == 0 ||
            std::equal(nonStrValue.objectValue->begin(),
                       nonStrValue.objectValue->end(),
                       other.nonStrValue.objectValue->begin()));
  default:
    unreachable();
  }
}

bool Value::operator!=(const Value &other) const { return !(*this == other); }

std::ostream &operator<<(std::ostream &os, const Value &value) {
  switch (value.kind) {
  case Value::Kind::Bool:
    return os << (value.nonStrValue.boolValue ? "true" : "false");
  case Value::Kind::Int:
    return os << std::to_string(value.nonStrValue.intValue);
  case Value::Kind::Float:
    return os << std::to_string(value.nonStrValue.floatValue);
  case Value::Kind::String:
    return os << escape(value.strValue);
  case Value::Kind::Null:
    return os << "null";
  case Value::Kind::Array: {
    os << "[";

    if (value.itemCount != 0) {
      for (uint32_t i = 0; i < value.itemCount; ++i) {
        if (i != 0) {
          os << ", ";
        }

        os << value.nonStrValue.arrayValue[i];
      }
    }

    return os << "]";
  }
  case Value::Kind::Object: {
    os << "{";

    if (value.itemCount != 0) {
      auto first = true;

      for (auto &entry : *value.nonStrValue.objectValue) {
        if (first) {
          first = false;
        } else {
          os << ", ";
        }

        os << escapeKey(entry.first) << ": " << entry.second;
      }
    }

    return os << "}";
  }
  default:
    unreachable();
  }
}

const Options DEFAULT_OPTIONS = {1024, throwError};

Value parse(std::istream &stream, const Options &options) {
  // TODO
}

} // namespace cppcson
