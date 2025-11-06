import argparse
import os

def check_pgo_exists(path):
    return os.path.exists(path)

def main():
    parser = argparse.ArgumentParser(description="Check if pgo profile exists")
    parser.add_argument('--pgo_path', type=str, required=True, help='pgo path')
    args = parser.parse_args()
    path = args.pgo_path
    abs_path = os.path.join(os.path.dirname(__file__), path)
    return check_pgo_exists(abs_path)

if __name__ == '__main__':
    value = main()
    if value:
        print("true")
    else:
        print("false")