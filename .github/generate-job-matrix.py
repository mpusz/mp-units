import argparse
import dataclasses
import json
import os
import random
import typing
from dataclasses import dataclass
from types import SimpleNamespace

from job_matrix_builder import CombinationCollector


@dataclass(frozen=True, order=True, kw_only=True)
class Compiler:
    type: typing.Literal["GCC", "CLANG", "APPLE_CLANG", "MSVC"]
    version: str | int
    cc: str
    cxx: str


@dataclass(frozen=True, order=True, kw_only=True)
class Features:
    cxx_modules: bool = False
    std_format: bool = False
    import_std: bool = False
    freestanding: bool = False


@dataclass(frozen=True, order=True, kw_only=True)
class Platform:
    """This is really mainly the compiler."""

    name: str
    os: str
    compiler: Compiler
    lib: typing.Literal["libc++", "libstdc++"] | None = None
    feature_support: Features

    def __str__(self):
        return self.name

    def for_github(self):
        ret = dataclasses.asdict(self)
        del ret["feature_support"]
        return ret


@dataclass(frozen=True, order=True, kw_only=True)
class Configuration(Features):
    platform: Platform
    std: typing.Literal[20, 23]
    contracts: typing.Literal["none", "gsl-lite", "ms-gsl"]
    build_type: typing.Literal["Release", "Debug"]

    @property
    def is_supported(self) -> bool:
        # check if selected features are supported by the platform
        s = self.platform.feature_support
        for field in dataclasses.fields(Features):
            if getattr(self, field.name) and not getattr(s, field.name):
                return False
        # additional checks for import_std
        if self.import_std:
            if self.std < 23:
                return False
            if not self.cxx_modules:
                return False
            if not self.std_format:
                return False
            if self.contracts != "none":
                return False
        return True

    def for_github(self):
        features = {
            field.name: str(getattr(self, field.name))
            for field in dataclasses.fields(Features)
        }
        ret = {
            field.name: getattr(self, field.name)
            for field in dataclasses.fields(self)
            if field.name not in features
        }
        ret["platform"] = self.platform.for_github()
        ret["formatting"] = "std::format" if self.std_format else "fmtlib"
        features["contracts"] = self.contracts
        ret["conan-config"] = " ".join(f"-o '&:{k}={v}'" for k, v in features.items())
        return ret


def make_gcc_platform(version: int) -> Platform:
    return Platform(
        name=f"GCC-{version}",
        os="ubuntu-24.04",
        compiler=Compiler(
            type="GCC",
            version=version,
            cc=f"gcc-{version}",
            cxx=f"g++-{version}",
        ),
        feature_support=Features(
            std_format=version >= 13,
            freestanding=True,
        ),
    )


def make_clang_platform(
    version: int, architecture: typing.Literal["x86-64", "arm64"] = "x86-64"
) -> Platform:
    cfg = SimpleNamespace(
        name=f"Clang-{version} ({architecture})",
        compiler=SimpleNamespace(
            type="CLANG",
            version=version,
        ),
        lib="libc++",
        feature_support=Features(
            cxx_modules=version >= 17,
            std_format=version >= 17,
            import_std=version >= 18,
            freestanding=True,
        ),
    )
    match architecture:
        case "x86-64":
            cfg.os = "ubuntu-22.04" if version < 17 else "ubuntu-24.04"
            cfg.compiler.cc = f"clang-{version}"
            cfg.compiler.cxx = f"clang++-{version}"
        case "arm64":
            cfg.os = "macos-14"
            pfx = f"/opt/homebrew/opt/llvm@{version}/bin"
            cfg.compiler.cc = f"{pfx}/clang"
            cfg.compiler.cxx = f"{pfx}/clang++"
        case _:
            raise KeyError(f"Unsupported architecture {architecture!r} for Clang")
    ret = cfg
    ret.compiler = Compiler(**vars(cfg.compiler))
    return Platform(**vars(ret))


def make_apple_clang_platform(
    os: str, version: str, std_format_support: bool
) -> Platform:
    ret = Platform(
        name=f"Apple Clang {version}",
        os=os,
        compiler=Compiler(
            type="APPLE_CLANG",
            version=version,
            cc="clang",
            cxx="clang++",
        ),
        feature_support=Features(std_format=std_format_support),
    )
    return ret


def make_msvc_platform(release: str, version: int) -> Platform:
    ret = Platform(
        name=f"MSVC {release}",
        os="windows-2022",
        compiler=Compiler(
            type="MSVC",
            version=version,
            cc="",
            cxx="",
        ),
        feature_support=Features(
            std_format=True,
        ),
    )
    return ret


platforms = {
    p.name: p
    for p in [make_gcc_platform(ver) for ver in [12, 13, 14]]
    + [
        make_clang_platform(ver, arch)
        for ver in [16, 17, 18, 20]
        for arch in ["x86-64", "arm64"]
        # arm64 runners are expensive; only consider one version
        if ver == 18 or arch != "arm64"
    ]
    + [
        make_apple_clang_platform("macos-13", ver, std_format_support=False)
        for ver in ["15.2"]
    ]
    # std::format is available in Xcode 16.1 or later
    + [
        make_apple_clang_platform("macos-14", ver, std_format_support=True)
        for ver in ["16.1"]
    ]
    + [make_msvc_platform(release="14.4", version=194)]
}

full_matrix = dict(
    platform=list(platforms.values()),
    std=[20, 23],
    std_format=[False, True],
    import_std=[False, True],
    cxx_modules=[False, True],
    freestanding=[False, True],
    contracts=["none", "gsl-lite", "ms-gsl"],
    build_type=["Release", "Debug"],
)


def main():
    parser = argparse.ArgumentParser()
    #    parser.add_argument("-I","--include",nargs="+",action="append")
    #    parser.add_argument("-X","--exclude",nargs="+",action="append")
    parser.add_argument("--seed", type=int, default=None)
    parser.add_argument("--preset", default=None)
    parser.add_argument("--debug", nargs="+", default=["combinations"])
    parser.add_argument("--suppress-output", default=False, action="store_true")

    args = parser.parse_args()

    if not args.seed:
        args.seed = random.randint(0, (1 << 32) - 1)

    print(f"Random-seed for this matrix is {args.seed}")

    rgen = random.Random(args.seed)

    collector = CombinationCollector(
        full_matrix=full_matrix,
        configuration_element_type=Configuration,
        hard_excludes=lambda c: (not c.is_supported)
        or (
            # TODO For some reason Clang-18 Debug with -ffreestanding does not pass CMakeTestCXXCompiler
            c.freestanding
            and c.platform.name.startswith("Clang-18")
            and c.build_type == "Debug"
        ),
    )
    if args.preset:
        # whatever the preset; we always want to have a test that does import_std;
        # that requires a very specific configuration
        collector.sample_combinations(
            rgen=rgen,
            min_samples=1,
            std_format=True,
            import_std=True,
            cxx_modules=True,
            freestanding=args.preset == "freestanding",
            std=23,
            contracts="none",
            platform=platforms["Clang-18 (x86-64)"],
        )
    match args.preset:
        case None:
            pass
        case "all":
            collector.all_combinations()
        case "conan" | "cmake":
            config = dict(
                contracts="gsl-lite",
                build_type="Debug",
                std=20,
                freestanding=False,
            )
            collector.all_combinations(
                std_format=True,
                **config,
            )
            # fmtlib for those platforms where we don't support std_format
            collector.all_combinations(
                filter=lambda me: not me.platform.feature_support.std_format,
                std_format=False,
                **config,
            )
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=1,
                freestanding=False,
            )
            # add more coverage to import_std=False configurations;
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=2,
                import_std=False,
                freestanding=False,
            )
        case "clang-tidy":
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=1,
                platform=platforms["Clang-18 (x86-64)"],
                freestanding=False,
            )
        case "freestanding":
            collector.all_combinations(
                platform=[platforms[c] for c in ["GCC-14", "Clang-20 (x86-64)"]],
                contracts="none",
                freestanding=True,
                std=23,
            )
        case _:
            raise KeyError(f"Unsupported preset {args.preset!r}")

    if not collector.combinations:
        raise ValueError("No combination has been produced")

    data = sorted(collector.combinations)

    json_data = [e.for_github() for e in data]

    output_file = os.environ.get("GITHUB_OUTPUT")
    if not args.suppress_output:
        if output_file:
            print(f"Writing outputs to {output_file}")
            with open(output_file, "wt") as fh:
                fh.write(f"matrix={json.dumps(json_data)}")
        else:
            print("No output file received!")

    for dbg in args.debug:
        match dbg:
            case "yaml":
                import yaml

                json_data = json.loads(json.dumps(json_data))
                print(yaml.safe_dump(json_data))
            case "json":
                print(json.dumps(json_data, indent=4))
            case "combinations":
                for e in data:
                    std_format = "yes" if e.std_format else "no "
                    cxx_modules = "yes" if e.cxx_modules else "no "
                    import_std = "yes" if e.import_std else "no "
                    print(
                        f"{e.platform!s:17s}  c++{e.std:2d}  "
                        f"{std_format=:3s}  {cxx_modules=:3s}  {import_std=:3s}  "
                        f"{e.contracts:8s}  {e.build_type:8s}"
                    )
            case "counts":
                print(f"Total combinations {len(data)}")
            case "none":
                pass
            case _:
                raise KeyError(f"Unknown debug mode {dbg!r}")


if __name__ == "__main__":
    main()
