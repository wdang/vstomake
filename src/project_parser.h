#pragma once

#include "libvs.h"

 // Interface for a visual studio
  // project parser
  struct ProjectParser {
    virtual ~ProjectParser();

    // Initializes the parser
    //
    // @buffer: in memory contents of a project file
    // @len: byte length of buffer
    //
    // returns true on successful initialization
    // and verification of buffer
    //
    virtual bool Parse(const std::string& path) = 0;


    // Collect global project properties
    //
    // @props
    //
    // returns true if props was modified
    virtual bool ProjectProperties(std::unordered_map<std::string, std::string>* props) = 0;

    // Collect compiler properties for the
    // specific project configuration 'config'
    //
    // @config
    // @props
    //
    // returns true if props was modified
    virtual bool CompilerProperties(const std::string& config,
                                    std::unordered_map<std::string, std::string>* props) = 0;

    // Collect librarian properties for the
    // specific project configuration 'config'
    //
    // @config
    // @props
    //
    // returns true if props was modified
    virtual bool LibrarianProperties(const std::string& config,
                                     std::unordered_map<std::string, std::string>* props) = 0;

    // Collect linker properties for the
    // specific project configuration 'config'
    //
    // @config
    // @props
    //
    // returns true if props was modified
    virtual bool LinkerProperties(const std::string& config,
                                  std::unordered_map<std::string, std::string>* props) = 0;

    // Collect all configurations in the project
    //
    // returns true if entries was added to the given vector
    //
    // @configs
    virtual bool Configurations(std::vector<vs::Configuration>* configs) = 0;

    // Collect all filters in the project
    // returns true if filters were found
    // @filter
    virtual bool Filters(std::vector<vs::Filter>* filter) = 0;

    // Collect all files referenced in the project
    //
    // returns true if entries was added to the given vector
    //
    // @files - return variable that is populated with
    // files referenced in the project
    virtual bool Files(std::vector<vs::File>* files)=0;
  };
  