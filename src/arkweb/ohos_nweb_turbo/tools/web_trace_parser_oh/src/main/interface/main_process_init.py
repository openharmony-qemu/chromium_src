from enum import Enum


class MainResourceType(Enum):
    reload_load_url = 0,
    web_jump_other_web = 1,
    single_page = 2


class MainProcessInitInfo:
    def __init__(self):
        self.create_nweb_start = 0
        self.main_resource_start = 0
        self.main_resource_net_start = 0
        self.main_resource_connect_start = 0
        self.main_resource_connect_end = 0
        self.main_resource_response_start = 0
        self.main_resource_download_start = 0
        self.main_resource_download_end = 0
        self.main_resource_net_end = 0
        self.main_resource_end = 0
        self.main_resource_type = 0
        self.commit_end = 0
        self.dom_content_loaded_start = 0
        self.useragent_changed = False

    def __getitem__(self, key):
        return getattr(self, key)

    def __setitem__(self, key, value):
        setattr(self, key, value)
