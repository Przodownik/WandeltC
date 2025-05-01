import os
import subprocess
import re
import difflib

from pathlib import Path
from colorama import init, Fore, Style

# Initialize colorama
init(autoreset=True)

COMPILER_PATH = r"..\out\build\x64-Debug\Wandelt.exe"
TEST_DIR = "wandelt"
OUTPUT_LL_PATH = "output.ll"

def extract_expected_ir(content):
    match = re.search(r"/\*\s*#expect(.*?)\*/", content, re.DOTALL)
    if match:
        return match.group(1).strip()
    return None

def normalize_ir(ir_text):
    lines = ir_text.splitlines()
    return "\n".join(
        line.strip()
        for line in lines
        if not any(line.strip().startswith(prefix) for prefix in [
            "target datalayout =",
            "target triple ="
        ])
    ).strip()

def run_test(index, total, file_path):
    with open(file_path, 'r') as f:
        content = f.read()
    
    expected_ir = extract_expected_ir(content)
    if expected_ir is None:
        print(f"{Fore.YELLOW}[SKIP]{Style.RESET_ALL} No #expect block in {file_path.name}")
        return False

    print(f"{Style.BRIGHT}[TEST] {index}/{total} {file_path.name}{Style.RESET_ALL}")

    try:
        result = subprocess.run(
            [COMPILER_PATH, "--files", str(file_path)],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=10
        )
    except subprocess.TimeoutExpired:
        print(f"{Fore.RED}[TIMEOUT]{Style.RESET_ALL} {file_path.name}")
        return False

    if result.returncode != 0:
        print(f"{Fore.RED}[FAIL]{Style.RESET_ALL} {file_path.name} - Compilation failed (non-zero exit code).")
        print("\n".join(line for line in result.stderr.splitlines() if line.strip()))
        return False

    if not os.path.exists(OUTPUT_LL_PATH):
        print(f"{Fore.RED}[FAIL]{Style.RESET_ALL} {file_path.name} - {OUTPUT_LL_PATH} not found.")
        return False

    with open(OUTPUT_LL_PATH, 'r') as f:
        actual_ir = f.read().strip()

    expected_ir_clean = normalize_ir(expected_ir)
    actual_ir_clean = normalize_ir(actual_ir)

    if actual_ir_clean == expected_ir_clean:
        print(f"{Fore.GREEN}[PASS]{Style.RESET_ALL} {file_path.name}")
        return True
    else:
        print(f"{Fore.RED}[FAIL]{Style.RESET_ALL} {file_path.name} - Output mismatch.")
        print(f"{Fore.CYAN}---- STDERR ----{Style.RESET_ALL}")
        print("\n".join(line for line in result.stderr.splitlines() if line.strip()))

        print(f"{Fore.MAGENTA}---- Diff (Expected ⟷ Got) ----{Style.RESET_ALL}")
        expected_lines = expected_ir_clean.splitlines(keepends=True)
        actual_lines = actual_ir_clean.splitlines(keepends=True)
        diff = difflib.unified_diff(
            expected_lines,
            actual_lines,
            fromfile="expected",
            tofile="actual",
            lineterm=""
        )
        for line in diff:
            if line.startswith("+") and not line.startswith("+++"):
                print(Fore.GREEN + line, end="")
            elif line.startswith("-") and not line.startswith("---"):
                print(Fore.RED + line, end="")
            elif line.startswith("@@"):
                print(Fore.CYAN + line, end="")
            else:
                print(line, end="")
        print()
        return False

def main():
    test_files = list(Path(TEST_DIR).rglob("*.wdt"))
    total = len(test_files)

    if total == 0:
        print(f"{Fore.YELLOW}No test files found.{Style.RESET_ALL}")
        return

    passed = 0
    print(f"{Style.BRIGHT}Running {total} tests...\n{Style.RESET_ALL}")
    for idx, test_file in enumerate(test_files, 1):
        if run_test(idx, total, test_file):
            passed += 1

    print(f"\n{Style.BRIGHT}Summary:{Style.RESET_ALL} {Fore.GREEN}{passed}{Style.RESET_ALL}/{total} tests passed.")

if __name__ == "__main__":
    main()
