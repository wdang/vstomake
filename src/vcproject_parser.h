#pragma once

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

#include "project_parser.h"

struct VCProjectParser : public ProjectParser {

  VCProjectParser();

  virtual bool CompilerProperties(const std::string& config,
    std::unordered_map<std::string, std::string>* props);

  virtual bool LibrarianProperties(const std::string& config,
    std::unordered_map<std::string, std::string>* props);

  virtual bool LinkerProperties(const std::string& config,
    std::unordered_map<std::string, std::string>* props);

  virtual bool Configurations(std::vector<vs::Configuration>* out);
  //virtual bool Files(std::vector<vs::Configuration>* configs, std::vector<vs::File>* files);
  virtual bool Files(std::vector<vs::File>* files);  
  virtual bool Filters(std::vector<vs::Filter>* filter);
  
  virtual bool Parse(char* buffer, size_t len);
  virtual bool Parse(const std::string& path);
  
  virtual bool ProjectProperties(std::unordered_map<std::string, std::string>* props);

private:
  std::string           origin;
  std::vector<char>        src;
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>*    root;
  std::vector<rapidxml::xml_node<>*> configurations;
  std::vector<rapidxml::xml_node<>*> files;
  std::unordered_map<std::string, std::string> macros;
};
