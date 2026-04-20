#include "rawdger/Core.h"

namespace rawdger
{

std::string getString()
{
    return "currently march 19th bro";
}

} // namespace rawdger

//clang-tidy tests 🚀
#if 1

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

void tidyExamples()
{
    // bugprone-use-after-move + performance-unnecessary-copy-initialization
    std::string s1  = "hello";
    std::string s2  = std::move (s1);
    auto        len = s1.size(); // using moved-from s1

    std::vector<int> nums = { 1, 2, 3 };

    // modernize-use-auto
    std::vector<int>::iterator it = std::find (nums.begin(), nums.end(), 2);

    // modernize-loop-convert
    for (std::vector<int>::size_type i = 0; i < nums.size(); ++i)
    {
        nums[i] *= 2;
    }

    // readability-container-size-empty
    if (nums.size() == 0)
    {
        return;
    }

    // readability-implicit-bool-conversion
    if (nums.size()) { /* ... */ }

    // readability-braces-around-statements
    if (len > 0)
        return;

    // performance-inefficient-string-concatenation
    std::string result;
    for (int i = 0; i < 10; ++i)
    {
        result = result + "x";
    }

    // cppcoreguidelines-init-variables
    int uninit;
    (void) uninit;
}

  //Then add a class for the Rule of Five / virtual-destructor checks:

  // cppcoreguidelines-special-member-functions (you already have TempWav flagged for this)
  // modernize-use-override
  struct Base
  {
      virtual void foo() {}
      virtual ~Base() = default;
  };

  struct Derived : Base
  {
      void foo() {} // missing 'override'
  };

  // cppcoreguidelines-prefer-member-initializer
  struct Widget
  {
      int count;
      Widget() { count = 42; } // should initialize in member init list
  };

  //And one more for bugprone-* fun:

  // bugprone-integer-division
  double ratio = 3 / 4;                           // integer div, then float conversion

  // bugprone-branch-clone
  int classify (int x)
  {
      if (x > 0)
      {
          return 1;
      }
      else if (x < 0)
      {
          return 1;
      } // same body as first branch
      else
      {
          return 0;
      }
  }

  // bugprone-suspicious-string-compare
  bool equal (const char* a, const char* b)
  {
      return strcmp (a, b); // suspicious: returns int, used as bool
  }
  #endif