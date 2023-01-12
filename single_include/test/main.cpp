#include <iostream>

#include "mblet/jsonator.h"
#include "test/already_include.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    mblet::Jsonator json;
    json["main"] = "foobar";
    test(json);
    std::cout << json.dump(2) << std::endl;
    return 0;
}