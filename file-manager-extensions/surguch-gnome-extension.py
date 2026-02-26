from subprocess import Popen
from urllib.parse import urlparse, unquote
from gi import require_version
from gettext import textdomain, gettext
import os

textdomain("surguch-gnome-extension")
_ = gettext
button_label = _("Sign files")

try:
    require_version('Nautilus', '4.1')
except ValueError:
    require_version('Nautilus', '4.0')

from gi.repository import Nautilus, GObject

class NautilusSurguch(Nautilus.MenuProvider, GObject.GObject):
    surguch = ["surguch", "-f"]

    def __init__(self):
        self.window = None

    def get_path(self,file):
        uri = file.get_uri()
        parsed = urlparse(uri)

        if parsed.scheme != "file":
            return None

        return unquote(parsed.path)

    def open_with_surguch(self, menu, files):
        paths = []

        for file in files:
            path = self.get_path(file)
            if path and os.path.isfile(path):
                paths.append(path)

        Popen(self.surguch + paths)

    def get_background_items(self, files):
        return

    def get_file_items(self, files):
        if not files or not self.surguch:
            return ()

        for file in files:
            path = self.get_path(file)
            if not path or not (os.path.isfile(path) and (not os.path.islink(path))):
                return ()

        menu_item = Nautilus.MenuItem(
                        name="NautilusSurguch::SignFiles",
                        label=button_label)

        menu_item.connect('activate', self.open_with_surguch, files)

        return menu_item,
