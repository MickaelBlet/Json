#include <iostream>

#include "blet/json.h"
#include "test/already_include.h"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    blet::Dict json;
    json["main"] = "foobar";
    test(json);
    std::cout << blet::json::dump(json, 2) << std::endl;
    return 0;
}
