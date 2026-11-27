import argparse
import ast
import json
import os
import pathlib
import random
import typing
from types import SimpleNamespace

from job_matrix import ToolchainFeatureSupport  # used by _make_feature_support
from job_matrix import (
    CombinationCollector,
    Compiler,
    ConanOptions,
    Configuration,
    Toolchain,
)


def _load_feature_compat() -> dict:
    """Parse _feature_compatibility from conanfile.py via AST (no Conan import needed)."""
    conanfile = pathlib.Path(__file__).parent.parent / "conanfile.py"
    tree = ast.parse(conanfile.read_text())
    for node in ast.walk(tree):
        if (
            isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef))
            and node.name == "_feature_compatibility"
        ):
            for stmt in node.body:
                if isinstance(stmt, ast.Return):
                    return ast.literal_eval(stmt.value)
    raise RuntimeError("Could not find _feature_compatibility in conanfile.py")


_FEATURE_COMPAT = _load_feature_compat()


def _make_feature_support(
    conan_compiler: str, version: int, *, freestanding: bool = False, max_std: int = 23
) -> ToolchainFeatureSupport:
    """Derive ToolchainFeatureSupport from conanfile.py's _feature_compatibility."""

    def supports(feature: str) -> bool:
        min_ver = _FEATURE_COMPAT[feature]["compiler"].get(conan_compiler)
        return bool(min_ver and version >= int(min_ver))

    return ToolchainFeatureSupport(
        cxx_modules=supports("cxx_modules"),
        std_format=supports("std_format"),
        import_std=supports("import_std"),
        explicit_this=supports("explicit_this"),
        std_contracts=supports("std_contracts"),
        freestanding=freestanding,
        max_std=max_std,
    )


def make_gcc_config(
    version: int, architecture: typing.Literal["x86-64", "arm64"] = "x86-64"
) -> Toolchain:
    # Ubuntu 24.04 packages GCC up to 14. Newer releases come from Homebrew, which ships
    # the released compilers, while the toolchain PPA carries only GCC 16 snapshots.
    pfx = "/home/linuxbrew/.linuxbrew/bin/" if version >= 15 else ""
    match architecture:
        case "x86-64":
            name, os = f"GCC-{version}", "ubuntu-24.04"
        case "arm64":
            name, os = f"GCC-{version} (arm64)", "ubuntu-24.04-arm"
        case _:
            raise KeyError(f"Unsupported architecture {architecture!r} for GCC")
    return Toolchain(
        name=name,
        os=os,
        compiler=Compiler(
            type="GCC",
            version=version,
            cc=f"{pfx}gcc-{version}",
            cxx=f"{pfx}g++-{version}",
        ),
        feature_support=_make_feature_support(
            "gcc",
            version,
            freestanding=True,
            # -std=c++26 is available since gcc-14
            max_std=26 if version >= 14 else 23,
        ),
    )


# The newest LLVM in Homebrew is the unversioned `llvm` formula; older ones are `llvm@N`.
# The install step checks the version, so a newer `llvm` fails the job instead of
# silently testing a different compiler.
_BREW_LATEST_LLVM = 23


def make_clang_config(
    version: int, platform: typing.Literal["x86-64", "arm64", "macOS"] = "x86-64"
) -> Toolchain:
    cfg = SimpleNamespace(
        name=f"Clang-{version} ({platform})",
        compiler=SimpleNamespace(
            type="CLANG",
            version=version,
        ),
        lib="libc++",
        feature_support=_make_feature_support(
            "clang",
            version,
            freestanding=True,
            # -std=c++26 is available since clang-17
            max_std=26 if version >= 17 else 23,
        ),
    )
    match platform:
        case "x86-64":
            cfg.os = "ubuntu-22.04" if version < 17 else "ubuntu-24.04"
            cfg.compiler.cc = f"clang-{version}"
            cfg.compiler.cxx = f"clang++-{version}"
        case "arm64":
            cfg.os = "ubuntu-24.04-arm"
            cfg.compiler.cc = f"clang-{version}"
            cfg.compiler.cxx = f"clang++-{version}"
        case "macOS":
            cfg.os = "macos-26"
            formula = "llvm" if version == _BREW_LATEST_LLVM else f"llvm@{version}"
            pfx = f"/opt/homebrew/opt/{formula}/bin"
            cfg.compiler.cc = f"{pfx}/clang"
            cfg.compiler.cxx = f"{pfx}/clang++"
            # macOS uses the SDK's libc++ which has no std.cppm; import_std only
            # works with Linux (apt) LLVM where we can set up the required symlinks.
            cfg.feature_support = ToolchainFeatureSupport(
                cxx_modules=cfg.feature_support.cxx_modules,
                std_format=cfg.feature_support.std_format,
                import_std=False,
                explicit_this=cfg.feature_support.explicit_this,
                std_contracts=cfg.feature_support.std_contracts,
                freestanding=cfg.feature_support.freestanding,
                max_std=cfg.feature_support.max_std,
            )
        case _:
            raise KeyError(f"Unsupported platform {platform!r} for Clang")
    ret = cfg
    ret.compiler = Compiler(**vars(cfg.compiler))
    return Toolchain(**vars(ret))


def make_apple_clang_config(
    os: str, version: str, xcode_version: str | None = None
) -> Toolchain:
    major_version = int(version.split(".", 1)[0])
    ret = Toolchain(
        name=f"Apple Clang {xcode_version or version}",
        os=os,
        compiler=Compiler(
            type="APPLE_CLANG",
            version=version,
            cc="clang",
            cxx="clang++",
            xcode_version=xcode_version,
        ),
        feature_support=_make_feature_support("apple-clang", major_version),
    )
    return ret


def make_msvc_config(release: str) -> Toolchain:
    match release:
        case "14.5":
            os = "windows-2025-vs2026"
            version = 195
        case _:
            raise KeyError(f"Unsupported MSVC release {release!r}")
    return Toolchain(
        name=f"MSVC {release}",
        os=os,
        compiler=Compiler(
            type="MSVC",
            version=version,
            cc="",
            cxx="",
        ),
        feature_support=_make_feature_support("msvc", version),
    )


toolchains = {
    t.name: t
    for t in [make_gcc_config(ver) for ver in [12, 13, 14, 15, 16]]
    + [make_clang_config(ver) for ver in [16, 17, 18, 20, 21, 23]]
    # One newest compiler per arm64 platform. Linux arm64 has a 128-bit `long double`
    # (80-bit on x86-64, 64-bit on macOS and MSVC); macOS runners run few jobs at once.
    + [make_gcc_config(16, "arm64")]
    + [make_clang_config(23, "arm64"), make_clang_config(23, "macOS")]
    + [
        make_apple_clang_config("macos-15", "16", xcode_version="16.1"),
        make_apple_clang_config("macos-26", "21", xcode_version="26.6"),
    ]
    + [make_msvc_config(release="14.5")]
}

full_matrix = dict(
    toolchain=list(toolchains.values()),
    std=[20, 23, 26],
    build_type=["Release", "Debug"],
    **ConanOptions.full_matrix(),
)


def _guarantee_api_coverage(
    collector: CombinationCollector,
    rgen: random.Random,
    toolchains_iter,
    *,
    freestanding: bool,
    contracts: str | None = None,
) -> None:
    """Guarantee ≥1 import_std, ≥1 no_crtp, and ≥2 C++26-contracts configurations
    per supporting toolchain."""
    no_crtp_extra = {} if contracts is None else {"contracts": contracts}
    for tc in toolchains_iter:
        # C++26 contracts require C++26; two samples so that later
        # `min_samples_per_value=2` passes do not have to hit this narrow
        # configuration by chance (the freestanding preset adds its own coverage).
        if tc.feature_support.std_contracts and not freestanding and contracts is None:
            collector.sample_combinations(
                rgen=rgen,
                min_samples=2,
                toolchain=tc,
                contracts="std",
                std=26,
                freestanding=freestanding,
            )
        # import_std is incompatible with freestanding: the pre-built std.pcm
        # is compiled without -ffreestanding and cannot be reused.
        if tc.feature_support.import_std and not freestanding:
            for cxx_modules in [True, False]:
                collector.sample_combinations(
                    rgen=rgen,
                    min_samples=1,
                    toolchain=tc,
                    import_std=True,
                    cxx_modules=cxx_modules,
                    std_format=True,
                    contracts="none",
                    std=23,
                    freestanding=freestanding,
                )
        if tc.feature_support.explicit_this:
            # import_std is incompatible with freestanding; pin it off.
            import_std_extra = {"import_std": False} if freestanding else {}
            collector.sample_combinations(
                rgen=rgen,
                min_samples=1,
                toolchain=tc,
                no_crtp=True,
                std=23,
                freestanding=freestanding,
                **import_std_extra,
                **no_crtp_extra,
            )


def _supported_values(tc: Toolchain) -> dict[str, typing.Any]:
    """The submatrix of values that `tc` supports on its own.

    `sample_combinations` gives up on a value it cannot reach and stops covering the
    rest, so a per-value request for one toolchain must not name an unsupported value.
    """
    fs = tc.feature_support
    matrix = ConanOptions.full_matrix()
    return dict(
        toolchain=tc,
        std=[s for s in full_matrix["std"] if s <= fs.max_std],
        cxx_modules=[v for v in matrix["cxx_modules"] if fs.cxx_modules or not v],
        import_std=[v for v in matrix["import_std"] if fs.import_std or not v],
        std_format=[v for v in matrix["std_format"] if fs.std_format or not v],
        no_crtp=[v for v in matrix["no_crtp"] if fs.explicit_this or not v],
        contracts=[c for c in matrix["contracts"] if fs.std_contracts or c != "std"],
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
        hard_excludes=lambda c: (not c.is_supported),
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
            )
            collector.all_combinations(
                std_format=True,
                **config,
            )
            # fmtlib for toolchains/configs where std::format is not viable: either the
            # toolchain doesn't support it at all, or it's MSVC + Debug (C7595 consteval
            # bug; cf. Configuration.is_supported in job_matrix.py).
            collector.all_combinations(
                filter=lambda me: not me.toolchain.feature_support.std_format
                or (me.toolchain.compiler.type == "MSVC" and me.build_type == "Debug"),
                std_format=False,
                **config,
            )
            # import_std and no_crtp are key library APIs: guarantee at least one
            # configuration per toolchain that supports each, so any regression in
            # the library interface is caught regardless of the random seed.
            _guarantee_api_coverage(
                collector, rgen, toolchains.values(), freestanding=False
            )
            # MSVC is the only Windows toolchain, and the per-value sampling below is
            # spread over all toolchains, so it would get only one or two jobs. Cover
            # every value MSVC supports at least once.
            for tc in toolchains.values():
                if tc.compiler.type == "MSVC":
                    collector.sample_combinations(
                        rgen=rgen,
                        min_samples_per_value=1,
                        **_supported_values(tc),
                        freestanding=False,
                    )
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=1,
                freestanding=False,
            )
            # add more random coverage across all configurations
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=2,
                freestanding=False,
            )

        case "clang-tidy":
            # Always run on the latest supported Clang for best analysis quality
            latest_clang = max(
                (
                    tc
                    for tc in toolchains.values()
                    if tc.compiler.type == "CLANG" and tc.os == "ubuntu-24.04"
                ),
                key=lambda tc: tc.compiler.version,
            )
            # Guarantee import_std and no_crtp API coverage before random sampling
            _guarantee_api_coverage(collector, rgen, [latest_clang], freestanding=False)
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=1,
                toolchain=latest_clang,
                # exclude contract backends the toolchain does not support so the
                # sampler is not asked for unsatisfiable combinations
                contracts=[
                    c
                    for c in ConanOptions.full_matrix()["contracts"]
                    if c != "std" or latest_clang.feature_support.std_contracts
                ],
                freestanding=False,
            )
        case "all-freestanding":
            collector.all_combinations(
                freestanding=True,
            )
        case "freestanding":
            freestanding_toolchains = [
                tc
                for tc in toolchains.values()
                if tc.feature_support.freestanding and not tc.os.startswith("macos")
            ]
            base = dict(
                contracts="none",
                freestanding=True,
                std=23,
                no_crtp=False,
                cxx_modules=False,
                import_std=False,
                build_type="Release",
            )
            # One baseline config per toolchain: std_format=True for those that support it
            collector.all_combinations(
                toolchain=freestanding_toolchains,
                std_format=True,
                **base,
            )
            # fmtlib for those toolchains where we don't support std_format
            collector.all_combinations(
                filter=lambda me: not me.toolchain.feature_support.std_format,
                toolchain=freestanding_toolchains,
                std_format=False,
                **base,
            )
            # TODO C++26 contracts in freestanding mode are supported by the build system
            # but not CI-covered yet: current GCC 16 snapshots reject `-ffreestanding`
            # with `-std=c++26` for ANY contracts setting (libstdc++ regression:
            # "'range_format' does not name a type" in <optional> when <ranges> is
            # included). Add coverage here when upstream is fixed:
            # collector.all_combinations(
            #     filter=lambda me: me.toolchain.feature_support.std_contracts,
            #     toolchain=freestanding_toolchains,
            #     std_format=True,
            #     **{**base, "contracts": "std", "std": 26},
            # )
            collector.sample_combinations(
                rgen=rgen,
                min_samples_per_value=1,
                toolchain=freestanding_toolchains,
                contracts="none",
                freestanding=True,
                import_std=False,
                std=23,
            )
            # Guarantee import_std and no_crtp API coverage per supporting toolchain.
            # (all_combinations above fixes no_crtp=False, so no_crtp=True needs an
            # explicit guarantee; import_std is also made explicit for clarity.)
            _guarantee_api_coverage(
                collector,
                rgen,
                freestanding_toolchains,
                freestanding=True,
                contracts="none",
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
            with open(output_file, "at") as fh:
                fh.write(f"matrix={json.dumps(json_data)}\n")
                fh.write(f"seed={args.seed}\n")
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
                msg = [e.infostr(adjusted=True) for e in data]
                for m in sorted(msg):
                    print(m)
            case "counts":
                print(f"Total combinations {len(data)}")
                per_value: dict[tuple[str, typing.Any], int] = {}
                for e in data:
                    for k, v in vars(e).items():
                        per_value[k, v] = per_value.get((k, v), 0) + 1
                for (k, v), n in sorted(
                    per_value.items(), key=lambda x: (x[0][0], str(x[0][1]))
                ):
                    print(f"  {k}={v}: {n}")
            case "none":
                pass
            case _:
                raise KeyError(f"Unknown debug mode {dbg!r}")


if __name__ == "__main__":
    main()
