import dataclasses
import itertools
import random
import typing
from dataclasses import dataclass


@dataclass(frozen=True, order=True)
class Compiler:
    type: typing.Literal["GCC", "CLANG", "APPLE_CLANG", "MSVC"]
    version: str | int
    cc: str
    cxx: str


@dataclass(frozen=True, order=True)
class Configuration:
    name: str
    os: str
    compiler: Compiler
    cxx_modules: bool
    std_format_support: bool
    conan_config: str = ""
    lib: typing.Literal["libc++", "libstdc++"] | None = None

    def __str__(self):
        return self.name


@dataclass(frozen=True, order=True)
class MatrixElement:
    config: Configuration
    std: typing.Literal[20, 23]
    formatting: typing.Literal["std::format", "fmtlib"]
    contracts: typing.Literal["none", "gsl-lite", "ms-gsl"]
    build_type: typing.Literal["Release", "Debug"]

    def as_json(self):
        def dataclass_to_json(obj):
            """Convert dataclasses to something json-serialisable"""
            if dataclasses.is_dataclass(obj):
                return {
                    k: dataclass_to_json(v) for k, v in dataclasses.asdict(obj).items()
                }
            return obj

        ret = dataclass_to_json(self)
        # patch boolean conan configuration options
        config = ret["config"]
        for k in ["cxx_modules"]:
            config[k] = "True" if config[k] else "False"
        return ret


class CombinationCollector:
    """Incremental builder of MatrixElements, allowing successive selection of entries."""

    def __init__(
        self,
        full_matrix: dict[str, list[typing.Any]],
        *,
        hard_excludes: typing.Callable[[MatrixElement], bool] | None = None,
    ):
        self.full_matrix = full_matrix
        self.hard_excludes = hard_excludes
        self.combinations: set[MatrixElement] = set()
        self.per_value_counts: dict[tuple[str, typing.Any], int] = {
            (k, v): 0 for k, options in full_matrix.items() for v in options
        }

    def _make_submatrix(self, **overrides):
        new_matrix = dict(self.full_matrix)
        for k, v in overrides.items():
            if not isinstance(v, list):
                v = [v]
            new_matrix[k] = v
        return new_matrix

    def _add_combination(self, e: MatrixElement):
        if e in self.combinations or (
            self.hard_excludes is not None and self.hard_excludes(e)
        ):
            return
        self.combinations.add(e)
        # update per_value_counts
        for k, v in vars(e).items():
            idx = (k, v)
            self.per_value_counts[idx] = self.per_value_counts.get(idx, 0) + 1

    def all_combinations(
        self,
        *,
        filter: typing.Callable[[MatrixElement], bool] | None = None,
        **overrides,
    ):
        """Adds all combinations in the submatrix defined by `overrides`."""
        matrix = self._make_submatrix(**overrides)
        keys = tuple(matrix.keys())
        for combination in itertools.product(*matrix.values()):
            cand = MatrixElement(**dict(zip(keys, combination)))
            if filter and not filter(cand):
                continue
            self._add_combination(cand)

    def sample_combinations(
        self,
        *,
        rgen: random.Random,
        min_samples_per_value: int = 1,
        filter: typing.Callable[[MatrixElement], bool] | None = None,
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
                missing[idx] = min_samples_per_value - self.per_value_counts.get(idx, 0)
        while missing:
            (force_key, force_option), remaining = next(iter(missing.items()))
            if remaining <= 0:
                missing.pop((force_key, force_option))
                continue
            choice = {}
            for key, options in matrix.items():
                choice[key] = force_option if key == force_key else rgen.choice(options)
            cand = MatrixElement(**choice)
            if filter and not filter(cand):
                continue
            self._add_combination(cand)
            for idx in choice.items():
                if missing.pop(idx, 0) <= 0:
                    continue
                remaining = min_samples_per_value - self.per_value_counts.get(idx, 0)
                if remaining > 0:
                    missing[idx] = remaining
