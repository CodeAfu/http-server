#include <cstdio>
#include <print>
#include <string>

void message(const std::string &msg) {
    std::println("{}", msg);
    std::printf("%s\n", msg.c_str());
}
