.. namespace:: units

Magnitudes
==========

The ratio of two Units of the same Dimension---say, ``inches`` and ``centimeters``---is some
constant number, which is known at compile time.  It's a positive real number---a *Magnitude*.

We also use Magnitudes for *Dimensionless* Units.  ``percent`` has a Magnitude of :math:`1/100`, and
``dozen`` would have a Magnitude of :math:`12`.

Interestingly, it turns out that the usual numeric types are not up to this task.  We need a
Magnitude representation that can do everything Units can do.  This means, among other things:

1. We need *exact* symbolic computation of the core operations of Quantity Calculus (i.e., products
   and rational powers).

2. We must support *irrational* Magnitudes, because they frequently occur in practice (e.g.,
   consider the ratio between ``degrees`` and ``radians``).

3. We should *avoid overflow* wherever possible (note that ``std::intmax_t`` can't even handle
   certain simple SI prefixes, such as ``yotta``, representing :math:`10^{24}`).

Integers' inadequacies are clear enough, but even floating point falls short.  Imagine if we
implemented all angular units in terms of ``radians``: then both ``degrees`` and ``revolutions``
pick up a factor of :math:`\pi`.  The arithmetic with *its floating point representation* is
unlikely to cancel *exactly*.

Another common alternative choice is ``std::ratio``, but this fails the first requirement: rational
numbers are (`rather infamously <https://hsm.stackexchange.com/a/7>`_!) *not* closed under rational
powers.

The only viable solution we have yet encountered is the *vector space representation*.  The
implementation is fascinating---but, for purposes of this present page, it's also a distraction.
*Here,* we're more focused on how to *use* these Magnitudes.

One type per Magnitude, one value per type
------------------------------------------

Each typical numeric type (``double``, ``int64_t``, ...) can represent a wide variety of values: the
more, the better.  However, Magnitudes are **not** like that.  Instead, they comprise a *variety* of
types, and each type can hold only *one* value.

.. tip::

    A given Magnitude represents the *same* number, whether you use it as a *type*, or as a *value*
    (i.e., an *instance* of that type).

    Use whichever is more convenient.  (In practice, this is usually the *value*: especially for end
    users rather than library developers.)

If these types can only represent one value each, why would we bother to instantiate them?  Because
*values are easier to use*.

- ``mag<N>()`` gives the Magnitude value corresponding to any integer ``N``. - You can combine
  values in the usual way using ``*``, ``/``, ``==``, and ``!=``, as well as ``pow<N>(m)`` and
  ``root<N>(m)`` for any Magnitude value ``m``.

Traits: integers and rational Magnitudes
----------------------------------------

If you have a Magnitude instance ``m``, we provide traits to help you reason about integers and
rational numbers, or manipulate integer or rational parts.

- ``is_integral(m)``: indicates whether ``m`` represents an *integral* Magnitude.
- ``is_rational(m)``: indicates whether ``m`` represents a *rational* Magnitude.

The above traits indicate what kind of Magnitude we already have.  The next traits *manipulate* a
Magnitude, letting us break it apart into *constituent* Magnitudes which may be more meaningful.
(For example, imagine going from ``inches`` to ``feet``.  Naively, we might multiply by the floating
point representation of ``1.0 / 12.0``.  However, if we broke this apart into separate numerator and
denominator, it would let us simply *divide by 12*, yielding **exact** results for inputs that
happen to be multiples of 12.)

- ``numerator(m)`` (value): a Magnitude representing the "numerator", i.e., the largest integer
  which divides ``m``, without turning any of its base powers' exponents negative (or making any
  previously-negative exponents *more* negative). - ``denominator(m)`` (value): the "numerator" of
  the *inverse* of ``m``.

These traits interact as one would hope.  For example, ``is_rational(m)`` is exactly equivalent to
``m == numerator(m) / denominator(m)``.

Why these particular definitions?  Because they are equivalent to the numerator and denominator when
we have a rational number, and they are compatible with how humans write numbers when we don't.
Example:

- :math:`m1 = \frac{27 \pi^2}{25}`.  Then ``numerator(m1) == mag<27>()``, and
  ``denominator(m1) == mag<25>()``.
- :math:`m2 = \sqrt{m1}`.  Then ``numerator(m2) == mag<3>()``, and ``denominator(m2) == mag<5>()``.
  Note that this is consistent with how humans would typically write ``m2``, as
  :math:`\frac{3\sqrt{3} \pi}{5}`.

Getting values out
------------------

Magnitude types represent numbers in non-numeric types.  They've got some amazing strengths (exact
rational powers!), and some significant weaknesses (no support for basic addition!).  So what if you
just want to turn a Magnitude ``m`` into a traditional numeric type ``T``?

You call ``get_value<T>(m)``.

This does what it looks like it does, and it does it at compile time.  Any intermediate computations
take place in the "widest type in category"---``long double`` for floating point, and
``std::intmax_t`` or ``std::uintmax_t`` for signed or unsigned integers---before ultimately being
cast back to the target type.  For ``T = float``, say, this means we get all the precision we'd have
with something like ``long double``, but without any speed penalty at runtime!

``get_value<T>(m)`` also has the protections you would hope: for example, if ``T`` is an integral
type, we require ``is_integral(m)``.

How to use Magnitudes
---------------------

- First, start with your basic inputs: this will typically be ``mag<N>()`` for any integer ``N``, or
  the built-in Magnitude constant ``mag_pi``.  (Again, these are all *values*, not types.)

- Next, combine and manipulate these using the various "Magnitude math" operations, all of which are
  **exact**: ``*``, ``/``, ``pow<N>``, ``root<N>``, ``numerator()``, ``denominator()``.

- If you need to translate a Magnitude ``m`` to a "real" numeric type ``T``, call
  ``get_value<T>(m)``.
