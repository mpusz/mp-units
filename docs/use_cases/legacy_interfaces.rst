.. namespace:: units

Working with Legacy Interfaces
==============================

In case we are working with a legacy/unsafe interface we may be forced to
extract a :term:`value of a quantity` with :func:`quantity::count()` and
pass it to the library's output:

.. code-block::
    :caption: legacy.h

    namespace legacy {

    void print_eta(double speed_in_mps);

    } // namespace legacy

.. code-block::

    #include "legacy.h"
    #include <units/physical/si/velocity.h>

    using namespace units;

    constexpr Velocity auto avg_speed(Length auto d, Time auto t)
    {
      return d / t;
    }

    void print_eta(Length auto d, Time auto t)
    {
      Velocity auto v = avg_speed(d, t);
      legacy::print_eta(quantity_cast<si::metre_per_second>(v).count());
    }

.. important::

    When dealing with a quantity of an unknown ``auto`` type please remember
    to always use `quantity_cast` to cast it to a desired unit before calling
    `quantity::count()` and passing the raw value to the legacy/unsafe interface.
