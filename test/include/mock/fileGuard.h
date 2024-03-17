#ifndef _TEST_BLET_FILE_GUARD_H_
#define _TEST_BLET_FILE_GUARD_H_

#include <cstdio>
#include <fstream>
#include <string>

namespace test {
namespace blet {

class FileGuard : public std::ofstream {
  public:
    FileGuard(const char* filename, std::ios_base::openmode openMode) :
        std::ofstream(filename, openMode),
        filename_(filename) {}
    ~FileGuard() {
        if (is_open()) {
            close();
        }
        ::remove(filename_.c_str());
    }

  private:
    std::string filename_;
};

} // namespace blet
} // namespace test

#endif // _TEST_BLET_FILE_GUARD_H_
