from typing import Tuple

def init(
    size: Tuple[int, int],
    position: Tuple[int, int],
    disable_move: bool = False,
    always_on_top: bool = False,
    tool_window: bool = False
): ...

def update(multiply_alpha: bool = True, flush: bool = True): ...
