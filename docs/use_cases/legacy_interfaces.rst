.. namespace:: units

Working with Legacy Interfaces
==============================

In case we are working with a legacy/unsafe interface we may be forced to
extract the :term:`value of a quantity` with :func:`quantity::number()`
(in addition
to the quantity of a `quantity_point` with :func:`quantity_point::relative()`,
or the quantity of a `quantity_kind` with :func:`quantity_kind::common()`,
or the quantity kind of a `quantity_point_kind`
with :func:`quantity_point_kind::relative()`)
and pass it to the library's output:

.. code-block::
    :caption: legacy.h

    namespace legacy {

    void print_eta(double speed_in_mps);
    void set_path_position(double meter);

    } // namespace legacy

.. code-block::

    #include "legacy.h"
    #include <units/isq/si/si.h>
    #include <units/quantity_point.h>

    using namespace units::isq;

    constexpr Speed auto avg_speed(Length auto d, Time auto t)
    {
      return d / t;
    }

    void print_eta(Length auto d, Time auto t)
    {
      Speed auto v = avg_speed(d, t);
      legacy::print_eta(quantity_cast<si::metre_per_second>(v).number());
    }

    template<QuantityPoint QP>
      requires Length<typename QP::quantity_type>
    void set_path_position(QP p)
    {
      legacy::set_path_position(quantity_point_cast<si::metre>(p).relative().number());
    }

.. important::

    When dealing with a quantity of an unknown ``auto`` type please remember
    to always use `quantity_cast` to cast it to a desired unit before calling
    `quantity::number()` and passing the raw value to the legacy/unsafe interface.
