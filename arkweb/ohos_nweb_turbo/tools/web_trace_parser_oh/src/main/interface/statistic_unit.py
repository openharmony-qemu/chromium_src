from dataclasses import dataclass, asdict


@dataclass
class StatisticUnit:
    def __init__(
            self,
            title: str = "",
            key_map: dict[str, str] = {},
            data_map: dict[str, str | int | float] = {}
    ):
        self._title = title
        self._key_map = key_map
        self._data_map = data_map

    @property
    def title(self) -> str:
        return self._title

    @property
    def key_map(self) -> dict[str, str]:
        return self._key_map

    @property
    def data_map(self) -> dict[str, str | int | float]:
        return self._data_map

