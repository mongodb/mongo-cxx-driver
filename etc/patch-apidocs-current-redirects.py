#!/usr/bin/env python3

# Copyright 2009-present MongoDB, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""
Patches HTML files within the latest API doc directory (under APIDOCSPATH) to
redirect users from `/api/current` to canonical URLs under `/api/mongocxx-X.Y.Z`.
"""

from concurrent.futures import ProcessPoolExecutor
from packaging.version import Version, InvalidVersion
from pathlib import Path
from typing import List, Tuple

import re
import os


def find_api_docs_path() -> str:
    """
    Return an absolute path to the directory containing the API docs.
    """
    api_docs_path: str | None = os.environ.get('APIDOCSPATH')
    if not api_docs_path:
        raise RuntimeError('APIDOCSPATH environment variable is not set!')

    if not os.path.exists(api_docs_path):
        raise RuntimeError('path to API docs does not exist!')

    return os.path.abspath(api_docs_path)


def find_api_docs(api_docs_path: str) -> List[str]:
    """
    Return a list of API doc directories by name.
    """
    api_docs: List[str] = []
    for dir in os.scandir(api_docs_path):
        if dir.is_dir() and not dir.is_symlink():
            api_docs.append(dir.name)

    # Sort by legacy vs. modern, then by SemVer. Example:
    #  - legacy-0.1.0
    #  - legacy-0.2.0
    #  - legacy-0.10.0
    #  - mongocxx-3.1.0
    #  - mongocxx-3.2.0
    #  - mongocxx-3.10.0
    # Skip directories with a version suffix, e.g. `mongocxx-1.2.3-rc0`.
    def by_version(p: str) -> Tuple[bool, Version] | None:
        is_legacy: bool = p.startswith('legacy-')
        try:
            version = p.removeprefix('legacy-') if is_legacy else p.removeprefix('mongocxx-')
            if version.find('-') != -1:
                print(f' - Skipping: {p}')
                return None
            return (not is_legacy, Version(version))
        except InvalidVersion:
            raise RuntimeError(f'unexpected API doc name "{p}": APIDOCSPATH may not be correct!') from None

    api_docs = [doc for doc in api_docs if by_version(doc) is not None]
    api_docs.sort(key=by_version)

    return api_docs


def patch_redirect_current_pages(apidocspath, latest):
    """
    Patch all HTML files under the latest API doc directory.
    """

    pages: List[Path] = []

    for (dirpath, _, filenames) in os.walk(os.path.join(apidocspath, latest)):
        for filename in filenames:
            page = Path(os.path.join(dirpath, filename))
            if page.suffix == '.html':
                pages.append(page)

    futures = []

    with ProcessPoolExecutor() as executor:
        for page in pages:
            futures.append(executor.submit(insert_current_redirect, apidocspath, page, latest))

    for future in futures:
        future.result()


def insert_current_redirect(apidocspath, page, latest):
    """
    Insert a <link> and <script> at the end of the <head> section.
    Skip modifying the document if the patch tag is found.
    """

    path = str(Path(page).relative_to(os.path.join(apidocspath, latest)))

    patch_tag = f'patch-apidocs-current-redirects: {latest}'

    is_patched = re.compile(patch_tag)
    end_of_head_re = re.compile(r'^(\s*)</head>$')

    with open(page, "r+") as file:
        lines = [line for line in file]

        idx = None
        indent = ''

        for idx, line in enumerate(lines):
            if is_patched.search(line):
                # This file has already been patched.
                return

            m = end_of_head_re.match(line)
            if m:
                # Patched index.html has 1-space indentation. The rest have none.
                indent = '' if m.group(1) == '' else '  '
                end_of_head = idx
                break

        if idx is None:
            raise RuntimeError(f'could not find end of `<head>` in {path}')

        # Insert patch tag to avoid repeated patch of the same file.
        lines.insert(end_of_head, indent + f'<!-- {patch_tag} -->\n')
        end_of_head += 1

        # Canonical URL. Inform search engines about the redirect.
        lines.insert(
            end_of_head,
            indent + f'<link rel="canonical" href="https://mongocxx.org/api/{latest}/{path}"/>\n')
        end_of_head += 1

        # Redirect script. Avoid generating history for the `/current` page during the redirect.
        script = ''
        script += indent + '<script type="text/javascript">\n'
        script += indent + 'if (window.location.pathname.startsWith("/api/current/")) {\n'
        script += indent + '  window.location.replace(\n'
        script += indent + f'    window.location.href.replace("/api/current/", "/api/{latest}/")\n'
        script += indent + '  )\n'
        script += indent + '}\n'
        script += indent + '</script>\n'
        lines.insert(end_of_head, script)
        end_of_head += 1

        file.seek(0)
        for line in lines:
            file.write(line)
        file.truncate()


def main():
    api_docs_path: str = find_api_docs_path()

    print(f'Patching API docs in: {api_docs_path}')

    print('Finding API docs...')
    api_docs = find_api_docs(api_docs_path)
    if len(api_docs) == 0:
        raise RuntimeError(f'no API docs found: APIDOCSPATH may not be correct!')
    print('Finding API docs... done.')

    print(f' - Found {len(api_docs)} API docs: {api_docs[0]} ... {api_docs[-1]}')

    latest_doc = api_docs[-1]
    print(f' - Using {latest_doc} as the latest API doc.')

    print(f'Patching latest API doc pages to redirect from /current to /{latest_doc}...')
    patch_redirect_current_pages(api_docs_path, latest_doc)
    print(f'Patching latest API doc pages to redirect from /current to /{latest_doc}... done.')


if __name__ == '__main__':
    main()
