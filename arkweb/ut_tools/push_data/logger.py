#Python
# coding=utf8
from colorama import init
import logging

# 创建自定义Formatter
class ColoredFormatter(logging.Formatter):
    def __init__(self, only_message = False):
        self.only_message_ = False
    FORMATS = {
        logging.DEBUG: "\033[94m[%(asctime)s][%(levelname)s][%(filename)s:%(lineno)d]: %(message)s\033[0m",
        logging.INFO: "\033[92m[%(asctime)s][%(levelname)s][%(filename)s:%(lineno)d]: %(message)s\033[0m",
        logging.WARNING: "\033[93m[%(asctime)s][%(levelname)s][%(filename)s:%(lineno)d]: %(message)s\033[0m",
        logging.ERROR: "\033[91m[%(asctime)s][%(levelname)s][%(filename)s:%(lineno)d]: %(message)s\033[0m",
        logging.CRITICAL: "\033[41m[%(asctime)s][%(levelname)s][%(filename)s:%(lineno)d]: %(message)s\033[0m"
    }
    FORMATS_ONLY_MESSAGE = {
        logging.DEBUG: "\033[94m%(message)s\033[0m",
        logging.INFO: "\033[92m%(message)s\033[0m",
        logging.WARNING: "\033[93m%(message)s\033[0m",
        logging.ERROR: "\033[91m%(message)s\033[0m",
        logging.CRITICAL: "\033[41m%(message)s\033[0m"
    }
    def format(self, record):
        if self.only_message_:
            log_fmt = self.FORMATS_ONLY_MESSAGE.get(record.levelno)
            formatter = logging.Formatter(log_fmt)
        else:
            log_fmt = self.FORMATS.get(record.levelno)
            formatter = logging.Formatter(log_fmt)
        return formatter.format(record) 

# 创建logger
logger=None
file_handler=None

def logger_init(logFileName):
    global logger, file_handler
    init(autoreset=True)
    logger = logging.getLogger(logFileName)
    logger.setLevel(logging.DEBUG)

    # 创建console handler并设置等级之后将其添加到logger
    console_handler = logging.StreamHandler()
    console_handler.setLevel(logging.DEBUG)
    formatter = ColoredFormatter(only_message=False)
    console_handler.setFormatter(formatter)
    logger.addHandler(console_handler)

    # 文件处理器
    file_handler = logging.FileHandler(logFileName, mode='w', encoding='utf-8')
    file_handler.setLevel(logging.DEBUG)
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)

    logger.info(f"logfile:{logFileName}, logger init success!")
    return logger, file_handler
