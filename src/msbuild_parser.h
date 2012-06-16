#pragma once

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

#include "project_parser.h"


struct MSBuildParser : public ProjectParser {
  MSBuildParser();
  
  virtual bool CompilerProperties(const std::string& config, 
    std::unordered_map<std::string,std::string>* props);
  virtual bool LibrarianProperties(const std::string& config, 
    std::unordered_map<std::string,std::string>* props);
  virtual bool LinkerProperties(const std::string& config, 
    std::unordered_map<std::string,std::string>* props);


  virtual bool Configurations(std::vector<vs::Configuration>* out);
  virtual bool Files(std::vector<vs::File>* files);
  virtual bool Filters(std::vector<vs::Filter>* filter);
  virtual bool Parse(char* buffer, size_t len);
  virtual bool ProjectProperties( std::unordered_map<std::string, std::string>* props );
  virtual bool Parse(const std::string& path);

private:
  std::unordered_map<std::string, std::string> macros;
  std::vector<char> src;
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* project; 
};


