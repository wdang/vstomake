#include "vcproject.h"
#include <gtest/gtest.h>
#include <fstream>
using std::ifstream;
using std::string;

namespace {

// VCProjectTest Test fixture.
class VCProjectTest : public ::testing::Test {
 protected:

  // Initialize static data.
  static void SetUpTestCase() {}

  // Set-up work for each test.
  VCProjectTest() {}

  // Called before each test(after the constructor).
  virtual void SetUp() {}

  // Called after each test(before the destructor).
  virtual void TearDown() {}

  // no throw
  virtual ~VCProjectTest() {}

  // Cleanup static data.
  static void TearDownTestCase() {}

};
TEST_F(VCProjectTest, Files) {
  ifstream fs("testing\\base.vcproj", std::ios::in | std::ios::binary | std::ios::ate);
  const int64_t size = fs.tellg();
  ASSERT_TRUE(size  != -1);

  string contents(static_cast<size_t>(size), '\0');
  ASSERT_TRUE(fs.is_open());
  fs.seekg(0, std::ios::beg);
  fs.read(&contents[0], contents.size());
  fs.close();

  VCProject project(contents);
  auto& config = project.configurations[0];
  EXPECT_EQ(config.files.size(), 327);
  EXPECT_EQ(config.excluded_files.size(), 119);
  EXPECT_EQ(config.files.size() + config.excluded_files.size(), 446);
  
}
} //namespace
