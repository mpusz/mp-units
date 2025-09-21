# GitHub Codespaces Integration for mp-units

This repository is configured to work seamlessly with GitHub Codespaces, providing a
complete development environment with all compilers and tools pre-installed. The environment
is built on [custom `trainiteu/mp-units` Docker image](https://github.com/train-it-eu/docker-images/blob/main/mp-units/Dockerfile)
which extends the build tool ecosystem with **mp-units**-specific requirements.

## Docker Image Hierarchy

The development environment is built on a layered Docker image stack:

1. **[`trainiteu/ubuntu`](https://github.com/train-it-eu/docker-images/blob/main/ubuntu/Dockerfile)**: Ubuntu 25.04 (Plucky Puffin) base with essential development tools
2. **[`trainiteu/cpp`](https://github.com/train-it-eu/docker-images/blob/main/cpp/Dockerfile)**: Adds CMake, Ninja, Python with pipx for tool management
3. **[`trainiteu/cpp-conan`](https://github.com/train-it-eu/docker-images/blob/main/cpp-conan/Dockerfile)**: Adds Conan 2.x and base compiler profiles
4. **[`trainiteu/mp-units`](https://github.com/train-it-eu/docker-images/blob/main/mp-units/Dockerfile)**: **mp-units-specific extensions** including:
   - Extended compiler matrix (GCC 12-15, Clang 16-21)
   - Python documentation ecosystem via pipx
   - Pre-configured Conan remotes and profiles for all compiler combinations
   - Development aliases and optimized build configuration

## Quick Start

### Option 1: Create Codespace from GitHub.com

1. Navigate to the [mp-units repository](https://github.com/mpusz/units) on GitHub
2. Click the green **"<> Code"** button
3. Select the **"Codespaces"** tab
4. Click **"Create codespace on master"** (or your desired branch)
5. Wait for the environment to set up (usually 3-5 minutes)

### Option 2: Create Codespace from VS Code

1. Install the [GitHub Codespaces extension](https://marketplace.visualstudio.com/items?itemName=GitHub.codespaces)
   in VS Code
2. Open the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`)
3. Type "Codespaces: Create New Codespace"
4. Select the **mp-units** repository
5. Choose your preferred branch and machine type

### Option 3: Use Dev Container Locally in VS Code

1. **Prerequisites**:
   - Install [Docker Desktop](https://www.docker.com/products/docker-desktop/) or Docker Engine
   - Install [VS Code](https://code.visualstudio.com/)
   - Install the [Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

2. **Open in container**:
   - Open the main **mp-units** directory in VS Code
   - VS Code will detect the `.devcontainer` configuration
   - Click **"Reopen in Container"** when prompted, or
   - Open Command Palette (`Ctrl+Shift+P`) → **"Dev Containers: Reopen in Container"**

3. **Wait for setup**: The container will build automatically (usually 3-5 minutes)

This option provides the same development environment as Codespaces but runs locally on your machine, offering:
- **Full offline development** (after initial setup)
- **Better performance** on powerful local machines
- **Direct file system access** for easier integration with local tools
- **No usage limits** unlike GitHub Codespaces

## Development Environment Features

The development environment includes all compilers and tools used for comprehensive testing:

- **Base OS**: Ubuntu 25.04 (Plucky Puffin)
- **GCC**: 12, 13, 14, 15
- **Clang**: 16, 17, 18, 19, 20, 21
- **Standard Library**: libc++-18 for backward compatibility with clang-16
- **Build Tools**: CMake, Ninja, Conan package manager
- **Code Quality**: clang-format, clang-tidy, include-what-you-use
- **Documentation**: MkDocs with Material theme
- **VS Code Extensions**: C++ IntelliSense, CMake Tools, Python support
- **Pre-configured Conan Profiles**: All compiler configurations ready to use
- **Pre-compiled dependencies**: Conan cache is persistent and pre-compiled dependencies
   for recommended configurations are uploaded on a pre-configured Conan remote

### Available Conan Profiles

The following Conan profiles are pre-configured:

- `gcc12`, `gcc13`, `gcc14`, `gcc15`
- `clang16`, `clang17`, `clang18`, `clang19`, `clang20`, `clang21`

### Multi-Compiler Testing Script

The Codespace includes a comprehensive testing script at `.devcontainer/check_all.sh`:

```bash
# Available script options
./.devcontainer/check_all.sh install   # Install dependencies for all configurations and provide CMake Presets
./.devcontainer/check_all.sh build     # Build with all compiler configurations
./.devcontainer/check_all.sh create    # Create Conan packages with all configurations
./.devcontainer/check_all.sh -d create # Include debug builds
```

Each iteration tests different combinations of:
- C++ standards: 20, 23, 26
- Standard library implementations: libstdc++11, libc++
- Feature flags: cxx_modules, import_std, std_format, contracts

#### API Reference Generation

The `.devcontainer/api_reference.sh` script automates the generation of API documentation:

```bash
# Generate API reference documentation
.devcontainer/api_reference.sh

# Generate API reference documentation (setup only, no build)
.devcontainer/api_reference.sh -s

# Use custom directory for dependencies
.devcontainer/api_reference.sh -d /path/to/deps
```

By default, the script performs both setup and build operations. Use the `-s` flag
to perform setup only (during development container initialization). The script runs
automatically during Codespace initialization to ensure the API documentation setup
is ready.

The `-d` parameter allows you to specify a custom directory for API documentation
dependencies. By default, dependencies are stored in `../api_reference_deps` relative
to the project root. This directory contains:

- **jegp.cmake_modules**: CMake modules required for documentation generation
- **draft**: C++ standard draft sources for standardese-style formatting
- **cxxdraft-htmlgen**: HTML generation tools and templates

### Documentation

```bash
# Serve documentation locally
mkdocs serve
```

The documentation will be available at `http://localhost:8000` and will
automatically forward in Codespaces.


## Tips for Codespaces Usage

### Machine Specifications

For **mp-units** development, GitHub Codespaces offers different machine types with varying
performance and costs. The free tier provides **120 core-hours per month** for public repositories.

| Machine Type | vCPUs | RAM   | Free Hours/Month* | Best For                                          |
|--------------|-------|-------|-------------------|---------------------------------------------------|
| **2-core**   | 2     | 8GB   | 60 hours          | Single-profile development, light testing         |
| **4-core**   | 4     | 16GB  | 30 hours          | `check_all.sh` usage, parallel builds             |
| **8-core**   | 8     | 32GB  | 15 hours          | Intensive multi-profile development               |
| **16-core**  | 16    | 64GB  | 7.5 hours         | Large-scale development, heavy parallel workloads |
| **32-core**  | 32    | 128GB | 3.75 hours        | Maximum performance for complex builds            |

\* *Based on 120 free core-hours per month for GitHub Free accounts on public repositories*

For current machine specifications and pricing, see:
- [GitHub Codespaces billing](https://docs.github.com/en/billing/concepts/product-billing/github-codespaces)
- [GitHub Pricing Calculator](https://github.com/pricing/calculator?feature=codespaces)

**Usage Recommendations:**
- **2-core**: Suitable for documentation, small changes, single compiler testing
- **4-core**: **Recommended** for most mp-units development - good balance of performance and free usage
- **8-core+**: Best for intensive development sessions or when you need maximum build speed


### Development Workflow

#### Recommended Development Process

1. **Start with single profile**: Test your changes with one compiler first

   ```bash
   conan create . -pr clang21 -c user.mp-units.build:all=True -b missing
   ```

2. **Test compatibility**: Check the oldest compilers that may have different C++20 support

   ```bash
   conan create . -pr gcc12 -c user.mp-units.build:all=True -b missing
   conan create . -pr clang16 -c user.mp-units.build:all=True -b missing
   ```

3. **Code quality**: Run pre-commit checks (automatically included in check_all.sh)

   ```bash
   pre-commit run --all-files
   ```

4. **Full validation**: Run the complete test suite across all configurations

   ```bash
   .devcontainer/check_all.sh create
   ```


## Getting Help

- Check the [mp-units documentation](https://mpusz.github.io/mp-units/) for project-specific information
- Refer to the [GitHub Codespaces documentation](https://docs.github.com/en/codespaces) for platform help
- Use the [discussion forums](https://github.com/mpusz/mp-units/discussions) or
  [issues](https://github.com/mpusz/mp-units/issues) in the repository for development questions
