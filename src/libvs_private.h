#pragma once

// utility functions used internally by libvs 
#include "libvs.h"
namespace vs {

  // returns a project format based on the contents of the
  // file located at path.
  // Returns Enum::Format_unknown on failure
  Enum::Format GetFormat(const std::string& path);
}
