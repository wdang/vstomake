#ifdef VSTOMAKE_RUN_TESTS
#include "vcproject_parser.h"
#include <gtest/gtest.h>
#include <fstream>

namespace {
using std::string;
using std::unordered_map;
using std::vector;
using std::ifstream;

// VCProjectTest Test fixture.
class VCProjectParserTest : public ::testing::Test {
protected:

  // Initialize static data.
  static void SetUpTestCase() {

    ifstream fs("testing\\base.vcproj",
                std::ios::in | std::ios::binary | std::ios::ate);

    const int64_t size = fs.tellg();
    ASSERT_TRUE(size  != -1);
    ASSERT_TRUE(fs.is_open());
    for(size_t i = 0; i < size; ++i) {
      Contents.push_back('\0');
    }

    fs.seekg(0, std::ios::beg);
    fs.read(&Contents[0], Contents.size());
    fs.close();
  }

  // Set-up work for each test.
  VCProjectParserTest() {}

  // Called before each test(after the constructor).
  virtual void SetUp() {}

  // Called after each test(before the destructor).
  virtual void TearDown() {}

  // no throw
  virtual ~VCProjectParserTest() {}

  // Cleanup static data.
  static void TearDownTestCase() {}

  static string Contents;
  static VCProjectParser Parser;
};

string VCProjectParserTest::Contents("");
VCProjectParser VCProjectParserTest::Parser;

TEST_F(VCProjectParserTest, Parse) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));

  //EXPECT_EQ(config.Files.size(), 327);
  //EXPECT_EQ(project.Files.size(),446);
}

TEST_F(VCProjectParserTest, Configurations) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));

  vector<vs::Configuration> configs;
  EXPECT_TRUE(Parser.Configurations(&configs));

  EXPECT_EQ(configs.size(), 4);
}

TEST_F(VCProjectParserTest, Files) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));
  vector<vs::Configuration> configs;
  EXPECT_TRUE(Parser.Configurations(&configs));

  vector<vs::File> files;
  EXPECT_TRUE(Parser.Files(&files));
  EXPECT_EQ(files.size(), 446);
}

TEST_F(VCProjectParserTest, Filters) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));

  //EXPECT_EQ(config.Files.size(), 327);
  //EXPECT_EQ(project.Files.size(),446);
}

TEST_F(VCProjectParserTest, ProjectProperties) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));
  unordered_map<string, string> props;
  Parser.ProjectProperties(&props);

  //EXPECT_EQ(config.Files.size(), 327);
  //EXPECT_EQ(project.Files.size(),446);
}


TEST_F(VCProjectParserTest, Other) {
  vs::Project proj;
  vs::Project::Parse("testing\\base.vcproj",&proj);

  //EXPECT_EQ(config.Files.size(), 327);
  //EXPECT_EQ(project.Files.size(),446);
}
} //namespace
#endif
