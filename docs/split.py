#!/usr/bin/env python3
"""여러 문제를 한 파일에 쌓아둔 소스에서 블록 하나를 뽑아 컴파일·실행한다.

swtest/ 의 7개 파일(28~33, 37)은 한 파일에 완성된 프로그램이 2~3개 들어 있어
그대로는 컴파일되지 않는다(전역 중복 정의). 블록은 `// BOJ <번호>` 주석으로 시작한다.

  python3 docs/split.py 30_BOJ_15649_15654_15663_순열.cpp          블록 목록
  python3 docs/split.py 30_...순열.cpp 15654                        해당 블록만 컴파일 후 실행
  python3 docs/split.py 30_...순열.cpp 15654 < input.txt            표준입력 넘겨서 실행
"""
import re, subprocess, sys, tempfile
from pathlib import Path

SRC = Path(__file__).resolve().parent.parent / "swtest"
HEADER = re.compile(r"^//\s*(?:BOJ|백준)\s*(\d+)", re.M)


def blocks(path):
    """[(문제번호, 시작줄, 끝줄, 소스텍스트), ...]"""
    lines = path.read_text(encoding="utf-8").splitlines(keepends=True)
    starts = [(i, m.group(1)) for i, l in enumerate(lines)
              if (m := HEADER.match(l))]
    if not starts:
        return [("전체", 1, len(lines), "".join(lines))]

    # 첫 헤더 앞의 내용은 첫 블록에 붙인다
    starts[0] = (0, starts[0][1])
    out = []
    for k, (begin, num) in enumerate(starts):
        end = starts[k + 1][0] if k + 1 < len(starts) else len(lines)
        out.append((num, begin + 1, end, "".join(lines[begin:end])))
    return out


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        return 2

    path = Path(sys.argv[1])
    if not path.exists():
        path = SRC / sys.argv[1]
    if not path.exists():
        print(f"파일 없음: {sys.argv[1]}")
        return 2

    found = blocks(path)

    if len(sys.argv) < 3:
        print(f"{path.name} — 블록 {len(found)}개\n")
        for num, begin, end, _ in found:
            print(f"  BOJ {num}   줄 {begin}~{end}")
        print(f"\n실행: python3 docs/split.py {path.name} {found[0][0]}")
        return 0

    want = sys.argv[2]
    match = [b for b in found if b[0] == want]
    if not match:
        print(f"블록 없음: {want}   (있는 것: {', '.join(b[0] for b in found)})")
        return 2

    num, begin, end, text = match[0]
    with tempfile.TemporaryDirectory() as tmp:
        cpp = Path(tmp) / f"boj{num}.cpp"
        exe = Path(tmp) / f"boj{num}"
        cpp.write_text(text, encoding="utf-8")

        build = subprocess.run(["g++", "-O2", "-o", str(exe), str(cpp)],
                               capture_output=True, text=True)
        if build.returncode != 0:
            print(f"컴파일 실패 (BOJ {num}, 줄 {begin}~{end})")
            print(build.stderr)
            return 1

        print(f"--- BOJ {num} (줄 {begin}~{end}) ---", file=sys.stderr)
        return subprocess.run([str(exe)], stdin=sys.stdin).returncode


if __name__ == "__main__":
    sys.exit(main())
