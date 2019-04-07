from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager()
    builder.add_common_builds(shared_option_name=False, pure_c=False,
                              remotes="https://api.bintray.com/conan/mpusz/conan-mpusz, https://api.bintray.com/conan/martinmoene/nonstd-lite")
    builder.run()
