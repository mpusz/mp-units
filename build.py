from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(remotes="https://api.bintray.com/conan/mpusz/conan-mpusz, https://api.bintray.com/conan/martinmoene/nonstd-lite")
    builder.add_common_builds(shared_option_name=False, pure_c=False)
    builder.run()
