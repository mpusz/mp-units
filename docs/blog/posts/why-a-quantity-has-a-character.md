---
date: 2026-06-29
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# Why a Quantity Has a Character

A few years ago at CppCon, an engineer who works with electrical power systems every day
stopped me after a talk. He told me that his team confuses _active power_, _reactive
power_, _apparent power_, and _complex power_ all the time, and that the mistake is easy
to make and expensive to find. Then he said the sentence that has stuck with me since: a
units library that will not make those four incompatible types is of no use in his
industry.

He is right. And he is not alone.

---

_Note: Revised on July 5, 2026 to incorporate the feedback in the comments below._

<!-- more -->

## The problem is not the dimension

Dimension safety is powerful, and it already catches more than people expect. The _mass_
versus _weight_ confusion is a good example. In SI the split is clean: _mass_ is measured
in kilograms and a _weight_ force in newtons, two different dimensions, so a dimension-safe
library catches the mix-up for free. The pound tells the more interesting half of the
story. At a Croydon ISO C++ evening session someone told me, with full confidence, that the
pound is a unit of _force_, and he was not simply wrong. In the gravitational
foot-pound-force units common in US engineering the pound routinely names a _force_
(`lbf`), which is exactly why pounds per square inch is a pressure and foot-pounds a
torque, while the international (avoirdupois) pound anchored to SI is a unit of _mass_.
One name, two different quantities, and which one you mean depends on
the system in play. That is less a mistake to correct than an ambiguity to pin down, and
pinning it down is exactly what a quantity-safe library forces you to do.

The hard cases are the ones that _share_ a dimension, where dimension safety goes blind:

- _speed_ and _velocity_. _Speed_ is the magnitude of _velocity_, in the same unit. A
  one-dimensional motion model often stores both as a plain `double`, where a negative value
  means "moving backwards." Same dimension, different character: _speed_ is a scalar,
  _velocity_ a vector. (That a _speed_ is also non-negative is a separate guarantee, one
  that comes from [absolute quantities](introducing-absolute-quantities.md), not from
  character.)
- _active_, _reactive_, _apparent_, and _complex power_, from the opening. Same dimension,
  and for _complex power_ a different character as well.

A library that is only unit-safe and dimension-safe cannot help with these. It happily
accepts a _velocity_ where a _speed_ is expected, or a _reactive power_ where an _active
power_ is expected, because the units and the dimensions match. A _speed_ is a _velocity_
with its direction discarded, and a function written for _velocities_ cannot tell when it
is handed a _speed_ instead:

=== "Plain `double`"

    ```cpp
    // one axis along the road lane, both readings backed by double
    double car_velocity(car id);                                     // signed, +/- along the lane
    double car_speed(car id) { return std::abs(car_velocity(id)); }  // speedometer: |velocity|

    // expects two velocities in the same frame; returns how fast the cars' gap closes
    double closing_speed(double v_a, double v_b)
    {
      return std::abs(v_a - v_b);
    }

    car a = ...;     // car A reverses at 12
    car b = ...;     // car B approaching at 30
    closing_speed(car_velocity(a), car_velocity(b));  // 42 m/s: correct, both are velocities
    closing_speed(car_speed(a),    car_velocity(b));  // 18 m/s: wrong, car A's reverse gear is gone
    closing_speed(car_speed(a),    car_speed(b));     // 18 m/s: wrong again, and nothing flags either slip
    ```

=== "Strong quantities with loose template parameters"

    ```cpp
    // one axis along the road lane, both readings backed by double
    auto car_velocity(car id);                                      // signed, +/- along the lane
    auto car_speed(car id) { return magnitude(car_velocity(id)); }  // speedometer: |velocity|

    // accepts everything and return anything (like the `double` case)
    auto closing_speed(auto v_a, auto v_b)
    {
      return magnitude(v_a - v_b);
    }

    car a = ...;     // car A reverses at 12
    car b = ...;     // car B approaching at 30
    closing_speed(car_velocity(a), car_velocity(b));  // ok: the magnitude of a velocity difference is a speed
    closing_speed(car_speed(a),    car_velocity(b));  // error: can't subtract different characters (V3)
    closing_speed(car_speed(a),    car_speed(b));     // error: can't take a magnitude of a scalar quantity
    ```

=== "Strong quantities with proper constraints"

    ```cpp
    // one axis along the road lane, both readings backed by double
    QuantityOf<isq::velocity> auto car_velocity(car id);                                   // +/- along the lane
    QuantityOf<isq::speed> auto car_speed(car id) { return magnitude(car_velocity(id)); }  // speedometer: |velocity|

    // constrained to velocities, so the wrong argument is turned away at the call site
    QuantityOf<isq::speed> auto closing_speed(QuantityOf<isq::velocity> auto v_a,
                                              QuantityOf<isq::velocity> auto v_b)
    {
      return magnitude(v_a - v_b);
    }

    car a = ...;     // car A reverses at 12
    car b = ...;     // car B approaching at 30
    closing_speed(car_velocity(a), car_velocity(b));  // ok: the magnitude of a velocity difference is a speed
    closing_speed(car_speed(a),    car_velocity(b));  // error: a speed is not a velocity
    closing_speed(car_speed(a),    car_speed(b));     // error: a speed is not a velocity
    ```

In the plain-`double` world all three calls compile, and two are silently wrong: the moment
a _speed_ stands in for a _velocity_, car A's reverse gear is gone and `closing_speed`
returns 18 instead of 42, with nothing to flag it. Strong quantities close the gap from two
directions. Even with loose `auto` parameters, character governs the arithmetic inside the
function: you cannot take the `magnitude` of a scalar _speed_, so a pair of speeds never
makes it past the norm, and in **V3** subtracting a scalar from a vector is rejected
outright. Constrain the parameters to `QuantityOf<isq::velocity>` and the mistake is caught
even earlier, at the call site, with a message that names the real problem: a _speed_ is not
a _velocity_.

**mp-units** tells these quantities apart through two complementary properties: quantity
_kind_ (the ISQ hierarchy, where _active_, _reactive_, _apparent_, and _complex power_ are
separate, incompatible kinds living in different trees) and quantity _character_. Character
does two jobs:

- **Rejecting a representation type that does not fit a strongly-typed quantity.** A plain
  `double` for a complex quantity such as a voltage phasor (a real type has no room for its
  phase), or a `cartesian_vector` where a scalar _speed_ is expected. That mismatch is easy
  to introduce in a large codebase, and character turns it into a compile-time error.
- **Governing which operations are legal, and which derived quantity comes out**, so a
  calculation cannot drift into nonsense. You can take the `magnitude()` of a _velocity_
  or a _stress_ tensor but not of a scalar _speed_, and you do not multiply two vectors,
  you take their _scalar product_ or their _vector product_, each yielding a different
  quantity.

The simplest of these guards already ship: a scalar has no `magnitude()`. The operations that
return a _different_ kind, like the products, are largely **V3**, built on the character model
in place today.

This post is about character: where the name came from, the design dead ends we hit on the
way (some of them more than once), and why none of it is the scope creep it is sometimes
accused of being. It is an engineering journey, and it is not finished. Comments at the
bottom are genuinely wanted.

## The objection, stated honestly

Before the history, the objection, because it is a good one. In issue
[#648](https://github.com/mpusz/mp-units/issues/648), Roth Michaels asked it plainly:

!!! quote "[#648](https://github.com/mpusz/mp-units/issues/648): when do we need `quantity_character`?"

    I'm not sure I fully understand why we need `quantity_character` to support vector,
    tensor, and complex types. Why will it not work if the representation type is a
    vector, tensor, or complex value type?

It is a fair question, and a tempting design: the representation type already knows what
it is. `std::complex<double>` is complex. `Eigen::Vector3d` is a vector. So why does a
quantity need to carry a separate _character_ at all? Why not just follow the type?

The objection has a sharper, more developed form. In discussion
[#683](https://github.com/mpusz/mp-units/discussions/683), Chip Hogg set out a working
hypothesis that vector character is a scope mistake, reasoning from "same program, only
safer": users already choose the vector, matrix, or scalar types that suit them, so a units
library should wrap that choice and add safety, not second-guess it. Including character
mostly just forbids combinations, and for that to pay off, the real mistakes it catches must
outweigh the legitimate uses it blocks. It is a good argument, and parts of it are simply
right. I will come back to it once the design is on the table.

The honest answer took us several years and a few wrong turns to pin down.

!!! important "The short version"

    **The representation type does not know, it cannot be trusted, and the character, both
    its field (real or complex) and its order (scalar, vector, or tensor), is a property of
    the quantity rather than of its storage.**

The rest of this post earns that claim.

## A short history of quantity character

### Before: dimension-safe and nothing more

In **mp-units** v0.8.0 a quantity was defined like this:

```cpp
template<Dimension D, UnitOf<D> U, Representation Rep = double>
class quantity {
  Rep number_;
  // ...
};
```

A quantity was a dimension, a unit, and a number. That is unit-safe and dimension-safe,
and it already prevents a large class of bugs. It does nothing for the conflations that
share a dimension: _speed_ and _velocity_, or the four _powers_ from the opening, were all
identical to the type system. There was simply no place to record the difference.

### The name comes from ISO 80000

We did not invent the term. ISO 80000-1:2009 says, when defining how a derived quantity's
dimension is formed:

!!! quote "ISO 80000-1:2009"

    In deriving the dimension of a quantity, no account is taken of its scalar, vector, or
    tensor character.

That single sentence is doing a lot of work. It tells you that a quantity has a
_character_, that the character is one of scalar, vector, or tensor, and that the
dimension deliberately ignores it. In other words, the standard itself says the dimension
is not enough to describe a quantity. The character is a first-class property of the
quantity in the ISQ, and the dimension throws it away on purpose. So when **mp-units** set
out to be not only unit-safe and dimension-safe but _quantity-safe_, the character was the
obvious missing property, and ISO handed us both the concept and the name.

We started with exactly those three: scalar, vector, tensor.

### The CppCon pivot: complex enters the game

Three characters were enough until the power-systems engineer from the opening. His domain
needs a distinction that scalar, vector, and tensor do not capture. We need to be able to
discriminate between a _real_ scalar and a _complex_ one. _Complex power_ is a complex
scalar, and _active_, _reactive_, and _apparent power_ are
its real part, imaginary part, and modulus. Telling the three real powers apart from one
another is a matter of quantity _kind_, since they share a character. What _character_ adds
is the orthogonal real-versus-complex distinction.

Consider how this plays out in code. _apparent power_ and _complex power_ are both measured
in volt-amperes, so in a units library that keys on the unit alone, this compiles and tells
you almost nothing:

```cpp
auto p = value * VA;   // apparent power? complex power? VA is the unit of both
```

The unit cannot decide which quantity `p` is, the library has no way to check that the type
of `value` is the right one, and the `auto` leaves the identifier `p` as the only hint of
what was meant. Everyone in the field knows the storage convention, _complex power_ in a
`std::complex` and the rest in a `double`, but the library cannot enforce a convention it
cannot see.

In **mp-units** you name the quantity, and character checks the representation against it:

```cpp
auto reading = read_meter();      // returns a double, today
auto phasor  = read_phasor();     // returns a std::complex<double>

quantity ap = reading * isq::apparent_power[VA];    // real scalar: a double satisfies it
quantity cp = phasor  * isq::complex_power[VA];     // complex scalar: a std::complex satisfies it
// quantity bad = reading * isq::complex_power[VA]; // error: reading is real, not complex
```

A `std::complex<double>` satisfies only `isq::complex_power`, and a `double` only
`isq::apparent_power`. The two are now distinct, incompatible types even though both are
spelled in `VA`, and a representation that does not match the character does not compile.

Note that the types of `reading` and `phasor` are nowhere in sight at the multiplication,
and they may be refactored later. You do not have to eyeball them, and you should not have
to. The day `read_phasor()` is changed to return a plain `double`, the `complex_power` line
stops compiling instead of silently discarding the phase.

The same character governs how these quantities are _defined_, not only how they are stored.
_apparent power_ is, by definition, the modulus of _complex power_, and in V3 that is exactly
how its specification will read:

```cpp
inline constexpr struct apparent_power : quantity_spec<modulus(complex_power)> {} apparent_power;
```

`modulus()` is meaningful only on a complex quantity. Strip the character away and
`modulus(apparent_power)`, or `modulus(active_power)`, would compile just as readily, defining
a quantity as the modulus of something that has no imaginary part. That is physical nonsense,
and character is what keeps the defining equation well-formed only where it makes sense.

So we extended the list. Scalar became `real_scalar` and `complex_scalar`, and for a while
the character was a flat enumeration of four values: `real_scalar`, `complex_scalar`,
`vector`, `tensor`.

### The recent realization: a flat list does not scale

A flat list of four hard-codes an assumption that turns out to be false: that "complex"
only ever happens to scalars. It does not. A _complex vector_ and a _complex tensor_ are
perfectly ordinary in electromagnetism and signal processing: the phasor _electric field
strength_ of a time-harmonic wave is a complex vector, and the _permittivity_ of a lossy
anisotropic medium is a complex second-order tensor. The flat enumeration could not name
either. We had quietly baked "real" into every vector and tensor.

That is what triggered the refactor this post accompanies, and the article itself. The fix
was to stop treating the character as a flat list and recognize that it was two
independent questions all along.

## Two axes, not four values

A quantity's character is a pair:

- a **field**: real or complex,
- an **order**: scalar, vector, or tensor.

The four flat values were just four of the six combinations, with the other two (complex
vector, complex tensor) accidentally unreachable. Splitting the character into two
orthogonal axes makes all six expressible and removes the special-casing.

The order axis stops at scalar, vector, and tensor, the three ISO 80000-2 recognizes. We did
look further: geometric algebra offers richer objects such as bivectors and pseudoscalars,
and for a while it was tempting to model them too. We decided against it. No ISQ quantity
needs the distinction (the standard treats those quantities as vectors and scalars), and a
character that no quantity uses is precisely the scope creep we are trying to avoid. This
is the rule the whole design follows: a character earns its place only when a real quantity
cannot be expressed without it, which is exactly how complex got in and exactly why
bivectors stayed out.

That choice is not free, and the cost is worth naming. "Order" is a projection of the richer
geometric-algebra notion of "grade," and the projection depends on the dimension of space.
A bivector such as _angular velocity_ is a single number in 2D and a pseudovector in 3D,
and the ISQ fixes the 3D view and calls it a vector, so its character is order 1. But the
standard form in rigid-body dynamics and robotics is the antisymmetric _angular velocity
tensor_, a 3×3 skew-symmetric matrix, which is order 2. The rank-ordering that lets a scalar
stand in for a vector does not run the other way, so that tensor cannot fill the vector slot,
and an engineer who reaches for the usual robotics representation of _angular velocity_ finds
it turned away. Chip Hogg pressed this point in the comments below, and he is right that it
is a real barrier for that audience. Two things bound it. The concern lives entirely in
ordinary 2D and 3D space, where a quantity's components are one homogeneous quantity. We do
not model more than three spatial dimensions, because a four-component "vector" mixes space
and time, its components stop being one homogeneous quantity, and it is no longer an ISQ
quantity at all. Users who need the tensor form today can already do so, defining it as a
separate quantity and bridging it with an explicit dual, as shown in the
[how-to guide](../../how_to_guides/advanced_usage/tensor_representation_of_axial_vectors.md).
Whether the library should provide that bridge itself is a question we return to at the end.

This is the point in the story where the temptation from
[#648](https://github.com/mpusz/mp-units/issues/648) returns with full force, because once
you have two clean axes it looks like the representation type could just answer both of them.
It cannot, and here is why.

## Why the type cannot tell you

### The type is underdetermined

The same storage type legitimately backs quantities of different characters. `double` is
the clearest case. It is a real scalar for _mass_. It is a one-dimensional real _vector_
for _velocity_ along a known axis, where the sign carries the direction. It is even a
degenerate real _tensor_ for a scalar _stress_ measure. All three are normal engineering
practice, and they all store the value in the same `double`. That a scalar may serve as a
vector or a tensor at all is ISO 80000-2's own ordering:

!!! quote "ISO 80000-2"

    A vector is a tensor of the first order and a scalar is a tensor of order zero.

```cpp
quantity m  = isq::mass(5.0 * kg);                 // real scalar
quantity v  = isq::velocity(-3.0 * m / s);         // 1-D vector: sign is direction
quantity sp = isq::speed(3.0 * m / s);             // real scalar, non-negative by meaning
quantity st = isq::stress(100.0 * Pa);             // degenerate tensor: a scalar stress measure
```

If the character came from the type, all three would be identical, and there would be no
way to say that a _speed_ is the magnitude of a _velocity_, a relationship that runs one
way only (a _velocity_ has a _speed_, but a _speed_ has no direction to recover a _velocity_
from), or that _speed_ is non-negative while _velocity_ is signed. The type cannot
distinguish them, because they are the same type. The distinguishing information lives in
the quantity, not in the number.

### A richer type is not the right type

The reverse mistake is just as easy. Conflate _speed_ and _velocity_ the other way and put
a 3D vector into a _speed_, which is a scalar quantity. A library that follows the type
inspects that vector, sees that it offers `scalar_product`, `vector_product`, and
`magnitude`, and happily accepts calling any of those on what was declared a _speed_. It is
not fine. _Speed_ is a scalar, and a 3D vector is the wrong representation for it no matter
how many vector operations it exposes. The representation type's rich API is exactly what
lures a follow-the-type library into accepting it. Character rejects it, because the quantity
says scalar and a scalar slot does not take an order-1 representation.

### The type lies

Even when the representation is a richer type, its surface is not a reliable witness of
its character. This is not hypothetical. It is exactly what the two most popular C++
linear algebra libraries do.

- **Eigen and Blaze expose `real()` and `imag()` on their _real_ matrices and vectors**,
  because a real value is a degenerate complex one. If you detect "complex" by the
  presence of `real()`/`imag()`, every real Eigen vector is misread as complex. We hit
  this during the refactor: the build classified a real `Eigen::Vector3d` quantity as
  complex until we stopped reading those members off the container and looked at its
  element instead. And because real and complex are matched
  exactly, that is worse than a stray label: a vector marked complex is no longer real,
  so it can no longer back the real quantity it actually is, a real _velocity_ for
  instance. A correct and common use simply stops compiling.
- **An Eigen column vector is an `N x 1` matrix**, so it exposes a two-index
  `operator()(i, j)`. If you detect "tensor" by the presence of two-index access, every
  Eigen vector is misread as an order-2 tensor.

Neither of these is a bug in Eigen or Blaze. Both are mathematically defensible: the reals
embed in the complex numbers, and a vector is a one-column matrix. That is the whole
point. The type's exposed surface is genuinely ambiguous about character, so a units
library that simply "follows the type" will follow it straight into a misclassification.

Underneath both is one fact: a representation type describes storage, not character. A
matrix is a rectangular array of numbers. A tensor is a different kind of object, defined
by how its components transform when the coordinate frame changes. Every second-order
tensor can be written as a 3×3 matrix once a basis is fixed, but not every 3×3 matrix is a
tensor. Whether an `Eigen::Matrix3d` is a _stress_ tensor or just a table of nine numbers
is a fact about the quantity, not the type, and no amount of inspecting the matrix recovers
it. That fact is the character, and it lives on the quantity.

### The operations a type offers are not the ones a quantity allows

The objection from [#648](https://github.com/mpusz/mp-units/issues/648) has an operational
form, aimed at character's second job. Grant that the character belongs on the quantity. Why
should it govern the arithmetic too? Why not just let the representation offer whatever
operations it happens to have? Because the operations a type exposes are a menu of what is
_syntactically_ possible, not what is _physically_ legal, and the gap between those two is
where the bugs live.

A `double` offers `*` and `abs()` whether it holds a _velocity_ or a _speed_, and nothing
stops you from reaching for them:

```cpp
double vx = 3.0, vy = 4.0;   // two velocity components, in m/s
auto k = vx * vy;            // compiles: 12. a scalar product? a vector product? neither, just m²/s²
auto s = std::abs(vx);       // a speed? only if vx was a velocity; on a value already a speed it does nothing
```

Both operations compile, and both are wrong or controversial. The product of two _velocity_
components is neither a _scalar product_ nor a _vector product_, just a number with a unit.
`abs()` is meaningful on a _velocity_ and meaningless on a _speed_, and the `double` cannot
tell the two apart. A real Eigen vector, as we just saw, even offers `real()` and `imag()`
it has no business offering. Let the available operations drive the calculation and you have
built a machine for confidently computing the wrong thing. Only the quantity's character knows
which operations make sense and what they produce, which is why, in V3, the operations are
defined on the character rather than scavenged from the representation.

This goes deeper than any single calculation. When you do
[pure dimensional analysis](../../how_to_guides/advanced_usage/pure_dimensional_analysis.md),
or when the ISQ itself _defines_ a derived quantity, there is no representation type in sight
at all, only quantity specifications and their characters. _velocity_ is _displacement_ over
_duration_, _work_ is the scalar product of _force_ and _displacement_, and _moment of force_
is the vector product of a _position vector_ and a _force_. Which of those equations is even
well-formed, and which quantity each one produces, is decided by the character of the
operands, with no number anywhere to consult. The correctness of the entire ISQ rests on
character governing operations at the specification level. Here the trouble with following
the representation is not that it is unreliable. There is no representation to follow.

### The field is a domain fact, not a storage fact

The power-systems case makes this sharpest. Whether a power is _active_, _reactive_,
_apparent_, or _complex_ is fixed by what the quantity _means_, before any C++ type is
chosen. A _complex power_ is complex because it carries a magnitude and a phase. An _active
power_ is real because it is a single signed number. That is a fact about the physics, not
about storage, and the storage is what must conform to it, not the other way around.

This is why **mp-units** matches the field _exactly_: a real quantity requires a real
representation, and a complex quantity requires a complex one, with no implicit lift
between them. It is tempting to allow the real-to-complex direction, since the reals embed
in the complex numbers: a `double` is a complex number with a zero imaginary part. We tried
it, and it is a trap (more on that below). A real representation has nowhere to hold an
imaginary part, so a quantity that starts in a `double` can never grow one, and the first
power-systems calculation that needs the phase has nowhere to put it.

### So the character lives on the quantity, and the bridge is a customization point

Put these observations together and the design follows. The character cannot be read
off the type, so it lives on the `quantity_spec`, which is where the quantity's meaning
already lives. It is declared once, with the quantity, and inherited through the equations
that derive other quantities:

```cpp
inline constexpr struct displacement : quantity_spec<length, quantity_tensor_order::vector> {} displacement;
inline constexpr struct velocity : quantity_spec<displacement / duration> {} velocity;  // vector, inherited
inline constexpr struct moment_of_inertia
  : quantity_spec<angular_momentum / angular_velocity, quantity_tensor_order::tensor> {} moment_of_inertia;
```

The representation type still has to answer a narrower, humbler question: can this storage
actually hold and manipulate a value of the required character? Two traits answer it, and
the two axes need different amounts of help:

- [`numeric_field<T>`](../../users_guide/framework_basics/representation_types.md#numeric_field)
  reports the field. It reads `real()`/`imag()` off a scalar _element_ of the type, reached
  by indexing, rather than off the container's surface. That is what defeats the Eigen and
  Blaze problem for free: a real matrix exposes `real()`/`imag()` on its surface, but its
  element is a plain `double`, so it is correctly classified real, no adapter override needed.
- [`tensor_order<T>`](../../users_guide/framework_basics/representation_types.md#tensor_order)
  reports the order, from a type's indexing operators (`t[i]` for a vector, `t(i, j)` for
  a tensor). An Eigen column vector is an `N x 1` matrix that exposes both, so its order is
  genuinely ambiguous: `tensor_order` has no default for it, and the Eigen adapter supplies
  the order from `RowsAtCompileTime` / `ColsAtCompileTime`.

The one override that remains is genuinely small. The entire Eigen adapter for the character
axes is a single line:

```cpp
template<typename T>
  requires /* T is an Eigen type */
constexpr std::size_t tensor_order<T> = (T::RowsAtCompileTime == 1 || T::ColsAtCompileTime == 1) ? 1 : 2;
```

This is the answer to [#648](https://github.com/mpusz/mp-units/issues/648). The
character is not redundant with the type, because the type
is underdetermined, unreliable, and the wrong place for a domain fact. And this is the
answer to the scope-creep charge: we did not bloat the quantity with a speculative
feature. We recorded a property the **ISO standard** says a quantity has, that a real
engineer says he cannot work without, and we kept the type's role as small as possible: a
reasonable default plus a one-line override. The default handles Blaze and the built-in
vector and tensor types unaided. Only Eigen, whose `N x 1` vector is genuinely ambiguous,
needs the override, so both the default and the escape hatch earn their place.

## The dead ends (where the design, and the AI, kept slipping)

Much of this design was worked out in conversation with an AI, leaned on not to write the
code but to reason about the physics and mathematics where my own footing is least sure:
geometric algebra, complex analysis, the corners of ISO 80000. Even there it was a good
measure of how subtle the space is. The intuitive answer is reliably the wrong one, and the
plausible-but-wrong model kept resurfacing, in its suggestions and in my own, until a
concrete engineering scenario or an ISO clause settled it. The interesting part is not that
a capable Artificial Intelligence stumbled. It is _where_ it stumbled, because those are
exactly the places a human designer slips too. Each dead end taught a reusable principle.

### Trap 1: read the character off the type

The tempting model: let the representation type answer everything. It
looks right for `std::complex` and `Eigen::Vector3d`. It is wrong because `double` backs
three characters at once and because Eigen and Blaze misreport both axes. This one was the
stickiest of all. It kept coming back every time the code needed the character, and only
the Eigen/Blaze reality finally settled it. **Principle: the character is the quantity's,
not the storage's.**

### Trap 2: let real quietly satisfy complex

Because the reals embed in the complex numbers, it seems harmless to let a real
representation fill a complex slot. It is not. Start a quantity in a `double` and you can
never grow an imaginary part into it: the first calculation that produces one has nowhere
to put it. In the power-systems domain that is the exact bug we are trying to prevent. The
fix is exact, disjoint field matching. **Principle: a representation is safe only when a
domain expert would sign off on it, and real-only storage for a complex quantity is not.**

### Trap 3: flag the character, take 1 (opt in)

The earliest V2 releases made character a manual opt-_in_. A representation declared itself
through `is_scalar`, `is_vector`, and `is_tensor`. Because vector and tensor defaulted to
`false`, every linear-algebra type a user wanted had to be marked by hand
(`inline constexpr bool mp_units::is_vector<my_type> = true;`). It worked, but the burden
fell on the caller: each user, for each type, had to remember to annotate something the
library could in principle work out for itself. 2.5.0 removed it. **Principle: detect what
you can, do not tax every user with an annotation the library could derive.**

### Trap 4: flag the character, take 2 (opt out)

The model that followed flipped the polarity. It detected a character by default and let a
type opt _out_ through `disable_vector` and `disable_tensor`. Less boilerplate, but the same
shape of mistake: a flag standing in for something that is not a matter of choice. A vector
and a tensor share the same algebraic API, the same addition, scaling, and magnitude, so
those operations cannot tell them apart. The difference is purely one of order, how many
indices it takes to address an element, so "a tensor is not a vector" is not an opt-out at
all but a fact about order. Replacing the flags with the intrinsic `tensor_order` trait made
the rule fall out for free: a tensor has order 2, a vector slot wants order at most 1, and
`2` does not fit. **Principle: do not encode a structural fact as a flag, compute it.**

### Trap 5: one narrow opt-out instead of one general one

Even after the traits landed, a single narrow knob survived, `disable_real`, whose only
real job was to keep `bool` out. But "should this type be a representation at all?" is a
different and more general question than "is it a real scalar?", and there was no knob for
it. We consolidated to one character-agnostic
[`disable_representation<T>`](../../users_guide/framework_basics/representation_types.md#disable_representation).
At first it defaulted to `false`, with a single explicit specialization for `bool`,
taking over the one job `disable_real` had. `NotQuantity` was still a separate concept
at that point, keeping quantities and quantity-like types out of a representation slot.
It was exactly the same category of opt-out, so we folded it into `disable_representation`
too, leaving one trait to answer for both `bool` and the quantity-like types. Fewer knobs,
and the general question finally has an answer. **Principle: when you find yourself adding
the third special-case opt-out, step back and look for the general one.**

### Trap 6: trust the type's `real()`/`imag()`, then patch the types that lie

The first approach was the naive one: a type is complex when it exposes `real()`/`imag()`,
and the types that lie about that, the Eigen and Blaze matrices that expose those members
even when real, get an explicit `numeric_field` override declaring the field from their
element type. It worked, but the override is per-type boilerplate, and worse, it makes a
type's field depend on whether the adapter is in scope, which is an ODR hazard Chip Hogg
flagged in review.

Chip also pointed to the way out: read the field off the scalar element from the start,
which removes the override entirely. Getting there took two wrong turns. His own spelling,
following the recursive `value_type` chain, walks straight through `std::complex`, whose own
`value_type` is `double`, so a complex vector comes out _real_. Reaching instead for the
value returned by `magnitude()` fails from the other side: the norm of a complex vector is
real, so a magnitude reports every complex quantity as real. One probe looks a level below
the field, the other a level above it. The landing, which the section above describes, is
to take one element by indexing and read `real()`/`imag()` off it directly, never following
the recursive `value_type` chain that would walk past `std::complex` to `double`. Reaching
the element structurally also keeps the field independent of whether an adapter is in scope,
closing the ODR hole for the field axis.
**Principle: detect a property where it lives, on the element, not off a container surface
you then patch type by type.**

### Trap 7: relax the vector slot to accept the tensor

_angular velocity_ is a vector in the ISQ (ISO 80000-3, item 3-12), yet robotics and
rigid-body dynamics routinely represent it as an antisymmetric 3×3 matrix, which is order
2. Chip Hogg made the case that shutting that representation out is a real barrier, and our
first answer was to let such quantities accept a _band_ of orders, so the matrix form would
fit the vector slot alongside the vector form. It does not hold up, for three reasons that
build on each other. There is no way to relax the slot for the bivector quantities alone
without first teaching the library which quantities are bivectors, and that is exactly the
geometric-algebra grade machinery we declined to add. Relaxing it for _all_ vectors instead
is absurd: a _force_ is not a matrix, and nothing would want that. And the band rests on a
false premise anyway, because the matrix is not the same quantity wearing a larger
representation. ISO 80000-3 defines the vector _angular velocity_ as a scalar rate of
rotation carrying the axis direction, not as a product of two vectors, and the matrix is a
different object, the operator form of the map that sends a position to its velocity, tied
to the vector by an explicit dual (the hat and vee maps) rather than by any operation that
yields the vector. Character is part of a quantity's identity, and the ISQ defines only the
vector, so the honest model is the opposite of a band: the antisymmetric-tensor form is a
_different_ quantity, defined in the user's own domain and bridged to the ISQ vector by that
explicit dual. **Principle: do not widen a quantity to swallow a representation of a
different character. A different character is a different quantity, and the bridge between
them is an explicit operation, not a relaxed constraint.**

## Back to the scope question

Which brings us back to Chip's working hypothesis from
[#683](https://github.com/mpusz/mp-units/discussions/683). Two of its points we took to
heart, and the design is better for them. The vector / pseudovector / multivector
sophistication really is a rabbit hole with no single right answer, so we do not model it:
the order axis stops at scalar, vector, and tensor. And the implementation he called fraught,
the scalar-as-vector workaround in particular, really was. Back then, character was
opt-in: a type counted as a vector only if you declared it one. The Kalman-filter examples
store their vector quantities in plain `double`s, so they had to opt those in by hand:

```cpp
template<class T>
  requires mp_units::is_scalar<T>
constexpr bool mp_units::is_vector<T> = true;
```

One line, but a sweeping one: it reclassifies every scalar in the program as a vector, just
to let a few `double`s act as one-dimensional vectors. That is exactly the kind of hand-written
flag the rank-ordering model removes. A scalar is now a degenerate vector on its own, because
its order is below the vector's, with nothing for the user to declare. His critique did its
job: it narrowed the scope and pushed the implementation toward something simpler.

Where we landed differently is one empirical question, whether the mistakes actually happen.
Chip's bet was that they would not, because few users sit down and pick a type with the
wrong character, and on that he is right. But the wrong character does not arrive by
deliberate choice. It arrives from ordinary code: one type serving two characters (a
`double` that is a _speed_ in one function and a _velocity_ in the next), a return type
refactored under its callers (`read_phasor()` quietly becoming a `double`), or a unit that
names two quantities at once (`VA` for both _apparent power_ and _complex power_). None of
those require anyone to choose badly, only to write a large program over time. And the
ledger is not only "forbidden combinations": character also decides which operations are
legal and how
derived quantities are formed, and at the specification level, in the ISQ's own defining
equations, there is no representation to choose in the first place. That is the part of the
cost/benefit the hypothesis did not weigh, and it is where most of the value turns out to
be.

## What shipped, and what is still V3

To be clear about scope, because it matters. What landed now is the _foundation_: the
two-axis character model on the `quantity_spec`, the representation concepts that match a
representation to a character, and the `numeric_field` / `tensor_order` /
`disable_representation` customization points. The whole character model is, at its core,
just this:

```cpp
// the two orthogonal axes
enum class quantity_tensor_order : std::int8_t { scalar, vector, tensor };  // rank-ordered: a lower order fills a higher
enum class quantity_field : std::int8_t { real, complex };                  // matched exactly: never one for the other

// a character is exactly one of each
struct quantity_character {
  quantity_tensor_order order = quantity_tensor_order::scalar;
  quantity_field field = quantity_field::real;
};
```

That is what makes _apparent power_ and _complex power_, or _speed_ and _velocity_
incompatible where they should be. Everything else, the concepts, the traits, the matching
rules, is built on top of those two little enums.

The richer machinery from [Bringing Quantity-Safety To The Next
Level](bringing-quantity-safety-to-the-next-level.md), the character-specific operations
such as `scalar_product` and `vector_product`, the affine-like relationships inside a
single quantity tree, and the full quantity-level complex story where _active power_ is
the real part of _complex power_, all land properly in **V3**. The power-systems
engineer's complete wish list is not in your package manager yet. The abstraction along
which it becomes expressible is.

## Open questions

The journey is genuinely unfinished, and one question matters more than the rest: is the
two-axis split the right granularity, or will a real domain eventually need a distinction
we have not anticipated, the way complex surprised us once already? Complex was not on the
roadmap until an engineer made the case for it, and the next axis, if there is one, will
probably arrive the same way.

One such question is already on the table. Because a quantity's character is part of its
identity, the order-2 antisymmetric-tensor form of _angular velocity_ is not the same
quantity as the vector one. The ISQ defines only the vector (ISO 80000-3 gives _angular
velocity_ as a scalar rate of rotation carrying the axis direction, with no tensor form),
so the antisymmetric-tensor representation that robotics and rigid-body dynamics treat as
standard is, in this model, a separate quantity living outside the ISQ, related to
`isq::angular_velocity` by an explicit dual map rather than by an implicit conversion. That
is deliberate, since the two are genuinely different mathematical objects, but it leaves a
real cost: a user working in the tensor form has to define those quantities and bridge them
by hand, as the
[how-to guide](../../how_to_guides/advanced_usage/tensor_representation_of_axial_vectors.md)
shows. Whether the library should provide that bridge, a ready-made dual between an axial
vector and its antisymmetric tensor, without introducing geometric-algebra grades as
first-class characters, is the open question. Relaxing the vector quantities to also accept
tensor storage is not the answer, for the reasons in
[Trap 7](#trap-7-relax-the-vector-slot-to-accept-the-tensor) above.

So if you work in a domain where these distinctions are load-bearing, electrical power,
structural mechanics, electromagnetism, robotics, or anywhere _mass_ and _weight_ have ever
been confused in a code review, we would like to hear how this model holds up against your
reality, and where it does not. The comments are open.
