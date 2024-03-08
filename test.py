import os

import layered_window

mem = layered_window.init((360, 360), (100, 100), tool_window=True)

while True:
    mem[:] = os.urandom(360 * 360 * 4)
    layered_window.update()
