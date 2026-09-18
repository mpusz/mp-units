# shellcheck shell=bash
# shellcheck disable=SC2034  # read by get_mrdocs.sh, which sources this file
# The MrDocs release the API reference is generated with, and the checksums of
# its binary archives.
#
# Pinned on purpose. The output is version-sensitive - page names, section
# headings and the tagfile all move between releases - so an unpinned `latest`
# would change the published site without anything in this repo changing.
#
# To upgrade: bump MRDOCS_VERSION, replace the checksums from
# https://github.com/cppalliance/mrdocs/releases, regenerate, and review the
# diff before committing.

MRDOCS_VERSION="2026.9.4"

MRDOCS_SHA256_Linux="1e0a455d3e68cb0bacc0884b10fbdf9865b260ae76ce46003c5542e2489af769"
MRDOCS_SHA256_Darwin="a1cfa12f2a980772f91610091978b2bb40302cbbc0740500026ea870c6893e2e"
