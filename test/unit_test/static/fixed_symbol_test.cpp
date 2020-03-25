#include "units/symbol_text.h"

using namespace units;

namespace {

constexpr basic_symbol_text sym1('b');
static_assert(sym1 == 'b');
static_assert(sym1 != 'a');
static_assert(sym1 != "ab");
static_assert(sym1 < 'c');
static_assert(sym1 > 'a');
static_assert(sym1 <= 'b');
static_assert(sym1 <= 'c');
static_assert(sym1 >= 'b');
static_assert(sym1 >= 'a');
static_assert(sym1.standard() == 'b');
static_assert(sym1.ascii() == 'b');

constexpr basic_symbol_text sym2('a', 'b');
static_assert(sym2 == 'a');
static_assert(sym2 != 'b');
static_assert(sym2.standard() == 'a');
static_assert(sym2.ascii() == 'b');

constexpr basic_symbol_text sym3("ab");
static_assert(sym3.standard() == "ab");
static_assert(sym3.ascii() == "ab");

constexpr basic_fixed_string txt1("bc");
constexpr basic_symbol_text sym4(txt1);
static_assert(sym4.standard() == "bc");
static_assert(sym4.ascii() == "bc");

constexpr basic_symbol_text sym5("bc", "de");
static_assert(sym5.standard() == "bc");
static_assert(sym5.ascii() == "de");

constexpr basic_fixed_string txt2("de");
constexpr basic_symbol_text sym6(txt1, txt2);
static_assert(sym6.standard() == "bc");
static_assert(sym6.ascii() == "de");

static_assert(sym6 == basic_symbol_text("bc", "de"));
static_assert(sym6 != basic_symbol_text("bc", "bc"));
static_assert(sym6 != basic_symbol_text("de", "de"));
static_assert(sym6 != basic_symbol_text("bcd", "ef"));
static_assert(sym6 != basic_symbol_text("bc", "def"));

static_assert(sym6 == basic_fixed_string("bc"));
static_assert(sym6 != basic_fixed_string("de"));
static_assert(sym6 != basic_fixed_string("fg"));
static_assert(sym6 != basic_fixed_string("bcd"));

static_assert(sym6 == "bc");
static_assert(sym6 != "de");
static_assert(sym6 != "fg");
static_assert(sym6 != "bcd");

static_assert(sym6 < basic_fixed_string("c"));
static_assert(sym6 > basic_fixed_string("a"));
static_assert(sym6 <= basic_fixed_string("c"));
static_assert(sym6 <= basic_fixed_string("bcd"));
static_assert(sym6 >= basic_fixed_string("a"));
static_assert(sym6 >= basic_fixed_string("bc"));

static_assert(sym6 < "c");
static_assert(sym6 > "a");
static_assert(sym6 <= "c");
static_assert(sym6 <= "bcd");
static_assert(sym6 >= "a");
static_assert(sym6 >= "bc");

static_assert(basic_symbol_text("a") + sym4 == basic_symbol_text("abc"));
static_assert(sym4 + basic_symbol_text("f") == basic_symbol_text("bcf"));

static_assert(basic_fixed_string("a") + sym4 == basic_symbol_text("abc"));
static_assert(sym4 + basic_fixed_string("f") == basic_symbol_text("bcf"));

static_assert(basic_symbol_text("a", "f") + sym6 == basic_symbol_text("abc", "fde"));
static_assert(sym6 + basic_symbol_text("a", "f") == basic_symbol_text("bca", "def"));

static_assert('a' + sym6 == basic_symbol_text("abc", "ade"));
static_assert(sym6 + 'f' == basic_symbol_text("bcf", "def"));

static_assert(basic_fixed_string("a") + sym6 == basic_symbol_text("abc", "ade"));
static_assert(sym6 + basic_fixed_string("f") == basic_symbol_text("bcf", "def"));

static_assert("a" + sym6 == basic_symbol_text("abc", "ade"));
static_assert(sym6 + "f" == basic_symbol_text("bcf", "def"));
}
