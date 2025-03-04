import shutil
import os

current_path = os.getcwd()
src_folder = current_path + '/../../v8/include'
dst_folder = current_path + '/v8-include'

if os.path.exists(dst_folder):
    shutil.rmtree(dst_folder)
shutil.copytree(src_folder, dst_folder)
