"""SDL3 vendor setup — clone, build, generate flags."""
import os
import shutil
import subprocess
from pathlib import Path

NAME = "sdl3"
DEPENDENCIES = ["git", "cmake"]
VERSION = "release-3.4.12"

HERE = Path(__file__).parent
SRC = HERE / "src"
BUILD = HERE / "build"
INSTALL = HERE / "install"
STAMP = INSTALL / ".glint-version"
GVF = HERE / f"{NAME}.gvf"


def setup():
    if STAMP.exists() and STAMP.read_text().strip() == VERSION:
        print(f"[{NAME}] {VERSION} already installed")
        _generate_gvf()
        return

    shutil.rmtree(BUILD, ignore_errors=True)
    shutil.rmtree(INSTALL, ignore_errors=True)

    if not SRC.exists():
        print(f"[{NAME}] Cloning SDL3 {VERSION}...")
        subprocess.run(
            ["git", "clone", "--depth", "1", "--branch", VERSION,
             "https://github.com/libsdl-org/SDL", str(SRC)],
            check=True)

    _cmake("-S", str(SRC), "-B", str(BUILD),
           "-DCMAKE_BUILD_TYPE=Release",
           "-DCMAKE_INSTALL_PREFIX=" + str(INSTALL),
           "-DBUILD_SHARED_LIBS=OFF",
           "-DSDL_TESTS=OFF", "-DSDL_EXAMPLES=OFF")
    _cmake("--build", str(BUILD), "--parallel")
    _cmake("--install", str(BUILD))

    STAMP.parent.mkdir(parents=True, exist_ok=True)
    STAMP.write_text(VERSION)
    _generate_gvf()
    print(f"[{NAME}] installed {VERSION}")


def clean():
    shutil.rmtree(BUILD, ignore_errors=True)
    shutil.rmtree(INSTALL, ignore_errors=True)
    GVF.unlink(missing_ok=True)
    print(f"[{NAME}] cleaned")


def distclean():
    clean()
    shutil.rmtree(SRC, ignore_errors=True)
    print(f"[{NAME}] source removed")


def _cmake(*args):
    subprocess.run(["cmake", *args], cwd=HERE, check=True)


def _generate_gvf():
    """Use pkg-config to get all SDL3 static link flags."""
    env = dict(os.environ, PKG_CONFIG_PATH=str(INSTALL / "lib" / "pkgconfig"))

    try:
        cflags = subprocess.run(
            ["pkg-config", "--cflags", "sdl3"],
            capture_output=True, text=True, check=True, env=env).stdout.strip()
        libs = subprocess.run(
            ["pkg-config", "--libs", "--static", "sdl3"],
            capture_output=True, text=True, check=True, env=env).stdout.strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        # Fallback to manual flags
        cflags = f"-I{INSTALL / 'include' / 'SDL3'}"
        libs = f"-L{INSTALL / 'lib'} -lSDL3 -pthread -lm"

    GVF.write_text(f"{cflags}\n{libs}\n")
    print(f"[{NAME}] generated {GVF.name}")


if __name__ == "__main__":
    import sys
    {"setup": setup, "clean": clean, "distclean": distclean}.get(
        sys.argv[1] if len(sys.argv) > 1 else "setup", setup)()
