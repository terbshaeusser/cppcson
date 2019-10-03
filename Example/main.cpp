#include <cppcson.hpp>
#include <fstream>
#include <iostream>

static void print(const cppcson::Value &value, bool isRoot, int32_t indent) {
  if (value.isNull()) {
    std::cout << "null";
  } else if (value.isBool()) {
    std::cout << value.asBool();
  } else if (value.isInt()) {
    std::cout << value.asInt();
  } else if (value.isFloat()) {
    std::cout << value.asFloat();
  } else if (value.isString()) {
    std::cout << cppcson::escape(value.asString());
  } else if (value.isArray()) {
    std::cout << "[";

    indent += 2;
    for (auto &item : value) {
      std::cout << std::endl << std::string(indent, ' ');
      print(item, false, indent);
    }
    indent -= 2;

    std::cout << std::endl << std::string(indent, ' ') << "]";
  } else if (value.isObject()) {
    if (!isRoot) {
      indent += 2;
    }

    auto skipNewline = isRoot;

    for (auto &key : value.keys()) {
      if (skipNewline) {
        skipNewline = false;
      } else {
        std::cout << std::endl;
      }

      std::cout << std::string(indent, ' ') << cppcson::escapeKey(key) << ": ";
      print(value.item(key), false, indent);
    }
  }
}

static void print(const cppcson::Value &value) {
  print(value, true, 0);
  std::cout << std::endl;
}

int main(int argc, char const *const *argv) {
  if (argc != 2) {
    std::cout << "Error: Please pass a path to a cson file" << std::endl;
    return 1;
  }

  std::string path(argv[1]);
  std::ifstream stream(path);

  if (stream.fail()) {
    std::cout << "Error: File " << path << " could not be read" << std::endl;
    return 2;
  }

  try {
    auto root = cppcson::parse(stream);

    print(root);
    return 0;
  } catch (const cppcson::Error &e) {
    std::cout << "Error: The file " << path << " could not be parsed as cson"
              << std::endl
              << "Details:" << std::endl
              << e.getLocation() << std::endl
              << e.what() << std::endl;
    return 3;
  }
}
