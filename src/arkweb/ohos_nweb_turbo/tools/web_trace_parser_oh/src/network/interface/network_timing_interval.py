class NetWorkTimgInterval:
    def __init__(self, start_time, end_time):
        self.start_time = start_time
        self.end_time = end_time

    def get_during_time(self) -> float:
        return self.end_time - self.start_time
