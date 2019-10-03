#include "cppcson.hpp"
#include <cmath>

namespace cppcson {

static const std::vector<Value> EMPTY_VECTOR;
static const std::map<std::string, Value> EMPTY_MAP;

static Location combine(const Location &start, uint32_t endLine,
                        uint32_t endColumn) {
  return Location(start.getStartLine(), start.getStartColumn(), endLine,
                  endColumn);
}

static Location combine(const Location &start, const Location &end) {
  return Location(start.getStartLine(), start.getStartColumn(),
                  end.getEndLine(), end.getEndColumn());
}

Location::Location(uint32_t line, uint32_t column)
    : startLine(line), startColumn(column), endLine(line), endColumn(column) {}

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

Error::Error(const std::string &message, const Location &location)
    : std::runtime_error(message), location(location) {}

const Location &Error::getLocation() const { return location; }

TypeError::TypeError(const std::string &expected, const std::string &actual,
                     const std::string &path, const Location &location)
    : Error("Expected " + expected + " value but found " + actual + " in " +
                path,
            location) {}

OutOfRangeError::OutOfRangeError(uint32_t index, const std::string &path,
                                 const Location &location)
    : Error("Index " + std::to_string(index) +
                " exceeds number of available items in " + path,
            location) {}

MissingKeyError::MissingKeyError(const std::string &key,
                                 const std::string &path,
                                 const Location &location)
    : Error("Key " + key + " does not exist under " + path, location) {}

SyntaxError::SyntaxError(const std::string &message, const Location &location)
    : Error(message, location) {}

NestingTooDeepError::NestingTooDeepError()
    : Error("Nesting of data is too deep", Location::unknown()) {}

[[noreturn]] static void unreachable() {
  throw std::runtime_error("Unreachable code reached");
}

Keys::Keys(const std::map<std::string, Value> &objectValue)
    : objectValue(objectValue) {}

Keys::iterator::iterator(
    const std::map<std::string, Value>::const_iterator &itr)
    : itr(itr) {}

bool Keys::iterator::operator==(const Keys::iterator &other) const {
  return itr == other.itr;
}

bool Keys::iterator::operator!=(const Keys::iterator &other) const {
  return itr != other.itr;
}

const std::string &Keys::iterator::operator*() const { return itr->first; }

const std::string &Keys::iterator::operator->() const { return itr->first; }

Keys::iterator &Keys::iterator::operator++() {
  ++itr;
  return *this;
}

Keys::iterator Keys::iterator::operator++(int) {
  auto result = *this;
  ++itr;
  return result;
}

Keys::iterator Keys::begin() const { return iterator(objectValue.begin()); }

Keys::iterator Keys::end() const { return iterator(objectValue.end()); }

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

Value::Value(Kind kind, const Location &location, const std::string &path,
             const std::string &strValue, const Value::NonStrValue &nonStrValue)
    : kind(kind), location(location), path(path), strValue(strValue),
      nonStrValue(nonStrValue) {}

Value Value::fromBool(const Location &location, const std::string &path,
                      bool value) {
  NonStrValue nonStrValue{false};
  nonStrValue.boolValue = value;

  return Value(Kind::Bool, location, path, "", nonStrValue);
}

Value Value::fromInt(const Location &location, const std::string &path,
                     int64_t value) {
  NonStrValue nonStrValue{false};
  nonStrValue.intValue = value;

  return Value(Kind::Int, location, path, "", nonStrValue);
}

Value Value::fromFloat(const Location &location, const std::string &path,
                       double value) {
  NonStrValue nonStrValue{false};
  nonStrValue.floatValue = value;

  return Value(Kind::Float, location, path, "", nonStrValue);
}

Value Value::fromString(const Location &location, const std::string &path,
                        const std::string &value) {
  return Value(Kind::String, location, path, value, NonStrValue());
}

Value Value::fromNull(const Location &location, const std::string &path) {
  return Value(Kind::Null, location, path, "", NonStrValue());
}

Value Value::fromArray(const Location &location, const std::string &path,
                       const std::vector<Value> *arrayValue) {
  NonStrValue nonStrValue{false};
  nonStrValue.arrayValue = arrayValue;

  return Value(Kind::Array, location, path, "", nonStrValue);
}

Value Value::fromObject(const Location &location, const std::string &path,
                        const std::map<std::string, Value> *objectValue) {
  NonStrValue nonStrValue{false};
  nonStrValue.objectValue = objectValue;

  return Value(Kind::Object, location, path, "", nonStrValue);
}

void Value::release() {
  switch (kind) {
  case Kind::Array: {
    if (nonStrValue.arrayValue != &EMPTY_VECTOR) {
      delete nonStrValue.arrayValue;
    }
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

void Value::ensureKind(Value::Kind expected) const {
  if (kind != expected) {
    throw TypeError(toString(expected), toString(kind), getPath(), location);
  }
}

Value::Value(Value &&other) noexcept
    : kind(other.kind), location(other.location), path(other.path),
      strValue(other.strValue), nonStrValue(other.nonStrValue) {
  switch (kind) {
  case Kind::Array: {
    other.nonStrValue.arrayValue = &EMPTY_VECTOR;
    break;
  }
  case Kind::Object: {
    other.nonStrValue.objectValue = &EMPTY_MAP;
    break;
  }
  default:
    break;
  }
}

Value::~Value() { release(); }

uint32_t Value::getItemCount() const {
  switch (kind) {
  case Kind::Array:
    return nonStrValue.arrayValue->size();
  case Kind::Object:
    return nonStrValue.objectValue->size();
  default:
    return 0;
  }
}

const Location &Value::getLocation() const { return location; }

const std::string &Value::getPath() const { return path; }

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

  if (index >= nonStrValue.arrayValue->size()) {
    throw OutOfRangeError(index, getPath(), location);
  }

  return nonStrValue.arrayValue->at(index);
}

const Value &Value::item(const std::string &key) const {
  ensureKind(Kind::Object);

  auto itr = nonStrValue.objectValue->find(key);
  if (itr == nonStrValue.objectValue->end()) {
    throw MissingKeyError(key, getPath(), location);
  }

  return itr->second;
}

bool Value::contains(const std::string &key) const {
  ensureKind(Kind::Object);

  return nonStrValue.objectValue->find(key) != nonStrValue.objectValue->end();
}

Keys Value::keys() const {
  ensureKind(Kind::Object);

  return Keys(*nonStrValue.objectValue);
}

Value::iterator Value::begin() const {
  ensureKind(Kind::Array);

  return nonStrValue.arrayValue->begin();
}

Value::iterator Value::end() const {
  ensureKind(Kind::Array);

  return nonStrValue.arrayValue->end();
}

Value &Value::operator=(Value &&other) noexcept {
  release();

  kind = other.kind;
  location = other.location;
  path = other.path;
  strValue = other.strValue;
  nonStrValue = other.nonStrValue;

  switch (kind) {
  case Kind::Array: {
    other.nonStrValue.arrayValue = &EMPTY_VECTOR;
    break;
  }
  case Kind::Object: {
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
    return nonStrValue.arrayValue->size() ==
               other.nonStrValue.arrayValue->size() &&
           std::equal(nonStrValue.arrayValue->begin(),
                      nonStrValue.arrayValue->end(),
                      other.nonStrValue.arrayValue->begin());
  case Kind::Object:
    return nonStrValue.objectValue->size() ==
               other.nonStrValue.objectValue->size() &&
           std::equal(nonStrValue.objectValue->begin(),
                      nonStrValue.objectValue->end(),
                      other.nonStrValue.objectValue->begin());
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

    auto first = true;
    for (auto &itemValue : *value.nonStrValue.arrayValue) {
      if (first) {
        first = false;
      } else {
        os << ", ";
      }

      os << itemValue;
    }

    return os << "]";
  }
  case Value::Kind::Object: {
    os << "{";

    auto first = true;
    for (auto &entry : *value.nonStrValue.objectValue) {
      if (first) {
        first = false;
      } else {
        os << ", ";
      }

      os << escapeKey(entry.first) << ": " << entry.second;
    }

    return os << "}";
  }
  default:
    unreachable();
  }
}

enum class TokenKind {
  EoD,
  Key,
  Int,
  Float,
  String,

  Comma,
  Colon,
  OpenBrace,
  CloseBrace,
  OpenCurly,
  CloseCurly,

  Null,
  False,
  True
};

static std::string toString(TokenKind kind) {
  switch (kind) {
  case TokenKind::EoD:
    return "end of data";
  case TokenKind::Key:
    return "key";
  case TokenKind::Int:
    return "integer";
  case TokenKind::Float:
    return "float";
  case TokenKind::String:
    return "string";
  case TokenKind::Comma:
    return ",";
  case TokenKind::Colon:
    return ":";
  case TokenKind::OpenBrace:
    return "[";
  case TokenKind::CloseBrace:
    return "]";
  case TokenKind::OpenCurly:
    return "{";
  case TokenKind::CloseCurly:
    return "}";
  case TokenKind::Null:
    return "null";
  case TokenKind::False:
    return "false";
  case TokenKind::True:
    return "true";
  default:
    unreachable();
  }
}

struct Token {
  TokenKind kind;
  Location location;
  std::string strValue;
  union {
    int64_t intValue;
    double floatValue;
  };

  Token() : kind(TokenKind::EoD), location(Location::unknown()), intValue(0) {}

  explicit Token(TokenKind kind, const Location &location)
      : kind(kind), location(location), intValue(0) {}

  explicit Token(const Location &location, int64_t value)
      : kind(TokenKind::Int), location(location), intValue(value) {}

  explicit Token(TokenKind kind, const Location &location,
                 const std::string &value)
      : kind(kind), location(location), strValue(value) {}

  static Token fromFloat(const Location &location, double value) {
    Token token;
    token.kind = TokenKind::Float;
    token.location = location;
    token.floatValue = value;
    return token;
  }
};

static std::string toString(const Token &token) {
  switch (token.kind) {
  case TokenKind::Key:
  case TokenKind::String:
    return token.strValue;
  case TokenKind::Int:
    return std::to_string(token.intValue);
  case TokenKind::Float:
    return std::to_string(token.floatValue);
  default:
    return toString(token.kind);
  }
}

class Parser {
private:
  std::istream &stream;
  const Options &options;
  uint32_t nextLine;
  uint32_t nextColumn;
  uint32_t objectIndent;
  bool hasLookahead;
  Token lookaheadToken;
  uint32_t depth;

  struct DepthHandler {
  private:
    uint32_t &ref;

  public:
    explicit DepthHandler(const Options &options, uint32_t &ref) : ref(ref) {
      if (ref > options.maxDepth) {
        throw NestingTooDeepError();
      }

      ++ref;
    }

    ~DepthHandler() { --ref; }
  };

  char nextChar(uint32_t &line, uint32_t &column) {
    auto c = stream.get();
    line = nextLine;
    column = nextColumn;

    if (stream.fail() || c == std::char_traits<char>::eof()) {
      return 0;
    }

    if (c == '\n') {
      ++nextLine;
      nextColumn = 1;
    } else if ((static_cast<uint8_t>(c) & 0xC0u) != 0x80u) {
      ++nextColumn;
    }

    return c;
  }

  char lookaheadChar() {
    auto c = stream.peek();

    if (stream.fail() || c == std::char_traits<char>::eof()) {
      return 0;
    }

    return c;
  }

  static bool isDelimiter(char c) {
    return isspace(c) || c == '#' || c == ':' || c == ',' || c == '\'' ||
           c == '"' || c == '[' || c == ']' || c == '{' || c == '}' || c == 0;
  }

  static bool isUTF16High(long utf16CodePoint) {
    return utf16CodePoint >= 0xD800 && utf16CodePoint <= 0xDBFF;
  }

  static bool isUTF16Low(long utf16CodePoint) {
    return utf16CodePoint >= 0xDC00 && utf16CodePoint <= 0xDFFF;
  }

  static std::string utf16ToUTF8(long utf16CodePoint1, long utf16CodePoint2) {
    uint32_t unicode;

    if (utf16CodePoint2 == 0) {
      unicode = static_cast<uint32_t>(utf16CodePoint1);
    } else {
      unicode =
          0x10000 + ((static_cast<uint32_t>(utf16CodePoint1 - 0xD800) << 10u) |
                     (static_cast<uint32_t>(utf16CodePoint2 - 0xDC00)));
    }

    if (unicode < 0x80) {
      return std::string(1, static_cast<char>(unicode));
    }
    if (unicode < 0x800) {
      return std::string(1, static_cast<char>(0xC0u | (unicode >> 6u))) +
             std::string(1, static_cast<char>(0x80u | (unicode & 0x3Fu)));
    }
    if (unicode < 0x10000) {
      return std::string(1, static_cast<char>(0xE0u | (unicode >> 12u))) +
             std::string(1,
                         static_cast<char>(0x80u | ((unicode >> 6u) & 0x3Fu))) +
             std::string(1, static_cast<char>(0x80u | (unicode & 0x3Fu)));
    }
    if (unicode < 0x10FFFF) {
      return std::string(1, static_cast<char>(0xF0u | (unicode >> 18u))) +
             std::string(
                 1, static_cast<char>(0x80u | ((unicode >> 12u) & 0x3Fu))) +
             std::string(1,
                         static_cast<char>(0x80u | ((unicode >> 6u) & 0x3Fu))) +
             std::string(1, static_cast<char>(0x80u | (unicode & 0x3Fu)));
    }

    return "";
  }

  Token nextNumber(const Location &startLocation, char startChar) {
    std::string text = std::string(1, startChar);
    auto endLine = startLocation.getStartLine();
    auto endColumn = startLocation.getStartColumn();
    auto foundE = false;
    auto foundDot = false;

    while (!isDelimiter(lookaheadChar())) {
      auto c = nextChar(endLine, endColumn);
      foundE = foundE || c == 'e' || c == 'E';
      foundDot = foundDot || c == '.';
      text += c;
    }

    Location location(startLocation.getStartLine(),
                      startLocation.getStartColumn(), endLine, endColumn);

    auto base = 10;
    auto sign = (text[0] == '-' || text[0] == '+') ? text[0] : ' ';
    auto startIndex = sign != ' ' ? 1 : 0;

    if (text.length() > startIndex + 1 && text[startIndex] == '0') {
      auto c = text[startIndex + 1];

      if (c == 'x' || c == 'X') {
        base = 16;
      } else if (c == 'b' || c == 'B') {
        base = 2;
      }
    }

    auto isFloat = foundDot || (base == 10 && foundE);
    if (!isFloat && base != 10) {
      text = text.erase(startIndex, 2);
    }

    char *endPtr;
    if (isFloat) {
      auto value = strtod(text.c_str(), &endPtr);

      if (endPtr != text.c_str() + text.length() || value == HUGE_VAL ||
          value == -HUGE_VAL) {
        throw SyntaxError("Invalid float literal", location);
      }

      return Token::fromFloat(location, value);
    } else {
      errno = 0;
      auto value = strtoll(text.c_str(), &endPtr, base);

      if (endPtr != text.c_str() + text.length() || errno == ERANGE) {
        throw SyntaxError("Invalid integer literal", location);
      }

      return Token(location, value);
    }
  }

  Token nextString(const Location &startLocation, char startChar) {
    std::string text;
    auto endLine = startLocation.getStartLine();
    auto endColumn = startLocation.getStartColumn();
    auto isMultiline = false;
    long lastCodeUnit = -1;
    auto lastCodeUnitLocation = Location::unknown();
    auto newLine = false;

    auto c = lookaheadChar();
    if (c == startChar) {
      nextChar(endLine, endColumn);
      isMultiline = lookaheadChar() == startChar;

      if (isMultiline) {
        nextChar(endLine, endColumn);
      } else {
        return Token(TokenKind::String,
                     combine(startLocation, endLine, endColumn), "");
      }
    } else if (c == 0) {
      throw SyntaxError("String is not closed", startLocation);
    }

    while (true) {
      uint32_t line;
      uint32_t column;
      c = nextChar(line, column);

      if (c == 0 || (!isMultiline && c == '\n')) {
        throw SyntaxError("String is not closed",
                          combine(startLocation, endLine, endColumn));
      }

      endLine = line;
      endColumn = column;

      if (lastCodeUnit != -1 && (c != '\\' || lookaheadChar() != 'u')) {
        throw SyntaxError("Expected other UTF-16 surrogate",
                          Location(line, column));
      }

      if (c == startChar) {
        if (isMultiline && lookaheadChar() == startChar) {
          nextChar(endLine, endColumn);

          if (lookaheadChar() == startChar) {
            nextChar(endLine, endColumn);
            break;
          } else {
            text += startChar;
          }
        } else {
          break;
        }
      }

      if (c == '\\') {
        auto escapeLine = line;
        auto escapeColumn = column;

        c = nextChar(endLine, endColumn);
        switch (c) {
        case '\\': {
          text += '\\';
          break;
        }
        case '"': {
          text += '"';
          break;
        }
        case '\'': {
          text += '\'';
          break;
        }
        case 'b': {
          text += '\b';
          break;
        }
        case 'f': {
          text += '\f';
          break;
        }
        case 'n': {
          text += '\n';
          break;
        }
        case 'r': {
          text += '\r';
          break;
        }
        case 't': {
          text += '\t';
          break;
        }
        case 'u': {
          std::string escape;

          for (auto i = 0; i < 4; ++i) {
            c = nextChar(line, column);

            if (c == 0) {
              throw SyntaxError(
                  "Invalid escape sequence in string",
                  Location(escapeLine, escapeColumn, line, column));
            }

            escape += c;
          }

          Location escapeLocation(escapeLine, escapeColumn, line, column);

          char *endPtr;
          auto utf16CodePoint = strtol(escape.c_str(), &endPtr, 16);
          if (endPtr != escape.c_str() + escape.length()) {
            throw SyntaxError("Invalid escape sequence in string",
                              escapeLocation);
          }

          std::string utf8;
          if (isUTF16Low(utf16CodePoint)) {
            if (lastCodeUnit == -1) {
              throw SyntaxError("Found no high UTF-16 surrogate",
                                escapeLocation);
            }

            utf8 = utf16ToUTF8(lastCodeUnit, utf16CodePoint);
            lastCodeUnit = -1;
          } else if (lastCodeUnit != -1) {
            throw SyntaxError("Expected low UTF-16 surrogate", escapeLocation);
          } else {
            if (isUTF16High(utf16CodePoint)) {
              lastCodeUnit = utf16CodePoint;
              lastCodeUnitLocation = escapeLocation;
            } else {
              utf8 = utf16ToUTF8(utf16CodePoint, 0);
            }
          }

          if (lastCodeUnit == -1) {
            if (utf8.empty()) {
              throw SyntaxError("Invalid escape sequence in string",
                                combine(lastCodeUnitLocation, escapeLocation));
            }

            text += utf8;
          }

          endLine = line;
          endColumn = column;
          break;
        }
        default: {
          throw SyntaxError("Invalid escape sequence in string",
                            Location(escapeLine, escapeColumn));
        }
        }
      } else if (!isspace(c) || !newLine) {
        text += c;
      }

      newLine = (newLine && (c == ' ' || c == '\t')) || c == '\n';
    }

    text.shrink_to_fit();
    return Token(TokenKind::String, combine(startLocation, endLine, endColumn),
                 text);
  }

  Token nextKey(const Location &startLocation, char startChar) {
    std::string text = std::string(1, startChar);
    auto endLine = startLocation.getStartLine();
    auto endColumn = startLocation.getStartColumn();

    while (!isDelimiter(lookaheadChar())) {
      text += nextChar(endLine, endColumn);
    }

    Location location(startLocation.getStartLine(),
                      startLocation.getStartColumn(), endLine, endColumn);

    text.shrink_to_fit();
    if (text == "true") {
      return Token(TokenKind::True, location);
    } else if (text == "false") {
      return Token(TokenKind::False, location);
    } else if (text == "null") {
      return Token(TokenKind::Null, location);
    } else {
      return Token(TokenKind::Key, location, text);
    }
  }

  Token next() {
    if (hasLookahead) {
      hasLookahead = false;
      return lookaheadToken;
    }

    char c;
    uint32_t startLine;
    uint32_t startColumn;

    do {
      c = nextChar(startLine, startColumn);

      if (c == 0) {
        return Token(TokenKind::EoD, Location(startLine, startColumn));
      }
    } while (isspace(c));

    Location location(startLine, startColumn);

    switch (c) {
    case '+':
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return nextNumber(location, c);
    case '\'':
    case '"':
      return nextString(location, c);
    case '#': {
      do {
        c = nextChar(startLine, startColumn);
      } while (c != '\n' && c != 0);

      return next();
    }
    case ':':
      return Token(TokenKind::Colon, location);
    case ',':
      return Token(TokenKind::Comma, location);
    case '{':
      return Token(TokenKind::OpenCurly, location);
    case '}':
      return Token(TokenKind::CloseCurly, location);
    case '[':
      return Token(TokenKind::OpenBrace, location);
    case ']':
      return Token(TokenKind::CloseBrace, location);
    default:
      return nextKey(location, c);
    }
  }

  const Token &lookahead() {
    if (!hasLookahead) {
      lookaheadToken = next();
      hasLookahead = true;
    }

    return lookaheadToken;
  }

  static std::string
  createExpectMessage(std::initializer_list<TokenKind> expected,
                      const Token &actual) {
    std::string msg = "Expected ";
    auto first = true;

    for (auto kind : expected) {
      if (first) {
        first = false;
      } else {
        msg += ", ";
      }

      msg += toString(kind);
    }

    msg += " but found " + toString(actual);
    return msg;
  }

  Token expect(std::initializer_list<TokenKind> kinds) {
    auto token = next();

    for (auto kind : kinds) {
      if (kind == token.kind) {
        return token;
      }
    }

    throw SyntaxError(createExpectMessage(kinds, token), token.location);
  }

  Token expect(TokenKind kind) { return expect({kind}); }

  Value parseArrayValue(const std::string &path, const Token &start) {
    std::vector<Value> *values = nullptr;

    auto token = lookahead();
    if (token.kind == TokenKind::CloseBrace) {
      next();
    } else {
      values = new std::vector<Value>();

      try {
        while (true) {
          values->push_back(
              parseValue(path + "[" + std::to_string(values->size()) + "]"));

          token = lookahead();
          if (token.kind == TokenKind::Comma) {
            next();
          } else if (token.kind == TokenKind::CloseBrace) {
            next();
            break;
          }
        }

        values->shrink_to_fit();
      } catch (...) {
        delete values;
        throw;
      }
    }

    return Value::fromArray(combine(start.location, token.location), path,
                            values != nullptr ? values : &EMPTY_VECTOR);
  }

  Value parseObjectValue(const std::string &path, const Token &start) {
    std::map<std::string, Value> *values = nullptr;

    Token token;
    if (start.kind == TokenKind::OpenCurly) {
      token =
          expect({TokenKind::Key, TokenKind::String, TokenKind::CloseCurly});
    } else {
      if (start.location.getStartColumn() <= objectIndent) {
        throw SyntaxError("Expected value but none found (check indentation?)",
                          start.location);
      }

      token = start;
    }

    Location endLocation = token.location;

    if (token.kind == TokenKind::CloseCurly) {
      // Can only occur if { was before
    } else {
      values = new std::map<std::string, Value>();

      try {
        auto oldObjectIndent = objectIndent;
        objectIndent = token.location.getStartColumn();

        while (true) {
          auto &itemKey = token.strValue;
          auto itemPath = path;
          if (path != ".") {
            itemPath += ".";
          }
          itemPath += escapeKey(itemKey);

          expect(TokenKind::Colon);

          auto itemValue = parseValue(itemPath);
          endLocation = itemValue.location;

          auto itr = values->find(itemKey);
          if (itr == values->end()) {
            values->emplace(itemKey, std::move(itemValue));
          } else {
            itr->second = std::move(itemValue);
          }

          token = lookahead();
          auto comma = token.kind == TokenKind::Comma;

          if (comma) {
            next();
            token = lookahead();
          } else if (token.kind == TokenKind::CloseCurly) {
            if (start.kind == TokenKind::OpenCurly) {
              next();
              endLocation = token.location;
            }

            break;
          }

          if (start.kind != TokenKind::OpenCurly &&
              token.location.getStartColumn() != objectIndent) {
            if (comma) {
              throw SyntaxError(
                  "Expected key but none found (check indentation?)",
                  token.location);
            }

            if (start.kind == TokenKind::OpenCurly) {
              expect(TokenKind::CloseCurly);
            }

            break;
          }

          token = expect({TokenKind::Key, TokenKind::String});
        }

        objectIndent = oldObjectIndent;
      } catch (...) {
        delete values;
        throw;
      }
    }

    return Value::fromObject(combine(start.location, endLocation), path,
                             values != nullptr ? values : &EMPTY_MAP);
  }

  Value parseValue(const std::string &path) {
    DepthHandler depthHandler(options, depth);

    auto token =
        expect({TokenKind::True, TokenKind::False, TokenKind::Int,
                TokenKind::Float, TokenKind::Key, TokenKind::String,
                TokenKind::Null, TokenKind::OpenBrace, TokenKind::OpenCurly});

    switch (token.kind) {
    case TokenKind::True:
      return Value::fromBool(token.location, path, true);
    case TokenKind::False:
      return Value::fromBool(token.location, path, false);
    case TokenKind::Int:
      return Value::fromInt(token.location, path, token.intValue);
    case TokenKind::Float:
      return Value::fromFloat(token.location, path, token.floatValue);
    case TokenKind::Key:
      return parseObjectValue(path, token);
    case TokenKind::String: {
      if (lookahead().kind == TokenKind::Colon) {
        return parseObjectValue(path, token);
      }

      return Value::fromString(token.location, path, token.strValue);
    }
    case TokenKind::Null:
      return Value::fromNull(token.location, path);
    case TokenKind::OpenBrace:
      return parseArrayValue(path, token);
    case TokenKind::OpenCurly:
      return parseObjectValue(path, token);
    default:
      unreachable();
    }
  }

public:
  explicit Parser(std::istream &stream, const Options &options)
      : stream(stream), options(options), nextLine(1), nextColumn(1),
        objectIndent(0), hasLookahead(false), depth(0) {}

  Value parse() {
    auto value = parseValue(".");
    expect(TokenKind::EoD);
    return value;
  }
};

const Options DEFAULT_OPTIONS = {1024};

Value parse(std::istream &stream, const Options &options) {
  return Parser(stream, options).parse();
}

std::string escapeKey(const std::string &str) {
  if (str.find_first_of(" \"\'\n\r\t\\.[],{}") == std::string::npos) {
    return str;
  }

  return escape(str);
}

std::string escape(const std::string &str) {
  std::string result = "\"";

  if (!str.empty()) {
    result.reserve(2 + str.length());

    size_t pos = 0;
    size_t nextUnprocessed = 0;

    while (true) {
      pos = str.find_first_of("\"\'\b\f\n\r\t\\", pos);
      if (pos == std::string::npos) {
        break;
      }

      if (pos != nextUnprocessed) {
        result += str.substr(nextUnprocessed, pos - nextUnprocessed);
      }

      switch (str[pos]) {
      case '"': {
        result += "\\\"";
        break;
      }
      case '\'': {
        result += "\\'";
        break;
      }
      case '\b': {
        result += "\\b";
        break;
      }
      case '\f': {
        result += "\\f";
        break;
      }
      case '\n': {
        result += "\\n";
        break;
      }
      case '\r': {
        result += "\\r";
        break;
      }
      case '\t': {
        result += "\\t";
        break;
      }
      case '\\': {
        result += "\\\\";
        break;
      }
      }

      ++pos;
      nextUnprocessed = pos;
    }

    if (nextUnprocessed != str.length()) {
      result += str.substr(nextUnprocessed);
    }
  }

  result += "\"";
  return result;
}

} // namespace cppcson
