.. namespace:: units

Magnitudes
==========

The ratio of two Units of the same Dimension---say, `inches` and `centi<meters>`---is some constant
number, which is known at compile time.  It's a positive real number---a _Magnitude_.

We also use Magnitudes for _Dimensionless_ Units.  `percent` has a Magnitude of $1/100$, and `dozen`
would have a Magnitude of $12$.

Interestingly, it turns out that the usual numeric types are not up to this task.  We need
a Magnitude representation that can do everything Units can do.  This means, among other things:

1. We need _exact_ symbolic computation of the core operations of Quantity Calculus (i.e., products
   and rational powers).

2. We must support _irrational_ Magnitudes, because they frequently occur in practice (e.g.,
   consider the ratio between `degrees` and `radians`).

3. We should _avoid overflow_ wherever possible (note that `std::intmax_t` can't even handle certain
   simple SI prefixes, such as `yotta`, representing $10^{24}$).

Integers' inadequacies are clear enough, but even floating point falls short.  Imagine if we
implemented all angular units in terms of `radians`: then both `degrees` and `revolutions` pick up
a factor of $\pi$.  The arithmetic with _its floating point representation_ is unlikely to cancel
_exactly_.

Another common alternative choice is `std::ratio`, but this fails the first requirement: rational
numbers are ([rather infamously](https://hsm.stackexchange.com/a/7)!) _not_ closed under rational
powers.

The only viable solution we have yet encountered is the _vector space representation_.  The
implementation is fascinating---but, for purposes of this present page, it's also a distraction.
_Here,_ we're more focused on how to _use_ these Magnitudes.

One type per Magnitude, one value per type
------------------------------------------

Each typical numeric type (`double`, `int64_t`, ...) can represent a wide variety of values: the
more, the better.  However, Magnitudes are **not** like that.  Instead, they comprise a _variety_ of
types, and each type can hold only _one_ value.

.. tip::

    A given Magnitude represents the _same_ number, whether you use it as a _type_, or as a _value_
    (i.e., an _instance_ of that type).

    Use whichever is more convenient.  (In practice, this is usually the _value_: especially for end
    users rather than library developers.)

If these types can only represent one value each, why would we bother to instantiate them?  Because
_values are easier to use_.

- `mag<N>()` gives the Magnitude value corresponding to any integer `N`.
- You can combine values in the usual way using `*`, `/`, `==`, and `!=`, as well as `pow<N>(m)` and
  `root<N>(m)` for any Magnitude value `m`.

Traits: integers and rational Magnitudes
----------------------------------------

If you have a Magnitude instance `m`, we provide traits to help you reason about integers and
rational numbers, or manipulate integer or rational parts.

- `is_integral(m)`: indicates whether `m` represents an _integral_ Magnitude.
- `is_rational(m)`: indicates whether `m` represents a _rational_ Magnitude.

The above traits indicate what kind of Magnitude we already have.  The next traits _manipulate_ a
Magnitude, letting us break it apart into _constituent_ Magnitudes which may be more meaningful.
(For example, imagine going from `inches` to `feet`.  Naively, we might multiply by the floating
point representation of `1.0 / 12.0`.  However, if we broke this apart into separate numerator and
denominator, it would let us simply _divide by 12_, yielding **exact** results for inputs that
happen to be multiples of 12.)

- `numerator(m)` (value): a Magnitude representing the "numerator", i.e., the largest integer which
  divides `m`, without turning any of its base powers' exponents negative (or making any
  previously-negative exponents _more_ negative).
- `denominator(m)` (value): the "numerator" of the _inverse_ of `m`.

These traits interact as one would hope.  For example, `is_rational(m)` is exactly equivalent to
`m == numerator(m) / denominator(m)`.

Why these particular definitions?  Because they are equivalent to the numerator and denominator when
we have a rational number, and they are compatible with how humans write numbers when we don't.
Example:

- $m1 = \frac{27 \pi^2}{25}$.  Then `numerator(m1) == mag<27>()`, and
  `denominator(m1) == mag<25>()`.
- $m2 = \sqrt{m1}$.  Then `numerator(m2) == mag<3>()`, and `denominator(m2) == mag<5>()`.  Note that
  this is consistent with how humans would typically write `m2`, as $\frac{3\sqrt{3} \pi}{5}$.

Getting values out
------------------

Magnitude types represent numbers in non-numeric types.  They've got some amazing strengths (exact
rational powers!), and some significant weaknesses (no support for basic addition!).  So what if you
just want to turn a Magnitude `m` into a traditional numeric type `T`?

You call `get_value<T>(m)`.

This does what it looks like it does, and it does it at compile time.  Any intermediate computations
take place in the "widest type in category"---`long double` for floating point, and `std::intmax_t`
or `std::uintmax_t` for signed or unsigned integers---before ultimately being cast back to the
target type.  For `T = float`, say, this means we get all the precision we'd have with something
like `long double`, but without any speed penalty at runtime!

`get_value<T>(m)` also has the protections you would hope: for example, if `T` is an integral type,
we require `is_integral(m)`.

How to use Magnitudes
---------------------

- First, start with your basic inputs: this will typically be `mag<N>()` for any integer `N`, or the
  built-in Magnitude constant `pi`.  (Again, these are all _values_, not types.)

- Next, combine and manipulate these using the various "Magnitude math" operations, all of which are
  **exact**: `*`, `/`, `pow<N>`, `root<N>`, `numerator()`, `denominator()`.

- If you need to translate a Magnitude `m` to a "real" numeric type `T`, call `get_value<T>(m)`.
