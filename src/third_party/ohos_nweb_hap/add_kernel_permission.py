import subprocess
import sys
import os

def main(args):
    llvm_objcopy = args[args.index("--llvm-objcopy") + 1]
    input_file = args[args.index("--input") + 1]
    encaps_json = args[args.index("--encaps-json") + 1]
    output_file = args[args.index("--output") + 1]
    os.chmod(llvm_objcopy, 0o777)

    cmd = [
        llvm_objcopy,
        "--add-section", f".kernelpermission={encaps_json}",
        input_file,
        output_file
    ]

    result = subprocess.run(cmd, capture_output=True, text=True)

    print("STDERR:")
    print(result.stderr)
    
    if result.returncode == 0:
        print("add kernelpermission successfully")
    else:
        print(f"Command failed with return code: {result.returncode}")
    
    sys.exit(result.returncode)

if __name__ == "__main__":
    main(sys.argv)