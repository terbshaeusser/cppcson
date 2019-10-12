#include "cppcson.hpp"
#include <gtest/gtest.h>
#include <sstream>

TEST(Null, keyword) {
  std::istringstream stream("null");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 4), root.getLocation());
  EXPECT_TRUE(root.isNull());
  EXPECT_EQ(".", root.getPath());
}

TEST(Bool, trueKeyword) {
  std::istringstream stream("true");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 4), root.getLocation());
  EXPECT_TRUE(root.isBool());
  EXPECT_TRUE(root.asBool());
  EXPECT_EQ(".", root.getPath());
}

TEST(Bool, falseKeyword) {
  std::istringstream stream("false");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 5), root.getLocation());
  EXPECT_TRUE(root.isBool());
  EXPECT_FALSE(root.asBool());
  EXPECT_EQ(".", root.getPath());
}

TEST(Int, decZero) {
  std::istringstream stream("0");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1), root.getLocation());
  EXPECT_TRUE(root.isInt());
  EXPECT_EQ(0, root.asInt());
  EXPECT_EQ(".", root.getPath());
}

TEST(Int, decMax) {
  auto str = std::to_string(std::numeric_limits<int64_t>::max());
  std::istringstream stream(str);

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, static_cast<uint32_t>(str.length())),
            root.getLocation());
  EXPECT_TRUE(root.isInt());
  EXPECT_EQ(std::numeric_limits<int64_t>::max(), root.asInt());
  EXPECT_EQ(".", root.getPath());
}

TEST(Int, decMin) {
  auto str = std::to_string(std::numeric_limits<int64_t>::min());
  std::istringstream stream(str);

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, static_cast<uint32_t>(str.length())),
            root.getLocation());
  EXPECT_TRUE(root.isInt());
  EXPECT_EQ(std::numeric_limits<int64_t>::min(), root.asInt());
  EXPECT_EQ(".", root.getPath());
}

TEST(Int, hexSimple) {
  std::istringstream stream("0x12EAf");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 7), root.getLocation());
  EXPECT_TRUE(root.isInt());
  EXPECT_EQ(0x12eaf, root.asInt());
  EXPECT_EQ(".", root.getPath());
}

TEST(Int, hexMax) {
  std::string str = "0x7FFFFFFFFFFFFFFF";
  std::istringstream stream(str);

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, static_cast<uint32_t>(str.length())),
            root.getLocation());
  EXPECT_TRUE(root.isInt());
  EXPECT_EQ(std::numeric_limits<int64_t>::max(), root.asInt());
  EXPECT_EQ(".", root.getPath());
}

TEST(Int, hexMin) {
  std::string str = "-0x8000000000000000";
  std::istringstream stream(str);

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, static_cast<uint32_t>(str.length())),
            root.getLocation());
  EXPECT_TRUE(root.isInt());
  EXPECT_EQ(std::numeric_limits<int64_t>::min(), root.asInt());
  EXPECT_EQ(".", root.getPath());
}

TEST(Int, binSimple) {
  std::istringstream stream("0B001101");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 8), root.getLocation());
  EXPECT_TRUE(root.isInt());
  EXPECT_EQ(13, root.asInt());
  EXPECT_EQ(".", root.getPath());
}

TEST(Int, binMax) {
  std::string str =
      "0b111111111111111111111111111111111111111111111111111111111111111";
  std::istringstream stream(str);

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, static_cast<uint32_t>(str.length())),
            root.getLocation());
  EXPECT_TRUE(root.isInt());
  EXPECT_EQ(std::numeric_limits<int64_t>::max(), root.asInt());
  EXPECT_EQ(".", root.getPath());
}

TEST(Int, binMin) {
  std::string str =
      "-0b1000000000000000000000000000000000000000000000000000000000000000";
  std::istringstream stream(str);

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, static_cast<uint32_t>(str.length())),
            root.getLocation());
  EXPECT_TRUE(root.isInt());
  EXPECT_EQ(std::numeric_limits<int64_t>::min(), root.asInt());
  EXPECT_EQ(".", root.getPath());
}

TEST(Float, zero) {
  std::istringstream stream("0.0");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 3), root.getLocation());
  EXPECT_TRUE(root.isFloat());
  EXPECT_EQ(0.0, root.asFloat());
  EXPECT_EQ(".", root.getPath());
}

TEST(Float, negative) {
  std::istringstream stream("-330.4");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 6), root.getLocation());
  EXPECT_TRUE(root.isFloat());
  EXPECT_EQ(-330.4, root.asFloat());
  EXPECT_EQ(".", root.getPath());
}

TEST(Float, exp) {
  std::istringstream stream("1.0e10");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 6), root.getLocation());
  EXPECT_TRUE(root.isFloat());
  EXPECT_EQ(1.0e10, root.asFloat());
  EXPECT_EQ(".", root.getPath());
}

TEST(Float, exp2) {
  std::istringstream stream("10.05E+10");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 9), root.getLocation());
  EXPECT_TRUE(root.isFloat());
  EXPECT_EQ(10.05E+10, root.asFloat());
  EXPECT_EQ(".", root.getPath());
}

TEST(Float, exp3) {
  std::istringstream stream("10.05E-20");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 9), root.getLocation());
  EXPECT_TRUE(root.isFloat());
  EXPECT_EQ(10.05E-20, root.asFloat());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, empty) {
  std::istringstream stream("''");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 2), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, empty2) {
  std::istringstream stream("\"\"");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 2), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, emptyMultiLine) {
  std::istringstream stream("''''''");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 6), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, emptyMultiLine2) {
  std::istringstream stream("\"\"\"\"\"\"");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 6), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, singleLine) {
  std::istringstream stream("'a single string'");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 17), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("a single string", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, singleLine2) {
  std::istringstream stream("\"a single string\"");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 17), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("a single string", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, multiLineSingle) {
  std::istringstream stream("'''a single string'''");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 21), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("a single string", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, multiLineSingle2) {
  std::istringstream stream("\"\"\"a single string\"\"\"");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 21), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("a single string", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, multiLine) {
  std::istringstream stream("'''\n  Hello \nWorld \n'''");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 4, 3), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("Hello \nWorld", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, escapes) {
  std::istringstream stream(R"('\\ \t \n \r \" \'')");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 19), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("\\ \t \n \r \" \'", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(String, utf16Escapes) {
  std::istringstream stream(R"('\u0001 \uD834\uDD1E')");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 21), root.getLocation());
  EXPECT_TRUE(root.isString());
  EXPECT_EQ("\x01 \xF0\x9D\x84\x9E", root.asString());
  EXPECT_EQ(".", root.getPath());
}

TEST(Array, empty) {
  std::istringstream stream("[]");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 2), root.getLocation());
  EXPECT_TRUE(root.isArray());
  EXPECT_EQ(".", root.getPath());
}

TEST(Array, single) {
  std::istringstream stream("[22]");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(1, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 4), root.getLocation());
  EXPECT_TRUE(root.isArray());
  EXPECT_EQ(".", root.getPath());

  auto &item = root.item(0);
  EXPECT_EQ(0, item.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 2, 1, 3), item.getLocation());
  EXPECT_TRUE(item.isInt());
  EXPECT_EQ(22, item.asInt());
  EXPECT_EQ(".[0]", item.getPath());
}

TEST(Array, multiple) {
  std::istringstream stream("[1,2\n  3]");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(3, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 2, 4), root.getLocation());
  EXPECT_TRUE(root.isArray());
  EXPECT_EQ(".", root.getPath());

  auto &item1 = root.item(0);
  EXPECT_EQ(0, item1.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 2), item1.getLocation());
  EXPECT_TRUE(item1.isInt());
  EXPECT_EQ(1, item1.asInt());
  EXPECT_EQ(".[0]", item1.getPath());

  auto &item2 = root.item(1);
  EXPECT_EQ(0, item2.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 4), item2.getLocation());
  EXPECT_TRUE(item2.isInt());
  EXPECT_EQ(2, item2.asInt());
  EXPECT_EQ(".[1]", item2.getPath());

  auto &item3 = root.item(2);
  EXPECT_EQ(0, item3.getItemCount());
  EXPECT_EQ(cppcson::Location(2, 3), item3.getLocation());
  EXPECT_TRUE(item3.isInt());
  EXPECT_EQ(3, item3.asInt());
  EXPECT_EQ(".[2]", item3.getPath());
}

TEST(Array, nested) {
  std::istringstream stream("[[1 2] 3]");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(2, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 9), root.getLocation());
  EXPECT_TRUE(root.isArray());
  EXPECT_EQ(".", root.getPath());

  auto &item1 = root.item(0);
  EXPECT_EQ(2, item1.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 2, 1, 6), item1.getLocation());
  EXPECT_TRUE(item1.isArray());
  EXPECT_EQ(".[0]", item1.getPath());

  auto &item11 = item1.item(0);
  EXPECT_EQ(0, item11.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 3), item11.getLocation());
  EXPECT_TRUE(item11.isInt());
  EXPECT_EQ(1, item11.asInt());
  EXPECT_EQ(".[0][0]", item11.getPath());

  auto &item12 = item1.item(1);
  EXPECT_EQ(0, item12.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 5), item12.getLocation());
  EXPECT_TRUE(item12.isInt());
  EXPECT_EQ(2, item12.asInt());
  EXPECT_EQ(".[0][1]", item12.getPath());

  auto &item2 = root.item(1);
  EXPECT_EQ(0, item2.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 8), item2.getLocation());
  EXPECT_TRUE(item2.isInt());
  EXPECT_EQ(3, item2.asInt());
  EXPECT_EQ(".[1]", item2.getPath());
}

TEST(Array, iterate) {
  std::istringstream stream("[1 2]");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(2, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 5), root.getLocation());
  EXPECT_TRUE(root.isArray());
  EXPECT_EQ(".", root.getPath());

  auto count = 0;
  for (auto &item : root) {
    if (count == 0) {
      EXPECT_EQ(0, item.getItemCount());
      EXPECT_EQ(cppcson::Location(1, 2), item.getLocation());
      EXPECT_TRUE(item.isInt());
      EXPECT_EQ(1, item.asInt());
      EXPECT_EQ(".[0]", item.getPath());
    } else if (count == 1) {
      EXPECT_EQ(0, item.getItemCount());
      EXPECT_EQ(cppcson::Location(1, 4), item.getLocation());
      EXPECT_TRUE(item.isInt());
      EXPECT_EQ(2, item.asInt());
      EXPECT_EQ(".[1]", item.getPath());
    }

    ++count;
  }

  EXPECT_EQ(2, count);
}

TEST(Object, simple) {
  std::istringstream stream("key1: false\nkey2: 1\n,\nkey3: 'a'");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(3, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 4, 9), root.getLocation());
  EXPECT_TRUE(root.isObject());
  EXPECT_EQ(".", root.getPath());
  EXPECT_TRUE(root.contains("key1"));
  EXPECT_TRUE(root.contains("key2"));
  EXPECT_TRUE(root.contains("key3"));

  auto &item1 = root.item("key1");
  EXPECT_EQ(0, item1.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 7, 1, 11), item1.getLocation());
  EXPECT_TRUE(item1.isBool());
  EXPECT_FALSE(item1.asBool());
  EXPECT_EQ(".key1", item1.getPath());

  auto &item2 = root.item("key2");
  EXPECT_EQ(0, item2.getItemCount());
  EXPECT_EQ(cppcson::Location(2, 7), item2.getLocation());
  EXPECT_TRUE(item2.isInt());
  EXPECT_EQ(1, item2.asInt());
  EXPECT_EQ(".key2", item2.getPath());

  auto &item3 = root.item("key3");
  EXPECT_EQ(0, item3.getItemCount());
  EXPECT_EQ(cppcson::Location(4, 7, 4, 9), item3.getLocation());
  EXPECT_TRUE(item3.isString());
  EXPECT_EQ("a", item3.asString());
  EXPECT_EQ(".key3", item3.getPath());
}

TEST(Object, eodAfterNewline) {
  std::istringstream stream("key1: false\n");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(1, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 1, 11), root.getLocation());
  EXPECT_TRUE(root.isObject());
  EXPECT_EQ(".", root.getPath());
  EXPECT_TRUE(root.contains("key1"));

  auto &item1 = root.item("key1");
  EXPECT_EQ(0, item1.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 7, 1, 11), item1.getLocation());
  EXPECT_TRUE(item1.isBool());
  EXPECT_FALSE(item1.asBool());
  EXPECT_EQ(".key1", item1.getPath());
}

TEST(Object, stringKey) {
  std::istringstream stream(
      "'key': true\n\"key.2\": false\n'''multi\n  linekey''': null");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(3, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 4, 18), root.getLocation());
  EXPECT_TRUE(root.isObject());
  EXPECT_EQ(".", root.getPath());
  EXPECT_TRUE(root.contains("key"));
  EXPECT_TRUE(root.contains("key.2"));
  EXPECT_TRUE(root.contains("multi\nlinekey"));

  auto &item1 = root.item("key");
  EXPECT_EQ(0, item1.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 8, 1, 11), item1.getLocation());
  EXPECT_TRUE(item1.isBool());
  EXPECT_TRUE(item1.asBool());
  EXPECT_EQ(".key", item1.getPath());

  auto &item2 = root.item("key.2");
  EXPECT_EQ(0, item2.getItemCount());
  EXPECT_EQ(cppcson::Location(2, 10, 2, 14), item2.getLocation());
  EXPECT_TRUE(item2.isBool());
  EXPECT_FALSE(item2.asBool());
  EXPECT_EQ(".\"key.2\"", item2.getPath());

  auto &item3 = root.item("multi\nlinekey");
  EXPECT_EQ(0, item3.getItemCount());
  EXPECT_EQ(cppcson::Location(4, 15, 4, 18), item3.getLocation());
  EXPECT_TRUE(item3.isNull());
  EXPECT_EQ(".\"multi\\nlinekey\"", item3.getPath());
}

TEST(Object, duplicateKey) {
  std::istringstream stream("a: true\na: false");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(1, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 2, 8), root.getLocation());
  EXPECT_TRUE(root.isObject());
  EXPECT_EQ(".", root.getPath());
  EXPECT_TRUE(root.contains("a"));

  auto &item1 = root.item("a");
  EXPECT_EQ(0, item1.getItemCount());
  EXPECT_EQ(cppcson::Location(2, 4, 2, 8), item1.getLocation());
  EXPECT_TRUE(item1.isBool());
  EXPECT_FALSE(item1.asBool());
  EXPECT_EQ(".a", item1.getPath());
}

TEST(Object, nested) {
  std::istringstream stream("a:\n  b: 2\nc: 3");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(2, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 3, 4), root.getLocation());
  EXPECT_TRUE(root.isObject());
  EXPECT_EQ(".", root.getPath());
  EXPECT_TRUE(root.contains("a"));
  EXPECT_TRUE(root.contains("c"));

  auto &item1 = root.item("a");
  EXPECT_EQ(1, item1.getItemCount());
  EXPECT_EQ(cppcson::Location(2, 3, 2, 6), item1.getLocation());
  EXPECT_TRUE(item1.isObject());
  EXPECT_EQ(".a", item1.getPath());
  EXPECT_TRUE(item1.contains("b"));

  auto &item11 = item1.item("b");
  EXPECT_EQ(0, item11.getItemCount());
  EXPECT_EQ(cppcson::Location(2, 6), item11.getLocation());
  EXPECT_TRUE(item11.isInt());
  EXPECT_EQ(2, item11.asInt());
  EXPECT_EQ(".a.b", item11.getPath());

  auto &item2 = root.item("c");
  EXPECT_EQ(0, item2.getItemCount());
  EXPECT_EQ(cppcson::Location(3, 4), item2.getLocation());
  EXPECT_TRUE(item2.isInt());
  EXPECT_EQ(3, item2.asInt());
  EXPECT_EQ(".c", item2.getPath());
}

TEST(Object, iterate) {
  std::istringstream stream("a: 1\nb: 2\nc: 3");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(3, root.getItemCount());
  EXPECT_EQ(cppcson::Location(1, 1, 3, 4), root.getLocation());
  EXPECT_TRUE(root.isObject());
  EXPECT_EQ(".", root.getPath());
  EXPECT_TRUE(root.contains("a"));
  EXPECT_TRUE(root.contains("b"));
  EXPECT_TRUE(root.contains("c"));

  auto count = 0;
  for (auto &key : root.keys()) {
    auto &item = root.item(key);

    if (count == 0) {
      EXPECT_EQ("a", key);

      EXPECT_EQ(0, item.getItemCount());
      EXPECT_EQ(cppcson::Location(1, 4), item.getLocation());
      EXPECT_TRUE(item.isInt());
      EXPECT_EQ(1, item.asInt());
      EXPECT_EQ(".a", item.getPath());
    } else if (count == 1) {
      EXPECT_EQ("b", key);

      EXPECT_EQ(0, item.getItemCount());
      EXPECT_EQ(cppcson::Location(2, 4), item.getLocation());
      EXPECT_TRUE(item.isInt());
      EXPECT_EQ(2, item.asInt());
      EXPECT_EQ(".b", item.getPath());
    } else if (count == 2) {
      EXPECT_EQ("c", key);

      EXPECT_EQ(0, item.getItemCount());
      EXPECT_EQ(cppcson::Location(3, 4), item.getLocation());
      EXPECT_TRUE(item.isInt());
      EXPECT_EQ(3, item.asInt());
      EXPECT_EQ(".c", item.getPath());
    }

    ++count;
  }

  EXPECT_EQ(3, count);
}

TEST(Comment, simple) {
  std::istringstream stream("# first line\nnull # another comment");

  auto root = cppcson::parse(stream);

  EXPECT_EQ(0, root.getItemCount());
  EXPECT_EQ(cppcson::Location(2, 1, 2, 4), root.getLocation());
  EXPECT_TRUE(root.isNull());
  EXPECT_EQ(".", root.getPath());
}

TEST(Print, nullKeyword) {
  auto value = cppcson::Value::newNull();

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("null", stream.str());
}

TEST(Print, trueKeyword) {
  auto value = cppcson::Value::newBool(true);

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("true", stream.str());
}

TEST(Print, falseKeyword) {
  auto value = cppcson::Value::newBool(false);

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("false", stream.str());
}

TEST(Print, intLiteral) {
  auto value = cppcson::Value::newInt(0);

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("0", stream.str());
}

TEST(Print, floatLiteral) {
  auto value = cppcson::Value::newFloat(10.5);

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("10.5", stream.str());
}

TEST(Print, stringLiteral) {
  auto value = cppcson::Value::newString("hello");

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("\"hello\"", stream.str());
}

TEST(Print, arrayEmpty) {
  auto value = cppcson::Value::newArray();

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("[]", stream.str());
}

TEST(Print, arraySimple) {
  auto value = cppcson::Value::newArray();
  value.add(cppcson::Value::newInt(1));
  value.add(cppcson::Value::newInt(2));
  value.add(cppcson::Value::newInt(3));

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("[\n  1\n  2\n  3\n]", stream.str());
}

TEST(Print, arrayNested) {
  auto nestedValue = cppcson::Value::newArray();
  nestedValue.add(cppcson::Value::newInt(1));

  auto value = cppcson::Value::newArray();
  value.add(std::move(nestedValue));
  value.add(cppcson::Value::newInt(3));

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("[\n  [\n    1\n  ]\n  3\n]", stream.str());
}

TEST(Print, arrayObject) {
  auto nestedValue = cppcson::Value::newObject();
  nestedValue.add("a", cppcson::Value::newInt(1));

  auto value = cppcson::Value::newArray();
  value.add(std::move(nestedValue));
  value.add(cppcson::Value::newInt(3));

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("[\n  a: 1\n,\n  3\n]", stream.str());
}

TEST(Print, objectEmpty) {
  auto value = cppcson::Value::newObject();

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("{}", stream.str());
}

TEST(Print, objectSimple) {
  auto value = cppcson::Value::newObject();
  value.add("a", cppcson::Value::newInt(1));
  value.add("b", cppcson::Value::newInt(2));
  value.add("c", cppcson::Value::newInt(3));

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("a: 1\nb: 2\nc: 3", stream.str());
}

TEST(Print, objectNested) {
  auto nestedValue = cppcson::Value::newObject();
  nestedValue.add("b", cppcson::Value::newInt(2));

  auto value = cppcson::Value::newObject();
  value.add("a", std::move(nestedValue));
  value.add("c", cppcson::Value::newInt(3));

  std::ostringstream stream;
  cppcson::print(stream, value);

  EXPECT_EQ("a:\n  b: 2\nc: 3", stream.str());
}
