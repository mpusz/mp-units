Design
======

.. note::

    For brevity all the code examples in this chapter assume::

        using namespace units;

.. toctree::
    :maxdepth: 2

    design/directories
    design/quantity

The Downcasting Facility
------------------------

.. 
    http://www.nomnoml.com

    [detail::derived_dimension_base<exp<si::dim_length, 2>>]<:-[dim_area]


.. 
    http://www.nomnoml.com

    [downcast_base<detail::derived_dimension_base<exp<si::dim_length, 2>>>]<:-[detail::derived_dimension_base<exp<si::dim_length, 2>>]
    [detail::derived_dimension_base<exp<si::dim_length, 2>>]<:-[downcast_child<dim_area, detail::derived_dimension_base<exp<si::dim_length, 2>>>]
    [downcast_child<dim_area, detail::derived_dimension_base<exp<si::dim_length, 2>>>]<:-[dim_area]
