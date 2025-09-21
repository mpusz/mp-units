---
date: 2024-11-04
authors:
 - mpusz
categories:
 - Metrology
comments: true
---

# International System of Quantities (ISQ): Part 5 - Benefits

In the previous articles, we introduced the International System of Quantities, described how we
can model and implement it in a programming language, and presented the issues of software that
does not use such abstraction to implement a units library.

Some of the issues raised in [Part 2](isq-part-2-problems-when-isq-is-not-used.md) of our series
were addressed in [Part 3](isq-part-3-modeling-isq.md) already. This article will present
how our ISQ model elegantly addresses the remaining problems.

<!-- more -->

## Articles from this series

- [Part 1 - Introduction](isq-part-1-introduction.md)
- [Part 2 - Problems when ISQ is not used](isq-part-2-problems-when-isq-is-not-used.md)
- [Part 3 - Modeling ISQ](isq-part-3-modeling-isq.md)
- [Part 4 - Implementing ISQ](isq-part-4-implementing-isq.md)
- Part 5 - Benefits
- [Part 6 - Challenges](isq-part-6-challenges.md)


## Generic but safe interfaces

Let's start with the implementation of a
[generic utility function that would calculate the average speed based on provided arguments](isq-part-2-problems-when-isq-is-not-used.md#no-way-to-specify-a-quantity-type-in-generic-interfaces).
The resulting quantity should use a derived unit of the provided arguments (e.g., `km/h` for
`km` and `h`, `m/s` for `m` and `s`, ...).

With C++ concepts backed up with ISQ quantities, we can simply type it as:

```cpp
constexpr QuantityOf<isq::speed> auto avg_speed(QuantityOf<isq::length> auto d,
                                                QuantityOf<isq::time> auto t)
{
  return d / t;
}
```

The above constrains the algorithm to proper quantity types and ensures that a quantity of speed
is returned. The latter is essential not only for the users to better understand what the function
does but also serves as a unit test for our implementation. It ensures that our quantity equations
are correct in the implementation part of the function, and we indeed return a quantity of _speed_.


## Non-convertible units of currency

Our second example was about
[disjoint units of _currency_](isq-part-2-problems-when-isq-is-not-used.md#disjoint-units-of-the-same-quantity-type-do-not-work).
We want to use various units of _currency_ but we can't provide compile-time known conversion
factors between those as such ratios are only known at runtime.

First, we define:

- a new dimension for _currency_ and a quantity type based on it,
- set of disjoint units of _currency_ for its quantity kind.

```cpp
inline constexpr struct dim_currency final : base_dimension<"$"> {} dim_currency;
inline constexpr struct currency final : quantity_spec<dim_currency> {} currency;

inline constexpr struct euro final : named_unit<"EUR", kind_of<currency>> {} euro;
inline constexpr struct us_dollar final : named_unit<"USD", kind_of<currency>> {} us_dollar;

namespace unit_symbols {

inline constexpr auto EUR = euro;
inline constexpr auto USD = us_dollar;

}

static_assert(!std::equality_comparable_with<quantity<euro, int>, quantity<us_dollar, int>>);
```

Next, we can provide a custom currency exchange facility that accounts for a specific point in time:

```cpp
template<Unit auto From, Unit auto To>
[[nodiscard]] double exchange_rate(std::chrono::sys_seconds timestamp)
{
  // user-provided logic...
}

template<UnitOf<currency> auto To, QuantityOf<currency> From>
QuantityOf<currency> auto exchange_to(From q, std::chrono::sys_seconds timestamp)
{
  const auto rate =
    static_cast<From::rep>(exchange_rate<From::unit, To>(timestamp) * q.numerical_value_in(q.unit));
  return rate * From::quantity_spec[To];
}
```

Finally, we can use our simple model in the following way:

```cpp
using namespace unit_symbols;
using namespace std::chrono;

const auto yesterday = time_point_cast<seconds>(system_clock::now() - hours{24});
const quantity price_usd = 100 * USD;
const quantity price_euro = exchange_to<euro>(price_usd, yesterday);

std::cout << price_usd << " -> " << price_euro << "\n";
// std::cout << price_usd + price_euro << "\n";  // does not compile
```

!!! note

    It would be better to model the above prices as quantity points, but this is a subject
    for a different article :wink:.


## Derived quantities of the same dimension but different kinds

Until now, the issues discussed have not actually required modeling of the ISQ. The introduction
of physical dimensions would be enough, and indeed, this is what most of the libraries on the
market do. However, we have more exciting challenges to solve as well.

The next issue was related to different quantities having the same dimension. In many cases, we
want to prevent conversions and any other compatibility between such distinct quantities.

Let's try to implement
[our _fuel consumption_ example](isq-part-2-problems-when-isq-is-not-used.md#derived-quantities-of-the-same-dimension-but-different-kinds).
First, we define the quantity type and a handy identifier for a derived unit that we want to use:

```cpp
inline constexpr struct fuel_consumption final : quantity_spec<isq::volume / isq::length> {} fuel_consumption;
inline constexpr auto L_per_100km = si::litre / (mag<100> * si::kilo<si::metre>);

static_assert(fuel_consumption != isq::area);
static_assert(fuel_consumption.dimension == isq::area.dimension);
```

Next, we define two quantities. The first one is based only on a derived unit of `L/[100 km]`,
while the second uses a strongly typed quantity type:

```cpp
quantity q1 = 5.8 * L_per_100km;
quantity q2 = fuel_consumption(6.7 * L_per_100km);
std::println("Fuel consumptions: {}, {}", q1, q2);

static_assert(implicitly_convertible(q1.quantity_spec, isq::area));
static_assert(!implicitly_convertible(q2.quantity_spec, isq::area));
static_assert(!explicitly_convertible(q2.quantity_spec, isq::area));
static_assert(!castable(q2.quantity_spec, isq::area));
```

As we can see, with just units (especially derived ones) and dimensions, we often can't achieve
the same level of safety as with adequately modeled hierarchies of quantities. Only in case of `q2`
we can prevent incorrect conversions to a different quantity of the same dimension.


## Various quantities of the same dimension and kinds

In the previous example, _area_ and _fuel consumption_ were quantities of the same dimension but
of different kinds. In engineering, there are also many cases where we need to model distinct
quantities of the same kind.

Let's try to improve the safety of
[our `Box` example](isq-part-2-problems-when-isq-is-not-used.md#various-quantities-of-the-same-dimension-and-kinds).

First, we need to extend our ISQ definitions by the _horizontal length_ quantity and a
_horizontal area_ derived from it:

```cpp
inline constexpr struct horizontal_length final : quantity_spec<isq::length> {} horizontal_length;
inline constexpr struct horizontal_area final : quantity_spec<isq::area, horizontal_length * isq::width> {} horizontal_area;
```

!!! note

    `isq::length` denotes any quantity of _length_ (not only the horizontal one).

```cpp
static_assert(implicitly_convertible(horizontal_length, isq::length));
static_assert(!implicitly_convertible(isq::length, horizontal_length));

static_assert(implicitly_convertible(horizontal_area, isq::area));
static_assert(!implicitly_convertible(isq::area, horizontal_area));

static_assert(implicitly_convertible(isq::length * isq::length, isq::area));
static_assert(!implicitly_convertible(isq::length * isq::length, horizontal_area));

static_assert(implicitly_convertible(horizontal_length * isq::width, isq::area));
static_assert(implicitly_convertible(horizontal_length * isq::width, horizontal_area));
```

With simple two lines of definition, we made the above logic automatically work without needing
additional customization for special cases. Based on hierarchies of derived quantities and their
recipes, the proposed model automatically inherits the properties of base quantities involved in
the recipe. This makes the composition of derived quantities very easy, which is not true for
alternative solutions based on tag types that do not compose their properties.

Now we can refactor our `Box` to benefit from the introduced safe abstractions:

```cpp
class Box {
  quantity<horizontal_length[m]> length_;
  quantity<isq::width[m]> width_;
  quantity<isq::height[m]> height_;
public:
  Box(quantity<horizontal_length[m]> l, quantity<isq::width[m]> w, quantity<isq::height[m]> h):
    length_(l), width_(w), height_(h)
  {}

  quantity<horizontal_area[m2]> floor() const { return length_ * width_; }
  // ...
};
```

It is important to note that the safety can be enforced only when a user provides typed quantities
as arguments to the functions:

```cpp
Box my_box1(2 * m, 3 * m, 1 * m);
Box my_box2(2 * horizontal_length[m], 3 * isq::width[m], 1 * isq::height[m]);
Box my_box3(horizontal_length(2 * m), isq::width(3 * m), isq::height(1 * m));
```

!!! important

    It is up to the user to decide when and where to care about explicit quantity types
    and when to prefer simple unit-only mode.


## Various kinds of dimensionless quantities

Most of the quantities hierarchies describe only one kind. There are some exceptions, though.
One of them is a [hierarchy of _dimensionless_ quantities](isq-part-4-implementing-isq.md#modeling-a-hierarchy-of-kind-dimensionless).
This tree defines quantities that denote:

- counts (e.g., _storage capacity_),
- ratios (e.g., _efficiency_),
- angles (e.g., _angular measure_, _solid angular measure_),
- scaled numbers.

Each of the above could form a separate tree of mutually comparable quantities. However, all of
them have a common property. Every quantity from this tree, despite often being measured in a
dedicated unit (e.g., `bit`, `rad`, `sr`), should also be able to be measured in a unit `one`.

We've seen how to model such a hierarchy in a
[previous article in our series](isq-part-4-implementing-isq.md#modeling-a-hierarchy-of-kind-dimensionless).
This time, we will see a simplified part of a concrete, real-life example for this use case.

We often need to provide strong types for different counts in the digital signal processing domain.
Abstractions like _samples_, _beats_, _MIDI clock_, and others should not be possible to be
intermixed with each other:

```cpp
namespace ni {

inline constexpr struct SampleCount final : quantity_spec<dimensionless, is_kind> {} SampleCount;
inline constexpr struct UnitSampleAmount final : quantity_spec<dimensionless, is_kind> {} UnitSampleAmount;
inline constexpr struct MIDIClock final : quantity_spec<dimensionless, is_kind> {} MIDIClock;
inline constexpr struct BeatCount final : quantity_spec<dimensionless, is_kind> {} BeatCount;
```

We should also be able to create derived quantities from those. For example, when we divide such
a quantity by _time_ we should get a new strong quantity that can be measured in both a dedicated
unit (e.g., `Smpl/s` for _sample rate_) and hertz:

```cpp
inline constexpr struct SampleDuration final : quantity_spec<isq::period_duration> {} SampleDuration;
inline constexpr struct SamplingRate final : quantity_spec<isq::frequency, SampleCount / SampleDuration> {} SamplingRate;

inline constexpr auto Amplitude = UnitSampleAmount;
inline constexpr auto Level = UnitSampleAmount;
inline constexpr struct Power final : quantity_spec<Level * Level> {} Power;

inline constexpr struct BeatDuration final : quantity_spec<isq::period_duration> {} BeatDuration;
inline constexpr struct Tempo final : quantity_spec<isq::frequency, BeatCount / BeatDuration> {} Tempo;
```

We can also define a collection of units associated with specific quantity kinds and their symbols:

```cpp
inline constexpr struct Sample final : named_unit<"Smpl", one, kind_of<SampleCount>> {} Sample;
inline constexpr struct SampleValue final : named_unit<"PCM", one, kind_of<UnitSampleAmount>> {} SampleValue;
inline constexpr struct MIDIPulse final : named_unit<"p", one, kind_of<MIDIClock>> {} MIDIPulse;

inline constexpr struct QuarterNote final : named_unit<"q", one, kind_of<BeatCount>> {} QuarterNote;
inline constexpr struct HalfNote final : named_unit<"h", mag<2> * QuarterNote> {} HalfNote;
inline constexpr struct DottedHalfNote final : named_unit<"h.", mag<3> * QuarterNote> {} DottedHalfNote;
inline constexpr struct WholeNote final : named_unit<"w", mag<4> * QuarterNote> {} WholeNote;
inline constexpr struct EightNote final : named_unit<"8th", mag_ratio<1, 2> * QuarterNote> {} EightNote;
inline constexpr struct DottedQuarterNote final : named_unit<"q.", mag<3> * EightNote> {} DottedQuarterNote;
inline constexpr struct QuarterNoteTriplet final : named_unit<"qt", mag_ratio<1, 3> * HalfNote> {} QuarterNoteTriplet;
inline constexpr struct SixteenthNote final : named_unit<"16th", mag_ratio<1, 2> * EightNote> {} SixteenthNote;
inline constexpr struct DottedEightNote final : named_unit<"q.", mag<3> * SixteenthNote> {} DottedEightNote;

inline constexpr auto Beat = QuarterNote;

inline constexpr struct BeatsPerMinute final : named_unit<"bpm", Beat / si::minute> {} BeatsPerMinute;
inline constexpr struct MIDIPulsePerQuarter final : named_unit<"ppqn", MIDIPulse / QuarterNote> {} MIDIPulsePerQuarter;

namespace unit_symbols {

inline constexpr auto Smpl = Sample;
inline constexpr auto pcm = SampleValue;
inline constexpr auto p = MIDIPulse;

inline constexpr auto n_wd = 3 * HalfNote;
inline constexpr auto n_w = WholeNote;
inline constexpr auto n_hd = DottedHalfNote;
inline constexpr auto n_h = HalfNote;
inline constexpr auto n_qd = DottedQuarterNote;
inline constexpr auto n_q = QuarterNote;
inline constexpr auto n_qt = QuarterNoteTriplet;
inline constexpr auto n_8thd = DottedEightNote;
inline constexpr auto n_8th = EightNote;
inline constexpr auto n_16th = SixteenthNote;

}

}  // namespace ni
```

With the above, we can safely work with each quantity and use SI or domain-specific units
as needed:

```cpp
using namespace ni::unit_symbols;
using namespace mp_units::si::unit_symbols;

const auto sr1 = ni::GetSampleRate();
const auto sr2 = 48'000.f * Smpl / s;

const auto samples = 512 * Smpl;

const auto sampleTime1 = (samples / sr1).in(s);
const auto sampleTime2 = (samples / sr2).in(ms);

const auto sampleDuration1 = (1 / sr1).in(ms);
const auto sampleDuration2 = (1 / sr2).in(ms);

const auto rampTime = 35.f * ms;
const auto rampSamples1 = (rampTime * sr1).force_in<int>(Smpl);
const auto rampSamples2 = (rampTime * sr2).force_in<int>(Smpl);

std::println("Sample rate 1 is: {}", sr1);
std::println("Sample rate 2 is: {}", sr2);

std::println("{} @ {} is {::N[.5f]}", samples, sr1, sampleTime1);
std::println("{} @ {} is {::N[.5f]}", samples, sr2, sampleTime2);

std::println("One sample @ {} is {::N[.5f]}", sr1, sampleDuration1);
std::println("One sample @ {} is {::N[.5f]}", sr2, sampleDuration2);

std::println("{} is {} @ {}", rampTime, rampSamples1, sr1);
std::println("{} is {} @ {}", rampTime, rampSamples2, sr2);
```

The above prints:

```text
Sample rate 1 is: 44100 Hz
Sample rate 2 is: 48000 Smpl/s
512 Smpl @ 44100 Hz is 0.01161 s
512 Smpl @ 48000 Smpl/s is 10.66667 ms
One sample @ 44100 Hz is 0.02268 ms
One sample @ 48000 Smpl/s is 0.02083 ms
35 ms is 1543 Smpl @ 44100 Hz
35 ms is 1680 Smpl @ 48000 Smpl/s
```

We can also do a bit more advanced computations to get the following:

```cpp
auto sampleValue = -0.4f * pcm;
auto power1 = sampleValue * sampleValue;
auto power2 = -0.2 * pow<2>(pcm);

auto tempo = ni::GetTempo();
auto reverbBeats = 1 * n_qd;
auto reverbTime = reverbBeats / tempo;

auto pulsePerQuarter = value_cast<float>(ni::GetPPQN());
auto transportPosition = ni::GetTransportPos();
auto transportBeats = (transportPosition / pulsePerQuarter).in(n_q);
auto transportTime = (transportBeats / tempo).in(s);

std::println("SampleValue is: {}", sampleValue);
std::println("Power 1 is: {}", power1);
std::println("Power 2 is: {}", power2);

std::println("Tempo is: {}", tempo);
std::println("Reverb Beats is: {}", reverbBeats);
std::println("Reverb Time is: {}", reverbTime.in(s));
std::println("Pulse Per Quarter is: {}", pulsePerQuarter);
std::println("Transport Position is: {}", transportPosition);
std::println("Transport Beats is: {}", transportBeats);
std::println("Transport Time is: {}", transportTime);
```

which prints:

```text
SampleValue is: -0.4 PCM
Power 1 is: 0.16000001 PCM²
Power 2 is: -0.2 PCM²
Tempo is: 110 bpm
Reverb Beats is: 1 q.
Reverb Time is: 0.8181818 s
Pulse Per Quarter is: 960 ppqn
Transport Position is: 15836 p
Transport Beats is: 16.495832 q
Transport Time is: 8.997726 s
```

!!! info

    More about this example can be found in
    ["Exploration of Strongly-typed Units in C++: A Case Study from Digital Audio"](https://www.youtube.com/watch?v=oxnCdIfC4Z4)
    CppCon 2023 talk by Roth Michaels.


## To be continued...

In the next part of this series, we will discuss the challenges and issues related to the modeling
of the ISQ with a programming language.
