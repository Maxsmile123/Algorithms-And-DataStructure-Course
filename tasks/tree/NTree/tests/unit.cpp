#include <chrono>
#include <future>
#include <map>
#include <iostream>
#include <string>
#include <thread>

#include <fmt/core.h>
#include <gtest/gtest.h>

#include "../filesystem/fs.hpp"

class FsTest: public testing::Test {
  protected:
    void SetUp() override {
      fs.MakeDir("/mnt/c/Users/Sysoev/VSCodeProjects/", true);
      fs.CreateFile("/mnt/c/Users/Sysoev/VSCodeProjects/test.txt", false);
      fs.CreateFile("/mnt/c/Users/Sysoev/VSCodeProjects/test1.txt", false);
      fs.CreateFile("/mnt/c/Users/Sysoev/VSCodeProjects/test2.txt", false);
      fs.CreateFile("/mnt/c/Users/Sysoev/VSCodeProjects/test3.txt", false);
      fs.CreateFile("/mnt/c/Users/Sysoev/test3.txt", false);
      fs.CreateFile("/mnt/c/Users/test1.txt", false);
    }
  
public:
  filesystem::Fs fs;
};


TEST(EmptyFsTest, PWDOnRoot) {
  filesystem::Fs fs;
  fs.PWD();
  // expect: "/"
}

TEST(EmptyFsTest, EmptyListFiles) {
  filesystem::Fs fs;
  fs.ListFiles(".");
  // expect: 
}

TEST(EmptyFsTest, SimpleMakeDir) {
  filesystem::Fs fs;
  fs.MakeDir("/dir", false);
  fs.ChangeDir("/dir");
  fs.PWD();
  // expect: "/dir"
}

TEST(EmptyFsTest, MakeDirWithFlag) {
  filesystem::Fs fs;
  fs.MakeDir("/dir/other_dir", true);
  fs.ChangeDir("/dir");
  fs.ChangeDir("other_dir");
  fs.PWD();
  // expect: "/dir/other_dir"
}

TEST(EmptyFsTest, MakeSeveralDirsWithoutFlag) {
  filesystem::Fs fs;
  EXPECT_THROW({
    fs.MakeDir("/dir/other_dir", false);
  }, filesystem::exceptions::FileNotFoundException);
}

TEST(EmptyFsTest, ChangeDirToNotExistingDir) {
  filesystem::Fs fs;
  EXPECT_THROW({
    fs.ChangeDir("/dir");
  }, filesystem::exceptions::FileNotFoundException);
}

TEST(EmptyFsTest, SimpleCreateFile) {
  filesystem::Fs fs;
  fs.CreateFile("file.txt", false);
  fs.ListFiles(".");
  // expect: "file.txt"
}

TEST(EmptyFsTest, ShowFileContent) {
  filesystem::Fs fs;
  std::ostringstream oss;
  oss << "Hello world!\n";
  fs.CreateFile("file.txt", false);
  fs.WriteToFile("file.txt", false, oss);
  fs.ShowFileContent("file.txt");
  // expect: "Hello world!"
}

TEST(EmptyFsTest, OverwriteFile) {
  filesystem::Fs fs;
  std::ostringstream oss;
  oss << "Hello world!\n";
  fs.CreateFile("file.txt", false);
  fs.WriteToFile("file.txt", false, oss);
  oss << "Hello Meeeen!\n";
  fs.WriteToFile("file.txt", true, oss);
  fs.ShowFileContent("file.txt");
  // expect: "Hello Meeeen!"
}

TEST(EmptyFsTest, OverwriteFileByCreate) {
  filesystem::Fs fs;
  std::ostringstream oss;
  oss << "Hello world!\n";
  fs.CreateFile("file.txt", false);
  fs.WriteToFile("file.txt", false, oss);
  fs.CreateFile("file.txt", true);
  fs.ShowFileContent("file.txt");
  // expect:
}

TEST(EmptyFsTest, OverwriteFirstCreateFile) {
  filesystem::Fs fs;
  std::ostringstream oss;
  oss << "Hello world!\n";
  fs.CreateFile("file.txt", true);
  fs.WriteToFile("file.txt", false, oss);
  fs.ShowFileContent("file.txt");
  // expect:
}

TEST(EmptyFsTest, SimpleRemoveFile) {
  filesystem::Fs fs;
  std::ostringstream oss;
  oss << "Hello world!\n";
  fs.CreateFile("file.txt", false);
  fs.RemoveFile("file.txt");
  fs.ListFiles(".");
  // expect:
}

TEST(EmptyFsTest, RemoveNotExistFile) {
  filesystem::Fs fs;
  EXPECT_THROW({
    fs.RemoveFile("file.txt");
  }, filesystem::exceptions::FileNotFoundException);
}


TEST_F(FsTest, SimpleFind) {
  fs.FindFile("text3.txt");
  // expect: 
  // /mnt/c/Users/Sysoev/VSCodeProjects/test3.txt
  // /mnt/c/Users/Sysoev/test3.txt
  // 
}

TEST_F(FsTest, FailedChangeDirToFile) {
  EXPECT_THROW({
    fs.ChangeDir("/mnt/c/Users/Sysoev/test3.txt");
  }, filesystem::exceptions::FileNotFoundException);
}

TEST_F(FsTest, DirNotExistForCreateFile) {
  EXPECT_THROW({
    fs.ChangeDir("/mnt/c/Users/Sysoev/VSCodeProjects");
    fs.PWD();
    // expect: /mnt/c/Users/Sysoev/VSCodeProjects/
    fs.CreateFile("/dir/file.txt", false);
  }, filesystem::exceptions::FileNotFoundException);
}

TEST_F(FsTest, FileNotFound) {
  EXPECT_THROW({
    fs.FindFile("file.cpp");
  }, filesystem::exceptions::FileNotFoundException);
}

TEST_F(FsTest, DeleteRoot) {
  fs.RemoveFile("/");
  fs.ListFiles(".");
  // expect: 
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}