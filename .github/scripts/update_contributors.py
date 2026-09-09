#!/usr/bin/env python3
"""Update the community contributor list in CONTRIBUTORS.md.

Contributors are gathered from two sources and merged:

- the GitHub contributors API, which reports everyone holding a commit on the default branch
- the ``[@handle](profile)`` credits in CHANGELOG.md, which catch the people who contributed
  something other than a commit and are therefore invisible to the API

No per-person contribution counts are published. A commit count measures how a change
happened to be squashed rather than what it was worth, and it cannot see reviews, bug
reports, or design discussions at all. Individual work is credited by name in CHANGELOG.md
and in the release notes instead.
"""

import os
import re
import sys
from datetime import date
from pathlib import Path
from typing import Dict, Tuple

try:
    import requests
except ImportError:
    print("Error: requests library not found. Install with: pip install requests")
    sys.exit(1)

DEFAULT_REPO = "mpusz/mp-units"
CONTRIBUTORS_FILE = Path("CONTRIBUTORS.md")
CHANGELOG_FILE = Path("CHANGELOG.md")

# Listed by hand in the "Core Team" section, so kept out of the generated list
CORE_TEAM = {"mpusz", "johelegp", "chiphogg"}

# GitHub matches a commit to an account through the e-mail address the commit carries.
# Anybody whose address is not registered with their account stays invisible to the
# contributors API, which reports them as "Anonymous" only when asked with `anon=1` and even
# then without a handle to link to. The table below maps such a commit e-mail (or the author
# name, for a commit carrying no address) to the handle that opened the pull request the work
# arrived in. Add an entry whenever this script reports an unresolved contributor.
ANONYMOUS_ALIASES = {
    "me@radnyx.com": "Radnyx",  # PR #730, #731
    "raporter@microsoft.com": "Radnyx",  # PR #730, second address of the same person
    "oliver@openbrackets.net": "oschonrock",
    "nbresler@anduril.com": "NoahBres",  # PR #563
    "elepain@scitec.com": "EALePain",  # PR #708
    "Cazadorro": "Cazadorro",  # PR #446, the commits carry no address at all
}

# The mirror image of the problem: an account holding an address that somebody else committed
# with, and so credited by the API for work it never contributed
MISATTRIBUTED_ACCOUNTS = {
    "mikeford1": "mikeford3",  # holds the commit address, but PR #112 - #265 are mikeford3
}

START_MARKER = "<!-- CONTRIBUTORS_START -->"
END_MARKER = "<!-- CONTRIBUTORS_END -->"

# "[@handle](https://github.com/handle)" as used by the changelog entries
CREDIT_PATTERN = re.compile(
    r"\[@([A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?)\]\((https://github\.com/[^)\s]+)\)"
)

# Lowercased handle -> (handle as displayed, profile URL)
Contributors = Dict[str, Tuple[str, str]]


def add(contributors: Contributors, handle: str, profile: str = "") -> None:
    """Record a contributor under their lowercased handle."""
    contributors[handle.lower()] = (handle, profile or f"https://github.com/{handle}")


def fetch_api_contributors(repo: str, headers: Dict[str, str]) -> Contributors:
    """Fetch every non-bot committer, including the ones GitHub cannot match to an account."""
    url = f"https://api.github.com/repos/{repo}/contributors"
    contributors: Contributors = {}
    unresolved = []
    page = 1

    while True:
        response = requests.get(
            url,
            headers=headers,
            params={"page": page, "per_page": 100, "anon": 1},
            timeout=30,
        )
        response.raise_for_status()

        batch = response.json()
        if not batch:
            break

        for entry in batch:
            if entry.get("type") == "Bot":
                continue

            if entry.get("type") == "Anonymous":
                handle = ANONYMOUS_ALIASES.get(
                    entry.get("email", "")
                ) or ANONYMOUS_ALIASES.get(entry.get("name", ""))
                if handle:
                    add(contributors, handle)
                else:
                    unresolved.append(f"{entry.get('name')} <{entry.get('email')}>")
                continue

            login = entry["login"]
            replacement = MISATTRIBUTED_ACCOUNTS.get(login.lower())
            if replacement:
                add(contributors, replacement)
            else:
                add(contributors, login, entry["html_url"])

        page += 1

    for author in unresolved:
        print(
            f"Warning: no GitHub account known for {author}, add it to ANONYMOUS_ALIASES"
        )

    return contributors


def fetch_changelog_credits(path: Path = CHANGELOG_FILE) -> Contributors:
    """Collect everyone thanked by name in the changelog."""
    if not path.exists():
        print(f"Warning: {path} not found, skipping changelog credits.")
        return {}

    credits: Contributors = {}
    for handle, profile in CREDIT_PATTERN.findall(path.read_text()):
        replacement = MISATTRIBUTED_ACCOUNTS.get(handle.lower())
        if replacement:
            handle, profile = replacement, ""
        if handle.lower() not in credits:
            add(credits, handle, profile)
    return credits


def collect_contributors(repo: str, headers: Dict[str, str]) -> Contributors:
    """Merge both sources, then drop the separately listed core team."""
    contributors = fetch_changelog_credits()
    # the API spells the handle the way its owner does, so let it win over the changelog
    contributors.update(fetch_api_contributors(repo, headers))

    for member in CORE_TEAM:
        contributors.pop(member, None)

    return contributors


def render_list(contributors: Contributors) -> str:
    """Render the contributors alphabetically, without ranking anybody."""
    if not contributors:
        return "_The list is being rebuilt, please check back shortly._"

    entries = sorted(contributors.values(), key=lambda entry: entry[0].lower())
    return "\n".join(f"- **[{handle}]({profile})**" for handle, profile in entries)


def update_contributors_file(
    contributors: Contributors, path: Path = CONTRIBUTORS_FILE
) -> None:
    """Replace the generated block of CONTRIBUTORS.md in place."""
    if not path.exists():
        sys.exit(f"{path} not found!")

    content = path.read_text()
    if START_MARKER not in content or END_MARKER not in content:
        sys.exit(f"Could not find the contributor markers in {path}!")

    before = content.split(START_MARKER)[0]
    after = content.split(END_MARKER)[1]
    stamp = date.today().isoformat()
    block = (
        f"{START_MARKER}\n\n{render_list(contributors)}\n\n"
        f"_{len(contributors)} people, last updated {stamp}._\n\n{END_MARKER}"
    )

    path.write_text(f"{before}{block}{after}")
    print(f"Updated {path} with {len(contributors)} community contributors")


def main() -> None:
    repo = os.getenv("GITHUB_REPOSITORY", DEFAULT_REPO)
    token = os.getenv("GITHUB_TOKEN")
    if not token:
        print("Warning: no GITHUB_TOKEN set, API requests may be rate limited.")

    headers = {"Authorization": f"token {token}"} if token else {}

    try:
        print(f"Fetching contributors of {repo}...")
        update_contributors_file(collect_contributors(repo, headers))
    except requests.RequestException as error:
        sys.exit(f"Error fetching contributors: {error}")


if __name__ == "__main__":
    main()
