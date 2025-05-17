import os
import subprocess
import re
import difflib
import time

from pathlib import Path
from colorama import init, Fore, Style

# Initialize colorama
init(autoreset=True)

COMPILER_PATH = r"..\out\build\x64-Debug\Wandelt.exe"
TEST_DIR = "wandelt"
OUTPUT_LL_PATH = "output.ll"

def extract_expected_ir(content):
    match = re.search(r"<\*\s*#expect(.*?)\*>", content, re.DOTALL)
    if match:
        return match.group(1).strip()
    return None

def extract_expected_fail(content):
    match = re.search(r"<\*\s*#expectfail(.*?)\*>", content, re.DOTALL)
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

def normalize_stderr(stderr_text):
    stderr_text = re.sub(r'\x1b\[[0-9;]*m', '', stderr_text)
    lines = [line.strip() for line in stderr_text.splitlines() if line.strip()]
    return "\n".join(lines).strip()

def run_test(index, total, file_path):
    start_time = time.perf_counter()

    with open(file_path, 'r') as f:
        content = f.read()

    expected_ir = extract_expected_ir(content)
    expected_fail = extract_expected_fail(content)

    if expected_ir is None and expected_fail is None:
        print(f"{Fore.YELLOW}[SKIP]{Style.RESET_ALL} No #expect or #expectfail block in {file_path}")
        return "skip"

    print(f"{Style.BRIGHT}[TEST] {index}/{total} {file_path}{Style.RESET_ALL}")

    try:
        result = subprocess.run(
            [COMPILER_PATH, "--files", str(file_path)],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=10
        )
    except subprocess.TimeoutExpired:
        print(f"{Fore.RED}[TIMEOUT]{Style.RESET_ALL} {file_path}")
        return "fail"

    elapsed_ms = (time.perf_counter() - start_time) * 1000

    if result.returncode != 0:
        # Handle #expectfail
        if expected_fail is not None:
            # Normalize both expected and actual stderr
            stderr_clean = normalize_stderr(result.stderr)
            expected_clean = normalize_stderr(expected_fail)

            if expected_clean == stderr_clean:
                print(f"{Fore.GREEN}[PASS]{Style.RESET_ALL} {file_path} "
                      f"{Style.DIM}({elapsed_ms:.1f} ms){Style.RESET_ALL}")
                return "pass"
            else:
                print(f"{Fore.RED}[FAIL]{Style.RESET_ALL} {file_path} - Expected failure output mismatch.")
                print(f"{Fore.MAGENTA}---- Expected stderr ----{Style.RESET_ALL}")
                print(expected_clean)
                print(f"{Fore.MAGENTA}---- Actual stderr ----{Style.RESET_ALL}")
                print(stderr_clean)
                print(f"{Fore.MAGENTA}---- Diff (Expected ⟷ Got) ----{Style.RESET_ALL}")
                expected_lines = expected_clean.splitlines(keepends=True)
                actual_lines = stderr_clean.splitlines(keepends=True)
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
                print(f"\n{Style.DIM}Time: {elapsed_ms:.1f} ms{Style.RESET_ALL}")
                return "fail"

        print(f"{Fore.RED}[FAIL]{Style.RESET_ALL} {file_path} - Compilation failed (non-zero exit code).")
        print("\n".join(line for line in result.stderr.splitlines() if line.strip()))
        print(f"{Style.DIM}Time: {elapsed_ms:.1f} ms{Style.RESET_ALL}")
        return "fail"

    if not os.path.exists(OUTPUT_LL_PATH):
        print(f"{Fore.RED}[FAIL]{Style.RESET_ALL} {file_path} - {OUTPUT_LL_PATH} not found.")
        print(f"{Style.DIM}Time: {elapsed_ms:.1f} ms{Style.RESET_ALL}")
        return "fail"

    with open(OUTPUT_LL_PATH, 'r') as f:
        actual_ir = f.read().strip()

    expected_ir_clean = normalize_ir(expected_ir)
    actual_ir_clean = normalize_ir(actual_ir)

    if actual_ir_clean == expected_ir_clean:
        print(f"{Fore.GREEN}[PASS]{Style.RESET_ALL} {file_path} "
              f"{Style.DIM}({elapsed_ms:.1f} ms){Style.RESET_ALL}")
        return "pass"
    else:
        print(f"{Fore.RED}[FAIL]{Style.RESET_ALL} {file_path} - Output mismatch.")
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
        print(f"\n{Style.DIM}Time: {elapsed_ms:.1f} ms{Style.RESET_ALL}")
        return "fail"

def main():
    test_files = list(Path(TEST_DIR).rglob("*.wdt"))
    total = len(test_files)

    if total == 0:
        print(f"{Fore.YELLOW}No test files found.{Style.RESET_ALL}")
        return

    passed = failed = skipped = 0

    print(f"{Style.BRIGHT}Running {total} tests...\n{Style.RESET_ALL}")
    total_start = time.perf_counter()

    for idx, test_file in enumerate(test_files, 1):
        result = run_test(idx, total, test_file)
        if result == "pass":
            passed += 1
        elif result == "fail":
            failed += 1
        elif result == "skip":
            skipped += 1

    total_elapsed = (time.perf_counter() - total_start) * 1000

    def percent(count):
        return f"{(count / total * 100):.1f}%"

    print(f"\n{Style.BRIGHT}Summary:{Style.RESET_ALL} "
          f"{Fore.GREEN}Passed: {passed} ({percent(passed)}){Style.RESET_ALL} | "
          f"{Fore.RED}Failed: {failed} ({percent(failed)}){Style.RESET_ALL} | "
          f"{Fore.YELLOW}Skipped: {skipped} ({percent(skipped)}){Style.RESET_ALL} | "
          f"{Style.BRIGHT}Total: {total}{Style.RESET_ALL}")
    print(f"{Style.DIM}Total time: {total_elapsed:.1f} ms{Style.RESET_ALL}")

if __name__ == "__main__":
    main()
