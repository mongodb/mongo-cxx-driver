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
Patches the root-level index.html file in each API doc site to match the state of the latest API doc site.
"""

from packaging.version import Version, InvalidVersion
from typing import List, Tuple

import bs4
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


def find_index_page(root: str, doc: str) -> str:
    """
    Return an absolute path to the index page of an API doc.
    """
    filename = 'index.html'
    parent = os.path.join(root, doc)
    index_page = os.path.join(parent, filename)
    if not os.path.exists(index_page):
        raise RuntimeError(f' - error: cannot find {filename} in {doc}!')
    if not os.path.isfile(index_page):
        raise RuntimeError(f' - error: {index_page} is not a file in {doc}!')
    return index_page


def format_latest_index_page(latest: str) -> None:
    """
    Format the latest index page to improve readability of future patch diffs.
    """
    with open(latest, "r+") as file:
        html = bs4.BeautifulSoup(file, 'html.parser')
        file.seek(0)
        file.write(html.prettify(formatter="html"))
        file.truncate()


def extract_latest_contents(latest) -> bs4.PageElement:
    """
    Return the page element corresponding to the contents of the latest index page.
    """
    with open(latest) as file:
        html = bs4.BeautifulSoup(file, 'html.parser')
        return html.find("div", class_='contents')


def patch_index_page(latest_contents: bs4.PageElement, index_page: str):
    """
    Replace the contents of the index page with the latest contents.
    """
    with open(index_page, "r+") as file:
        html = bs4.BeautifulSoup(file, 'html.parser')
        contents = html.find("div", class_="contents")
        contents.replace_with(latest_contents)

        file.seek(0)
        file.write(html.prettify(formatter="html"))
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

    print('Searching for index pages...')
    index_pages = [find_index_page(api_docs_path, doc) for doc in api_docs]
    print('Searching for index pages... done.')

    print(f' - Found {len(index_pages)} index pages within {len(api_docs)} API docs.')
    latest_doc = api_docs[-1]
    print(f' - Using {latest_doc} as the latest API doc.')

    (latest_index, index_pages) = (index_pages[-1], index_pages[:-1])

    print('Formatting latest index page...')
    format_latest_index_page(latest_index)
    print('Formatting latest index page... done.')

    print('Extracting latest index page contents...')
    latest_contents = extract_latest_contents(latest_index)
    print('Extracting latest index page contents... done.')

    print(f'Replacing contents of {len(index_pages)} index pages...')
    for page in index_pages:
        patch_index_page(latest_contents, page)
    print(f'Replacing contents of {len(index_pages)} index pages... done.')


if __name__ == '__main__':
    main()
