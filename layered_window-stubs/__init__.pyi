from typing import Tuple

def init(
    size: Tuple[int, int],
    position: Tuple[int, int] = (0, 0),
    disable_move: bool = False,
    always_on_top: bool = False,
    tool_window: bool = False,
    title: str = 'layered_window',
) -> memoryview: ...

def update(multiply_alpha: bool = True, flush: bool = True) -> None: ...
