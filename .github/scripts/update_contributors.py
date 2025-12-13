#!/usr/bin/env python3
"""
Update CONTRIBUTORS.md with current GitHub contributors
This script fetches contributor information from GitHub API and updates the contributors list
"""

import subprocess
import sys
from pathlib import Path
from typing import Dict, List, Optional

try:
    import requests
except ImportError:
    print("Error: requests library not found. Install with: pip install requests")
    sys.exit(1)


class ContributorUpdater:
    """Manages updating the contributors list"""

    def __init__(self, repo_owner: str, repo_name: str, token: Optional[str] = None):
        self.repo_owner = repo_owner
        self.repo_name = repo_name
        self.token = token
        self.headers = {}
        if token:
            self.headers["Authorization"] = f"token {token}"

    def fetch_contributors(self) -> List[Dict]:
        """Fetch contributors from GitHub API"""
        url = f"https://api.github.com/repos/{self.repo_owner}/{self.repo_name}/contributors"

        all_contributors = []
        page = 1

        while True:
            params = {"page": page, "per_page": 100}
            response = requests.get(url, headers=self.headers, params=params)

            if response.status_code != 200:
                print(f"Error fetching contributors: {response.status_code}")
                print(response.text)
                break

            contributors = response.json()
            if not contributors:
                break

            all_contributors.extend(contributors)
            page += 1

        return all_contributors

    def get_contributor_details(self, username: str) -> Dict:
        """Get detailed information about a contributor"""
        url = f"https://api.github.com/users/{username}"
        response = requests.get(url, headers=self.headers)

        if response.status_code == 200:
            return response.json()
        return {}

    def categorize_contributors(
        self, contributors: List[Dict]
    ) -> Dict[str, List[Dict]]:
        """Categorize contributors by contribution level"""
        # Core team members (manually maintained)
        core_team = {"mpusz", "JohelEGP", "chiphogg"}
        core_team_lower = {name.lower() for name in core_team}

        # Categorize by contribution count
        major_contributors = []  # 50+ contributions
        regular_contributors = []  # 10-49 contributions
        occasional_contributors = []  # 1-9 contributions

        for contributor in contributors:
            username = contributor["login"]
            contributions = contributor["contributions"]

            # Skip core team members (handled separately)
            if username.lower() in core_team_lower:
                continue

            # Skip bots
            if contributor.get("type") == "Bot":
                continue

            if contributions >= 50:
                major_contributors.append(contributor)
            elif contributions >= 10:
                regular_contributors.append(contributor)
            else:
                occasional_contributors.append(contributor)

        return {
            "major": major_contributors,
            "regular": regular_contributors,
            "occasional": occasional_contributors,
        }

    def generate_contributor_section(
        self, contributors: List[Dict], include_contributions: bool = True
    ) -> str:
        """Generate markdown for a list of contributors"""
        if not contributors:
            return "*No contributors in this category yet.*\n"

        lines = []
        for contributor in contributors:
            username = contributor["login"]
            profile_url = contributor["html_url"]
            contributions = contributor["contributions"]

            if include_contributions:
                line = (
                    f"- **[{username}]({profile_url})** ({contributions} contributions)"
                )
            else:
                line = f"- **[{username}]({profile_url})**"

            lines.append(line)

        return "\n".join(lines) + "\n"

    def update_contributors_file(self, contributors: List[Dict]):
        """Update the CONTRIBUTORS.md file"""
        contributors_file = Path("CONTRIBUTORS.md")

        if not contributors_file.exists():
            print("CONTRIBUTORS.md not found!")
            return

        # Read current content
        content = contributors_file.read_text()

        # Categorize contributors
        categorized = self.categorize_contributors(contributors)

        # Generate new contributor sections
        major_section = self.generate_contributor_section(categorized["major"])
        regular_section = self.generate_contributor_section(categorized["regular"])
        occasional_section = self.generate_contributor_section(
            categorized["occasional"], include_contributions=False
        )

        # Generate statistics (excluding core team)
        core_team = {"mpusz", "JohelEGP", "chiphogg"}
        core_team_lower = {name.lower() for name in core_team}
        non_core_contributors = [
            c
            for c in contributors
            if c["login"].lower() not in core_team_lower and c.get("type") != "Bot"
        ]
        total_contributors = len(non_core_contributors)
        total_contributions = sum(c["contributions"] for c in non_core_contributors)

        stats_section = f"""## Statistics

- **Total Contributors**: {total_contributors}
- **Total Contributions**: {total_contributions}
- **Major Contributors** (50+ contributions): {len(categorized['major'])}
- **Regular Contributors** (10-49 contributions): {len(categorized['regular'])}
- **Occasional Contributors** (1-9 contributions): {len(categorized['occasional'])}

_Last updated: {self.get_current_date()}_
"""

        # Update the contributors section
        # Look for the CONTRIBUTORS_START/END markers
        start_marker = "<!-- CONTRIBUTORS_START -->"
        end_marker = "<!-- CONTRIBUTORS_END -->"

        if start_marker in content and end_marker in content:
            # Replace the content between markers
            before = content.split(start_marker)[0]
            after = content.split(end_marker)[1]

            new_content = f"""{before}{start_marker}

{stats_section}

### Major Contributors

_50+ contributions_

{major_section}

### Regular Contributors

_10-49 contributions_

{regular_section}

### All Contributors

_Everyone who has contributed to mp-units_

{occasional_section}

{end_marker}{after}"""

            contributors_file.write_text(new_content)
            print(f"Updated CONTRIBUTORS.md with {total_contributors} contributors")
        else:
            print("Could not find contributor markers in CONTRIBUTORS.md")

    def get_current_date(self) -> str:
        """Get current date in a readable format"""
        from datetime import datetime

        return datetime.now().strftime("%Y-%m-%d")


def get_github_token() -> Optional[str]:
    """Try to get GitHub token from various sources"""
    import os

    # Try environment variable
    token = os.getenv("GITHUB_TOKEN")
    if token:
        return token

    # Try git config
    try:
        result = subprocess.run(
            ["git", "config", "--get", "github.token"], capture_output=True, text=True
        )
        if result.returncode == 0:
            return result.stdout.strip()
    except Exception:
        pass

    return None


def main():
    """Main function"""
    # Get GitHub token (optional but recommended to avoid rate limits)
    token = get_github_token()
    if not token:
        print("Warning: No GitHub token found. You may hit API rate limits.")
        print("Set GITHUB_TOKEN environment variable for better performance.")

    # Initialize updater
    updater = ContributorUpdater("mpusz", "units", token)

    try:
        # Fetch contributors
        print("Fetching contributors from GitHub...")
        contributors = updater.fetch_contributors()
        print(f"Found {len(contributors)} contributors")

        # Update contributors file
        updater.update_contributors_file(contributors)

    except Exception as e:
        print(f"Error updating contributors: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
