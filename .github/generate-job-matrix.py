import argparse
import json
import os
import random
import typing
from types import SimpleNamespace

from job_matrix import CombinationCollector, Compiler, Configuration


def make_gcc_config(version: int) -> Configuration:
    return Configuration(
        name=f"GCC-{version}",
        os="ubuntu-24.04",
        compiler=Compiler(
            type="GCC",
            version=version,
            cc=f"gcc-{version}",
            cxx=f"g++-{version}",
        ),
        cxx_modules=False,
        std_format_support=version >= 13,
    )


def make_clang_config(
    version: int, platform: typing.Literal["x86-64", "arm64"] = "x86-64"
) -> Configuration:
    cfg = SimpleNamespace(
        name=f"Clang-{version} ({platform})",
        compiler=SimpleNamespace(
            type="CLANG",
            version=version,
        ),
        lib="libc++",
        cxx_modules=version >= 17,
        std_format_support=version >= 17,
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
    return Configuration(**vars(ret))


def make_apple_clang_config(
    os: str, version: str, std_format_support: bool
) -> Configuration:
    ret = Configuration(
        name=f"Apple Clang {version}",
        os=os,
        compiler=Compiler(
            type="APPLE_CLANG",
            version=version,
            cc="clang",
            cxx="clang++",
        ),
        cxx_modules=False,
        std_format_support=std_format_support,
    )
    return ret


def make_msvc_config(release: str, version: int) -> Configuration:
    ret = Configuration(
        name=f"MSVC {release}",
        os="windows-2022",
        compiler=Compiler(
            type="MSVC",
            version=version,
            cc="",
            cxx="",
        ),
        cxx_modules=False,
        std_format_support=True,
    )
    return ret


configs = {
    c.name: c
    for c in [make_gcc_config(ver) for ver in [12, 13, 14]]
    + [
        make_clang_config(ver, platform)
        for ver in [16, 17, 18, 20, 21]
        for platform in ["x86-64", "arm64"]
        # arm64 runners are expensive; only consider one version
        if ver == 18 or platform != "arm64"
    ]
    # std::format is available in Xcode 16.1 or later
    + [
        make_apple_clang_config("macos-14", ver, std_format_support=True)
        for ver in ["16.1"]
    ]
    + [make_msvc_config(release="14.4", version=194)]
}

full_matrix = dict(
    config=list(configs.values()),
    std=[20, 23],
    formatting=["std::format", "fmtlib"],
    contracts=["none", "gsl-lite", "ms-gsl"],
    build_type=["Release", "Debug"],
)


def main():
    parser = argparse.ArgumentParser()
    #    parser.add_argument("-I","--include",nargs="+",action="append")
    #    parser.add_argument("-X","--exclude",nargs="+",action="append")
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--preset", default=None)
    parser.add_argument("--debug", nargs="+", default=["combinations"])
    parser.add_argument("--suppress-output", default=False, action="store_true")

    args = parser.parse_args()

    rgen = random.Random(args.seed)

    collector = CombinationCollector(
        full_matrix,
        hard_excludes=lambda e: (
            e.formatting == "std::format" and not e.config.std_format_support
        ),
    )
    match args.preset:
        case None:
            pass
        case "all":
            collector.all_combinations()
        case "conan" | "cmake":
            collector.all_combinations(
                formatting="std::format",
                contracts="gsl-lite",
                build_type="Debug",
                std=20,
            )
            collector.all_combinations(
                filter=lambda me: not me.config.std_format_support,
                formatting="fmtlib",
                contracts="gsl-lite",
                build_type="Debug",
                std=20,
            )
            collector.sample_combinations(rgen=rgen, min_samples_per_value=2)
        case "clang-tidy":
            collector.all_combinations(config=configs["Clang-18 (x86-64)"])
        case "freestanding":
            collector.all_combinations(
                config=[configs[c] for c in ["GCC-14", "Clang-21 (x86-64)"]],
                contracts="none",
                std=23,
            )
        case _:
            raise KeyError(f"Unsupported preset {args.preset!r}")

    if not collector.combinations:
        raise ValueError("No combination has been produced")

    data = sorted(collector.combinations)

    json_data = [e.as_json() for e in data]

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
                    print(
                        f"{e.config!s:17s}  c++{e.std:2d}  {e.formatting:11s}  {e.contracts:8s}  {e.build_type:8s}"
                    )
            case "counts":
                print(f"Total combinations {len(data)}")
                for (k, v), n in sorted(collector.per_value_counts.items()):
                    print(f"  {k}={v}: {n}")
            case "none":
                pass
            case _:
                raise KeyError(f"Unknown debug mode {dbg!r}")


if __name__ == "__main__":
    main()
