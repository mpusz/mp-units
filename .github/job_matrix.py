import dataclasses
import itertools
import random
import typing
from dataclasses import dataclass
from types import MappingProxyType

T = typing.TypeVar("T")


@dataclass(frozen=True, order=True, kw_only=True)
class Compiler:
    type: typing.Literal["GCC", "CLANG", "APPLE_CLANG", "MSVC"]
    version: str | int
    cc: str
    cxx: str
    xcode_version: str | None = None


@dataclass(frozen=True, order=True, kw_only=True)
class ToolchainFeatureSupport:
    cxx_modules: bool = False
    std_format: bool = False
    import_std: bool = False
    freestanding: bool = False
    explicit_this: bool = False
    std_contracts: bool = False
    max_std: int = 23


@dataclass(frozen=True, order=True, kw_only=True)
class ConanOptions:
    cxx_modules: bool
    import_std: bool
    std_format: bool
    no_crtp: bool
    contracts: typing.Literal["none", "std", "gsl-lite", "ms-gsl"]
    freestanding: bool

    def is_supported_on(self, feat: ToolchainFeatureSupport) -> bool:
        """Defines mapping of conan options to toolchain feature support."""
        feat = dataclasses.asdict(feat)
        # handle features without 1:1 mapping first
        explicit_this = feat.pop("explicit_this")
        if self.no_crtp and not explicit_this:
            return False
        std_contracts = feat.pop("std_contracts")
        if self.contracts == "std" and not std_contracts:
            return False
        feat.pop("max_std")  # checked in Configuration.is_supported (needs std)
        # now, the rest
        for k, v in feat.items():
            if getattr(self, k) and not v:
                return False
        return True

    @classmethod
    def full_matrix(cls) -> dict[str, typing.Any]:
        ret = {}
        for field in dataclasses.fields(cls):
            t = field.type
            if t == bool:
                opts = (False, True)
            elif typing.get_origin(t) == typing.Literal:
                opts = typing.get_args(t)
            else:
                raise TypeError(f"Unsupported type {t}")
            ret[field.name] = tuple(opts)
        return ret


@dataclass(frozen=True, order=True, kw_only=True)
class Toolchain:
    name: str
    os: str
    compiler: Compiler
    lib: typing.Literal["libc++", "libstdc++"] | None = None
    feature_support: ToolchainFeatureSupport

    def __str__(self):
        return self.name

    def for_github(self):
        ret = dataclasses.asdict(self)
        del ret["feature_support"]
        return ret


@dataclass(frozen=True, order=True, kw_only=True)
class Configuration(ConanOptions):
    toolchain: Toolchain
    std: typing.Literal[20, 23, 26]
    build_type: typing.Literal["Release", "Debug"]

    @property
    def is_supported(self) -> bool:
        """Defines allowable option combinations beyond what the toolchain defines."""
        # check if selected features are supported by the toolchain
        if not self.is_supported_on(self.toolchain.feature_support):
            return False
        # maximum standard supported by the toolchain
        if self.std > self.toolchain.feature_support.max_std:
            return False
        # minimum standard
        if self.std < 23 and any([self.import_std, self.no_crtp]):
            return False
        if self.std < 26 and self.contracts == "std":
            return False
        # third-party contract libraries require a hosted implementation
        if self.freestanding and self.contracts not in ("none", "std"):
            return False
        # additional checks for import_std
        if self.import_std:
            if not self.std_format:
                return False
            # third-party contract libraries textually include std headers, which
            # conflict with `import std;`; C++26 contracts are a language feature
            if self.contracts not in ("none", "std"):
                return False
        # MSVC Debug + std::format trips error C7595 ("call to immediate function is not a
        # constant expression") on heavily templated mp-units quantity arguments — the
        # `std::basic_format_string` consteval ctor can't evaluate them under Debug's stricter
        # constant evaluator.  Release works; {fmt} works; only this combination breaks.
        if (
            self.toolchain.compiler.type == "MSVC"
            and self.build_type == "Debug"
            and self.std_format
        ):
            return False
        return True

    def for_github(self):
        features = {
            field.name: str(getattr(self, field.name))
            for field in dataclasses.fields(ConanOptions)
        }
        ret = dict(
            toolchain=self.toolchain.for_github(),
            std=self.std,
            build_type=self.build_type,
            formatting="std::format" if self.std_format else "fmtlib",
        )
        # expose individual ConanOptions fields as top-level matrix properties
        # (needed for YAML conditions and cache keys)
        ret.update(features)
        options = " ".join(f"-o '&:{k}={v}'" for k, v in features.items())
        ret["config-summary-str"] = self.infostr(adjusted=False)
        # Pre-build the -s flags so workflows and repro scripts can use them directly
        # without sed-patching the auto-detected Conan profile.
        _COMPILER_TYPE_MAP = {
            "GCC": "gcc",
            "CLANG": "clang",
            "APPLE_CLANG": "apple-clang",
            "MSVC": "msvc",
        }
        settings = [
            f"-s compiler={_COMPILER_TYPE_MAP[self.toolchain.compiler.type]}",
            f"-s compiler.version={self.toolchain.compiler.version}",
            f"-s compiler.cppstd={self.std}",
            f"-s build_type={self.build_type}",
        ]
        if self.toolchain.lib:
            settings.append(f"-s compiler.libcxx={self.toolchain.lib}")
        ret["conan-settings"] = " ".join(settings)
        ret["conan-build-folder"] = (
            f"{_COMPILER_TYPE_MAP[self.toolchain.compiler.type]}-{self.toolchain.compiler.version}-{self.std}"
        )
        cc = self.toolchain.compiler.cc
        cxx = self.toolchain.compiler.cxx
        if cc and cxx:
            conf = f'-c \'tools.build:compiler_executables={{"c": "{cc}", "cpp": "{cxx}"}}\''
            ret["conan-args"] = f"{conf} {options}"
        else:
            ret["conan-args"] = options
        return ret

    def _formatters():
        def yesno(k):
            return (lambda v: f"{k}=" + ("yes" if v else "no"), len(k) + 4)

        # freestanding is excluded on purpose - it runs in a completely different CI
        return dict(
            toolchain=(lambda v: str(v), 17),
            std=(lambda v: f"C++{v:2d}", 5),
            std_format=(lambda v: "std::format" if v else "fmtlib", 11),
            cxx_modules=yesno("modules"),
            import_std=yesno("import_std"),
            no_crtp=yesno("no_crtp"),
            contracts=(lambda v: str(v), 8),
            build_type=(lambda v: str(v), 8),
        )

    _formatters = MappingProxyType(_formatters())

    def infostr(self, adjusted: bool) -> str:
        ret = []
        for k, (fmt, w) in self._formatters.items():
            ret.append(fmt(getattr(self, k)).ljust(w if adjusted else 0))
        return " ".join(ret)


class CombinationCollector(typing.Generic[T]):
    """Incremental builder of MatrixElements, allowing successive selection of entries."""

    def __init__(
        self,
        *,
        full_matrix: dict[str, list[typing.Any]],
        configuration_element_type: type[T],
        hard_excludes: typing.Callable[[T], bool] | None = None,
    ):
        self.full_matrix = full_matrix
        self.configuration_element_type = configuration_element_type
        self.hard_excludes = hard_excludes
        self.combinations: set[T] = set()

    def _make_submatrix(self, **overrides):
        new_matrix = dict(self.full_matrix)
        for k, v in overrides.items():
            if not isinstance(v, list):
                v = [v]
            new_matrix[k] = v
        return new_matrix

    def _add_combination(self, e: T):
        if e in self.combinations or (
            self.hard_excludes is not None and self.hard_excludes(e)
        ):
            return False
        self.combinations.add(e)
        return True

    def _new_candidates(self, matrix, filter):
        """Every combination of `matrix` that is allowed and not collected yet."""
        keys = tuple(matrix.keys())
        ret = []
        for combination in itertools.product(*matrix.values()):
            cand = self.configuration_element_type(**dict(zip(keys, combination)))
            if cand in self.combinations or (filter and not filter(cand)):
                continue
            if self.hard_excludes is not None and self.hard_excludes(cand):
                continue
            ret.append(cand)
        return ret

    def all_combinations(
        self,
        *,
        filter: typing.Callable[[T], bool] | None = None,
        **overrides,
    ):
        """Adds all combinations in the submatrix defined by `overrides`."""
        matrix = self._make_submatrix(**overrides)
        keys = tuple(matrix.keys())
        for combination in itertools.product(*matrix.values()):
            cand = self.configuration_element_type(**dict(zip(keys, combination)))
            if filter and not filter(cand):
                continue
            self._add_combination(cand)

    def sample_combinations(
        self,
        *,
        rgen: random.Random,
        min_samples_per_value: int = 0,
        min_samples: int = 0,
        filter: typing.Callable[[T], bool] | None = None,
        **overrides,
    ):
        """Adds samples from the submatrix defined by `overrides`,
        ensuring each individual value appears at least n times.
        """
        matrix = self._make_submatrix(**overrides)
        missing: dict[tuple[str, typing.Any], int] = {}
        for key, options in matrix.items():
            for value in options:
                idx = (key, value)
                missing[idx] = min_samples_per_value
        n_in_submatrix = 0
        for e in self.combinations:
            for k, v in vars(e).items():
                if k not in matrix or v not in matrix[k]:
                    break
            else:
                # this combination is in the submatrix
                n_in_submatrix += 1
                for k, v in vars(e).items():
                    if k in matrix and v in matrix[k]:
                        missing[k, v] -= 1
        for done in [idx for idx, n in missing.items() if n <= 0]:
            del missing[done]

        def add(cand):
            nonlocal n_in_submatrix
            self._add_combination(cand)
            n_in_submatrix += 1
            for idx in vars(cand).items():
                remaining = missing.pop(idx, 0) - 1
                if remaining > 0:
                    missing[idx] = remaining

        # Pick from the allowed combinations rather than drawing every key at random and
        # retrying: a value that few combinations support, such as an old toolchain, would
        # otherwise exhaust the retries and leave the values queued after it uncovered.
        while missing:
            (force_key, force_option), _ = next(iter(missing.items()))
            candidates = self._new_candidates(
                {**matrix, force_key: [force_option]}, filter
            )
            if not candidates:
                print(
                    "Unable to reach the requested minimum number of samples for"
                    f" {force_key}={force_option!s}"
                )
                del missing[force_key, force_option]
                continue
            add(rgen.choice(candidates))
        if n_in_submatrix < min_samples:
            candidates = self._new_candidates(matrix, filter)
            needed = min_samples - n_in_submatrix
            if len(candidates) < needed:
                print(
                    "Unable to reach the requested minimum number of samples,"
                    f" {n_in_submatrix + len(candidates)} of {min_samples}"
                )
            for cand in rgen.sample(candidates, min(needed, len(candidates))):
                add(cand)
