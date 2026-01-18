#!/usr/bin/env python3
"""
Generate reference documentation for mp-units systems from C++ header files.

This script parses system header files and generates markdown documentation including:
- Dimensions index (alphabetical list)
- Quantities index (alphabetical list)
- Units index (alphabetical list)
- Per-system reference pages with base/derived units separated
"""

import hashlib
import json
import re
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Set


@dataclass
class Dimension:
    """Represents a dimension definition"""

    name: str
    symbol: str
    namespace: str
    file: str


@dataclass
class Quantity:
    """Represents a quantity specification"""

    name: str
    parent: Optional[str] = None
    dimension: Optional[str] = None
    equation: Optional[str] = None
    namespace: str = ""
    file: str = ""
    is_kind: bool = False
    alias_target: Optional[str] = (
        None  # If this is an alias, the name of the original quantity
    )
    # Fields from C++ extraction
    dimensional_formula: Optional[str] = (
        None  # Calculated dimensional formula (e.g., "L²MT⁻³")
    )
    character: str = "Real"  # Quantity character: Real, Complex, Vector, Tensor
    kind_of: str = ""  # e.g., "isq::length" - from C++ get_kind()
    parent_from_cpp: str = ""  # e.g., "isq::length" or "<root>" - from C++ qs._parent_
    secondary_namespaces: list = (
        None  # Namespaces where accessible via using declarations
    )


@dataclass
class Unit:
    """Represents a unit definition"""

    name: str
    symbol: str
    unit_symbols: list = None  # List of unit_symbol names (e.g., ['Ω', 'ohm'])
    definition: str = ""
    namespace: str = ""  # Full namespace path (e.g., "mp_units::usc::survey1893")
    file: str = ""
    is_alias: bool = False
    is_base: bool = False  # True if kind_of<> with no equation
    subnamespace: Optional[str] = None  # Relative subnamespace (e.g., "survey1893")
    origin_namespace: Optional[str] = (
        None  # Original namespace where defined (e.g., "non_si")
    )
    secondary_namespaces: list = (
        None  # Namespaces where accessible via using declarations
    )
    alias_target: Optional[str] = (
        None  # If this is an alias, the name of the original entity
    )

    def __post_init__(self):
        if self.unit_symbols is None:
            self.unit_symbols = []


@dataclass
class PointOrigin:
    """Represents a point origin definition"""

    name: str
    origin_type: str  # 'absolute' or 'relative'
    definition: str = (
        ""  # Template parameter (e.g., "isq::thermodynamic_temperature" or "point<...>")
    )
    namespace: str = ""
    file: str = ""
    alias_target: Optional[str] = (
        None  # If this is an alias, the name of the original entity
    )
    secondary_namespaces: list = (
        None  # Namespaces where accessible via using declarations
    )


@dataclass
class Prefix:
    """Represents a prefix definition (like 'kilo' or 'mega')"""

    name: str
    symbol: str
    definition: str = (
        ""  # Magnitude/scale (e.g., "mag_power<10, 3>" or "mag_power<2, 10>")
    )
    namespace: str = ""
    secondary_namespaces: list = (
        None  # Namespaces where accessible via using declarations
    )
    file: str = ""


@dataclass
class SystemInfo:
    """Information about a system (namespace-based)"""

    namespace: str
    files: List[Path] = field(default_factory=list)
    dimensions: List[Dimension] = field(default_factory=list)
    quantities: List[Quantity] = field(default_factory=list)
    units: List[Unit] = field(default_factory=list)
    point_origins: List[PointOrigin] = field(default_factory=list)
    prefixes: List[Prefix] = field(default_factory=list)
    inline_subnamespaces: Set[str] = field(
        default_factory=set
    )  # Track inline subnamespaces (e.g., "si2019", "codata2018")


class SystemsParser:
    """Parser for mp-units system header files"""

    def __init__(self, systems_dir: Path):
        self.systems_dir = systems_dir
        self.systems: Dict[str, SystemInfo] = {}
        self.parsed_files: Set[Path] = set()  # Track parsed files to avoid duplicates
        # Determine the source root directory for parsing core framework files
        # systems_dir is src/systems/include/mp-units/systems, so we need to go up 5 levels to get to repo root
        self.source_root = systems_dir.parent.parent.parent.parent.parent

    def parse_all_systems(self):
        """Parse all system header files, following include order"""
        # First, parse core framework entities
        self._parse_core_framework()

        # Find all main system headers (both top-level and in subdirectories)
        main_headers = []

        # Top-level headers
        for header_file in self.systems_dir.glob("*.h"):
            if not header_file.name.startswith("_"):
                main_headers.append(header_file)

        # Subdirectory main headers (like si/si.h, isq/isq.h)
        for subdir in self.systems_dir.iterdir():
            if subdir.is_dir():
                # Look for main header with same name as directory
                main_header = subdir.parent / f"{subdir.name}.h"
                if main_header.exists() and main_header not in main_headers:
                    main_headers.append(main_header)

        # Parse each main header and its includes
        # The parse_system_with_includes method follows include directives,
        # ensuring dependencies are parsed before dependents (just like C++ compilation)
        for main_header in main_headers:
            self.parse_system_with_includes(main_header)

    def populate_secondary_namespaces(self):
        """Populate secondary_namespaces for entities based on using-declarations"""
        # For now, this is a placeholder. We'll populate this when we parse
        # using-declarations like "using si::unit_symbols::cm;" in CGS
        # This will map cm as a secondary namespace access point to centimetre
        pass

    def _parse_core_framework(self):
        """Parse core framework entities (built-in units and quantities)"""
        # Create a synthetic "core" system (empty namespace = mp_units::)
        core_system = SystemInfo(namespace="")
        self.systems["core"] = core_system

        # Hardcode dimensionless quantity - it's a fundamental identity
        dimensionless_qty = Quantity(
            name="dimensionless",
            parent="",
            dimension="dimensionless",
            equation="",  # No equation for dimensionless
            namespace="mp_units",
            file="quantity_spec.h",
            is_kind=True,
            alias_target="",
            dimensional_formula="1",
            character="Real",
            kind_of="dimensionless",
            parent_from_cpp="<root>",
        )
        core_system.quantities.append(dimensionless_qty)

        # Hardcode dimension_one - the identity dimension for dimensionless quantities
        dimension_one_dim = Dimension(
            name="dimension_one",
            symbol="1",
            namespace="mp_units",
            file="dimension.h",
        )
        core_system.dimensions.append(dimension_one_dim)

        # Hardcode 'one' unit - it's a fundamental identity
        one_unit = Unit(
            name="one",
            symbol="1",
            definition="",  # No definition for one
            namespace="mp_units",
            file="unit.h",
            is_base=True,
        )
        core_system.units.append(one_unit)

        # Parse other units from unit.h using existing parsers
        unit_path = self.source_root / "src/core/include/mp-units/framework/unit.h"
        if unit_path.exists():
            try:
                content = unit_path.read_text()
                # Only parse content after "common dimensionless units" comment
                # and before "Common unit" comment to avoid parsing examples
                start_marker = content.find("// common dimensionless units")
                end_marker = content.find("// Common unit")
                if start_marker != -1 and end_marker != -1:
                    content = content[start_marker:end_marker]
                    # Parse units at mp_units namespace level (no sub-namespace)
                    self._parse_units(
                        content, core_system, str(unit_path), namespace_to_search=None
                    )
                    self._parse_unit_symbols(content, core_system)
                    self._parse_aliases(content, core_system, str(unit_path))
            except Exception as e:
                print(f"Warning: Could not parse {unit_path}: {e}")
            # Add core.h as the public header (not unit.h which is internal)
            core_header = self.source_root / "src/core/include/mp-units/core.h"
            core_system.files.append(core_header)

    def parse_system_with_includes(self, main_header: Path):
        """Parse a system header and all its includes in order"""
        content = main_header.read_text()

        # Extract includes from this header
        include_pattern = r"#include\s+<mp-units/systems/([^>]+)>"
        includes = []

        for match in re.finditer(include_pattern, content):
            include_path = match.group(1)
            # Resolve relative to systems directory (already at mp-units/systems/)
            included_file = self.systems_dir / include_path
            if included_file.exists():
                includes.append(included_file)

        # Parse all included files first (in order)
        for included in includes:
            self.parse_system_with_includes(included)

        # Parse the main file
        self.parse_system_header(main_header)

        # If this is an umbrella header (has includes but didn't establish its own namespace),
        # add it to the system determined by its included files
        if includes:
            # Check if main_header established its own namespace
            namespace_match = re.search(r"namespace\s+mp_units::(\w+)", content)
            if not namespace_match:
                namespace_match = re.search(
                    r"namespace\s+mp_units\s*\{[^}]*namespace\s+(\w+)", content
                )

            # If no namespace found, this is an umbrella header
            if not namespace_match:
                # Find which system the included files belong to
                for included in includes:
                    for system in self.systems.values():
                        if included in system.files and main_header not in system.files:
                            system.files.insert(
                                0, main_header
                            )  # Add as first entry (primary header)
                            break
                    break  # Only need to check first included file

    def parse_system_header(self, header_file: Path):
        """Parse a single system header file"""
        # Skip if already parsed
        if header_file in self.parsed_files:
            return

        self.parsed_files.add(header_file)
        content = header_file.read_text()

        # Extract namespace
        namespace_match = re.search(r"namespace\s+mp_units::(\w+)", content)
        if not namespace_match:
            namespace_match = re.search(
                r"namespace\s+mp_units\s*\{[^}]*namespace\s+(\w+)", content
            )

        if not namespace_match:
            return

        namespace = namespace_match.group(1)

        # Get or create system info
        if namespace not in self.systems:
            self.systems[namespace] = SystemInfo(namespace=namespace)

        system = self.systems[namespace]
        if header_file not in system.files:
            system.files.append(header_file)

        # Parse content
        self._parse_dimensions(content, system, str(header_file))
        self._parse_quantities(content, system, str(header_file))
        self._parse_units(content, system, str(header_file))
        self._parse_point_origins(content, system, str(header_file))
        self._parse_prefixes(content, system, str(header_file))
        self._parse_aliases(content, system, str(header_file))
        # Parse unit_symbols in the same pass
        self._parse_unit_symbols(content, system)
        # Detect inline subnamespaces
        self._detect_inline_subnamespaces(content, system)

    def _detect_inline_subnamespaces(self, content: str, system: SystemInfo):
        """Detect and store inline subnamespaces"""
        # Pattern: inline namespace NAME {
        inline_ns_pattern = r"inline\s+namespace\s+(\w+)\s*\{"
        for match in re.finditer(inline_ns_pattern, content):
            subns_name = match.group(1)
            if subns_name not in ["unit_symbols"]:  # Skip unit_symbols
                system.inline_subnamespaces.add(subns_name)

    def _is_in_namespace(self, content: str, pos: int, namespace: str) -> bool:
        """Check if position is inside the specified namespace (not in a nested namespace)"""
        if not namespace:
            return True  # No namespace filtering

        before_pos = content[:pos]

        # Find the last namespace declaration before this position
        # Handle both "namespace astronomy {" and "namespace mp_units::astronomy {"
        namespace_pattern = rf"namespace\s+(?:mp_units::)?{re.escape(namespace)}\s*{{"
        last_match = None
        for match in re.finditer(namespace_pattern, before_pos):
            last_match = match

        if not last_match:
            return False  # Not in the namespace at all

        # Check if we're still inside that namespace (not closed yet)
        section_after = before_pos[last_match.end() :]
        open_braces = (
            section_after.count("{") + 1
        )  # +1 for the opening brace of namespace
        close_braces = section_after.count("}")

        return open_braces > close_braces

    def _detect_origin_namespace(
        self, content: str, match_pos: int, system_namespace: str
    ) -> Optional[str]:
        """Detect the origin namespace path (e.g., 'mp_units::non_si' or 'mp_units::si::si2019')"""
        before_match = content[:match_pos]

        # Track ALL namespaces
        namespace_stack = []
        brace_depth = 0

        i = 0
        while i < len(before_match):
            char = before_match[i]

            if char == "{":
                brace_depth += 1
            elif char == "}":
                brace_depth -= 1
                while namespace_stack and brace_depth < namespace_stack[-1][1]:
                    namespace_stack.pop()

            if before_match[i : i + 6] == "inline":
                # Check if this is "inline namespace"
                rest = before_match[i + 6 :]
                inline_ns_match = re.match(r"\s+namespace\s+(\S+)\s*\{", rest)
                if inline_ns_match:
                    ns_name = inline_ns_match.group(1)
                    # Track inline namespaces except unit_symbols
                    if ns_name not in ["unit_symbols"]:
                        # The namespace will be at the NEXT brace depth (after its opening brace)
                        target_depth = brace_depth + 1
                        # Handle qualified names like "mp_units::si" - add each part
                        if "::" in ns_name:
                            for part in ns_name.split("::"):
                                namespace_stack.append((part, target_depth))
                        else:
                            namespace_stack.append((ns_name, target_depth))
                    # Skip past "inline namespace name {" and manually increment brace depth
                    i += (
                        6 + len(inline_ns_match.group(0)) - 1
                    )  # -1 because we want to land on the {
                    brace_depth += 1  # Account for the opening brace
                    i += 1  # Move past the {
                    continue
            elif before_match[i : i + 9] == "namespace":
                rest = before_match[i + 9 :]
                # Match "namespace name {" - name can contain ::
                ns_match = re.match(r"\s+(\S+)\s*\{", rest)
                if ns_match:
                    ns_name = ns_match.group(1)
                    # Track all namespaces except unit_symbols
                    if ns_name not in ["unit_symbols"]:
                        # The namespace will be at the NEXT brace depth (after its opening brace)
                        target_depth = brace_depth + 1
                        # Handle qualified names like "mp_units::si" - add each part
                        if "::" in ns_name:
                            for part in ns_name.split("::"):
                                namespace_stack.append((part, target_depth))
                        else:
                            namespace_stack.append((ns_name, target_depth))
                    # Skip past "namespace name {" and manually increment brace depth
                    i += (
                        9 + len(ns_match.group(0)) - 1
                    )  # -1 because we want to land on the {
                    brace_depth += 1  # Account for the opening brace
                    i += 1  # Move past the {
                    continue

            i += 1

        # Build the full namespace path
        if namespace_stack:
            ns_names = [ns[0] for ns in namespace_stack]

            # If we have more than one namespace level (system + subnamespace(s)), return full path
            # E.g., ["si", "si2019"] or ["usc", "survey1893"]
            if len(ns_names) > 1:
                return "::".join(ns_names)
            # Also return if we have special standalone namespaces like "non_si"
            elif ns_names and ns_names[0] in ["non_si"]:
                return "::".join(ns_names)

        return None

    def _get_nested_namespace(
        self, content: str, match_pos: int, system_namespace: str
    ) -> Optional[str]:
        """Detect if a match is inside a nested namespace and return the nested namespace name"""
        before_match = content[:match_pos]

        # Namespaces that should NOT be treated as subnamespaces for display purposes
        excluded_namespaces = {
            "mp_units",
            "unit_symbols",
            "non_si",
            "si2019",
            system_namespace,
        }

        # Track all namespace openings with their brace depth
        namespace_stack = []
        brace_depth = 0

        # Process character by character to accurately track braces
        i = 0
        while i < len(before_match):
            char = before_match[i]

            if char == "{":
                brace_depth += 1
            elif char == "}":
                brace_depth -= 1
                # Pop namespace when scope closes
                while namespace_stack and brace_depth <= namespace_stack[-1][1]:
                    namespace_stack.pop()

            # Check for namespace declaration (but skip inline namespaces)
            if before_match[i : i + 9] == "namespace":
                # Check if this is an inline namespace (look backwards for "inline" keyword)
                before_namespace = before_match[max(0, i - 20) : i]
                is_inline = bool(re.search(r"\binline\s+$", before_namespace))

                if not is_inline:
                    # Extract namespace name
                    rest = before_match[i + 9 :]
                    ns_match = re.match(r"\s+(\w+)\s*\{", rest)
                    if ns_match:
                        ns_name = ns_match.group(1)
                        # Skip excluded namespaces and namespace with ::
                        if ns_name not in excluded_namespaces and "::" not in ns_name:
                            namespace_stack.append((ns_name, brace_depth))

            i += 1

        # Return the innermost nested namespace if any
        if namespace_stack:
            return namespace_stack[-1][0]
        return None

    def _parse_dimensions(self, content: str, system: SystemInfo, file: str):
        """Parse dimension definitions"""
        dim_pattern = (
            r"inline\s+constexpr\s+struct\s+(dim_\w+)\s+final\s*:\s*"
            r'base_dimension<(?:"([^"]+)"|symbol_text\{[^}]+\})>\s*\{\}\s+\1\s*;'
        )

        for match in re.finditer(dim_pattern, content):
            dim_name = match.group(1)
            symbol = (
                match.group(2)
                if match.group(2)
                else self._extract_symbol_text(match.group(0))
            )

            dimension = Dimension(
                name=dim_name,
                symbol=symbol,
                namespace=f"mp_units::{system.namespace}",
                file=file,
            )
            system.dimensions.append(dimension)

    def _extract_symbol_text(self, text: str) -> str:
        """Extract symbol from symbol_text{u8"...", "..."}"""
        match = re.search(r'symbol_text\{u8"([^"]+)"', text)
        if match:
            return match.group(1)
        match = re.search(r'symbol_text\{[^,]+,\s*"([^"]+)"', text)
        if match:
            return match.group(1)
        return "?"

    def _parse_quantities(self, content: str, system: SystemInfo, file: str):
        """Parse QUANTITY_SPEC definitions"""
        # Use a simpler pattern and extract the full content manually
        for match in re.finditer(r"QUANTITY_SPEC\s*\(", content):
            # Check if this line is commented out
            line_start = content.rfind("\n", 0, match.start()) + 1
            line_prefix = content[line_start : match.start()].strip()
            if line_prefix.startswith("//"):
                continue

            start = match.end() - 1  # Position of opening '('
            depth = 1
            i = start + 1

            # Find matching closing parenthesis
            while i < len(content) and depth > 0:
                if content[i] == "(":
                    depth += 1
                elif content[i] == ")":
                    depth -= 1
                i += 1

            if depth != 0:
                continue

            # Extract the arguments
            args_str = content[start + 1 : i - 1]
            args = self._split_macro_args(args_str)

            if len(args) < 2:
                continue

            qty_name = args[0].strip()
            second_param = args[1].strip()
            third_param = args[2].strip() if len(args) > 2 else None
            has_is_kind_keyword = "is_kind" in args_str

            # Determine what the second parameter is:
            # - If it starts with 'dim_' → it's a dimension (and this is a kind/root)
            # - If it contains operators, function calls, or parentheses → it's an
            #   equation (and this is a kind/root if no parent)
            # - If it's a simple name (including 'dimensionless') → it's a parent quantity
            # Note: 'dimensionless' is a quantity (kind), not a dimension
            is_dimension = second_param.startswith("dim_")

            # Check for equations: operators, function calls (word followed by '('), or complex expressions
            has_operators = any(
                op in second_param
                for op in [
                    "*",
                    "/",
                    "+",
                    "-",
                    "pow<",
                    "sqrt",
                    "cbrt",
                    "square",
                    "cubic",
                ]
            )
            has_function_call = re.search(r"\w+\s*\(", second_param) is not None

            parent = None
            dimension = None
            equation = None
            is_kind = has_is_kind_keyword

            if is_dimension:
                dimension = second_param
                is_kind = (
                    True  # Having a dim_* dimension as parent makes this a kind/root
                )
                # Third param might be an equation
                if (
                    third_param
                    and third_param != "is_kind"
                    and not third_param.startswith("quantity_character::")
                ):
                    equation = third_param
            elif has_operators or has_function_call:
                # Second param is an equation
                equation = second_param
                # This is a kind/root only if explicitly marked with is_kind or has no parent
                if has_is_kind_keyword:
                    is_kind = True
                else:
                    # If it has an equation and no parent, it's implicitly a kind
                    is_kind = True
            else:
                # Second param is a parent quantity (including 'dimensionless')
                parent = second_param
                # Third param might be an equation
                if (
                    third_param
                    and third_param != "is_kind"
                    and not third_param.startswith("quantity_character::")
                ):
                    equation = third_param
                # is_kind remains as set by has_is_kind_keyword

            quantity = Quantity(
                name=qty_name,
                parent=parent,
                dimension=dimension,
                equation=equation,
                namespace=f"mp_units::{system.namespace}",
                file=file,
                is_kind=is_kind,
            )
            system.quantities.append(quantity)

    def _split_macro_args(self, args_str: str) -> List[str]:
        """Split macro arguments respecting nested brackets"""
        args = []
        current = []
        depth = 0
        angle_depth = 0

        for char in args_str:
            if char == "(" or char == "{":
                depth += 1
                current.append(char)
            elif char == ")" or char == "}":
                depth -= 1
                current.append(char)
            elif char == "<":
                angle_depth += 1
                current.append(char)
            elif char == ">":
                angle_depth -= 1
                current.append(char)
            elif char == "," and depth == 0 and angle_depth == 0:
                args.append("".join(current))
                current = []
            else:
                current.append(char)

        if current:
            args.append("".join(current))

        return args

    def _parse_units(
        self,
        content: str,
        system: SystemInfo,
        file: str,
        namespace_to_search: Optional[str] = "auto",
    ):
        """Parse unit definitions from content

        Args:
            content: The content to parse
            system: The system to add units to
            file: The file path
            namespace_to_search: Namespace to look for units in.
                                 "auto" (default) = use system.namespace
                                 None = parse at top level (no namespace filtering)
                                 string = specific namespace to search
        """
        # Determine which namespace to search for
        if namespace_to_search == "auto":
            namespace_to_search = system.namespace

        # Pattern 1a: inline constexpr struct NAME final : named_unit<"symbol", ...> {} NAME;
        unit_pattern_simple = (
            r"inline\s+constexpr\s+struct\s+(\w+)\s+final\s*:\s*"
            r'named_unit<"([^"]+)",\s*(.+?)>\s*\{\}\s*(\w+)\s*;'
        )

        # Pattern 1b: inline constexpr struct NAME final :
        # named_unit<symbol_text{u8"unicode", "ascii"}, ...> {} NAME;
        # Handle optional comments inside symbol_text like /* U+2030 PER MILLE SIGN */
        unit_pattern_text = (
            r"inline\s+constexpr\s+struct\s+(\w+)\s+final\s*:\s*"
            r'named_unit<symbol_text\{u8"([^"]+)"(?:\s*/\*[^*]*\*/)?\s*,\s*'
            r'"([^"]+)"\},\s*(.+?)>\s*\{\}\s*(\w+)\s*;'
        )

        # Parse units with symbol_text first
        for match in re.finditer(unit_pattern_text, content, re.DOTALL):
            unicode_symbol = match.group(2)
            ascii_symbol = match.group(3)
            definition_raw = match.group(4)
            var_name = match.group(5)

            # Combine both symbols for display, escaping backticks for markdown
            ascii_escaped = ascii_symbol.replace("`", "\\`")
            symbol = f"{unicode_symbol} ({ascii_escaped})"

            definition = self._extract_template_arg(definition_raw)

            # Skip if inside unit_symbols namespace
            match_pos = match.start()
            before_match = content[:match_pos]
            last_unit_symbols_pos = before_match.rfind("namespace unit_symbols")
            if last_unit_symbols_pos != -1:
                section_after = before_match[last_unit_symbols_pos:]
                open_braces = section_after.count("{")
                close_braces = section_after.count("}")
                if open_braces > close_braces:
                    continue

            # Check namespace filtering
            if namespace_to_search is not None:
                # We're looking for units in a specific namespace (including its subnamespaces)
                if not self._is_in_namespace(content, match_pos, namespace_to_search):
                    continue
            # If namespace_to_search is None, accept all units (no filtering)

            # Determine if base unit
            is_base = "kind_of<" in definition and not any(
                op in definition.split("kind_of<")[0]
                for op in ["*", "/", "+", "-", "pow", "square", "cubic"]
            )

            # Detect nested namespace
            nested_ns = self._get_nested_namespace(
                content,
                match_pos,
                system.namespace if namespace_to_search != None else "",
            )
            full_namespace = (
                f"mp_units::{system.namespace}::{nested_ns}"
                if nested_ns and system.namespace
                else f"mp_units::{system.namespace}" if system.namespace else "mp_units"
            )

            # Detect origin namespace
            origin_ns = self._detect_origin_namespace(
                content, match_pos, system.namespace
            )

            unit = Unit(
                name=var_name,
                symbol=symbol,
                definition=definition,
                namespace=full_namespace,
                file=file,
                is_base=is_base,
                subnamespace=nested_ns,
                origin_namespace=origin_ns,
            )
            system.units.append(unit)

        # Parse units with simple string symbols
        for match in re.finditer(unit_pattern_simple, content, re.DOTALL):
            symbol = match.group(2)
            definition_raw = match.group(3)
            var_name = match.group(4)

            definition = self._extract_template_arg(definition_raw)

            # Skip if inside unit_symbols namespace
            match_pos = match.start()
            before_match = content[:match_pos]
            # Check if we're inside unit_symbols namespace by counting braces
            last_unit_symbols_pos = before_match.rfind("namespace unit_symbols")
            if last_unit_symbols_pos != -1:
                # Count opening and closing braces after the last unit_symbols namespace declaration
                section_after = before_match[last_unit_symbols_pos:]
                open_braces = section_after.count("{")
                close_braces = section_after.count("}")
                if open_braces > close_braces:  # Still inside unit_symbols namespace
                    continue

            # Check namespace filtering
            if namespace_to_search is not None:
                # We're looking for units in a specific namespace (including its subnamespaces)
                if not self._is_in_namespace(content, match_pos, namespace_to_search):
                    continue
            # If namespace_to_search is None, accept all units (no filtering)

            # Determine if base unit (kind_of<> with no equation before it)
            is_base = "kind_of<" in definition and not any(
                op in definition.split("kind_of<")[0]
                for op in ["*", "/", "+", "-", "pow", "square", "cubic"]
            )

            # Detect nested namespace
            nested_ns = self._get_nested_namespace(
                content,
                match_pos,
                system.namespace if namespace_to_search != None else "",
            )
            full_namespace = (
                f"mp_units::{system.namespace}::{nested_ns}"
                if nested_ns and system.namespace
                else f"mp_units::{system.namespace}" if system.namespace else "mp_units"
            )

            # Detect origin namespace (e.g., non_si)
            origin_ns = self._detect_origin_namespace(
                content, match_pos, system.namespace
            )

            unit = Unit(
                name=var_name,
                symbol=symbol,
                definition=definition,
                namespace=full_namespace,
                file=file,
                is_base=is_base,
                subnamespace=nested_ns,
                origin_namespace=origin_ns,
            )
            system.units.append(unit)

        # Pattern 2: inline constexpr auto NAME = expression; (not in unit_symbols)
        auto_pattern = r"inline\s+constexpr\s+auto\s+(\w+)\s*=\s*([^;]+);"

        # Skip unit_symbols namespace for auto patterns - split on namespace declaration, not include
        unit_symbols_ns_match = re.search(r"namespace\s+.*unit_symbols", content)
        if unit_symbols_ns_match:
            main_content = content[: unit_symbols_ns_match.start()]
        else:
            main_content = content

        for match in re.finditer(auto_pattern, main_content):
            var_name = match.group(1)
            definition = match.group(2).strip()

            # Skip if it looks like a constant (handled separately)
            if any(
                keyword in var_name
                for keyword in ["constant", "speed", "mass", "charge"]
            ):
                continue

            # Skip simple aliases for now - they'll be processed later
            if "<" not in definition:
                continue

            # Detect nested namespace
            match_pos = match.start()
            nested_ns = self._get_nested_namespace(
                main_content, match_pos, system.namespace
            )
            full_namespace = (
                f"mp_units::{system.namespace}::{nested_ns}"
                if nested_ns
                else f"mp_units::{system.namespace}"
            )

            # Detect origin namespace (e.g., non_si)
            origin_ns = self._detect_origin_namespace(
                main_content, match_pos, system.namespace
            )

            # This is a unit definition
            unit = Unit(
                name=var_name,
                symbol="",  # Will try to determine from definition
                definition=definition,
                namespace=full_namespace,
                file=file,
                is_base=False,
                subnamespace=nested_ns,
                origin_namespace=origin_ns,
            )
            system.units.append(unit)

    def _extract_template_arg(self, text: str) -> str:
        """Extract template argument by balancing angle brackets"""
        depth = 0
        result = []
        for char in text:
            if char == "<":
                depth += 1
                result.append(char)
            elif char == ">":
                if depth > 0:
                    depth -= 1
                    result.append(char)
                else:
                    break
            else:
                result.append(char)
        return "".join(result).strip()

    def _parse_point_origins(self, content: str, system: SystemInfo, file: str):
        """Parse point origin definitions"""
        # Pattern for absolute and relative point origins:
        # inline constexpr struct NAME final : absolute_point_origin<...> {} NAME;
        # inline constexpr struct NAME final : relative_point_origin<...> {} NAME;

        origin_pattern = (
            r"inline\s+constexpr\s+struct\s+(\w+)\s+final\s*:\s*"
            r"(absolute|relative)_point_origin<(.+?)>\s*\{\}\s*(\w+)\s*;"
        )

        for match in re.finditer(origin_pattern, content, re.DOTALL):
            origin_type = match.group(2)  # 'absolute' or 'relative'
            template_param_raw = match.group(3)
            var_name = match.group(4)

            # Extract template parameter properly (handle nested angle brackets)
            template_param = self._extract_template_arg(template_param_raw)

            # Skip if inside unit_symbols namespace
            match_pos = match.start()
            before_match = content[:match_pos]
            last_unit_symbols_pos = before_match.rfind("namespace unit_symbols")
            if last_unit_symbols_pos != -1:
                section_after = before_match[last_unit_symbols_pos:]
                open_braces = section_after.count("{")
                close_braces = section_after.count("}")
                if open_braces > close_braces:
                    continue

            point_origin = PointOrigin(
                name=var_name,
                origin_type=origin_type,
                definition=template_param,
                namespace=f"mp_units::{system.namespace}",
                file=file,
            )
            system.point_origins.append(point_origin)

    def _parse_prefixes(self, content: str, system: SystemInfo, file: str):
        """Parse prefix template definitions (two-line pattern: class template + variable template)"""
        # Pattern for class template: template<PrefixableUnit U> struct NAME_ final :
        #   prefixed_unit<...> {};
        # Followed by: template<PrefixableUnit auto U> constexpr NAME_<...> NAME;

        # First, find all class template definitions
        class_pattern = (
            r"template<PrefixableUnit U>\s+struct\s+(\w+)_\s+final\s*:\s*"
            r'prefixed_unit<(?:symbol_text\{u8"([^"]+)"\s*,\s*"([^"]+)"\}'
            r'|"([^"]+)"),\s*(.+?),\s*U\{\}>\s*\{\}\s*;'
        )

        # Store class template info: name -> (symbol, definition)
        class_templates = {}

        for match in re.finditer(class_pattern, content, re.DOTALL):
            class_name = match.group(1)  # e.g., "kilo"
            unicode_symbol = match.group(2)  # If symbol_text
            ascii_symbol = match.group(3)  # If symbol_text
            simple_symbol = match.group(4)  # If simple string
            definition_raw = match.group(5)

            # Extract clean definition (magnitude)
            definition = self._extract_template_arg(definition_raw.strip())

            # Combine symbols if symbol_text was used
            if unicode_symbol and ascii_symbol:
                ascii_escaped = ascii_symbol.replace("`", "\\`")
                symbol = f"{unicode_symbol} ({ascii_escaped})"
            else:
                symbol = simple_symbol

            class_templates[class_name] = (symbol, definition)

        # Now find variable template definitions and match with class templates
        var_pattern = (
            r"template<PrefixableUnit auto U>\s+constexpr\s+(\w+)_<[^>]+>\s+(\w+)\s*;"
        )

        for match in re.finditer(var_pattern, content):
            class_ref = match.group(1)  # e.g., "kilo"
            var_name = match.group(2)  # e.g., "kilo" (the user-facing name)

            # Look up the class template
            if class_ref in class_templates:
                symbol, definition = class_templates[class_ref]

                prefix = Prefix(
                    name=var_name,
                    symbol=symbol,
                    definition=definition,
                    namespace=f"mp_units::{system.namespace}",
                    file=file,
                )
                system.prefixes.append(prefix)

        # Also handle chrono_point_origin template pattern:
        # template<typename C> struct chrono_point_origin_ final :
        #   absolute_point_origin<isq::time> { using clock = C; };
        # template<typename C> constexpr chrono_point_origin_<C> chrono_point_origin;

        # First, find the class template
        chrono_class_pattern = (
            r"template<typename C>\s+struct\s+chrono_point_origin_\s+final\s*:\s*"
            r"absolute_point_origin<(.+?)>\s*\{[^}]*\}\s*;"
        )
        chrono_class_match = re.search(chrono_class_pattern, content, re.DOTALL)

        if chrono_class_match:
            template_param = self._extract_template_arg(chrono_class_match.group(1))

            # Now find the variable template
            chrono_var_pattern = (
                r"template<typename C>\s+constexpr\s+"
                r"chrono_point_origin_<C>\s+(\w+)\s*;"
            )
            chrono_var_match = re.search(chrono_var_pattern, content)

            if chrono_var_match:
                var_name = chrono_var_match.group(1)

                # Add it as a point origin (not a prefix)
                point_origin = PointOrigin(
                    name=var_name,
                    origin_type="absolute",
                    definition=template_param,
                    namespace=f"mp_units::{system.namespace}",
                    file=file,
                )
                system.point_origins.append(point_origin)

    def _parse_aliases(self, content: str, system: SystemInfo, file: str):
        """Parse alias assignments and add them to the appropriate entity collection"""
        # Pattern: inline constexpr auto NAME = other_name;
        # Support Unicode identifiers (e.g., π) and comments between name and =
        alias_pattern = r"inline\s+constexpr\s+auto\s+([\w\u0080-\uFFFF]+)\s*(?:/\*.*?\*/)?\s*=\s*([^;]+);"

        # Skip unit_symbols namespace
        unit_symbols_ns_match = re.search(r"namespace\s+.*unit_symbols", content)
        if unit_symbols_ns_match:
            main_content = content[: unit_symbols_ns_match.start()]
        else:
            main_content = content

        for match in re.finditer(alias_pattern, main_content):
            alias_name = match.group(1)
            target_name = match.group(2).strip()

            # Skip if it has angle brackets (it's a unit definition, not an alias)
            if "<" in target_name:
                continue

            # Check if target has namespace qualification
            if "::" in target_name:
                # Qualified name - look only in specified system
                target_sys_name = target_name.split("::")[0]
                target_lookup = target_name.split("::")[-1]
                search_systems = (
                    [self.systems.get(target_sys_name)]
                    if target_sys_name in self.systems
                    else []
                )
            else:
                # Unqualified name - search current system first, then others
                target_lookup = target_name
                search_systems = [system] + [
                    s for s in self.systems.values() if s != system
                ]

            # Check if target is a point origin
            target_origin = None
            for search_system in search_systems:
                if search_system is None:
                    continue
                for origin in search_system.point_origins:
                    if origin.name == target_lookup:
                        target_origin = origin
                        break
                if target_origin:
                    break

            if target_origin:
                # Determine the display name for alias_target
                if "::" in target_name:
                    # Qualified name in source - use it as-is
                    alias_target_display = target_name
                else:
                    # Unqualified name - strip namespace if same system
                    target_system_name = target_origin.namespace.replace(
                        "mp_units::", ""
                    )
                    if target_system_name == system.namespace:
                        alias_target_display = target_origin.name
                    else:
                        alias_target_display = (
                            f"{target_system_name}::{target_origin.name}"
                        )

                # Add as an alias point origin (use current system's namespace, not target's)
                alias_origin = PointOrigin(
                    name=alias_name,
                    origin_type=target_origin.origin_type,
                    definition=target_origin.definition,
                    namespace=f"mp_units::{system.namespace}",
                    file=file,
                    alias_target=alias_target_display,
                )
                system.point_origins.append(alias_origin)
                continue

            # Check if target is a quantity
            target_quantity = None
            for search_system in search_systems:
                if search_system is None:
                    continue
                for qty in search_system.quantities:
                    if qty.name == target_lookup:
                        target_quantity = qty
                        break
                if target_quantity:
                    break

            if target_quantity:
                # Determine the display name for alias_target
                if "::" in target_name:
                    # Qualified name in source - use it as-is
                    alias_target_display = target_name
                else:
                    # Unqualified name - strip namespace if same system
                    target_system_name = target_quantity.namespace.replace(
                        "mp_units::", ""
                    )
                    if target_system_name == system.namespace:
                        alias_target_display = target_quantity.name
                    else:
                        alias_target_display = (
                            f"{target_system_name}::{target_quantity.name}"
                        )

                # Add as an alias quantity
                alias_quantity = Quantity(
                    name=alias_name,
                    parent=target_quantity.parent,
                    dimension=target_quantity.dimension,
                    equation=target_quantity.equation,
                    namespace=f"mp_units::{system.namespace}",
                    file=file,
                    is_kind=target_quantity.is_kind,
                    alias_target=alias_target_display,
                )
                system.quantities.append(alias_quantity)
                continue

            # Check if target is a unit
            target_unit = None
            for search_system in search_systems:
                if search_system is None:
                    continue
                for unit in search_system.units:
                    if unit.name == target_lookup:
                        target_unit = unit
                        break
                if target_unit:
                    break

            if target_unit:
                # Determine the display name for alias_target
                if "::" in target_name:
                    # Qualified name in source - use it as-is
                    alias_target_display = target_name
                else:
                    # Unqualified name - strip namespace if same system
                    target_system_name = target_unit.namespace.replace("mp_units::", "")
                    if target_system_name == system.namespace:
                        alias_target_display = target_unit.name
                    else:
                        alias_target_display = (
                            f"{target_system_name}::{target_unit.name}"
                        )

                # Add as an alias unit (use current system's namespace, not target's)
                # Don't copy origin_namespace/subnamespace - the alias is defined in the current system
                alias_unit = Unit(
                    name=alias_name,
                    symbol=target_unit.symbol,
                    definition=target_unit.definition,
                    namespace=f"mp_units::{system.namespace}",
                    file=file,
                    is_base=target_unit.is_base,
                    subnamespace=None,  # Alias is in current system, not a subnamespace
                    origin_namespace=None,  # Alias is defined here, not inherited from target
                    alias_target=alias_target_display,
                )
                system.units.append(alias_unit)

    def _parse_unit_symbols(self, content: str, system: SystemInfo):
        """Parse unit_symbols namespace for short aliases and using declarations"""
        # Find ALL unit_symbols namespace blocks (there can be multiple)
        # Pattern matches: inline constexpr auto NAME = UNIT_REF;
        # But NOT compound expressions like: inline constexpr auto mph = mile / si::hour;
        # Allow qualified names like si::ohm
        symbol_pattern = r"inline\s+constexpr\s+auto\s+(\w+)\s*=\s*([\w:]+)\s*;"
        # Pattern for using declarations: using namespace::name;
        using_pattern = r"using\s+([\w:]+)\s*;"

        for unit_symbols_match in re.finditer(
            r"namespace\s+(?:[\w:]+::)?unit_symbols\s*\{(.*?)\}", content, re.DOTALL
        ):
            symbols_content = unit_symbols_match.group(1)

            # Parse inline constexpr assignments
            for match in re.finditer(symbol_pattern, symbols_content):
                symbol_name = match.group(1)
                unit_ref = match.group(2)

                # Extract just the unit name (strip namespace if present)
                unit_name = unit_ref.split("::")[-1]

                for unit in system.units:
                    if unit.name == unit_name:
                        if symbol_name not in unit.unit_symbols:
                            unit.unit_symbols.append(symbol_name)
                        break

            # Parse using declarations (e.g., using si::unit_symbols::cm;)
            for match in re.finditer(using_pattern, symbols_content):
                qualified_name = match.group(1)
                parts = qualified_name.split("::")

                # Check if this is a using declaration from another system's unit_symbols
                if len(parts) >= 2 and parts[-2] == "unit_symbols":
                    # This is like: using si::unit_symbols::cm;
                    symbol_name = parts[-1]  # e.g., "cm"
                    source_namespace = parts[0]  # e.g., "si"

                    # Find the definition of this symbol in the source system's unit_symbols
                    source_system = self.systems.get(source_namespace)
                    if source_system:
                        # Look for this symbol's definition in the source system's files
                        symbol_definition = None
                        for file in source_system.files:
                            if file.exists():
                                file_content = file.read_text()
                                # Match: inline constexpr auto cm = centi<metre>;
                                symbol_def_pattern = rf"inline\s+constexpr\s+auto\s+{re.escape(symbol_name)}\s*=\s*([\w<>:]+)\s*;"
                                symbol_def_match = re.search(
                                    symbol_def_pattern, file_content
                                )
                                if symbol_def_match:
                                    symbol_definition = symbol_def_match.group(
                                        1
                                    )  # e.g., "centi<metre>"
                                    break

                        if symbol_definition:
                            # Now find the unit in the current system that matches this definition
                            # Strip namespace prefixes from definition for matching
                            clean_definition = re.sub(
                                r"\b[\w]+::", "", symbol_definition
                            )

                            for unit in system.units:
                                # Clean the unit definition too
                                clean_unit_def = re.sub(
                                    r"\b[\w]+::", "", unit.definition
                                )
                                if (
                                    clean_unit_def == clean_definition
                                    or unit.definition == symbol_definition
                                ):
                                    if symbol_name not in unit.unit_symbols:
                                        unit.unit_symbols.append(symbol_name)
                                    break
                else:
                    # Simple using declaration like: using si::ohm;
                    unit_name = parts[-1]
                    for unit in system.units:
                        if unit.name == unit_name:
                            if unit_name not in unit.unit_symbols:
                                unit.unit_symbols.append(unit_name)
                            break

    def _parse_using_declarations(self, content: str, system: SystemInfo, file: str):
        """Parse using declarations for imported units (excluding math functions)"""
        using_pattern = r"using\s+([\w:]+)::([\w]+)\s*;"

        for match in re.finditer(using_pattern, content):
            full_namespace = match.group(1)
            unit_name = match.group(2)

            # Skip if importing from std namespace (likely functions)
            if "std" in full_namespace:
                continue

            unit = Unit(
                name=unit_name,
                symbol=f"(imported from {full_namespace})",
                definition=f"using {full_namespace}::{unit_name}",
                namespace=f"mp_units::{system.namespace}",
                file=file,
                is_alias=True,
            )
            system.units.append(unit)


class DocumentationGenerator:
    """Generates markdown documentation from parsed systems"""

    def __init__(self, parser: SystemsParser, output_dir: Path):
        self.parser = parser
        self.output_dir = output_dir
        self.output_dir.mkdir(parents=True, exist_ok=True)

    @staticmethod
    def _write_auto_generated_header(f):
        """Write auto-generation warning header to a file"""
        f.write("<!-- This file is auto-generated. Do not edit manually. -->\n")
        f.write("<!-- Run: python3 scripts/systems_reference.py --force -->\n\n")

    @staticmethod
    def _get_prefix_magnitude(prefix: Prefix) -> float:
        """Extract magnitude from prefix definition for sorting.

        Parses definitions like 'mag_power<10, 3>' to get 10^3 = 1000.
        Returns 1.0 if parsing fails.
        """
        import re

        match = re.search(r"mag_power<\s*(\d+)\s*,\s*(-?\d+)\s*>", prefix.definition)
        if match:
            base = int(match.group(1))
            exponent = int(match.group(2))
            return base**exponent
        return 1.0

    @staticmethod
    def _get_system_display_name(namespace: str) -> str:
        """Get display name for a system namespace."""
        if namespace == "isq_angle":
            return "ISQ Angle"
        elif namespace in ["cgs", "hep", "iau", "iec", "iec80000", "isq", "si", "usc"]:
            return namespace.upper()
        else:
            return namespace.replace("_", " ").title()

    def update_mkdocs_config(self, mkdocs_path: Path):
        """Update mkdocs.yml with generated pages"""
        if not mkdocs_path.exists():
            print(f"Warning: mkdocs.yml not found at {mkdocs_path}")
            return

        try:
            content = mkdocs_path.read_text()
            ref_start = content.find("  - Reference:")
            if ref_start == -1:
                print("Warning: Could not find Reference section in mkdocs.yml")
                return

            ref_end = content.find("\n  - ", ref_start + 1)
            if ref_end == -1:
                ref_end = len(content)

            systems_ref_lines = self._build_systems_reference_yaml()
            systems_ref_start = content.find(
                "      - Systems Reference:", ref_start, ref_end
            )

            if systems_ref_start != -1:
                next_item = ref_end
                for match in re.finditer(
                    r"\n      - [A-Z]", content[systems_ref_start + 1 : ref_end]
                ):
                    next_item = systems_ref_start + 1 + match.start()
                    break

                new_content = (
                    content[:systems_ref_start]
                    + systems_ref_lines.rstrip("\n")
                    + content[next_item:]
                )
            else:
                insert_pos = content.find(
                    "      - Supported Systems Overview:", ref_start, ref_end
                )
                if insert_pos == -1:
                    insert_pos = content.find(
                        "      - Cheat Sheet:", ref_start, ref_end
                    )

                if insert_pos != -1:
                    line_end = content.find("\n", insert_pos)
                    new_content = (
                        content[: line_end + 1]
                        + systems_ref_lines
                        + content[line_end + 1 :]
                    )
                else:
                    print("Warning: Could not find insertion point in mkdocs.yml")
                    return

            mkdocs_path.write_text(new_content)
            print("  ✓ Updated mkdocs.yml with generated pages")

        except Exception as e:
            print(f"Warning: Could not update mkdocs.yml: {e}")

    def _build_systems_reference_yaml(self):
        """Build the YAML text for Systems Reference section"""
        lines = ["      - Systems Reference:\n"]
        lines.append("          - Overview: reference/systems_reference/index.md\n")
        lines.append(
            "          - Dimensions: reference/systems_reference/dimensions_index.md\n"
        )
        lines.append(
            "          - Quantities: reference/systems_reference/quantities_index.md\n"
        )
        lines.append("          - Units: reference/systems_reference/units_index.md\n")
        lines.append(
            "          - Prefixes: reference/systems_reference/prefixes_index.md\n"
        )
        lines.append(
            "          - Point Origins: reference/systems_reference/point_origins_index.md\n"
        )
        lines.append("          - Systems:\n")

        for namespace in sorted(self.parser.systems.keys()):
            system = self.parser.systems[namespace]
            if not (system.units or system.dimensions or system.quantities):
                continue

            display_name = self._get_system_display_name(namespace)
            lines.append(
                f"              - {display_name}: reference/systems_reference/systems/{namespace}.md\n"
            )

        # Add Quantity Hierarchies section
        lines.append("          - Quantity Hierarchies:\n")
        lines.append(
            "              - Overview: reference/systems_reference/hierarchies/index.md\n"
        )

        # Collect all root quantities with their systems
        root_to_systems = defaultdict(list)  # root_name -> [system_namespaces]
        for namespace in sorted(self.parser.systems.keys()):
            system = self.parser.systems[namespace]
            if system.quantities:
                for qty in system.quantities:
                    # Check if root using C++ extracted parent
                    is_root = (
                        hasattr(qty, "parent_from_cpp")
                        and qty.parent_from_cpp == "<root>"
                    )
                    if is_root:
                        root_to_systems[qty.name].append(namespace)

        # Add hierarchy pages with system names if multiple systems have same root
        for root_name in sorted(root_to_systems.keys()):
            systems = root_to_systems[root_name]
            # dimensionless is always cross-system
            if root_name == "dimensionless":
                lines.append(
                    f"              - {root_name}: reference/systems_reference/hierarchies/{root_name}.md\n"
                )
            elif len(systems) == 1:
                lines.append(
                    f"              - {root_name}: reference/systems_reference/hierarchies/{root_name}.md\n"
                )
            else:
                # Multiple systems: add with system name suffix in TOC
                for ns in sorted(systems):
                    lines.append(
                        f"              - {root_name} ({ns}): "
                        f"reference/systems_reference/hierarchies/{root_name}_{ns}.md\n"
                    )

        return "".join(lines)

    def generate_systems_index(self):
        """Generate the main systems index page"""
        output_file = self.output_dir / "index.md"

        with open(output_file, "w") as f:
            self._write_auto_generated_header(f)
            f.write("# Systems Reference\n\n")
            f.write(
                "Automatically generated reference documentation "
                "for all **mp-units** systems.\n\n"
            )
            f.write("## Indexes\n\n")
            f.write("- [Dimensions](dimensions_index.md) - All base dimensions\n")
            f.write("- [Quantities](quantities_index.md) - All quantities\n")
            f.write("- [Units](units_index.md) - All units\n")
            f.write("- [Prefixes](prefixes_index.md) - All prefixes\n")
            f.write("- [Point Origins](point_origins_index.md) - All point origins\n\n")
            f.write("## Systems\n\n")

            # Write table header
            f.write(
                "| System | Dimensions | Quantities | Units | Prefixes | Point Origins |\n"
            )
            f.write(
                "|--------|:----------:|:----------:|:-----:|:--------:|:-------------:|\n"
            )

            for namespace in sorted(self.parser.systems.keys()):
                system = self.parser.systems[namespace]
                display = self._get_system_display_name(namespace)
                dims = len(system.dimensions)
                qtys = len(system.quantities)
                units = len(system.units)
                prefixes = len(system.prefixes)
                origins = len(system.point_origins)

                # Format counts with em-dash for zero
                dims_str = str(dims) if dims else "—"
                qtys_str = str(qtys) if qtys else "—"
                units_str = str(units) if units else "—"
                prefixes_str = str(prefixes) if prefixes else "—"
                origins_str = str(origins) if origins else "—"

                f.write(
                    f"| [{display}](systems/{namespace}.md) | {dims_str} | {qtys_str} | {units_str} | {prefixes_str} | {origins_str} |\n"
                )

    def generate_dimensions_index(self):
        """Generate alphabetical dimensions index"""
        output_file = self.output_dir / "dimensions_index.md"

        all_dimensions = []
        for sys_key, system in self.parser.systems.items():
            for dim in system.dimensions:
                display_ns = system.namespace if system.namespace else "mp_units"
                all_dimensions.append((dim.name, dim.symbol, display_ns, sys_key))

        with open(output_file, "w") as f:
            self._write_auto_generated_header(f)
            f.write("# Dimensions Index\n\n")
            f.write("Alphabetical list of all base dimensions.\n\n")

            # Sort by name first, then namespace
            for name, symbol, display_ns, sys_key in sorted(
                all_dimensions, key=lambda x: (x[0], x[2])
            ):
                f.write(f"- [{name} ({display_ns})](systems/{sys_key}.md#{name})\n")

            f.write(f"\n**Total dimensions:** {len(all_dimensions)}\n")

    def generate_quantities_index(self):
        """Generate alphabetical quantities index"""
        output_file = self.output_dir / "quantities_index.md"

        all_quantities = []
        for sys_key, system in self.parser.systems.items():
            for qty in system.quantities:
                # Use "mp_units" for empty namespace (core system)
                display_ns = system.namespace if system.namespace else "mp_units"
                all_quantities.append((qty.name, sys_key, display_ns))

        with open(output_file, "w") as f:
            self._write_auto_generated_header(f)
            f.write("# Quantities Index\n\n")
            f.write("Alphabetical list of all quantities.\n\n")

            # Sort by name first, then system key
            for name, sys_key, display_ns in sorted(
                all_quantities, key=lambda x: (x[0], x[1])
            ):
                f.write(f"- [`{name}` ({display_ns})](systems/{sys_key}.md#{name})\n")

            f.write(f"\n**Total quantities:** {len(all_quantities)}\n")

    def generate_units_index(self):
        """Generate alphabetical units index"""
        output_file = self.output_dir / "units_index.md"

        all_units = []

        for sys_key, system in self.parser.systems.items():
            for unit in system.units:
                # Use origin_namespace if it exists (strip mp_units:: for display)
                if unit.origin_namespace:
                    display_namespace = unit.origin_namespace.replace("mp_units::", "")
                else:
                    # Extract the namespace path after mp_units::
                    full_ns = unit.namespace.replace("mp_units::", "")
                    # If empty (core system), show "mp_units"
                    display_namespace = full_ns if full_ns else "mp_units"

                all_units.append((unit.name, sys_key, display_namespace, unit))

        with open(output_file, "w") as f:
            self._write_auto_generated_header(f)
            f.write("# Units Index\n\n")
            f.write("Alphabetical list of all units.\n\n")

            # Sort by unit name first, then full namespace
            for name, sys_key, full_ns, unit in sorted(
                all_units, key=lambda x: (x[0], x[2])
            ):
                # Determine anchor - include subnamespace prefix if present
                subns_prefix = None
                if unit.origin_namespace:
                    parts = unit.origin_namespace.replace("mp_units::", "").split("::")
                    if len(parts) > 1:
                        subns_prefix = parts[-1]
                elif unit.subnamespace:
                    subns_prefix = unit.subnamespace

                anchor = f"{subns_prefix}-{name}" if subns_prefix else name
                f.write(f"- [`{name}` ({full_ns})](systems/{sys_key}.md#{anchor})\n")

            f.write(f"\n**Total units:** {len(all_units)}\n")

    def generate_cross_system_hierarchies(self):
        """Generate quantity hierarchies (separate files when same root exists in multiple systems)
        Returns the number of hierarchy files generated.
        """
        hierarchies_dir = self.output_dir / "hierarchies"
        hierarchies_dir.mkdir(exist_ok=True)

        # Clean up old hierarchy files
        for old_file in hierarchies_dir.glob("*.md"):
            if old_file.name != "index.md":
                old_file.unlink()

        # Collect all root quantities by (name, namespace)
        root_to_systems = defaultdict(list)  # root_name -> [(system_namespace, system)]
        hierarchy_count = 0

        for namespace, system in self.parser.systems.items():
            if not system.quantities:
                continue

            # Check if this system has dimensionless children
            has_dimensionless_children = any(
                hasattr(q, "parent_from_cpp") and q.parent_from_cpp == "dimensionless"
                for q in system.quantities
            )

            if has_dimensionless_children:
                # Check if dimensionless root already exists in this system
                has_dimensionless_root = any(
                    q.name == "dimensionless"
                    and hasattr(q, "parent_from_cpp")
                    and q.parent_from_cpp == "<root>"
                    for q in system.quantities
                )

                if not has_dimensionless_root:
                    # Add synthetic dimensionless root
                    dimensionless_root = Quantity(
                        name="dimensionless",
                        parent="",
                        dimension="dimensionless",
                        equation="",
                        namespace=f"mp_units::{namespace}",
                        file="",
                        is_kind=True,
                        alias_target="",
                        dimensional_formula="1",
                        character="Real",
                        kind_of="dimensionless",
                        parent_from_cpp="<root>",
                    )
                    system.quantities.append(dimensionless_root)

            # Find roots - use C++ extracted parent only, skip aliases
            for qty in system.quantities:
                # Skip aliases - they should not be counted as roots
                if qty.alias_target:
                    continue

                is_root = False
                if hasattr(qty, "parent_from_cpp"):
                    is_root = (
                        qty.parent_from_cpp == "<root>" or qty.parent_from_cpp == ""
                    )

                if is_root:
                    root_to_systems[qty.name].append((namespace, system))

        # Decide how to generate files for each root name
        for root_name in sorted(root_to_systems.keys()):
            systems_info = root_to_systems[root_name]

            # Special case: dimensionless is always cross-system
            if root_name == "dimensionless":
                self._generate_hierarchy_file(
                    hierarchies_dir / "dimensionless.md",
                    root_name,
                    systems_info,
                    cross_system=True,
                )
                hierarchy_count += 1
            # If only one system has this root, create single file
            elif len(systems_info) == 1:
                namespace, system = systems_info[0]
                hierarchy_file = hierarchies_dir / f"{root_name}.md"
                self._generate_hierarchy_file(
                    hierarchy_file, root_name, [(namespace, system)], cross_system=False
                )
                hierarchy_count += 1
            # Multiple systems have roots with same name: create separate files for each
            else:
                for namespace, system in systems_info:
                    hierarchy_file = hierarchies_dir / f"{root_name}_{namespace}.md"
                    self._generate_hierarchy_file(
                        hierarchy_file,
                        root_name,
                        [(namespace, system)],
                        cross_system=False,
                    )
                    hierarchy_count += 1

        return hierarchy_count

    def _generate_hierarchy_file(
        self,
        hierarchy_file: Path,
        root_name: str,
        systems_info: list,
        cross_system: bool,
    ):
        """Generate a single hierarchy file"""
        # Collect all quantities from the specified systems
        all_quantities = []
        for namespace, system in systems_info:
            all_quantities.extend(system.quantities)

        # Get the root quantity from the first system (they should all be equivalent)
        first_namespace, first_system = systems_info[0]
        root_qty = next(
            (
                q
                for q in first_system.quantities
                if q.name == root_name
                and hasattr(q, "parent_from_cpp")
                and q.parent_from_cpp == "<root>"
            ),
            None,
        )

        if not root_qty:
            return

        with open(hierarchy_file, "w") as hf:
            self._write_auto_generated_header(hf)
            hf.write(f"# {root_name} Hierarchy\n\n")

            # List all systems contributing to this hierarchy
            system_names = []
            for ns, _ in systems_info:
                if ns == "isq_angle":
                    system_names.append("ISQ Angle")
                elif ns in ["cgs", "hep", "iau", "iec", "iec80000", "isq", "si", "usc"]:
                    system_names.append(ns.upper())
                else:
                    system_names.append(ns.replace("_", " ").title())

            if len(system_names) == 1:
                hf.write(f"**System:** {system_names[0]}\n\n")
            else:
                hf.write(f"**Systems:** {', '.join(system_names)}\n\n")

            if root_qty.dimension:
                hf.write(f"**Dimension:** {root_qty.dimension}\n\n")

            # Create qualified names for all quantities (namespace::name, but no prefix for dimensionless)
            qualified_quantities = []
            for qty in all_quantities:
                sys_ns = qty.namespace.replace("mp_units::", "")
                if qty.name == "dimensionless":
                    qualified_name = "dimensionless"
                else:
                    qualified_name = f"{sys_ns}::{qty.name}"
                qualified_quantities.append((qualified_name, qty))

            # Build children map using qualified names - use C++ extracted parent only
            qty_children = defaultdict(list)
            for qualified_name, qty in qualified_quantities:
                parent_name = None
                if (
                    hasattr(qty, "parent_from_cpp")
                    and qty.parent_from_cpp
                    and qty.parent_from_cpp != "<root>"
                ):
                    parent_name = qty.parent_from_cpp

                if parent_name:
                    # Get the qualified parent name
                    if parent_name == "dimensionless":
                        qualified_parent = "dimensionless"
                    else:
                        # C++ parent already includes namespace (e.g., 'isq::length')
                        qualified_parent = parent_name
                    qty_children[qualified_parent].append((qualified_name, qty))

            # Generate Mermaid diagram
            hf.write(
                self._build_mermaid_hierarchy(
                    root_name, qty_children, qualified_quantities
                )
            )
            hf.write("\n")

    def generate_hierarchies_overview(self):
        """Generate overview page for all quantity hierarchies"""
        hierarchies_dir = self.output_dir / "hierarchies"
        hierarchies_dir.mkdir(exist_ok=True)
        output_file = hierarchies_dir / "index.md"

        # Collect root quantities grouped by dimensional formula
        dimension_to_roots = defaultdict(
            list
        )  # dimensional_formula -> [qualified_name]

        # Also collect all root names globally to determine if namespace suffix is needed
        global_root_counts = defaultdict(set)  # root_name -> set of namespaces

        for namespace, system in self.parser.systems.items():
            if not system.quantities:
                continue

            for qty in system.quantities:
                # Check if root using C++ extracted parent
                is_root = (
                    hasattr(qty, "parent_from_cpp") and qty.parent_from_cpp == "<root>"
                )
                if is_root:
                    dim_formula = (
                        qty.dimensional_formula
                        if hasattr(qty, "dimensional_formula")
                        and qty.dimensional_formula
                        else "?"
                    )
                    # Create qualified name
                    if qty.name == "dimensionless":
                        qualified_name = "dimensionless"
                    else:
                        qualified_name = f"{namespace}::{qty.name}"
                    dimension_to_roots[dim_formula].append(qualified_name)
                    global_root_counts[qty.name].add(namespace)

        with open(output_file, "w") as f:
            self._write_auto_generated_header(f)
            f.write("# Quantity Hierarchies\n\n")
            f.write(
                "This section contains all quantity hierarchy trees across all systems, "
                "grouped by their dimensional formula to help identify dimensionally "
                "equivalent quantities.\n\n"
            )

            # Sort by: "1" first, then by string length (shortest to longest), then alphabetically, "?" last
            def dim_sort_key(dim_formula):
                if dim_formula == "1":
                    return (0, 0, dim_formula)  # First: dimensionless
                elif dim_formula == "?":
                    return (2, 0, dim_formula)  # Last: unknown
                else:
                    # Sort by length first (base quantities like L, M, T before derived like LT⁻¹)
                    # Then alphabetically for same length
                    return (1, len(dim_formula), dim_formula)

            sorted_dims = sorted(dimension_to_roots.keys(), key=dim_sort_key)
            for idx, dim_formula in enumerate(sorted_dims):
                roots = dimension_to_roots[dim_formula]
                if not roots:
                    continue

                f.write(f"## Dimension: {dim_formula}\n\n")

                # Sort roots and create bullet list with links
                for qualified_name in sorted(set(roots)):
                    # Extract root name and namespace for link
                    root_name = qualified_name.split("::")[-1]

                    # Determine link based on whether there are multiple systems with same root GLOBALLY
                    if root_name == "dimensionless":
                        link = f"[`{qualified_name}`]({root_name}.md)"
                    else:
                        # Check if multiple systems have this root name (globally across all dimensions)
                        if len(global_root_counts[root_name]) > 1:
                            # Multiple systems have this root - use namespace-specific file
                            namespace = qualified_name.split("::")[0]
                            link = f"[`{qualified_name}`]({root_name}_{namespace}.md)"
                        else:
                            link = f"[`{qualified_name}`]({root_name}.md)"

                    f.write(f"- {link}\n")

                # Add blank line between sections, but not after the last one
                if idx < len(sorted_dims) - 1:
                    f.write("\n")

    def generate_point_origins_index(self):
        """Generate alphabetical point origins index"""
        output_file = self.output_dir / "point_origins_index.md"

        all_origins = []
        for system in self.parser.systems.values():
            for origin in system.point_origins:
                # Use system.namespace for display since that's where it's documented
                all_origins.append((origin.name, system.namespace, origin))

        with open(output_file, "w") as f:
            self._write_auto_generated_header(f)
            f.write("# Point Origins Index\n\n")
            f.write("Alphabetical list of all point origins.\n\n")

            # Sort by origin name first, then system namespace
            for name, sys_ns, origin in sorted(all_origins, key=lambda x: (x[0], x[1])):
                f.write(f"- [`{name}` ({sys_ns})](systems/{sys_ns}.md#{name})\n")

            f.write(f"\n**Total point origins:** {len(all_origins)}\n")

    def generate_prefixes_index(self):
        """Generate alphabetical prefixes index"""
        output_file = self.output_dir / "prefixes_index.md"

        all_prefixes = []
        for system in self.parser.systems.values():
            for prefix in system.prefixes:
                # Use system.namespace for display since that's where it's documented
                all_prefixes.append((prefix.name, system.namespace, prefix))

        with open(output_file, "w") as f:
            self._write_auto_generated_header(f)
            f.write("# Prefixes Index\n\n")
            f.write("Alphabetical list of all prefixes.\n\n")

            # Sort by prefix name first, then system namespace
            for name, sys_ns, prefix in sorted(
                all_prefixes, key=lambda x: (x[0], x[1])
            ):
                f.write(f"- [`{name}` ({sys_ns})](systems/{sys_ns}.md#{name})\n")

            f.write(f"\n**Total prefixes:** {len(all_prefixes)}\n")

    def _compute_global_root_counts(self):
        """Compute which root names exist in multiple systems"""
        global_root_counts = defaultdict(set)  # root_name -> set of namespaces

        for namespace, system in self.parser.systems.items():
            for qty in system.quantities:
                is_root = (
                    hasattr(qty, "parent_from_cpp") and qty.parent_from_cpp == "<root>"
                )
                if is_root and not qty.alias_target:
                    global_root_counts[qty.name].add(namespace)

        return global_root_counts

    def _get_hierarchy_filename(self, root_name, system_namespace, global_root_counts):
        """Get the correct hierarchy filename for a root"""
        if root_name == "dimensionless":
            return f"{root_name}.md"
        elif len(global_root_counts[root_name]) > 1:
            # Multiple systems have this root - use namespace-specific file
            return f"{root_name}_{system_namespace}.md"
        else:
            # Only one system has this root - no namespace suffix
            return f"{root_name}.md"

    def generate_per_system_pages(self):
        """Generate individual pages for each system"""
        # Compute global root counts for hierarchy filename determination
        global_root_counts = self._compute_global_root_counts()

        # Create systems subdirectory
        systems_dir = self.output_dir / "systems"
        systems_dir.mkdir(parents=True, exist_ok=True)

        for namespace in sorted(self.parser.systems.keys()):
            system = self.parser.systems[namespace]

            # Create system file in systems subdirectory
            output_file = systems_dir / f"{namespace}.md"

            with open(output_file, "w") as f:
                self._write_auto_generated_header(f)
                # Special case for compound names like isq_angle
                if namespace == "isq_angle":
                    display_name = "ISQ Angle"
                elif namespace in [
                    "cgs",
                    "hep",
                    "iau",
                    "iec",
                    "iec80000",
                    "isq",
                    "si",
                    "usc",
                ]:
                    display_name = namespace.upper()
                else:
                    display_name = namespace.replace("_", " ").title()
                f.write(f"# {display_name} System\n\n")
                # Core system has empty namespace, meaning mp_units::
                if system.namespace:
                    f.write(f"**Namespace:** `mp_units::{system.namespace}`\n\n")
                else:
                    f.write("**Namespace:** `mp_units`\n\n")

                # Add Module line
                if system.namespace:
                    f.write("**Module:** `mp_units.systems`\n\n")
                else:
                    f.write("**Module:** `mp_units.core`\n\n")

                if system.files:
                    # Check if first file is a primary header (umbrella header at top level)
                    first_file = system.files[0]
                    first_file_str = str(first_file)
                    if "/include/" in first_file_str:
                        first_rel_path = first_file_str.split("/include/", 1)[1]
                    else:
                        first_rel_path = str(
                            first_file.relative_to(
                                first_file.parent.parent.parent.parent
                            )
                        )

                    # Check if it's a primary header (e.g., isq.h, si.h - no subdirectory)
                    is_primary = (
                        "/" not in first_rel_path.split("/systems/", 1)[1]
                        if "/systems/" in first_rel_path
                        else False
                    )

                    if is_primary:
                        # Show primary header inline (like namespace)
                        f.write(f"**Header:** `<{first_rel_path}>`\n")

                        # Show secondary headers if there are any
                        if len(system.files) > 1:
                            f.write("\n**Secondary Headers:**\n\n")
                            # Sort secondary headers alphabetically
                            secondary_headers = []
                            for file in system.files[1:]:
                                file_str = str(file)
                                if "/include/" in file_str:
                                    rel_path = file_str.split("/include/", 1)[1]
                                else:
                                    rel_path = file.relative_to(
                                        file.parent.parent.parent.parent
                                    )
                                secondary_headers.append(str(rel_path))
                            secondary_headers.sort()
                            for rel_path in secondary_headers:
                                f.write(f"- `<{rel_path}>`\n")
                    elif len(system.files) == 1:
                        # Single header, show inline
                        f.write(f"**Header:** `<{first_rel_path}>`\n")
                    else:
                        # Multiple headers at same level, show as list
                        f.write("**Headers:**\n\n")
                        for file in system.files:
                            file_str = str(file)
                            if "/include/" in file_str:
                                rel_path = file_str.split("/include/", 1)[1]
                            else:
                                rel_path = file.relative_to(
                                    file.parent.parent.parent.parent
                                )
                            f.write(f"- `<{rel_path}>`\n")

                # Track if we need a separator before next section
                need_separator = bool(system.files)

                # Dimensions
                if system.dimensions:
                    if need_separator:
                        f.write("\n")
                    f.write("## Dimensions\n\n")
                    f.write("| Name | Symbol |\n")
                    f.write("|------|:------:|\n")
                    for dim in sorted(system.dimensions, key=lambda d: d.name):
                        f.write(
                            f'| <span id="{dim.name}"></span>`{dim.name}` | {dim.symbol} |\n'
                        )
                    need_separator = True

                # Quantities
                if system.quantities:
                    if need_separator:
                        f.write("\n")
                    f.write("## Quantities\n\n")

                    # Helper to add word breaks to long identifiers
                    def add_word_breaks(name: str) -> str:
                        if "_" in name:
                            return name.replace("_", "_<wbr>")
                        return name

                    # Write table of quantities - columns reordered: Character, Dimension (3rd), Kind of, Parent
                    f.write(
                        "| Quantity | Character | Dimension | Kind of | Parent | Equation | Hierarchy |\n"
                    )
                    f.write(
                        "|----------|:---------:|:---------:|:-------:|:------:|----------|:---------:|\n"
                    )
                    for qty in sorted(system.quantities, key=lambda q: q.name):
                        character = (
                            qty.character if hasattr(qty, "character") else "Real"
                        )
                        # Get dimensional formula
                        dim_formula = (
                            qty.dimensional_formula
                            if hasattr(qty, "dimensional_formula")
                            and qty.dimensional_formula
                            else "—"
                        )

                        if qty.alias_target:
                            # This is an alias - find the target and get its data
                            target_qty = None
                            for q in system.quantities:
                                if q.name == qty.alias_target:
                                    target_qty = q
                                    break

                            if target_qty:
                                # Get root name from kind_of (e.g., 'isq::length' -> 'length')
                                root_name = (
                                    target_qty.kind_of.split("::")[-1]
                                    if hasattr(target_qty, "kind_of")
                                    and target_qty.kind_of
                                    else "dimensionless"
                                )
                                # Only generate hierarchy link if this root actually exists
                                if root_name in global_root_counts:
                                    hierarchy_file = self._get_hierarchy_filename(
                                        root_name, system.namespace, global_root_counts
                                    )
                                    hierarchy_link = (
                                        f"[view](../hierarchies/{hierarchy_file})"
                                    )
                                else:
                                    hierarchy_link = "—"
                                # Get character and dimension from target
                                character = (
                                    target_qty.character
                                    if hasattr(target_qty, "character")
                                    else "Real"
                                )
                                dim_formula = (
                                    target_qty.dimensional_formula
                                    if hasattr(target_qty, "dimensional_formula")
                                    and target_qty.dimensional_formula
                                    else "—"
                                )
                                # Use C++ extracted kind_of from target and linkify it
                                if (
                                    hasattr(target_qty, "kind_of")
                                    and target_qty.kind_of
                                ):
                                    kind_of = f"<code>{self._linkify_definition(target_qty.kind_of, system)}</code>"
                                else:
                                    kind_of = "—"
                                # Use C++ extracted parent from target and linkify it
                                parent_cpp = (
                                    target_qty.parent_from_cpp
                                    if hasattr(target_qty, "parent_from_cpp")
                                    and target_qty.parent_from_cpp
                                    else ""
                                )
                                if parent_cpp and parent_cpp != "<root>":
                                    parent_display = f"<code>{self._linkify_definition(parent_cpp, system)}</code>"
                                else:
                                    parent_display = "—"
                            else:
                                hierarchy_link = "—"
                                kind_of = "—"
                                parent_display = "—"

                            qty_name_display = add_word_breaks(qty.name)
                            f.write(
                                f'| <span id="{qty.name}"></span><code>{qty_name_display}</code> | '
                                f"{character} | {dim_formula} | {kind_of} | "
                                f"{parent_display} | alias to {self._linkify_definition(qty.alias_target, system)} | "
                                f"{hierarchy_link} |\n"
                            )
                        else:
                            # Get root name from kind_of (e.g., 'isq::length' -> 'length')
                            root_name = (
                                qty.kind_of.split("::")[-1]
                                if hasattr(qty, "kind_of") and qty.kind_of
                                else "dimensionless"
                            )
                            # Only generate hierarchy link if this root actually exists
                            if root_name in global_root_counts:
                                hierarchy_file = self._get_hierarchy_filename(
                                    root_name, system.namespace, global_root_counts
                                )
                                hierarchy_link = (
                                    f"[view](../hierarchies/{hierarchy_file})"
                                )
                            else:
                                hierarchy_link = "—"
                            # Use C++ extracted kind_of and linkify it
                            if hasattr(qty, "kind_of") and qty.kind_of:
                                kind_of = f"<code>{self._linkify_definition(qty.kind_of, system)}</code>"
                            else:
                                kind_of = "—"
                            # Use C++ extracted parent and linkify it
                            parent_cpp = (
                                qty.parent_from_cpp
                                if hasattr(qty, "parent_from_cpp")
                                and qty.parent_from_cpp
                                else ""
                            )
                            if parent_cpp and parent_cpp != "<root>":
                                parent_display = f"<code>{self._linkify_definition(parent_cpp, system)}</code>"
                            else:
                                parent_display = "—"
                            # Normalize and linkify equation
                            if qty.equation:
                                normalized = self._normalize_equation(qty.equation)
                                equation = f"<code>{self._linkify_definition(normalized, system)}</code>"
                            else:
                                equation = "—"
                            qty_name_display = add_word_breaks(qty.name)
                            f.write(
                                f'| <span id="{qty.name}"></span><code>{qty_name_display}</code> | '
                                f"{character} | {dim_formula} | {kind_of} | "
                                f"{parent_display} | {equation} | {hierarchy_link} |\n"
                            )
                    need_separator = True

                # Units - separate non-SI (for SI system only)
                regular_units = [
                    u
                    for u in system.units
                    if not (
                        u.origin_namespace and u.origin_namespace.endswith("non_si")
                    )
                ]
                non_si_units = (
                    [
                        u
                        for u in system.units
                        if u.origin_namespace and u.origin_namespace.endswith("non_si")
                    ]
                    if namespace == "si"
                    else []
                )

                if regular_units:
                    if need_separator:
                        f.write("\n")
                    f.write("## Units\n\n")
                    f.write("| Unit Name | Symbol | unit_symbol | Definition |\n")
                    f.write("|-----------|:------:|:-----------:|------------|\n")

                    # Sort by display name (including subnamespace prefix)
                    def get_unit_display_name(unit):
                        """Get the display name for sorting (includes subnamespace prefix)"""
                        subns_prefix = None
                        if unit.origin_namespace:
                            parts = unit.origin_namespace.replace(
                                "mp_units::", ""
                            ).split("::")
                            if len(parts) > 1:
                                subns_prefix = parts[-1]
                        elif unit.subnamespace:
                            subns_prefix = unit.subnamespace
                        return (
                            f"{subns_prefix}::{unit.name}"
                            if subns_prefix
                            else unit.name
                        )

                    for unit in sorted(regular_units, key=get_unit_display_name):
                        self._write_unit_row(f, unit, system)

                    # Add admonition for inline subnamespaces if any units are from them
                    inline_subns_used = set()
                    for unit in regular_units:
                        if unit.origin_namespace:
                            parts = unit.origin_namespace.replace(
                                "mp_units::", ""
                            ).split("::")
                            if len(parts) > 1:
                                subns = parts[-1]
                                if subns in system.inline_subnamespaces:
                                    inline_subns_used.add(subns)

                    if inline_subns_used:
                        f.write("\n")
                        f.write('!!! note "Inline Namespaces"\n\n')
                        for subns in sorted(inline_subns_used):
                            full_ns = f"mp_units::{namespace}::{subns}"
                            parent_ns = f"mp_units::{namespace}"
                            f.write(
                                f"    The `{full_ns}` namespace is inline in `{parent_ns}`, "
                                f"making its units directly accessible from the parent namespace.\n\n"
                            )

                    need_separator = True

                if non_si_units:
                    if need_separator:
                        f.write("\n")
                    f.write("## Non-SI units accepted for use with the SI\n\n")
                    f.write("| Unit Name | Symbol | unit_symbol | Definition |\n")
                    f.write("|-----------|:------:|:-----------:|------------|\n")
                    for unit in sorted(non_si_units, key=get_unit_display_name):
                        self._write_unit_row(f, unit, system)
                    f.write("\n")
                    f.write('!!! note "Namespace"\n\n')
                    f.write(
                        "    These units are defined in the `mp_units::non_si` namespace "
                        "and are made available in the `mp_units::si` namespace "
                        "through a using-directive.\n"
                    )
                    need_separator = True

                # Prefixes
                if system.prefixes:
                    if need_separator:
                        f.write("\n")
                    f.write("## Prefixes\n\n")
                    f.write("| Name | Symbol | Definition |\n")
                    f.write("|------|:------:|------------|\n")
                    for prefix in sorted(
                        system.prefixes, key=lambda p: self._get_prefix_magnitude(p)
                    ):
                        definition = prefix.definition.replace("|", "\\|")
                        f.write(
                            f'| <span id="{prefix.name}"></span>`{prefix.name}` | {prefix.symbol} | `{definition}` |\n'
                        )
                    need_separator = True

                # Point Origins
                if system.point_origins:
                    if need_separator:
                        f.write("\n")
                    f.write("## Point Origins\n\n")
                    f.write("| Name | Type | Definition |\n")
                    f.write("|------|:----:|------------|\n")
                    for origin in sorted(system.point_origins, key=lambda o: o.name):
                        if origin.alias_target:
                            # This is an alias - show reference to original (linkified)
                            alias_target_linked = self._linkify_definition(
                                origin.alias_target, system
                            )
                            f.write(
                                f'| <span id="{origin.name}"></span>`{origin.name}` | — | '
                                f"alias to {alias_target_linked} |\n"
                            )
                        else:
                            # Regular definition - linkify and wrap in code tags
                            definition = origin.definition.replace("|", "\\|")
                            definition_linked = self._linkify_definition(
                                definition, system
                            )
                            f.write(
                                f'| <span id="{origin.name}"></span>`{origin.name}` | '
                                f"{origin.origin_type} | <code>{definition_linked}</code> |\n"
                            )
                    # Note: Last section, no need_separator update needed

    def _write_unit_row(self, f, unit: Unit, system: SystemInfo):
        """Write a unit table row"""

        # Helper to add word breaks to long identifiers
        def add_word_breaks(name: str) -> str:
            if "_" in name:
                return name.replace("_", "_<wbr>")
            return name

        # Determine the subnamespace prefix to display
        # Use origin_namespace if available, otherwise use subnamespace
        subns_prefix = None
        if unit.origin_namespace:
            # Extract subnamespace from origin_namespace (e.g., "mp_units::si::si2019" -> "si2019")
            parts = unit.origin_namespace.replace("mp_units::", "").split("::")
            if len(parts) > 1:  # Has a subnamespace
                subns_prefix = parts[-1]  # Last component is the subnamespace
        elif unit.subnamespace:
            subns_prefix = unit.subnamespace

        # Show namespace prefix if it exists
        unit_display = f"{subns_prefix}::{unit.name}" if subns_prefix else unit.name
        unit_display_with_breaks = add_word_breaks(unit_display)

        # Anchor ID should include subnamespace to avoid conflicts
        anchor_id = f"{subns_prefix}-{unit.name}" if subns_prefix else unit.name

        if unit.alias_target:
            # This is an alias - show reference to original (linkified)
            alias_target_linked = self._linkify_definition(unit.alias_target, system)
            f.write(
                f'| <span id="{anchor_id}"></span><code>{unit_display_with_breaks}</code> | — | — | alias to {alias_target_linked} |\n'
            )
        else:
            # Regular definition
            # Handle multiple unit_symbols
            if unit.unit_symbols:
                # Multiple unit_symbols: display as comma-separated list in backticks
                short_symbol = ", ".join(f"`{s}`" for s in unit.unit_symbols)
            else:
                short_symbol = "—"

            symbol = unit.symbol if unit.symbol else "—"

            # Process definition to add namespace prefixes for units from same subnamespace
            definition = self._add_namespace_prefixes_to_definition(unit)

            # Format kind_of constraint - handle multiple patterns:
            # 1. "..., kind_of<...>" (comma before kind_of)
            # 2. "kind_of<...>" (starts with kind_of - e.g., ampere, metre)
            # 3. "kind_of<...>, other" (kind_of followed by comma - e.g., kelvin with origin)
            kind_line = ""

            # Check for pattern: "kind_of<...>, ..." or "kind_of<...>"
            if definition.startswith("kind_of<"):
                # Find the closing > for kind_of
                kind_end = definition.find(">")
                if kind_end != -1:
                    kind_content = definition[
                        8:kind_end
                    ]  # Extract content between kind_of< and >
                    # Linkify the kind content
                    kind_line = f"kind: `{kind_content}`"
                    # Remove the kind_of<...> part (and comma if present)
                    rest = definition[kind_end + 1 :].strip()
                    if rest.startswith(","):
                        rest = rest[1:].strip()
                    definition = rest
            # Check for pattern: "..., kind_of<...>"
            elif ", kind_of<" in definition:
                parts = definition.rsplit(", kind_of<", 1)
                if len(parts) == 2:
                    definition = parts[0].strip()
                    kind_part = parts[1].strip()
                    if kind_part.endswith(">"):
                        kind_content = kind_part[:-1]  # Remove trailing >
                        # Linkify the kind content
                        kind_line = f"kind: `{kind_content}`"

            # Format offset unit origins more clearly (only for the 4 offset units)
            # After extracting kind, check if remaining definition has an origin
            origin_line = ""
            if definition:
                # Check if it's a simple origin identifier (no operators, just a name)
                # This handles kelvin which becomes just "zeroth_kelvin" after kind extraction
                if (
                    (
                        "zeroth_" in definition
                        or definition in ["absolute_zero", "ice_point"]
                    )
                    and not any(
                        op in definition for op in ["*", "/", "+", "-", "(", ")"]
                    )
                    and ", " not in definition
                ):
                    # Linkify the origin
                    origin_line = f"origin: `{definition}`"
                    definition = ""
                # Check for "unit, origin" pattern (e.g., "kelvin, zeroth_degree_Celsius")
                elif ", " in definition:
                    parts = definition.rsplit(", ", 1)
                    if len(parts) == 2:
                        base_def = parts[0].strip()
                        origin = parts[1].strip()
                        # Check if origin looks like a point origin (not a regular expression with commas)
                        if (
                            "zeroth_" in origin
                            or "point<" in origin
                            or origin in ["absolute_zero", "ice_point"]
                        ) and "<" not in origin.replace("point<", ""):
                            definition = base_def
                            # Store origin without backticks - will be linkified later
                            origin_line = f"origin: {origin}"

            # Linkify the main definition (before wrapping in backticks)
            if definition:
                definition = self._linkify_definition(definition, system)
                # Actually, since links have [`text`](url) format, we don't wrap the whole thing
                # definition = f"`{definition}`"  # Don't do this - links already have backticks

            # Linkify kind_line content
            if kind_line:
                # Extract the content after "kind: "
                kind_content = kind_line.replace("kind: ", "").replace("`", "")
                kind_content_linked = self._linkify_definition(kind_content, system)
                kind_line = f"kind: {kind_content_linked}"

            # Linkify origin_line content
            if origin_line:
                # Extract the content after "origin: "
                origin_content = origin_line.replace("origin: ", "").replace("`", "")
                origin_content_linked = self._linkify_definition(origin_content, system)
                origin_line = f"origin: {origin_content_linked}"

            # Build the final definition cell with line breaks
            parts = []
            if definition:
                parts.append(definition)
            if kind_line:
                parts.append(kind_line)
            if origin_line:
                parts.append(origin_line)

            # Join parts and escape pipes
            if parts:
                definition_cell = "<br>".join(parts)
                definition_cell = definition_cell.replace("|", "\\|")
                # Wrap in HTML code tags to preserve code font for non-linked parts
                definition_cell = f"<code>{definition_cell}</code>"
            else:
                definition_cell = "—"

            # unit_symbol_cell is already formatted with backticks or em-dash
            f.write(
                f'| <span id="{anchor_id}"></span><code>{unit_display_with_breaks}</code> | {symbol} | '
                f"{short_symbol} | {definition_cell} |\n"
            )

    def _add_namespace_prefixes_to_definition(self, unit: Unit) -> str:
        """Add namespace prefixes to unit references in definition if they're from same subnamespace"""
        if not unit.subnamespace:
            return unit.definition

        definition = unit.definition

        # Find all units in the same system and subnamespace
        system_namespace = unit.namespace.replace(f"::{unit.subnamespace}", "").replace(
            "mp_units::", ""
        )
        system = self.parser.systems.get(system_namespace)

        if not system:
            return definition

        # Get all unit names from the same subnamespace, sorted by length (longest first)
        # to avoid replacing substrings of longer names
        sibling_units = [
            u.name
            for u in system.units
            if u.subnamespace == unit.subnamespace and u.name != unit.name
        ]
        sibling_units.sort(key=len, reverse=True)

        # Replace unit names with prefixed versions using word boundaries
        for sibling_name in sibling_units:
            # Use word boundary pattern to avoid partial matches
            pattern = r"\b" + re.escape(sibling_name) + r"\b"
            replacement = f"{unit.subnamespace}::{sibling_name}"
            definition = re.sub(pattern, replacement, definition)

        return definition

    def _linkify_definition(self, definition: str, current_system: SystemInfo) -> str:
        """Convert unit/quantity/origin references in definition to markdown links while preserving code font.

        Converts references like `yard` to [`yard`](#yard) or [`si::metre`](si.md#metre).
        Preserves operators, numbers, and template syntax without linkification.
        """
        if not definition:
            return definition

        # Pattern to match identifiers (including namespace-qualified ones)
        # Matches: word, namespace::word, nested::namespace::word
        # Allow both lowercase and uppercase letters for names like Julian_year and zeroth_degree_Celsius
        # Support Unicode identifiers (e.g., π)
        identifier_pattern = r"\b([a-zA-Z_\u0080-\uFFFF][a-zA-Z0-9_\u0080-\uFFFF]*(?:::[a-zA-Z_\u0080-\uFFFF][a-zA-Z0-9_\u0080-\uFFFF]*)*)\b"

        # Collect all possible references from all systems
        all_refs = {}  # name -> (system_namespace, anchor_name)

        for sys_ns, system in self.parser.systems.items():
            # Add units
            for unit in system.units:
                all_refs[unit.name] = (sys_ns, unit.name)
                # Also add qualified names
                all_refs[f"{sys_ns}::{unit.name}"] = (sys_ns, unit.name)
                # Add secondary namespace qualified names
                if unit.secondary_namespaces:
                    for sec_ns in unit.secondary_namespaces:
                        all_refs[f"{sec_ns}::{unit.name}"] = (sys_ns, unit.name)

            # Add point origins
            for origin in system.point_origins:
                all_refs[origin.name] = (sys_ns, origin.name)
                all_refs[f"{sys_ns}::{origin.name}"] = (sys_ns, origin.name)
                if origin.secondary_namespaces:
                    for sec_ns in origin.secondary_namespaces:
                        all_refs[f"{sec_ns}::{origin.name}"] = (sys_ns, origin.name)

            # Add quantities
            for qty in system.quantities:
                all_refs[qty.name] = (sys_ns, qty.name)
                all_refs[f"{sys_ns}::{qty.name}"] = (sys_ns, qty.name)
                if qty.secondary_namespaces:
                    for sec_ns in qty.secondary_namespaces:
                        all_refs[f"{sec_ns}::{qty.name}"] = (sys_ns, qty.name)

            # Add prefixes
            for prefix in system.prefixes:
                # Prefixes link to their specific anchor
                all_refs[prefix.name] = (sys_ns, prefix.name)
                all_refs[f"{sys_ns}::{prefix.name}"] = (sys_ns, prefix.name)
                if prefix.secondary_namespaces:
                    for sec_ns in prefix.secondary_namespaces:
                        all_refs[f"{sec_ns}::{prefix.name}"] = (sys_ns, prefix.name)

        def replace_identifier(match):
            identifier = match.group(1)

            # Skip keywords and known functions/templates
            skip_words = {
                "mag",
                "mag_ratio",
                "mag_power",
                "kind_of",
                "kind",
                "square",
                "cubic",
                "pow",
                "sqrt",
                "cbrt",
                "abs",
                "inverse",
                "ratio",
                "power",
                "root",
                "si",
                "isq",
                "iec",
                "usc",
                "cgs",
                "iau",
                "hep",
                "imperial",
                "non_si",
                "typographic",
                "angular",
                "natural",
                "isq_angle",
                "mp_units",
                "mp_units::point",  # Template functions
            }

            if identifier in skip_words:
                return match.group(0)

            # If it's a qualified name, check if we should strip the namespace prefix
            display_text = identifier
            if "::" in identifier:
                parts = identifier.split("::")
                namespace_prefix = "::".join(parts[:-1])
                unqualified_name = parts[-1]

                # Strip namespace if it matches current system
                if namespace_prefix == current_system.namespace:
                    display_text = unqualified_name

            # Helper function to create link with word breaks for long identifiers
            def make_link(text, url):
                # For identifiers with underscores, insert <wbr> at underscores to allow breaking
                # Markdown link text supports HTML, so we can use <wbr> tags in the text
                if "_" in text:
                    # Insert <wbr> after each underscore for better line breaking
                    text_with_breaks = text.replace("_", "_<wbr>")
                    return f"[{text_with_breaks}]({url})"
                else:
                    # Standard markdown link
                    return f"[{text}]({url})"

            # Check if this identifier is a reference we can link
            # First check in current system for the unqualified name
            unqualified_check = (
                identifier.split("::")[-1] if "::" in identifier else identifier
            )

            # Special handling for sub-namespace references (e.g., non_si::day)
            # Look for units where origin_namespace matches the namespace prefix
            if "::" in identifier:
                parts = identifier.split("::")
                if len(parts) == 2:
                    potential_origin_ns = parts[0]
                    entity_name = parts[1]
                    # Check all systems for a unit with this origin_namespace
                    for sys_ns, system in self.parser.systems.items():
                        for unit in system.units:
                            # origin_namespace contains full path like "mp_units::non_si"
                            # Check if it ends with the potential_origin_ns
                            if (
                                unit.name == entity_name
                                and unit.origin_namespace
                                and unit.origin_namespace.endswith(potential_origin_ns)
                            ):
                                return make_link(
                                    display_text, f"{sys_ns}.md#{entity_name}"
                                )
                        # Check point_origins too
                        for origin in system.point_origins:
                            if (
                                origin.name == entity_name
                                and hasattr(origin, "origin_namespace")
                                and origin.origin_namespace
                                and origin.origin_namespace.endswith(
                                    potential_origin_ns
                                )
                            ):
                                return make_link(
                                    display_text, f"{sys_ns}.md#{entity_name}"
                                )

            # Build a key to check current system first
            current_sys_key = f"{current_system.namespace}::{unqualified_check}"

            # Special case: dimensionless is only defined in core, never link to self in other systems
            if (
                unqualified_check == "dimensionless"
                and current_system.namespace != "core"
            ):
                # Always link to core for dimensionless
                if "core::dimensionless" in all_refs:
                    return make_link(display_text, "core.md#dimensionless")
                elif "dimensionless" in all_refs:
                    target_sys, anchor = all_refs["dimensionless"]
                    return make_link(display_text, f"{target_sys}.md#{anchor}")

            if current_sys_key in all_refs:
                # Found in current system
                target_sys, anchor = all_refs[current_sys_key]
                return make_link(display_text, f"#{anchor}")
            elif identifier in all_refs:
                # Found with full identifier
                target_sys, anchor = all_refs[identifier]

                # Determine if same system or cross-system
                if target_sys == current_system.namespace:
                    # Same system - use anchor link
                    return make_link(display_text, f"#{anchor}")
                else:
                    # Cross-system - use relative link
                    return make_link(display_text, f"{target_sys}.md#{anchor}")

            # If it's a qualified name that wasn't found, try the unqualified name
            # (handles cases like iau::astronomical_unit where the unit is in si)
            if "::" in identifier:
                parts = identifier.split("::")
                unqualified_name = parts[-1]
                if unqualified_name in all_refs:
                    target_sys, anchor = all_refs[unqualified_name]
                    # Use the actual system where the unit is defined
                    if target_sys == current_system.namespace:
                        return make_link(display_text, f"#{anchor}")
                    else:
                        return make_link(display_text, f"{target_sys}.md#{anchor}")

            # Not a linkable reference, return as-is
            return match.group(0)

        # Apply the replacement
        result = re.sub(identifier_pattern, replace_identifier, definition)

        # Add word break opportunities at natural break points to prevent table overflow
        # Only add after operators, not within markdown link syntax
        result = result.replace(" / ", " / <wbr>")
        result = result.replace(" * ", " * <wbr>")

        return result

    def _write_quantity_tree(self, f, qty: Quantity, qty_children: dict, indent: int):
        """Write quantity hierarchy tree recursively"""
        prefix = "  " * indent + ("├─ " if indent > 0 else "")
        kind_marker = " [kind]" if qty.is_kind else ""
        equation_info = f" = {qty.equation}" if qty.equation else ""
        f.write(f"{prefix}{qty.name}{kind_marker}{equation_info}\n")

        children = sorted(qty_children.get(qty.name, []), key=lambda q: q.name)
        for child in children:
            self._write_quantity_tree(f, child, qty_children, indent + 1)

    def _normalize_equation(self, equation: str) -> str:
        """Normalize equation formatting by ensuring proper spacing around operators"""
        if not equation:
            return equation
        # Add space before * if not already there
        equation = re.sub(r"(\w)(\*)", r"\1 \2", equation)
        # Add space after * if not already there
        equation = re.sub(r"(\*)(\w)", r"\1 \2", equation)
        # Add space before / if not already there
        equation = re.sub(r"(\w)(\/)", r"\1 \2", equation)
        # Add space after / if not already there
        equation = re.sub(r"(\/)(\w)", r"\1 \2", equation)
        return equation

    def _build_mermaid_hierarchy(
        self, root_name: str, qty_children: dict, qualified_quantities: list
    ) -> str:
        """Build Mermaid flowchart for quantity hierarchy using qualified names"""
        lines = ["```mermaid", "flowchart LR"]

        # Build a map of qualified_name -> quantity
        qty_map = {qname: qty for qname, qty in qualified_quantities}

        # Build a map of aliases: target_qualified_name -> [alias_qualified_names]
        aliases_map = defaultdict(list)
        for qname, qty in qualified_quantities:
            if qty.alias_target:
                # Find the target's qualified name
                sys_ns = qty.namespace.replace("mp_units::", "")
                if qty.alias_target == "dimensionless":
                    target_qname = "dimensionless"
                else:
                    target_qname = f"{sys_ns}::{qty.alias_target}"
                aliases_map[target_qname].append(qname)

        def add_node(qualified_name: str, parent_id: str = None):
            qty = qty_map.get(qualified_name)
            if not qty:
                return

            # Skip if this is an alias (will be included in target's box)
            if qty.alias_target:
                return

            # Create node ID from qualified name (sanitized for Mermaid)
            node_id = qualified_name.replace("::", "_").replace("-", "_")

            # Build node label - use the qualified name directly (no prefix for dimensionless)
            name_display = qualified_name

            # Add aliases to the name (e.g., "isq::height | isq::depth | isq::altitude")
            if qualified_name in aliases_map:
                alias_names = sorted(aliases_map[qualified_name])
                name_display = name_display + " | " + " | ".join(alias_names)

            # Format equation in italics inside parentheses with normalized spacing
            equation = ""
            if qty.equation:
                normalized_eq = self._normalize_equation(qty.equation)
                equation = f"<br><i>({normalized_eq})</i>"

            label = f"<b>{name_display}</b>{equation}"

            # Add node definition
            lines.append(f'    {node_id}["{label}"]')

            # Add edge from parent
            if parent_id:
                lines.append(f"    {parent_id} --- {node_id}")

            # Process children (excluding aliases) - children are (qualified_name, qty) tuples
            children = sorted(qty_children.get(qualified_name, []), key=lambda x: x[0])
            for child_qname, child_qty in children:
                add_node(child_qname, node_id)

        # Start with the root (which should be the qualified root name, or just 'dimensionless')
        if root_name == "dimensionless":
            add_node("dimensionless")
        else:
            # Find the first root with this name
            for qname, qty in qualified_quantities:
                is_root = (
                    hasattr(qty, "parent_from_cpp") and qty.parent_from_cpp == "<root>"
                )
                if qty.name == root_name and is_root:
                    add_node(qname)
                    break

        lines.append("```")
        return "\n".join(lines)


class CppMetadataExtractor:
    """Extract quantity metadata by compiling and running a C++ program"""

    def __init__(self, parser: SystemsParser, source_dir: Path):
        self.parser = parser
        self.source_dir = source_dir
        self.metadata = (
            {}
        )  # (namespace, qty_name) -> {dimension, kind_of, parent, character}

    def extract_metadata(self):
        """Generate C++ program, compile it, run it, and parse the output"""
        import shutil
        import subprocess
        import tempfile

        # Generate C++ program
        cpp_code = self._generate_cpp_program()

        # Write to temporary file
        with tempfile.NamedTemporaryFile(mode="w", suffix=".cpp", delete=False) as f:
            cpp_file = Path(f.name)
            f.write(cpp_code)

        # Find available C++ compiler (try multiple options)
        compiler = None
        for candidate in [
            "g++-14",
            "g++-13",
            "g++-12",
            "g++",
            "clang++-18",
            "clang++-17",
            "clang++",
            "c++",
        ]:
            if shutil.which(candidate):
                compiler = candidate
                break

        if not compiler:
            print("Warning: No suitable C++23 compiler found (tried g++, clang++, c++)")
            print("         Quantity metadata extraction skipped")
            cpp_file.unlink()
            return

        # Compile
        exe_file = cpp_file.with_suffix("")
        compilation_failed = False
        try:
            compile_cmd = [
                compiler,
                "-std=c++23",
                f"-I{self.source_dir}/src/core/include",
                f"-I{self.source_dir}/src/systems/include",
                str(cpp_file),
                "-o",
                str(exe_file),
            ]
            result = subprocess.run(
                compile_cmd, capture_output=True, text=True, timeout=60
            )
            if result.returncode != 0:
                compilation_failed = True
                print(
                    f"Warning: Failed to compile metadata extraction program with {compiler}:"
                )
                print(result.stderr)
                print(f"         C++ file preserved for inspection: {cpp_file}")
                return

            # Run and capture output
            result = subprocess.run(
                [str(exe_file)], capture_output=True, text=True, timeout=10
            )
            if result.returncode != 0:
                print("Warning: Failed to run metadata extraction program:")
                print(result.stderr)
                return

            # Parse output
            self._parse_output(result.stdout)

        finally:
            # Cleanup - preserve .cpp file only on compilation failure for debugging
            if cpp_file.exists() and not compilation_failed:
                cpp_file.unlink()
            if exe_file.exists():
                exe_file.unlink()

    def _generate_cpp_program(self) -> str:
        """Generate C++ program that outputs metadata for all quantities"""
        lines = [
            "// Auto-generated program to extract quantity metadata",
            "#include <mp-units/systems/isq.h>",
            "#include <mp-units/systems/isq_angle.h>",
            "#include <mp-units/systems/angular.h>",
            "#include <mp-units/systems/natural.h>",
            "#include <mp-units/systems/iec80000.h>",
            "#include <iostream>",
            "",
            "using namespace mp_units;",
            "",
            "constexpr std::string_view get_parent(QuantitySpec auto qs)",
            "{",
            "  if constexpr (requires { qs._parent_; })",
            "    return detail::type_name<std::remove_const_t<decltype(qs._parent_)>>();",
            "  else",
            '    return "<root>";',
            "}",
            "",
            "constexpr std::string_view character_to_string(quantity_character ch)",
            "{",
            "  switch (ch) {",
            "    case quantity_character::real_scalar:",
            '      return "Real";',
            "    case quantity_character::complex_scalar:",
            '      return "Complex";',
            "    case quantity_character::vector:",
            '      return "Vector";',
            "    case quantity_character::tensor:",
            '      return "Tensor";',
            "    default:",
            '      return "Unknown";',
            "  }",
            "}",
            "",
            "template<QuantitySpec QS>",
            "void print_quantity(std::string_view namespace_name, std::string_view name, QS qs)",
            "{",
            '  std::cout << namespace_name << ","',
            '            << name << ","',
            '            << character_to_string(qs.character) << ","',
            '            << dimension_symbol(qs.dimension) << ","',
            '            << detail::type_name<decltype(get_kind(qs))>() << ","',
            "            << get_parent(qs) << '\\n';",
            "}",
            "",
            "#define PRINT_QTY(ns, qty) print_quantity(#ns, #qty, ns::qty)",
            "",
            "int main()",
            "{",
        ]

        # Add dimensionless (special case - no namespace)
        lines.append('  print_quantity("", "dimensionless", dimensionless);')

        # Add all quantities from each system
        for system_key, system in self.parser.systems.items():
            if system.quantities:
                for qty in sorted(system.quantities, key=lambda q: q.name):
                    if not qty.alias_target:  # Skip aliases
                        if system.namespace:  # Regular namespace
                            lines.append(
                                f"  PRINT_QTY({system.namespace}, {qty.name});"
                            )
                        else:  # Empty namespace (core) = mp_units::
                            # dimensionless is already handled above
                            if qty.name != "dimensionless":
                                lines.append(
                                    f'  print_quantity("", "{qty.name}", {qty.name});'
                                )

        lines.extend(
            [
                "  return 0;",
                "}",
            ]
        )

        return "\n".join(lines)

    def _parse_output(self, output: str):
        """Parse C++ program output and store metadata"""
        for line in output.strip().split("\n"):
            if not line:
                continue

            # Format: namespace,name,character,dimension_symbol,kind_of_type,parent_type
            parts = line.split(",", 5)
            if len(parts) != 6:
                continue

            namespace, name, character, dim_symbol, kind_of, parent = parts

            # Extract kind_of from: mp_units::kind_of_<mp_units::isq::length>; std::string_view = ...
            # We want: isq::length
            kind_match = kind_of.split("mp_units::kind_of_<mp_units::")
            if len(kind_match) > 1:
                kind_str = kind_match[1].split(">")[0]
            else:
                kind_str = ""

            # Extract parent from: mp_units::isq::length; std::string_view = ...
            # We want: isq::length
            if "<root>" in parent:
                parent_str = "<root>"
            else:
                parent_match = parent.split("mp_units::")
                if len(parent_match) > 1:
                    parent_str = parent_match[1].split(";")[0]
                else:
                    parent_str = parent

            # Handle dimensionless specially
            if not namespace:
                namespace = ""

            # Store metadata
            key = (namespace, name)
            self.metadata[key] = {
                "dimension": dim_symbol,
                "kind_of": kind_str,
                "parent": parent_str,
                "character": character,
            }

    def apply_metadata(self):
        """Apply extracted metadata to quantity objects"""
        for namespace, system in self.parser.systems.items():
            for qty in system.quantities:
                key = (namespace, qty.name)
                if key in self.metadata:
                    meta = self.metadata[key]
                    qty.dimensional_formula = (
                        meta["dimension"] if meta["dimension"] != "1" else "—"
                    )
                    if (
                        qty.dimensional_formula == "—"
                        and namespace == ""
                        and qty.name == "dimensionless"
                    ):
                        qty.dimensional_formula = "1"
                    # Apply all metadata from C++ extraction
                    qty.character = meta["character"]
                    qty.kind_of = meta["kind_of"]
                    qty.parent_from_cpp = meta["parent"]


def main():
    """Main entry point"""
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    systems_dir = project_root / "src" / "systems" / "include" / "mp-units" / "systems"
    output_dir = project_root / "docs" / "reference" / "systems_reference"
    mkdocs_file = project_root / "mkdocs.yml"

    if not systems_dir.exists():
        print(f"Error: Systems directory not found: {systems_dir}", file=sys.stderr)
        return 1

    print(f"Parsing systems from: {systems_dir}")
    parser = SystemsParser(systems_dir)
    parser.parse_all_systems()
    parser.populate_secondary_namespaces()

    print(f"Found {len(parser.systems)} systems")
    for ns, system in parser.systems.items():
        base_units = len([u for u in system.units if u.is_base])
        derived_units = len(
            [u for u in system.units if not u.is_base and not u.is_alias]
        )
        point_origins = len(system.point_origins)
        prefixes = len(system.prefixes)
        print(
            f"  - {ns}: {len(system.dimensions)} dims, "
            f"{len(system.quantities)} qtys, {base_units} base units, "
            f"{derived_units} derived units, {point_origins} point origins, "
            f"{prefixes} prefixes"
        )

    print("\nExtracting metadata from C++...")
    extractor = CppMetadataExtractor(parser, project_root)
    extractor.extract_metadata()
    extractor.apply_metadata()
    print(f"  ✓ Extracted metadata for {len(extractor.metadata)} quantities")

    print(f"\nGenerating documentation to: {output_dir}")
    generator = DocumentationGenerator(parser, output_dir)
    generator.generate_systems_index()
    generator.generate_dimensions_index()
    generator.generate_quantities_index()
    generator.generate_units_index()
    generator.generate_prefixes_index()
    generator.generate_point_origins_index()
    hierarchy_count = generator.generate_cross_system_hierarchies()
    generator.generate_hierarchies_overview()
    generator.generate_per_system_pages()

    print("\nUpdating mkdocs configuration...")
    generator.update_mkdocs_config(mkdocs_file)

    total_dims = sum(len(s.dimensions) for s in parser.systems.values())
    total_qtys = sum(len(s.quantities) for s in parser.systems.values())
    total_units = sum(len(s.units) for s in parser.systems.values())
    total_origins = sum(len(s.point_origins) for s in parser.systems.values())
    total_prefixes = sum(len(s.prefixes) for s in parser.systems.values())

    print("\nDocumentation generation complete!")
    print(f"  - {len(parser.systems)} systems processed")
    print(f"  - {total_dims} dimensions documented")
    print(f"  - {total_qtys} quantities documented")
    print(f"  - {hierarchy_count} quantity hierarchies documented")
    print(f"  - {total_units} units documented")
    print(f"  - {total_origins} point origins documented")
    print(f"  - {total_prefixes} prefixes documented")

    return 0


def compute_source_hash(source_root: Path) -> str:
    """Compute hash of all source files to detect changes"""
    hasher = hashlib.sha256()

    # Hash all header files that could affect generation
    # Only systems headers + unit.h (for core dimensionless units)
    patterns = [
        "src/systems/include/mp-units/systems/**/*.h",
        "src/core/include/mp-units/framework/unit.h",
    ]

    files = []
    for pattern in patterns:
        files.extend(sorted(source_root.glob(pattern)))

    for file_path in files:
        hasher.update(str(file_path.relative_to(source_root)).encode())
        hasher.update(file_path.read_bytes())

    return hasher.hexdigest()


def should_regenerate(source_root: Path, cache_file: Path) -> bool:
    """Check if regeneration is needed based on source file changes"""
    if not cache_file.exists():
        return True

    try:
        with open(cache_file, "r") as f:
            cache_data = json.load(f)
            cached_hash = cache_data.get("source_hash", "")
    except (json.JSONDecodeError, OSError):
        return True

    current_hash = compute_source_hash(source_root)
    return current_hash != cached_hash


def save_cache(source_root: Path, cache_file: Path):
    """Save current source hash to cache"""
    cache_file.parent.mkdir(parents=True, exist_ok=True)
    current_hash = compute_source_hash(source_root)

    with open(cache_file, "w") as f:
        json.dump({"source_hash": current_hash}, f, indent=2)
        f.write("\n")  # Add trailing newline for pre-commit


def generate_if_needed(source_root: Path, force: bool = False) -> int:
    """Generate documentation only if sources changed or forced"""
    cache_file = source_root / "docs/reference/systems_reference/.cache.json"

    if not force and not should_regenerate(source_root, cache_file):
        print("Systems reference documentation is up to date (sources unchanged)")
        return 0

    print("Generating systems reference documentation...")
    result = main()

    if result == 0:
        save_cache(source_root, cache_file)

    return result


# MkDocs hook integration
def on_pre_build(config, **kwargs):
    """MkDocs hook: generate systems reference before building docs"""
    # Determine source root from mkdocs config
    docs_dir = Path(config["docs_dir"])
    source_root = docs_dir.parent

    return generate_if_needed(source_root, force=False)


if __name__ == "__main__":
    # Support both direct execution and --force flag
    force = "--force" in sys.argv
    source_root = Path(__file__).parent.parent

    sys.exit(generate_if_needed(source_root, force=force))
