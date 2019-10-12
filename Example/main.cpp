#include <cppcson.hpp>
#include <fstream>
#include <iostream>

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

    cppcson::print(std::cout, root);
    std::cout << std::endl;
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
