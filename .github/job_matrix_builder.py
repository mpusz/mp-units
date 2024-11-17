import itertools
import random
import typing

T = typing.TypeVar("T")


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
                if v not in matrix[k]:
                    break
            else:
                # this combination is in the submatrix
                n_in_submatrix += 1
                for k, v in vars(e).items():
                    if v in matrix[k]:
                        missing[k, v] -= 1
        for done in [idx for idx, n in missing.items() if n <= 0]:
            del missing[done]
        n_failed_tries = 0
        failed_cand = set()

        def try_handle_choice(choice):
            nonlocal n_failed_tries, n_in_submatrix
            cand = self.configuration_element_type(**choice)
            if filter is None or filter(cand):
                added = self._add_combination(cand)
            else:
                added = False
            if added:
                n_failed_tries = 0
                failed_cand.clear()
            else:
                if cand not in self.combinations:
                    failed_cand.add(cand)
                n_failed_tries += 1
                return
            n_in_submatrix += 1
            for idx in choice.items():
                remaining = missing.pop(idx, 0) - 1
                if remaining <= 0:
                    continue
                missing[idx] = remaining

        while missing:
            (force_key, force_option), remaining = next(iter(missing.items()))
            if remaining <= 0:
                missing.pop((force_key, force_option))
                continue
            choice = {}
            for key, options in matrix.items():
                choice[key] = force_option if key == force_key else rgen.choice(options)
            try_handle_choice(choice)
            if n_failed_tries > 100:
                tries = "\n".join(  # noqa: F841
                    ", ".join(f"{k}:{v!s}" for k, v in vars(cand).items())
                    + f" -> {cand.is_supported=}"
                    for cand in sorted(failed_cand)
                )
                print(
                    "Unable to reach the requested minimum number of samples,"
                    f" {missing=}"
                )
                n_failed_tries = 0
                failed_cand.clear()
                break
        while n_in_submatrix < min_samples:
            choice = {}
            for key, options in matrix.items():
                choice[key] = rgen.choice(options)
            try_handle_choice(choice)
            if n_failed_tries > 100:
                print(
                    "Unable to reach the requested minimum number of samples,"
                    f" {n_in_submatrix=}"
                )
                n_failed_tries = 0
                failed_cand.clear()
                break
