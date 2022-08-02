# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options.
# For a full list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import re


def get_version():
    try:
        with open("../src/CMakeLists.txt", "r") as file:
            content = file.read()
        version = re.search(
            r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content
        ).group(1)
        return version.strip()
    except Exception:
        return None


# -- Project information -----------------------------------------------------

project = "mp-units"
copyright = "2018-present, Mateusz Pusz"
author = "Mateusz Pusz"

# The major project version, used as the replacement for |version|.
version = get_version()

# The full project version, used as the replacement for |release| and
# e.g. in the HTML templates.
release = get_version()

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "sphinx.ext.autosectionlabel",
    "sphinx.ext.githubpages",
    "sphinx.ext.graphviz",
    "sphinx_rtd_theme",
    "recommonmark",
    "breathe",
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ["_templates"]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# If true, Sphinx will warn about all references where the target cannot
# be found. Default is False.
nitpicky = True

# A list of (type, target) tuples (by default empty) that should be ignored
# when generating warnings in “nitpicky mode”. Note that type should include
# the domain name if present. Example entries would be ('py:func', 'int')
# or ('envvar', 'LD_LIBRARY_PATH').
nitpick_ignore = []

# True to prefix each section label with the name of the document it is in,
# followed by a colon. Useful for avoiding ambiguity when the same section
# heading appears in different documents.
autosectionlabel_prefix_document = True

# -- C++ configuration ---------------------------------------------------

# The name of the default domain. Can also be None to disable a default
# domain. The default is 'py'.
primary_domain = "cpp"

# The reST default role (used for this markup: `text`) to use for all documents.
default_role = "cpp:any"

# The default language to highlight source code in. The default is 'python3'.
# The value should be a valid Pygments lexer name (https://pygments.org/docs/lexers).
highlight_language = "cpp"

# The style name to use for Pygments highlighting of source code. If not set,
# either the theme’s default style or 'sphinx' is selected for HTML output.
pygments_style = "default"

# A list of prefixes that will be ignored when sorting C++ objects in the global
# index. For example ['awesome_lib::'].
cpp_index_common_prefix = ["units::"]


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = "sphinx_rtd_theme"

# A dictionary of options that influence the look and feel of the selected theme.
# These are theme-specific.
html_theme_options = {
    # WARNING: unsupported theme option 'github_url' given
    # 'github_url': 'https://github.com/mpusz/units'
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ["_static"]

# If given, this must be the name of an image file (path relative to the
# configuration directory) that is the logo of the docs. It is placed at the
# top of the sidebar; its width should therefore not exceed 200 pixels.
# Default: None.
# html_logo =

# These paths are either relative to html_static_path or fully qualified
# paths (eg. https://...)
html_css_files = ["css/custom.css"]


# -- Breathe configuration ---------------------------------------------------


# This should be a dictionary in which the keys are project names and the values
# are paths to the folder containing the doxygen output for that project.
breathe_projects = {"mp-units": "build/xml"}

# This should match one of the keys in the breathe_projects dictionary and
# indicates which project should be used when the project is not specified on
# the directive.
breathe_default_project = "mp-units"

# Allows you to specify domains for particular files according to their extension.
breathe_domain_by_extension = {"h": "cpp"}

# Provides the directive flags that should be applied to all directives which
# take :members:, :private-members: and :undoc-members: options. By default,
# this is set to an empty list, which means no members are displayed.
breathe_default_members = ("members",)
