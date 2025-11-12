import argparse
import json
import os
import random
import typing
from types import SimpleNamespace

from job_matrix import (
    CombinationCollector,
    Compiler,
    ConanOptions,
    Configuration,
    Toolchain,
    ToolchainFeatureSupport,
)


def make_gcc_config(version: int) -> Toolchain:
    return Toolchain(
        name=f"GCC-{version}",
        os="ubuntu-24.04",
        compiler=Compiler(
            type="GCC",
            version=version,
            cc=f"gcc-{version}",
            cxx=f"g++-{version}",
        ),
        feature_support=ToolchainFeatureSupport(
            std_format=version >= 13,
            freestanding=True,
        ),
    )


def make_clang_config(
    version: int, platform: typing.Literal["x86-64", "arm64"] = "x86-64"
) -> Toolchain:
    cfg = SimpleNamespace(
        name=f"Clang-{version} ({platform})",
        compiler=SimpleNamespace(
            type="CLANG",
            version=version,
        ),
        lib="libc++",
        feature_support=ToolchainFeatureSupport(
            cxx_modules=version >= 17,
            std_format=version >= 17,
            import_std=version >= 18,
            freestanding=True,
        ),
    )
    match platform:
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
            raise KeyError(f"Unsupported platform {platform!r} for Clang")
    ret = cfg
    ret.compiler = Compiler(**vars(cfg.compiler))
    return Toolchain(**vars(ret))


def make_apple_clang_config(
    os: str, version: str, std_format_support: bool
) -> Toolchain:
    ret = Toolchain(
        name=f"Apple Clang {version}",
        os=os,
        compiler=Compiler(
            type="APPLE_CLANG",
            version=version,
            cc="clang",
            cxx="clang++",
        ),
        feature_support=ToolchainFeatureSupport(std_format=std_format_support),
    )
    return ret


def make_msvc_config(release: str, version: int) -> Toolchain:
    ret = Toolchain(
        name=f"MSVC {release}",
        os="windows-2022",
        compiler=Compiler(
            type="MSVC",
            version=version,
            cc="",
            cxx="",
        ),
        feature_support=ToolchainFeatureSupport(
            std_format=True,
        ),
    )
    return ret


toolchains = {
    t.name: t
    for t in [make_gcc_config(ver) for ver in [12, 13, 14]]
    + [
        make_clang_config(ver, platform)
        for ver in [16, 17, 18, 20, 21]
        for platform in ["x86-64", "arm64"]
        # arm64 runners are expensive; only consider one version
        if ver == 18 or platform != "arm64"
    ]
    + [
        make_apple_clang_config("macos-13", ver, std_format_support=False)
        for ver in ["15.2"]
    ]
    # std::format is available in Xcode 16.1 or later
    + [
        make_apple_clang_config("macos-14", ver, std_format_support=True)
        for ver in ["16.1"]
    ]
    + [make_msvc_config(release="14.4", version=194)]
}

full_matrix = dict(
    toolchain=list(toolchains.values()),
    std=[20, 23],
    build_type=["Release", "Debug"],
    **ConanOptions.full_matrix(),
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
            and c.toolchain.name.startswith("Clang-18")
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
            toolchain=toolchains["Clang-18 (x86-64)"],
        )
    match args.preset:
        case None:
            pass
        case "all":
            collector.all_combinations()
        case "all-hosted" | "all-conan" | "all-cmake":
            collector.all_combinations(
                freestanding=False,
            )
        case "conan" | "cmake":
            config = dict(
                contracts="gsl-lite",
                build_type="Debug",
                std=20,
                freestanding=False,
                no_crtp=False,
                natural_units=True,
            )
            collector.all_combinations(
                std_format=True,
                **config,
            )
            # fmtlib for those toolchains where we don't support std_format
            collector.all_combinations(
                filter=lambda me: not me.toolchain.feature_support.std_format,
                std_format=False,
                **config,
            )
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=1,
                freestanding=False,
            )
            # add more coverage to the mode usual configurations without import_std
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=2,
                import_std=False,
                freestanding=False,
                no_crtp=False,
            )
        case "clang-tidy":
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=1,
                toolchain=toolchains["Clang-18 (x86-64)"],
                freestanding=False,
            )
        case "all-freestanding":
            collector.all_combinations(
                freestanding=True,
            )
        case "freestanding":
            collector.all_combinations(
                toolchain=[toolchains[c] for c in ["GCC-14", "Clang-21 (x86-64)"]],
                contracts="none",
                freestanding=True,
                std=23,
                no_crtp=False,
                natural_units=True,
            )
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=1,
                toolchain=toolchains["Clang-21 (x86-64)"],
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
                msg = []
                for e in data:
                    std_format = "yes" if e.std_format else "no "
                    cxx_modules = "yes" if e.cxx_modules else "no "
                    import_std = "yes" if e.import_std else "no "
                    no_crtp = "yes" if e.no_crtp else "no "
                    nat_u = "yes" if e.natural_units else "no "
                    msg.append(
                        f"{e.toolchain!s:17s} c++{e.std:2d} "
                        f"{std_format=:3s} {cxx_modules=:3s} {import_std=:3s} {no_crtp=:3s} {nat_u=:3s} "
                        f"{e.contracts:8s} {e.build_type:8s}"
                    )
                for m in sorted(msg):
                    print(m)
            case "counts":
                print(f"Total combinations {len(data)}")
            case "none":
                pass
            case _:
                raise KeyError(f"Unknown debug mode {dbg!r}")


if __name__ == "__main__":
    main()
