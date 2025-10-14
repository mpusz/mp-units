# Tutorial 4: Working with Temperatures

Temperature conversions are one of the most error-prone areas in scientific and
engineering calculations. The distinction between _temperature points_ (absolute values)
and _temperature differences_ (deltas) is crucial but often overlooked, leading to
subtle bugs that can have serious real-world consequences.

This tutorial will teach you how to use **mp-units** to handle temperature calculations
correctly and safely, preventing common conversion errors that plague many codebases.


## Problem statement

For example, the difference between 20 °C (68 °F) and 10 °C (50 °F) is 10 °C (18 °F).
However, if you incorrectly apply the point conversion formula to the difference
(10 °C × 9/5 + 32 = 50 °F), you get the wrong answer!

The key insight is that temperature _differences_ only need scaling (×9/5 for C→F),
while temperature _points_ additionally require an offset (+32 for C→F).

## Your task

!!! quote "The weather forecast in Europe"

    Today there is 19 ℃ outside. Tomorrow we can expect increase to 21 ℃. Unfortunately,
    a cold front is approaching so in the following days the temperature will drop by 5 ℃.

A US citizen listening to this forecast wants to convert those temperatures to degree
Fahrenheit. Please help him with this task:

1. Express both temperature points (today and tomorrow) in ℉.
2. Find out how much warmer it will be tomorrow (temperature difference in both ℃ and ℉).
3. Calculate the temperature that is forecasted for the rest of the week (after the 5 ℃ drop).

**Challenge**: Refactor the code below from using raw doubles and unsafe macros to
**mp-units** strong types that prevent temperature conversion errors.

```cpp
// ce-embed height=650 compiler=clang2110 flags="-std=c++23 -stdlib=libc++ -O3"
#include <mp-units/systems/si.h>
#include <mp-units/systems/usc.h>
#include <iostream>

using namespace mp_units;
using namespace mp_units::si::unit_symbols;
using namespace mp_units::usc::unit_symbols;

#define POINT_C_TO_F(temp) ((temp) * 9.0 / 5.0 + 32.0)
#define DELTA_C_TO_F(temp) ((temp) * 9.0 / 5.0)

void print_temp(std::string_view title, double t)
{
  std::cout << title << ": " << t << " (" << POINT_C_TO_F(t) << ")\n";
}

int main()
{
  double today_pt_C = 19;
  double tomorrow_pt_C = 21;
  double diff_C = tomorrow_pt_C - today_pt_C;
  double later_pt_C = tomorrow_pt_C - 5;

  std::cout << "Today: " << today_pt_C << " ℃ (" << POINT_C_TO_F(today_pt_C) << " ℉)\n";
  std::cout << "Tomorrow: " << tomorrow_pt_C << " ℃ (" << POINT_C_TO_F(tomorrow_pt_C) << " ℉)\n";
  std::cout << "Diff: " << diff_C << " ℃ (" << DELTA_C_TO_F(diff_C) << " ℉)\n";
  std::cout << "Later: " << later_pt_C << " ℃ (" << POINT_C_TO_F(later_pt_C) << " ℉)\n";
}
```

??? "Solution"

    ```cpp
    #include <mp-units/systems/si.h>
    #include <mp-units/systems/usc.h>
    #include <iostream>

    using namespace mp_units;
    using namespace mp_units::si::unit_symbols;
    using namespace mp_units::usc::unit_symbols;

    void print_temp(std::string_view title, Quantity auto t)
    {
      std::cout << title << ": " << t << " (" << t.in(deg_F) << ")\n";
    }

    void print_temp(std::string_view title, QuantityPoint auto t)
    {
      std::cout << title << ": " << t.quantity_from_zero() << " (" << t.in(deg_F).quantity_from_zero() << ")\n";
    }

    int main()
    {
      quantity_point today = point<deg_C>(19.);
      quantity_point tomorrow = point<deg_C>(21.);
      quantity diff = tomorrow - today;
      quantity_point later = tomorrow - delta<deg_C>(5.);

      print_temp("Today", today);
      print_temp("Tomorrow", tomorrow);
      print_temp("Diff", diff);
      print_temp("Later", later);
    }
    ```

## Takeaways

- **Temperature points vs. deltas**: Absolute temperatures and temperature differences
  require different conversion formulas
- **Type safety**: **mp-units** prevents common temperature conversion errors at
  compile time
- **Automatic conversions**: The `.in()` method handles proper unit conversions automatically
- **Getting deltas from points**: `.quantity_from_zero()` allows us to get a delta for a
  temperature point from 0 degrees in the current temperature system
- **Overloaded functions**: Use function overloading to handle different quantity types
  elegantly
- **Real-world safety**: Strong typing prevents bugs that could occur in critical
  applications like weather systems or industrial processes
