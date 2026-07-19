#!/usr/bin/env python3
"""
Glint Engine Build System
- Auto-discovers vendors under vendor/*/vendor.py
- Reads vendor/*/*.gvf for compiler flags
- Generates compile_commands.json for clangd
- Builds base/ unity object → build/base/base.o
- Discovers and builds plugins as shared libraries
- Compiles and runs tests
"""
import importlib.util
import json
import platform
import shutil
import subprocess
import sys
from pathlib import Path


def log(msg, color=""):
    print(f"{color}{msg}{Color.RESET}")


# ── Paths ──────────────────────────────────────────────────────────────────
ROOT = Path(__file__).resolve().parent
BUILD = ROOT / "build"
BASE = ROOT / "base"
BASE_UNITY = BASE / "base.c"
BASE_OBJ = BUILD / "base" / "base.o"
TEST_SRC = ROOT / "tests"

# Platform detection
HOST_OS = platform.system().lower()  # "linux", "windows", "darwin"
PLATFORM_DIR = {"linux": "linux", "windows": "windows",
                "darwin": "darwin"}.get(HOST_OS)
if not PLATFORM_DIR:
    log(f"unsupported OS: {HOST_OS}")
    sys.exit(1)

IS_LINUX = HOST_OS == "linux"
IS_WINDOWS = HOST_OS == "windows"
IS_MAC = HOST_OS == "darwin"

# Shared library extension per platform
SO_EXT = ".so" if IS_LINUX else ".dylib" if IS_MAC else ".dll"


# Include dirs
INCLUDES = [
    "-I.", "-Ibase",
    "-Ibase/core", "-Ibase/math", "-Ibase/memory",
    "-Ibase/os", "-Ibase/string", "-Ibase/util",
    "-Ibase/log", "-Ibase/duration",
]

# Standard C flags
FLAGS = {"std": "-std=c23", "warnings": []}


# Compiler — prefer gcc on Linux, clang on macOS
if IS_LINUX:
    COMPILER = shutil.which("gcc") or "clang"
    FLAGS["std"] = "-std=gnu23"
elif IS_MAC:
    COMPILER = "clang"
else:
    COMPILER = "clang"

# ── Utilities ──────────────────────────────────────────────────────────────


class Color:
    RED = "\033[0;31m"
    GREEN = "\033[0;32m"
    YELLOW = "\033[1;33m"
    BLUE = "\033[0;34m"
    CYAN = "\033[0;36m"
    RESET = "\033[0m"


def run(cmd, **kw):
    try:
        subprocess.run(cmd, check=True, **kw)
    except subprocess.CalledProcessError as e:
        log(f"fatal: {' '.join(cmd)}", Color.RED)
        sys.exit(1)


# ── Vendor discovery ──────────────────────────────────────────────────────


def discover_vendors():
    """Yield (name, module_path) for every vendor/*/vendor.py found."""
    vendor_dir = ROOT / "vendor"
    if not vendor_dir.exists():
        return
    count = 0
    for d in sorted(vendor_dir.iterdir()):
        vp = d / "vendor.py"
        if d.is_dir() and vp.exists():
            yield d.name, vp
            count += 1
    if count:
        log(f"discovered {count} vendors", Color.CYAN)


def run_vendor_setup(name, module_path):
    """Import vendor.py and call setup()."""
    spec = importlib.util.spec_from_file_location(
        f"vendor_{name}", module_path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    log(f"running {name}/vendor.py: setup...", Color.CYAN)
    mod.setup()


def collect_vendor_flags():
    """Read all vendor/*/*.gvf files, return (includes, libraries)."""
    includes = []
    libraries = []
    vendor_dir = ROOT / "vendor"
    if not vendor_dir.exists():
        return includes, libraries
    for d in sorted(vendor_dir.iterdir()):
        gvf = d / f"{d.name}.gvf"
        if gvf.exists():
            for line in gvf.read_text().splitlines():
                line = line.strip()
                if line.startswith("-I"):
                    includes.append(line)
                elif line.startswith("-L") or line.startswith("-l"):
                    libraries.append(line)
    return includes, libraries


# ── Plugin discovery ───────────────────────────────────────────────────────


def discover_plugins():
    """Yield (dir, manifest) for every plugins/*/plugin.json found."""
    plugins_dir = ROOT / "plugins"
    if not plugins_dir.exists():
        return
    count = 0
    for d in sorted(plugins_dir.iterdir()):
        mf = d / "plugin.json"
        if d.is_dir() and mf.exists():
            manifest = json.loads(mf.read_text())
            yield d, manifest
            count += 1
    if count:
        log(f"discovered {count} plugins", Color.CYAN)


def build_plugin(plugin_dir, manifest, flags, vendor_libs):
    """Build a single plugin as a shared library."""
    name = manifest["name"]
    log(f"building plugin {name}...", Color.BLUE)

    # Find source files in the plugin directory
    sources = sorted(plugin_dir.glob("*.c"))
    if not sources:
        log(f"  no .c sources in {plugin_dir}", Color.YELLOW)
        return None

    # Output path: build/plugins/<name>.so (dots become paths)
    out_name = name.replace(".", "/") + SO_EXT
    out_path = BUILD / "plugins" / out_name
    out_path.parent.mkdir(parents=True, exist_ok=True)

    # Compile with -shared -fPIC for shared library
    # Only link vendor libs declared in manifest
    plugin_libs = []
    for dep in manifest.get("vendor_deps", []):
        # Match vendor libs by checking if the dep name appears in the lib flag
        for lib in vendor_libs:
            if dep in lib:
                plugin_libs.append(lib)

    cmd = [COMPILER, *flags, "-shared", "-fPIC"]
    cmd.extend(str(s) for s in sources)
    cmd.extend(["-o", str(out_path)])
    cmd.extend(plugin_libs)

    run(cmd)
    log(f"  -> {out_path}", Color.GREEN)
    return out_path


# ── Build helpers ─────────────────────────────────────────────────────────


def clean_build():
    log("cleaning build/...", Color.YELLOW)
    shutil.rmtree(BUILD, ignore_errors=True)


def build_base_object(flags):
    """Build base.o from base/base.c unity file."""
    log("building base.o (unity)...", Color.BLUE)
    (BUILD / "base").mkdir(parents=True, exist_ok=True)
    run([
        COMPILER,
        *flags,
        "-c", str(BASE_UNITY),
        "-o", str(BASE_OBJ)])


def generate_compile_commands(flags):
    """Write compile_commands.json for clangd, covering base/base.c and every test."""
    entries = [{
        "directory": str(ROOT),
        "command": f"{COMPILER} {' '.join(flags)} -c base/base.c -o build/base/base.o",
        "file": "base/base.c",
    }]
    for test_dir in sorted(TEST_SRC.glob("g_test_*")):
        if not test_dir.is_dir():
            continue
        for src in sorted(test_dir.glob("*.c")):
            name = test_dir.name.replace("g_test_", "")
            entries.append({
                "directory": str(ROOT),
                "command": f"{COMPILER} {' '.join(flags)} -c {src.relative_to(ROOT)} -o build/tests/g_test_{name}",
                "file": str(src.relative_to(ROOT)),
            })
    (ROOT / "compile_commands.json").write_text(json.dumps(entries, indent=2))
    log("compile_commands.json updated", Color.GREEN)


def compile_test(name, flags, libs):
    """Compile a single test into one binary.

    Pulls in every .c in the test directory, plus tests/g_test_main.c
    which defines main() and the global test registry/stats.
    """
    test_dir = TEST_SRC / f"g_test_{name}"
    test_sources = sorted(test_dir.glob("*.c"))
    if not test_sources:
        log(f"no .c sources in {test_dir}", Color.YELLOW)
        return None
    main_c = TEST_SRC / "g_test_main.c"
    sources = [*test_sources, main_c] if main_c.exists() else test_sources
    log(f"compiling test {name}...", Color.BLUE)
    out = BUILD / "tests" / f"g_test_{name}"
    out.parent.mkdir(parents=True, exist_ok=True)
    cmd = [COMPILER, *flags] + [str(s)
                                for s in sources] + ["-o", str(out)] + libs
    run(cmd)
    return out


def run_test(bin_path):
    log(f"running {bin_path.name}...", Color.CYAN)
    result = subprocess.run([str(bin_path)], capture_output=False)
    if result.returncode == 0:
        log(f"  {bin_path.name} passed", Color.GREEN)
    else:
        log(f"  {bin_path.name} failed", Color.RED)
    return result.returncode == 0


def find_tests():
    return sorted(d.name.replace("g_test_", "") for d in TEST_SRC.glob("g_test_*") if d.is_dir())

# ── Main ──────────────────────────────────────────────────────────────────


def main():
    args = sys.argv[1:]
    debug = "--debug" in args
    clean = "--clean" in args
    test_only = "--test" in args
    vendor_cmd = None
    test_filter = None

    for a in args:
        if a.startswith("--vendor="):
            vendor_cmd = a.split("=", 1)[1]
        if a.startswith("--test="):
            test_filter = a.split("=", 1)[1]

    # ── Clean ──────────────────────────────────────────────────────────
    if clean:
        clean_build()

    # ── Vendors ────────────────────────────────────────────────────────
    if vendor_cmd:
        for name, vp in discover_vendors():
            if name == vendor_cmd or vendor_cmd == "all":
                spec = importlib.util.spec_from_file_location(
                    f"vendor_{name}", vp)
                mod = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(mod)
                subcmd = "setup"
                if "--distclean" in args:
                    subcmd = "distclean"
                elif clean:
                    subcmd = "clean"
                log(f"vendor {name}: {subcmd}...", Color.YELLOW)
                getattr(mod, subcmd)()
        if vendor_cmd in ("all",):
            return

    if not clean:
        for name, vp in discover_vendors():
            run_vendor_setup(name, vp)

    # ── Flags ──────────────────────────────────────────────────────────
    vendor_includes, vendor_libs = collect_vendor_flags()

    flags = [FLAGS["std"]] + FLAGS["warnings"]
    flags += INCLUDES + vendor_includes
    if debug:
        flags += ["-g", "-O0"]
    else:
        flags += ["-O2", "-DNDEBUG"]

    # ── compile_commands.json ──────────────────────────────────────────
    generate_compile_commands(flags)

    # ── Build base.o ───────────────────────────────────────────────────
    if not test_only:
        build_base_object(flags)

        # ── Plugins ────────────────────────────────────────────────────────
        log("building plugins...", Color.BLUE)
        plugins_built = 0
        for plugin_dir, manifest in discover_plugins():
            out = build_plugin(plugin_dir, manifest, flags, vendor_libs)
            if out:
                plugins_built += 1
        if plugins_built:
            log(f"built {plugins_built} plugins", Color.GREEN)
        else:
            log("no plugins built", Color.YELLOW)

    # ── Tests ──────────────────────────────────────────────────────────
    test_names = find_tests()
    if test_filter:
        test_names = [t for t in test_names if test_filter in t]

    bins = []
    for t in test_names:
        b = compile_test(t, flags, vendor_libs)
        if b:
            bins.append(b)

    passed = failed = 0
    for b in bins:
        if run_test(b):
            passed += 1
        else:
            failed += 1

    print()
    if bins:
        log(f"results: {passed} passed, {failed} failed",
            Color.GREEN if failed == 0 else Color.RED)
    log("done.", Color.GREEN)


if __name__ == "__main__":
    main()
