#!/usr/bin/env python3
"""복습 노트 검증 — docs/ 안에서 실행.

  python3 docs/check.py          깨진 링크 + 진도 카운트
  python3 docs/check.py --todo   아직 안 쓴 노트 목록
"""
import re, sys
from pathlib import Path
from urllib.parse import unquote

DOCS = Path(__file__).resolve().parent
ROOT = DOCS.parent
LINK = re.compile(r"\[[^\]]*\]\(([^)]+)\)")


def links():
    for md in sorted(DOCS.rglob("*.md")):
        for target in LINK.findall(md.read_text(encoding="utf-8")):
            if target.startswith(("http", "#")):
                continue
            # 마크다운 링크는 공백을 %20 으로 이스케이프한다 → 파일 경로로 되돌린다
            yield md, unquote(target.split("#")[0])


def main():
    todo_only = "--todo" in sys.argv

    broken, pending = [], []
    for md, target in links():
        path = (md.parent / target).resolve()
        if path.exists():
            continue
        # 아직 안 쓴 문제 노트는 정상 (앞으로 채울 자리).
        # 상대경로(../G05/...)도 잡히도록 해석된 경로로 판단한다.
        is_note = (DOCS / "review") in path.parents
        (pending if is_note else broken).append((md.relative_to(ROOT), target))

    if todo_only:
        print(f"아직 안 쓴 노트 {len(pending)}개\n")
        for md, target in pending:
            print(f"  {target}")
        return 0

    if broken:
        print(f"❌ 깨진 링크 {len(broken)}개")
        for md, target in broken:
            print(f"   {md} -> {target}")
    else:
        print("✅ 깨진 링크 없음")

    notes = list((DOCS / "review").rglob("*.md")) if (DOCS / "review").exists() else []
    srcs = list((ROOT / "swtest").glob("*.cpp"))
    boxes = (DOCS / "INDEX.md").read_text(encoding="utf-8")
    done = boxes.count("- [x]")
    todo = boxes.count("- [ ]")
    open_issue = boxes.count("- [\U0001f534]")   # 미해결(소스 없음/스텁)
    total = done + todo + open_issue

    print(f"\n진도    {done} / {total} 문제" +
          (f"  (미해결 {open_issue})" if open_issue else ""))
    print(f"노트    {len(notes)} 장 작성됨  (미작성 {len(pending)})")
    print(f"소스    {len(srcs)} 개")
    if total != len(srcs):
        print(f"⚠️  INDEX 체크박스({total}) != 소스({len(srcs)})")
    return 1 if broken else 0


if __name__ == "__main__":
    sys.exit(main())
