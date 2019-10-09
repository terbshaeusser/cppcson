#include <cppcson.hpp>
#include <fstream>
#include <iostream>

static void print(const cppcson::Value &value, bool isRoot, int32_t indent) {
  switch (value.getKind()) {
  case cppcson::Value::Kind::Bool: {
    std::cout << value.asBool();
    break;
  }
  case cppcson::Value::Kind::Int: {
    std::cout << value.asInt();
    break;
  }
  case cppcson::Value::Kind::Float: {
    std::cout << value.asFloat();
    break;
  }
  case cppcson::Value::Kind::String: {
    std::cout << cppcson::escape(value.asString());
    break;
  }
  case cppcson::Value::Kind::Null: {
    std::cout << "null";
    break;
  }
  case cppcson::Value::Kind::Array: {
    std::cout << "[";

    indent += 2;
    for (auto &item : value) {
      std::cout << std::endl << std::string(indent, ' ');
      print(item, false, indent);
    }
    indent -= 2;

    std::cout << std::endl << std::string(indent, ' ') << "]";
    break;
  }
  case cppcson::Value::Kind::Object: {
    if (!isRoot) {
      indent += 2;
    }

    auto skipNewline = isRoot;

    if (value.getItemCount() == 0) {
      std::cout << "{}";
    } else {
      for (auto &key : value.keys()) {
        if (skipNewline) {
          skipNewline = false;
        } else {
          std::cout << std::endl;
        }

        std::cout << std::string(indent, ' ') << cppcson::escapeKey(key)
                  << ": ";
        print(value.item(key), false, indent);
      }
    }
    break;
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
