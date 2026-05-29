import os
import re
import xml.etree.ElementTree as ET


def get_project_name(cmake_path):
    """Return the project name, preferring package.xml over CMakeLists.txt."""
    package_xml = os.path.join(os.path.dirname(cmake_path), "package.xml")
    if os.path.exists(package_xml):
        name = ET.parse(package_xml).getroot().findtext("name")
        if name:
            return name.strip()
    with open(cmake_path, "r", encoding="utf-8") as f:
        content = f.read()
    m = re.search(r"project\s*\(\s*(\w+)", content)
    return m.group(1) if m else "Unknown"
