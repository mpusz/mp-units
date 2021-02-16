# -*- coding: utf-8 -*-
"""
    pygments.lexers.c_cpp
    ~~~~~~~~~~~~~~~~~~~~~

    Lexers for C/C++ languages.

    :copyright: Copyright 2006-2019 by the Pygments team, see AUTHORS.
    :license: BSD, see LICENSE for details.
"""

import re

from pygments.lexer import RegexLexer, include, bygroups, using, \
    this, inherit, default, words
from pygments.util import get_bool_opt
from pygments.token import Text, Comment, Operator, Keyword, Name, String, \
    Number, Punctuation, Error
from sphinx.highlighting import lexers

class MyCFamilyLexer(RegexLexer):
    #: optional Comment or Whitespace
    _ws = r'(?:\s|//.*?\n|/[*].*?[*]/)+'

    # The trailing ?, rather than *, avoids a geometric performance drop here.
    #: only one /* */ style comment
    _ws1 = r'\s*(?:/[*].*?[*]/\s*)?'

    tokens = {
        'whitespace': [
            # preprocessor directives: without whitespace
            (r'^#if\s+0', Comment.Preproc, 'if0'),
            ('^#', Comment.Preproc, 'macro'),
            # or with whitespace
            ('^(' + _ws1 + r')(#if\s+0)',
             bygroups(using(this), Comment.Preproc), 'if0'),
            ('^(' + _ws1 + ')(#)',
             bygroups(using(this), Comment.Preproc), 'macro'),
            (r'\n', Text),
            (r'\s+', Text),
            (r'\\\n', Text),  # line continuation
            (r'//(\n|[\w\W]*?[^\\]\n)', Comment.Single),
            (r'/(\\\n)?[*][\w\W]*?[*](\\\n)?/', Comment.Multiline),
            # Open until EOF, so no ending delimeter
            (r'/(\\\n)?[*][\w\W]*', Comment.Multiline),
        ],
        'statements': [
            (r'(L?)(")', bygroups(String.Affix, String), 'string'),
            (r"(L?)(')(\\.|\\[0-7]{1,3}|\\x[a-fA-F0-9]{1,2}|[^\\\'\n])(')",
             bygroups(String.Affix, String.Char, String.Char, String.Char)),
            (r'\*/', Error),
            (r'[~!%^&*+=|?:<>/-]', Operator),
            (r'[()\[\],.]', Punctuation),
            (words(('asm', 'auto', 'break', 'case', 'const', 'continue',
                    'default', 'do', 'else', 'enum', 'extern', 'for', 'goto',
                    'if', 'register', 'restricted', 'return', 'sizeof',
                    'static', 'struct', 'switch', 'typedef', 'union',
                    'volatile', 'while'),
                   suffix=r'\b'), Keyword),
            (r'(bool|int|long|float|short|double|char|unsigned|signed|void)\b',
             Keyword.Type),
            (words(('inline', '_inline', '__inline', 'naked', 'restrict',
                    'thread', 'typename'), suffix=r'\b'), Keyword.Reserved),
            # Vector intrinsics
            (r'(__m(128i|128d|128|64))\b', Keyword.Reserved),
            # Microsoft-isms
            (words((
                'asm', 'int8', 'based', 'except', 'int16', 'stdcall', 'cdecl',
                'fastcall', 'int32', 'declspec', 'finally', 'int64', 'try',
                'leave', 'wchar_t', 'w64', 'unaligned', 'raise', 'noop',
                'identifier', 'forceinline', 'assume'),
                prefix=r'__', suffix=r'\b'), Keyword.Reserved),
            (r'(true|false|NULL)\b', Name.Builtin),
            (r'([a-zA-Z_]\w*)(\s*)(:)(?!:)', bygroups(Name.Label, Text, Punctuation)),
            (r'[a-zA-Z_]\w*', Name),
        ],
        'root': [
            include('whitespace'),
            # functions
            (r'((?:[\w*\s])+?(?:\s|[*]))'  # return arguments
             r'([a-zA-Z_]\w*)'             # method name
             r'(\s*\([^;]*?\))'            # signature
             r'([^;{]*)(\{)',
             bygroups(using(this), Name.Function, using(this), using(this),
                      Punctuation),
             'function'),
            # function declarations
            (r'((?:[\w*\s])+?(?:\s|[*]))'  # return arguments
             r'([a-zA-Z_]\w*)'             # method name
             r'(\s*\([^;]*?\))'            # signature
             r'([^;]*)(;)',
             bygroups(using(this), Name.Function, using(this), using(this),
                      Punctuation)),
            default('statement'),
        ],
        'statement': [
            include('whitespace'),
            include('statements'),
            ('[{}]', Punctuation),
            (';', Punctuation, '#pop'),
        ],
        'function': [
            include('whitespace'),
            include('statements'),
            (';', Punctuation),
            (r'\{', Punctuation, '#push'),
            (r'\}', Punctuation, '#pop'),
        ],
        'string': [
            (r'"', String, '#pop'),
            (r'\\([\\abfnrtv"\']|x[a-fA-F0-9]{2,4}|'
             r'u[a-fA-F0-9]{4}|U[a-fA-F0-9]{8}|[0-7]{1,3})', String.Escape),
            (r'[^\\"\n]+', String),  # all other characters
            (r'\\\n', String),  # line continuation
            (r'\\', String),  # stray backslash
        ],
        'macro': [
            (r'(include)(' + _ws1 + r')([^\n]+)',
             bygroups(Comment.Preproc, Text, Comment.PreprocFile)),
            (r'[^/\n]+', Comment.Preproc),
            (r'/[*](.|\n)*?[*]/', Comment.Multiline),
            (r'//.*?\n', Comment.Single, '#pop'),
            (r'/', Comment.Preproc),
            (r'(?<=\\)\n', Comment.Preproc),
            (r'\n', Comment.Preproc, '#pop'),
        ],
        'if0': [
            (r'^\s*#if.*?(?<!\\)\n', Comment.Preproc, '#push'),
            (r'^\s*#el(?:se|if).*\n', Comment.Preproc, '#pop'),
            (r'^\s*#endif.*?(?<!\\)\n', Comment.Preproc, '#pop'),
            (r'.*?\n', Comment),
        ]
    }

    stdlib_types = {
        'size_t', 'ssize_t', 'off_t', 'wchar_t', 'ptrdiff_t', 'sig_atomic_t', 'fpos_t',
        'clock_t', 'time_t', 'va_list', 'jmp_buf', 'FILE', 'DIR', 'div_t', 'ldiv_t',
        'mbstate_t', 'wctrans_t', 'wint_t', 'wctype_t'}
    c99_types = {
        '_Bool', '_Complex', 'int8_t', 'int16_t', 'int32_t', 'int64_t', 'uint8_t',
        'uint16_t', 'uint32_t', 'uint64_t', 'int_least8_t', 'int_least16_t',
        'int_least32_t', 'int_least64_t', 'uint_least8_t', 'uint_least16_t',
        'uint_least32_t', 'uint_least64_t', 'int_fast8_t', 'int_fast16_t', 'int_fast32_t',
        'int_fast64_t', 'uint_fast8_t', 'uint_fast16_t', 'uint_fast32_t', 'uint_fast64_t',
        'intptr_t', 'uintptr_t', 'intmax_t', 'uintmax_t'}
    linux_types = {
        'clockid_t', 'cpu_set_t', 'cpumask_t', 'dev_t', 'gid_t', 'id_t', 'ino_t', 'key_t',
        'mode_t', 'nfds_t', 'pid_t', 'rlim_t', 'sig_t', 'sighandler_t', 'siginfo_t',
        'sigset_t', 'sigval_t', 'socklen_t', 'timer_t', 'uid_t'}

    def __init__(self, **options):
        self.stdlibhighlighting = get_bool_opt(options, 'stdlibhighlighting', True)
        self.c99highlighting = get_bool_opt(options, 'c99highlighting', True)
        self.platformhighlighting = get_bool_opt(options, 'platformhighlighting', True)
        RegexLexer.__init__(self, **options)

    def get_tokens_unprocessed(self, text):
        for index, token, value in \
                RegexLexer.get_tokens_unprocessed(self, text):
            if token is Name:
                if self.stdlibhighlighting and value in self.stdlib_types:
                    token = Keyword.Type
                elif self.c99highlighting and value in self.c99_types:
                    token = Keyword.Type
                elif self.platformhighlighting and value in self.linux_types:
                    token = Keyword.Type
            yield index, token, value


class MyCppLexer(MyCFamilyLexer):
    """
    For C++ source code with preprocessor directives.
    """
    name = 'My C++'
    aliases = ['cpp', 'c++']
    filenames = ['*.cpp', '*.hpp', '*.c++', '*.h++',
                 '*.cc', '*.hh', '*.cxx', '*.hxx',
                 '*.C', '*.H', '*.cp', '*.CPP']
    mimetypes = ['text/x-c++hdr', 'text/x-c++src']
    priority = 0.1

    tokens = {
        'statements': [
            (words((
                'catch', 'const_cast', 'delete', 'dynamic_cast', 'explicit',
                'export', 'friend', 'mutable', 'namespace', 'new', 'operator',
                'private', 'protected', 'public', 'reinterpret_cast',
                'restrict', 'static_cast', 'template', 'this', 'throw', 'throws',
                'try', 'typeid', 'typename', 'using', 'virtual',
                'constexpr', 'nullptr', 'decltype', 'thread_local',
                'alignas', 'alignof', 'static_assert', 'noexcept', 'override',
                'final', 'constinit', 'consteval', 'constinit', 'concept', 'co_await',
                'co_return', 'co_yield', 'requires', 'import', 'module'), suffix=r'\b'), Keyword),
            (r'char(16_t|32_t|8_t)\b', Keyword.Type),
            (r'(class)(\s+)', bygroups(Keyword, Text), 'classname'),
            # C++11 raw strings
            (r'(R)(")([^\\()\s]{,16})(\()((?:.|\n)*?)(\)\3)(")',
             bygroups(String.Affix, String, String.Delimiter, String.Delimiter,
                      String, String.Delimiter, String)),
            # C++11 UTF-8/16/32 strings
            (r'(u8|u|U)(")', bygroups(String.Affix, String), 'string'),
            # C++14 number separators
            (r'(\d[\d\']*\.[\d\']*|\.\d[\d\']*|\d[\d\']*)[eE][+-]?\d[\d\']*[LlUu]*', Number.Float),
            (r'(\d[\d\']*\.[\d\']*|\.\d[\d\']*|\d[\d\']*[fF])[fF]?', Number.Float),
            (r'0x[0-9a-fA-F\']+[LlUu]*', Number.Hex),
            (r'0[0-7\']+[LlUu]*', Number.Oct),
            (r'\d[\d\']*[LlUu]*', Number.Integer),
            inherit,
        ],
        'root': [
            inherit,
            # C++ Microsoft-isms
            (words(('virtual_inheritance', 'uuidof', 'super', 'single_inheritance',
                    'multiple_inheritance', 'interface', 'event'),
                   prefix=r'__', suffix=r'\b'), Keyword.Reserved),
            # Offload C++ extensions, http://offload.codeplay.com/
            (r'__(offload|blockingoffload|outer)\b', Keyword.Pseudo),
        ],
        'classname': [
            (r'[a-zA-Z_]\w*', Name.Class, '#pop'),
            # template specification
            (r'\s*(?=>)', Text, '#pop'),
        ],
    }

    def analyse_text(text):
        if re.search('#include <[a-z_]+>', text):
            return 0.2
        if re.search('using namespace ', text):
            return 0.4

lexers['cpp'] = MyCppLexer(startinline=True)

# TODO Remove the above when Pygments release with fixes is available


# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options.
# For a full list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import subprocess, os, re

def get_version():
    try:
        with open('../src/CMakeLists.txt', 'r') as file:
            content = file.read()
        version = re.search(r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content).group(1)
        return version.strip()
    except Exception:
        return None


# -- Project information -----------------------------------------------------

project = 'mp-units'
copyright = '2018-present, Mateusz Pusz'
author = 'Mateusz Pusz'

# The major project version, used as the replacement for |version|.
version = get_version()

# The full project version, used as the replacement for |release| and
# e.g. in the HTML templates.
release = get_version()

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx.ext.autosectionlabel',
    'sphinx.ext.githubpages',
    'sphinx.ext.graphviz',
    'recommonmark',
    'breathe'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

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
primary_domain = 'cpp'

# The reST default role (used for this markup: `text`) to use for all documents.
default_role = 'cpp:any'

# The default language to highlight source code in. The default is 'python3'.
# The value should be a valid Pygments lexer name (https://pygments.org/docs/lexers).
highlight_language = 'cpp'

# The style name to use for Pygments highlighting of source code. If not set,
# either the theme’s default style or 'sphinx' is selected for HTML output.
pygments_style = 'default'

# A list of prefixes that will be ignored when sorting C++ objects in the global
# index. For example ['awesome_lib::'].
cpp_index_common_prefix = ['units::']


# -- Options for HTML output -------------------------------------------------

# A list of paths that contain custom themes, either as subdirectories or as
# zip files. Relative paths are taken as relative to the configuration directory.

html_theme_path = ["_themes", ]

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
html_theme = 'sphinx_rtd_theme'

# A dictionary of options that influence the look and feel of the selected theme.
# These are theme-specific.
html_theme_options = {
    # WARNING: unsupported theme option 'github_url' given
    # 'github_url': 'https://github.com/mpusz/units'
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# If given, this must be the name of an image file (path relative to the
# configuration directory) that is the logo of the docs. It is placed at the
# top of the sidebar; its width should therefore not exceed 200 pixels.
# Default: None.
# html_logo = 

# These paths are either relative to html_static_path or fully qualified
# paths (eg. https://...)
html_css_files = [
    'css/custom.css'
]


# -- Breathe configuration ---------------------------------------------------

def configureDoxyfile(input_dir, output_dir):
    with open('Doxyfile.in', 'r') as file:
        filedata = file.read()
 
    filedata = filedata.replace('@DOXYGEN_INPUT_DIR@', input_dir)
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', output_dir)
 
    with open('Doxyfile', 'w') as file:
        file.write(filedata)

# Check if we're running on Read the Docs' servers
read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

# This should be a dictionary in which the keys are project names and the values
# are paths to the folder containing the doxygen output for that project.
breathe_projects = {}
if read_the_docs_build:
    input_dir = '../src'
    output_dir = 'build'
    configureDoxyfile(input_dir, output_dir)
    subprocess.call('doxygen', shell=True)
    breathe_projects['mp-units'] = output_dir + '/xml'

# This should match one of the keys in the breathe_projects dictionary and
# indicates which project should be used when the project is not specified on
# the directive.
breathe_default_project = 'mp-units'

# Allows you to specify domains for particular files according to their extension.
breathe_domain_by_extension = {"h" : "cpp"}

# Provides the directive flags that should be applied to all directives which
# take :members:, :private-members: and :undoc-members: options. By default,
# this is set to an empty list, which means no members are displayed.
breathe_default_members = ('members', )
